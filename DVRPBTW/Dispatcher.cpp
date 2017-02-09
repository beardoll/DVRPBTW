#include "Dispatcher.h"
#include<algorithm>
#include "Simulator.h"
#include "PublicFunction.h"
#include "TxtRecorder.h"

const float MAX_FLOAT = numeric_limits<float>::max();

bool ascendSortForCustId(Customer* item1, Customer* item2) {
	return item1->id < item2->id;
}

Dispatcher::Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, Customer depot, float capacity, int timeSlotLen, 
					   int timeSlotNum, int samplingRate): depot(depot), capacity(capacity), timeSlotLen(timeSlotLen), timeSlotNum(timeSlotNum), samplingRate(samplingRate){
	int custNum = staticCustomerSet.end() - staticCustomerSet.begin();
	custNum += dynamicCustomerSet.end() - dynamicCustomerSet.begin(); // 总顾客数
	ServedCustomerId.reserve(custNum);     // 已经服务过的顾客id
	promisedCustomerId.reserve(custNum);
	waitCustomerId.reserve(custNum);
	rejectCustomerId.reserve(custNum);
	vector<Customer*>::iterator custIter = staticCustomerSet.begin();
	for(custIter; custIter < staticCustomerSet.end(); custIter++) {
		// 在计划开始前已经提出需求的顾客都属于promiseCustomer
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
	sort(promisedCustomerId.begin(), promisedCustomerId.end());
	sort(dynamicCustomerId.begin(), dynamicCustomerId.end());
}


void Dispatcher::carFinishedTask(int carIndex){       // 收车 
	vector<Car*>::iterator carIter;
	ostringstream ostr;
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		if((*carIter)->getCarIndex() == carIndex) {
			Car *newCar = new Car(**carIter);
			currentPlan.erase(carIter);
			finishedPlan.push_back(newCar);
			break;
		}
	}
	if(currentPlan.size() == 0) {
		ostr.str("");
		ostr << "=============== END ================" << endl;
		ostr << "----OH!! Finished!" << endl;
		ostr << "----" << ServedCustomerId.size() << " customers get served finally" << endl;
		ostr << "----" << rejectCustomerId.size() << " customers get rejected finally" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
	} else {
		// 如果所有剩余的车辆都处于wait状态，则也算是完成了任务
		bool mark = true;
		int num = 0;
		for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
			if((*carIter)->getState() != wait) {
				mark = false;
				break;
			} else {
				num++;
			}
		}
		if(mark == true) {
			ostr.str("");
			ostr << "=============== END ================" << endl;
			ostr << "----There are " << num << " cars remains unused!" << endl;
			ostr << "----" << ServedCustomerId.size() << " customers get served finally" << endl;
			ostr << "----" << rejectCustomerId.size() << " customers get rejected finally" << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();	
		}
	}
}

