#include "Dispatcher.h"
#include<algorithm>
#include "Simulator.h"
#include "PublicFunction.h"

const float MAX_FLOAT = numeric_limits<float>::max();

bool ascendSortForCustId(Customer* item1, Customer* item2) {
	return item1->id < item2->id;
}

Dispatcher::Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, Customer depot, float capacity, int timeSlotLen, 
					   int samplingRate = 30): depot(depot), capacity(capacity), timeSlotLen(timeSlotLen){
	int custNum = staticCustomerSet.end() - staticCustomerSet.begin() + dynamicCustomerSet.end() - dynamicCustomerSet.begin(); // 总顾客数
	ServedCustomerId.reserve(custNum);     // 已经服务过的顾客id
	promisedCustomerId.reserve(custNum);
	waitCustomerId.reserve(custNum);
	rejectCustomerId.reserve(custNum);
	vector<Customer*>::iterator custIter = staticCustomerSet.begin();
	for(custIter; custIter < staticCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		allCustomer.push_back(newCust);
		promisedCustomerId.push_back(newCust->id);
	}
	for(custIter = dynamicCustomerSet.begin(); custIter < dynamicCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		allCustomer.push_back(newCust);	
		dynamicCustomerId.push_back(newCust->id);
	}
	sort(allCustomer.begin(), allCustomer.end(), ascendSortForCustId);  // 按id进行递增排序
}

void clearPlan(vector<Car*> &plan) {  // 清空plan
	vector<Car*>::iterator carIter;
	for(carIter = plan.begin(); carIter < plan.end(); carIter++) {
		delete(*carIter);
	}
	plan.resize(0);
}

void Dispatcher::carFinishedTask(int carIndex){       // 收车 
	vector<Car*>::iterator carIter;
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		if((*carIter)->getCarIndex == carIndex) {
			Car *newCar = new Car(**carIter);
			currentPlan.erase(carIter);
			finishedPlan.push_back(newCar);
			break;
		}
	}
}

// wait, depature, arrived, serving
vector<Car*> captureFuturePlan(vector<Car*> currentPlan, int currentTime) {
	// 从currentPlan中抓取还没有走过的计划
	// 这里需要判断货车的状态，如果货车已出发且未到目的地，则futurePlan的出发点是一个虚拟的点
	// 如果车辆正在某个顾客处，则futurePlan的出发点是一个真实的点
	vector<Car*>::iterator carIter;
	Customer depot = currentPlan[0]->getRoute().getRearNode();
	vector<Car*> outputPlan;
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		Customer pastNode = (*carIter)->getRoute().currentPos();  // 从该点出发
		Customer nextNode = (*carIter)->getRoute().nextPos();  // 目的地是该点
		Customer* startNode = new Customer;
		startNode->id = 0;
		startNode->type = 'D';
		switch((*carIter)->getState()){
		case depature: {
			// 货车正在路上
			float nearestDepartTime = (*carIter)->getDepatureTime();
			float dist = sqrt(pow(pastNode.x - nextNode.x, 2) + pow(pastNode.y - nextNode.y, 2));
			startNode->x = (currentTime - nearestDepartTime) / dist * (nextNode.x - pastNode.x) + pastNode.x;
			startNode->y = (currentTime - nearestDepartTime) / dist * (nextNode.y - pastNode.y) + pastNode.y;
			startNode->arrivedTime = currentTime;
			startNode->serviceTime = 0;
			startNode->startTime = currentTime;
			break;
					   }
		case wait: {
			startNode->x = pastNode.x;
			startNode->y = pastNode.y;
			startNode->arrivedTime = currentTime;
			startNode->serviceTime = 0;
			startNode->startTime = currentTime;
			break;
				   }
		case arrived: {  // 这种情况应该是不会发生的
			startNode->x = nextNode.x;
			startNode->y = nextNode.y;
			startNode->arrivedTime = currentTime;
			startNode->serviceTime = nextNode.serviceTime;
			startNode->startTime = currentTime;
			break;
					  }
		case serving: {
			startNode->x = pastNode.x;
			startNode->y = pastNode.y;
			startNode->arrivedTime = currentTime;
			startNode->serviceTime = pastNode.serviceTime - (currentTime - pastNode.arrivedTime);  // 服务时间已经过去了一部分，注意顾客到达后应该确定arrivedTime
			startNode->startTime = currentTime;
			break;
					  }
		}
		float leftQuantity = (*carIter)->getRoute().getLeftQuantity();  // 货车剩余容量
		float capacity = (*carIter)->getRoute().getCapacity();
		int carIndex = (*carIter)->getCarIndex();
		Car* newCar = new Car(*startNode, depot, leftQuantity, carIndex, false);
		Route tempRoute = (*carIter)->getRoute().capture();        // 抓取current指针后的路径
		vector<Customer*> tempCust = tempRoute.getAllCustomer();   // 获得current指针后的所有顾客
		vector<Customer*>::iterator custIter;
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			newCar->getRoute().insertAtRear(**custIter);
		}
		outputPlan.push_back(newCar);
	}
	return outputPlan;
}

