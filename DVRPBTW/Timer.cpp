#include "Timer.h"
#include "Dispatcher.h"
#include<cassert>
#include<algorithm>

bool ascendSortEvent(EventElement &a, EventElement &b){  // 递增排序
	return a.time < b.time;
}

EventElement& Timer::pop(){  // 弹出最近事件，顺带将其从eventList中删除掉
	assert(eventList.size() > 0);
	vector<EventElement>::iterator iter = eventList.begin(); 
	EventElement *newElement = new EventElement(*iter);
	eventList.erase(iter);
	return *newElement;
}

Timer::Timer(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, int timeSlotLen, int timeSlotNum, float capacity, Customer depot): 
	staticCustomerSet(staticCustomerSet), dynamicCustomerSet(dynamicCustomerSet), timeSlotLen(timeSlotLen), timeSlotNum(timeSlotNum), capacity(capacity), depot(depot)
{   
	// 构造函数，输入参数为所有顾客，以及各时间段开始值
	EventElement *newEvent;
	int i;
	for(i=0; i<timeSlotNum; i++) {  
		// 增加“时间段到达”事件
		newEvent = new EventElement(i*timeSlotLen, newTimeSlot, -1, -1);
		eventList.push_back(*newEvent);
	}
	vector<Customer*>::iterator iter = dynamicCustomerSet.begin();
	for(iter; iter< dynamicCustomerSet.end(); iter++) {  
		// 增加“新顾客到达事件”
		newEvent = new EventElement((*iter)->startTime, newCustomer, -1, (*iter)->id);
		eventList.push_back(*newEvent);
	}
	sort(eventList.begin(), eventList.end(), ascendSortEvent);
}

void Timer::addEventElement(EventElement &newEvent){  
	// 往事件表中增加事件
	eventList.push_back(newEvent);
	sort(eventList.begin(), eventList.end(), ascendSortEvent);
}

void Timer::deleteEventElement(int carIndex){     
	// 删除与carIndex相关的事件
	vector<EventElement>::iterator iter = eventList.begin();
	for(iter; iter<eventList.end(); ){
		if((*iter).carIndex == carIndex) {
			iter = eventList.erase(iter);
			break;
		} else {
			iter++;
		}
	}
}

Customer searchCustomer(int customerId, vector<Customer*> customerSet) {
	// 根据customerId在customerSet中寻找顾客
	vector<Customer*>::iterator custIter;
	for(custIter = customerSet.begin(); custIter < customerSet.end(); custIter++) {
		if((*custIter)->id == customerId) {
			return **custIter;
			break;
		}
	}
}

void Timer::updateEventElement(EventElement &newEvent){  
	// 更新事件
	vector<EventElement>::iterator iter = eventList.begin();
	if(newEvent.time == -1) {
		// 传入无效事件，不更新
		return;	
	} else {
		bool found = false;
		for(iter; iter<eventList.end(); iter++){
			if((*iter).carIndex == newEvent.carIndex) {
				// 判断更新事件的位置，并更新之
				// 注意对于每一辆车，在事件表中仅存放一个事件
				found = true;
				(*iter).time = newEvent.time;
				(*iter).eventType = newEvent.eventType;
				(*iter).customerId = newEvent.customerId;
			}
		}
		if(found == false) {
			// 找不到是因为货车在此之前没有将时间表录入
			// 这种情况发生在货车在仓库等待了一段时间后才收到服务任务
			addEventElement(newEvent);
		}
	}
}

// enum EventType{newCustomer, carArrived, finishedService, carDepature, newTimeSlot, carOffWork};
void Timer::run() {
	Dispatcher disp(staticCustomerSet, dynamicCustomerSet, depot, capacity, timeSlotLen, timeSlotNum, 1);  // 调度中心初始化
	int slotIndex = 0;  // 第0个时间段
	while(eventList.size() != 0) {
		EventElement currentEvent = pop();  // 弹出最近事件
		switch(currentEvent.eventType) {
		case newCustomer: {   // 新顾客到达
			EventElement newEvent = disp.handleNewCustomer(slotIndex, searchCustomer(currentEvent.customerId, dynamicCustomerSet));
			updateEventElement(newEvent);
			break;
						  }
		case carArrived: {
			EventElement newEvent = disp.handleCarArrived(currentEvent.time, currentEvent.carIndex);
			updateEventElement(newEvent);
			break;
						 }
		case finishedService: {
			EventElement newEvent = disp.handleFinishedService(currentEvent.time, currentEvent.carIndex);
			updateEventElement(newEvent);
			break;
							  }
		case newTimeSlot: {
			vector<EventElement> newEventList = disp.handleNewTimeSlot(slotIndex);
			vector<EventElement>::iterator eventIter;
			for(eventIter = newEventList.begin(); eventIter < newEventList.end(); eventIter++) {
				// 逐个加入事件列表中
				updateEventElement(*eventIter);
			}
			slotIndex++;
			break;
						  }
		case carOffWork: {
			// do nothing now
			break;
						 }
		}
	}
}