bool Dispatcher::checkFeasible(vector<Car*> carSet){
	// 判断promiseCustomerSet中的顾客是否都在carSet中
	vector<int> tempId = promisedCustomerId;  // 得到了promise的顾客id
	sort(tempId.begin(), tempId.end());
	vector<Car*>::iterator carIter;
	for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
		vector<Customer*> tempCust = (*carIter)->getAllCustomer();
		vector<Customer*>::iterator custIter;
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			vector<int>::iterator intIter = find(tempId.begin(), tempId.end(), (*custIter)->id);
			if(intIter < tempId.end()) {
				// 如果找到了，就删掉
				tempId.erase(intIter);
			}
		}
	}
	if(tempId.size() != 0) {
		return false;
	} else {
		return true;
	}
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
	ostringstream ostr;
	if(slotIndex == 0) {  // 路径计划需要初始化
		ostr.str("");
		ostr << "============ Now Initialize the routing plan ===========" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		Simulator smu(samplingRate, timeSlotLen, timeSlotNum, slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, currentPlan);
		updatedPlan = smu.initialPlan(depot, capacity);
		currentPlan = copyPlan(updatedPlan);
		withdrawPlan(updatedPlan);
		vector<Car*>::iterator carIter;
		for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
			EventElement newEvent = (*carIter)->launchCar(0);  // 将车辆发动
			newEventList.push_back(newEvent);
		}
		ostr.str("");
		ostr << "----Initialization Finished! Now there are " << currentPlan.size() << " cars dispatched!" << endl << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
	} else {
		// 调取每辆车未走过的路径进行仿真，得到新的路径计划
		ostr.str("");
		ostr << "============ Now replan, the time slot is: " << slotIndex << "============" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		float currentTime = slotIndex * timeSlotLen;
		for(custIdIter = waitCustomerId.begin(); custIdIter < waitCustomerId.end(); custIdIter++) {
			waitCustomerSet.push_back(allCustomer[*custIdIter - 1]);
		}
		vector<Car*> futurePlan;
		for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
			// 抓取车子还没有走过的计划
			Car *tempCar = new Car((*carIter)->capturePartRoute(currentTime));
			futurePlan.push_back(tempCar);
		}
		Simulator smu(samplingRate, timeSlotLen, timeSlotNum, slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, futurePlan);
		vector<int> newServedCustomerId;
		vector<int> newAbandonedCustomerId;
		vector<int> delayCustomerId;
		updatedPlan = smu.replan(newServedCustomerId, newAbandonedCustomerId, delayCustomerId, capacity);
		vector<Customer*>::iterator custIter;

		// 更新promiseCustomerId, rejectCustomerId以及waitCustomerId
		vector<int>::iterator intIter;
		vector<int> tempVec;
		for(intIter = newServedCustomerId.begin(); intIter < newServedCustomerId.end(); intIter++) {
			promisedCustomerId.push_back(*intIter);
			tempVec.push_back(*intIter);
		}
		sort(promisedCustomerId.begin(), promisedCustomerId.end());
		for(intIter = newAbandonedCustomerId.begin(); intIter < newAbandonedCustomerId.end(); intIter++) {
			rejectCustomerId.push_back(*intIter);
			tempVec.push_back(*intIter);
		}

		sort(rejectCustomerId.begin(), rejectCustomerId.end());
		sort(waitCustomerId.begin(), waitCustomerId.end());
		sort(tempVec.begin(), tempVec.end());
		vector<int> tempVec2;
		set_difference(waitCustomerId.begin(), waitCustomerId.end(), tempVec.begin(), tempVec.end(), tempVec2.begin());
		waitCustomerId = tempVec2;

		// 将变更后的future plan安插到currentPlan对应位置之后
		int count = 0;
		for(carIter = updatedPlan.begin(); carIter < updatedPlan.end(); carIter++) {
			currentPlan[count]->replaceRoute(**carIter, currentTime);
			EventElement newEvent;
			if(currentPlan[count]->getState() == wait) {
				// 如果货车原来处于wait状态，则需要将其发动
				newEvent = currentPlan[count]->launchCar(currentTime);
			} else {
			    newEvent = currentPlan[count]->getCurrentAction(currentTime);
			}
			newEventList.push_back(newEvent);
			count++;
		}
	}
	ostr.str("");
	ostr <<  "----Replan Finished! Now there are " << currentPlan.size() << " cars working!" << endl << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	return newEventList;
} 