vector<EventElement> Dispatcher::handleNewTimeSlot(int slotIndex){ // 新时间段开始
	vector<Customer*> promiseCustomerSet;
	vector<Customer*> waitCustomerSet;
	vector<Customer*> dynamicCustomerSet;
	vector<int>::iterator custIdIter;
	vector<Car*>::iterator carIter;
	vector<Car*> updatedPlan;
	for(custIdIter = promisedCustomerId.begin(); custIdIter< promisedCustomerId.end(); custIdIter++) {
		promiseCustomerSet.push_back(allCustomer[*custIdIter - 1]);  // id从1开始编号
	}
	for(custIdIter = dynamicCustomerId.begin(); custIdIter < dynamicCustomerId.end(); custIdIter++) {
		dynamicCustomerSet.push_back(allCustomer[*custIdIter - 1]);
	}

	vector<EventElement> newEventList;
	if(slotIndex == 0) {  // 路径计划需要初始化
		Car* newCar = new Car(depot, depot, capacity, 0);
		vector<Car*> currentPlan;
		currentPlan.push_back(newCar);
		Simulator smu(samplingRate, timeSlotLen, slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, currentPlan);
		updatedPlan = smu.initialPlan();
		vector<Car*>::iterator carIter;
		for(carIter = updatedPlan.begin(); carIter < updatedPlan.end(); carIter++) {
			currentPlan.push_back(*carIter);
			Customer nextCustomer = (*carIter)->getRoute().nextPos();
			float time = sqrt(pow(depot.x - nextCustomer.x, 2) + pow(depot.y - nextCustomer.y, 2));
			EventElement newEvent(time, carArrived, (*carIter)->getCarIndex(), nextCustomer.id);
			newEventList.push_back(newEvent);
		}
	} else {
		for(custIdIter = waitCustomerId.begin(); custIdIter < waitCustomerId.end(); custIdIter++) {
			waitCustomerSet.push_back(allCustomer[*custIdIter - 1]);
		}
		vector<Car*> futurePlan = captureFuturePlan(currentPlan, slotIndex*timeSlotLen);
		Simulator smu(samplingRate, timeSlotLen, slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, futurePlan);
		// 这里传递的应该是一个“阉割版”的路径计划
		vector<Customer*> newServedCustomer;
		vector<Customer*> newAbandonedCustomer;
		vector<Customer*> delayCustomer;
		updatedPlan = smu.replan(newServedCustomer, newAbandonedCustomer, delayCustomer);

		// 更新promiseCustomer, rejectCustomer以及waitCustomer
		vector<Customer*>::iterator custIter;
		vector<int>::iterator intIter;
		vector<int> tempVec;
		for(custIter = newServedCustomer.begin(); custIter < newServedCustomer.end(); custIter++) {
			promisedCustomerId.push_back((*custIter)->id);
			tempVec.push_back((*custIter)->id);
		}
		for(custIter = newAbandonedCustomer.begin(); custIter < newAbandonedCustomer.end(); custIter++) {
			rejectCustomerId.push_back((*custIter)->id);
		}
		vector<int> tempVec2;
		sort(waitCustomerId.begin(), waitCustomerId.end());
		sort(tempVec.begin(), tempVec.end());
		set_difference(waitCustomerId.begin(), waitCustomerId.end(), tempVec.begin(), tempVec.end(), tempVec2.begin());
		waitCustomerId = tempVec2;

		// 更新路径计划，根据变更的下一站目的地产生newEventList
		int count = 0;
		for(carIter = updatedPlan.begin(); carIter < updatedPlan.end(); carIter++) {
			// 将得到的新计划安插到原计划的current指针后面
			Car* currentCar = currentPlan[count++];
			currentCar->getRoute().replaceRoute((*carIter)->getRoute());  // 替换currentCar的curren指针后路径
			switch(currentCar->getState()) {
			case wait: {
				// do nothing
				break;
					   }
			case depature:{
				Customer nextCustomer = currentCar->getRoute().nextPos(); // 下一个顾客的id
				Customer startNode = (*carIter)->getRoute().getHeadNode();  // 虚拟的出发点
				float time = slotIndex*timeSlotLen + sqrt(pow(startNode.x - nextCustomer.x, 2) + pow(startNode.y - nextCustomer.y, 2));
				int carIndex = currentCar->getCarIndex();
				EventElement newEvent(time, carArrived, carIndex, nextCustomer.id);
				newEventList.push_back(newEvent);
				break;
						  }
			case serving: {
				// do nothing
				break;
						  }
			case arrived: {
				// do nothing
				break;
						  }
			}
		}
	}
	return newEventList;
} 