EventElement Dispatcher::handleNewCustomer(int slotIndex, Customer& newCustomer){  
	// 处理新顾客到达
	ostringstream ostr;
	ostr.str("");
	ostr<< "----Customer with id #" << newCustomer.id << " is arriving..." << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	vector<int>::iterator intIter = find(dynamicCustomerId.begin(), dynamicCustomerId.end(), newCustomer.id);
	dynamicCustomerId.erase(intIter);
	float minInsertCost = MAX_FLOAT;
	pair<int, Customer> insertPos;   // 第一个int是货车编号（于currentPlan中的位置），第二个Customer是插入点前面的顾客
	vector<Car*>::iterator carIter;
	float currentTime = newCustomer.startTime;       // 顾客提出需求的时间正好是时间窗开始的时间
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		// 求newCustomer在每条route的最小插入代价
		Car tempCar = (*carIter)->capturePartRoute(currentTime);
		Customer customer1, customer2;
		float minValue, secondValue;
		tempCar.computeInsertCost(newCustomer, minValue, customer1, secondValue, customer2);
		if(minValue < minInsertCost) {
			int pos = carIter - currentPlan.begin();  
			minInsertCost = minValue;
			insertPos = make_pair(pos, customer1);
		}
	}
	EventElement newEvent;
	if(minInsertCost == MAX_FLOAT) {
		// 没有可行插入点
		if(newCustomer.tolerantTime < (slotIndex+1)*timeSlotLen) { 
			// 等不到replan，则reject
			ostr.str("");
			ostr << "He is rejected!" << endl << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			rejectCustomerId.push_back(newCustomer.id);
		} else {  // 否则，进入等待的顾客序列
			ostr.str("");
			ostr << "He will wait for replan!" << endl << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			waitCustomerId.push_back(newCustomer.id);  
			sort(waitCustomerId.begin(), waitCustomerId.end());
		}
	} else {
		promisedCustomerId.push_back(newCustomer.id);  // 这些顾客一定会得到服务
		sort(promisedCustomerId.begin(), promisedCustomerId.end());
		int selectedCarPos = insertPos.first;
		Customer selectedCustomer = insertPos.second;
		currentPlan[selectedCarPos]->insertAfter(selectedCustomer, newCustomer);
		if(currentPlan[selectedCarPos]->getState() == wait) {
			newEvent = currentPlan[selectedCarPos]->launchCar(currentTime);
		} else {
			newEvent = currentPlan[selectedCarPos]->getCurrentAction(currentTime);
		}
		int carIndex = currentPlan[selectedCarPos]->getCarIndex();
		ostr.str("");
		ostr << "He is arranged to car #" << carIndex << endl << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
 	}
	return newEvent;
}

EventElement Dispatcher::handleCarArrived(float time, int carIndex){                 
	// 处理货车到达事件
	// 在此处更新顾客的到达时间
	ostringstream ostr;
	vector<Car*>::iterator carIter;
	int pos;  // carIndex对应的车辆在currentPlan中的位置
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		if((*carIter)->getCarIndex() == carIndex) {
			pos = carIter - currentPlan.begin();
			break;
		}
	}
	EventElement tempEvent = currentPlan[pos]->getCurrentAction(time);
	if(currentPlan[pos]->getState() == offwork) {
		// 收车
		ostr.str("");
		ostr << "----Time " << time << ", Car #" << currentPlan[pos]->getCarIndex() << " finished its task!" << endl << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		carFinishedTask(tempEvent.carIndex);
	} else {
		// 更新newServedCustomerId以及promisedCustomerId
		int currentId = currentPlan[pos]->getCurrentNode().id;
		vector<int>::iterator intIter = find(promisedCustomerId.begin(), promisedCustomerId.end(), currentId);
		promisedCustomerId.erase(intIter);
		ServedCustomerId.push_back(currentId);
		sort(ServedCustomerId.begin(), ServedCustomerId.end());
		ostr.str("");
		ostr << "----Time " << time << ", Car #" << currentPlan[pos]->getCarIndex() << " arrives at customer #" << currentId << endl << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
	}
	return tempEvent;
}

EventElement Dispatcher::handleFinishedService(float time, int carIndex){       // 处理货车完成服务事件
	ostringstream ostr;
	EventElement newEvent(-1, carArrived, -1, -1);     // 无效事件
	vector<Car*>::iterator carIter;
	int pos;  // carIndex对应的车辆在currentPlan中的位置
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		if((*carIter)->getCarIndex() == carIndex) {
			pos = carIter - currentPlan.begin();
			break;
		}
	}

	newEvent = currentPlan[pos]->getCurrentAction(time);
	int currentId = currentPlan[pos]->getCurrentNode().id;
	ostr.str("");
	ostr << "----Time " << time << ", car #" << carIndex << " finished service in customer #" << currentId << endl << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	return newEvent;
}