EventElement Dispatcher::handleNewCustomer(int slotIndex, Customer& newCustomer){  // 处理新顾客到达
	vector<int>::iterator intIter;
	for(intIter = dynamicCustomerId.begin(); intIter < dynamicCustomerId.end(); intIter++) { // 从未知顾客中删除
		if(*intIter == newCustomer.id) {
			dynamicCustomerId.erase(intIter);
			break;
		}
	}
	float minInsertCost = MAX_FLOAT;
	pair<int, Customer> insertPos;   // 第一个int是货车编号（于currentPlan中的位置），第二个Customer是插入点前面的顾客
	vector<Car*>::iterator carIter;
	vector<Car*> futurePlan = captureFuturePlan(currentPlan, newCustomer.startTime);  // 只可能插入到current指针后面
	EventElement tempEvent(-1, carArrived, -1, -1);  // 一个无效的事件
	float time;
	for(carIter = futurePlan.begin(); carIter < futurePlan.end(); carIter++) {
		// 求newCustomer在每条route的最小插入代价
		Customer customer1, customer2;
		float minValue, secondValue;
		(*carIter)->getRoute().computeInsertCost(newCustomer, minValue, customer1, secondValue, customer2);
		if(minValue < minInsertCost) {
			insertPos = make_pair(carIter - currentPlan.begin(), customer1);
			if((*carIter)->getState() == depature && customer1.id == 0) {
				// 如果车子在路途中且新节点成为下一站目的地
				time = customer1.startTime + sqrt(pow(customer1.x - newCustomer.x, 2) + pow(customer1.y - newCustomer.y, 2));
				tempEvent.carIndex = (*carIter)->getCarIndex();
				tempEvent.time = time;
				tempEvent.eventType = carArrived;
				tempEvent.customerId = newCustomer.id;
			}
		}
	}
	EventElement newEvent(-1, carArrived, -1, -1);  // 一个无效的事件
	if(minInsertCost == MAX_FLOAT) {
		// 没有可行插入点
		if(newCustomer.tolerantTime < (slotIndex+1)*timeSlotLen) {  // 等不到replan，则reject
			rejectCustomerId.push_back(newCustomer.id);
		} else {  // 否则，进入等待的顾客序列
			waitCustomerId.push_back(newCustomer.id);  
		}
	} else {
		int selectedCarPos = insertPos.first;
		if(insertPos.second.id == 0 && currentPlan[selectedCarPos]->getState() == depature) { // 在虚拟节点后面插入
			Customer frontCustomer = currentPlan[selectedCarPos]->getRoute().currentPos();    // current指针对应节点
			currentPlan[selectedCarPos]->getRoute().insertAfter(frontCustomer, newCustomer);
			newEvent = tempEvent;  // 肯定是最后赋值的tempEvent
		} else {
			currentPlan[selectedCarPos]->getRoute().insertAfter(insertPos.second, newCustomer);
		}
	}
	return newEvent;
}

EventElement Dispatcher::handleCarArrived(float time, int carIndex){                 // 处理货车到达事件
	currentPlan[carIndex]->updateState(serving);    // 更改货车状态
	Customer servedCustomer = currentPlan[carIndex]->getRoute().currentPos();        // 被服务的顾客
	float finishedTime = time + servedCustomer.serviceTime;
	currentPlan[carIndex]->decreaseLeftQuantity(servedCustomer.quantity); // 更新货车的剩余容量
	EventElement newEvent(finishedTime, finishedService, carIndex, servedCustomer.id);
	return newEvent;
}

EventElement Dispatcher::handleFinishedService(float time, int carIndex){       // 处理货车完成服务事件
	bool mark = currentPlan[carIndex]->moveForward();
	EventElement newEvent(-1, carArrived, -1, -1);     // 无效事件
	if(mark == false) { 
		currentPlan[carIndex]->updateState(offwork);   // 收车
		newEvent.time = time;
		newEvent.carIndex = carIndex;
		newEvent.eventType = carOffWork;
		carFinishedTask(carIndex);
	} else {
		Customer currentCustomer = currentPlan[carIndex]->getRoute().currentPos();
		Customer nextCustomer = currentPlan[carIndex]->getRoute().nextPos();
		ServedCustomerId.push_back(currentCustomer.id);    // 加入已服务完顾客
		vector<int>::iterator intIter;
		for(intIter = promisedCustomerId.begin(); intIter < promisedCustomerId.end(); intIter++) {
			// 从收到'OK'promise的顾客中删除
			if(*intIter == currentCustomer.id) {
				promisedCustomerId.erase(intIter);
				break;
			}
		}
		currentPlan[carIndex]->updateDepatureTime(time);  // 更新出发时间
		currentPlan[carIndex]->updateState(depature);     // 出发
		time += sqrt(pow(currentCustomer.x - nextCustomer.x, 2) + pow(currentCustomer.y - nextCustomer.y, 2));
		newEvent.time = time;
		newEvent.carIndex = carIndex;
		newEvent.eventType = carDepature;
		newEvent.customerId = nextCustomer.id;
	}
	return newEvent;
}


