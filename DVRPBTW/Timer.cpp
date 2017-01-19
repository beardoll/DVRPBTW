#include "Timer.h"
#include "Dispatcher.h"
#include<cassert>
#include<algorithm>

bool ascendSort(EventElement &a, EventElement &b){  // 递增排序
	return a.time < b.time;
}

EventElement& Timer::pop(){  // 弹出最近事件，顺带将其从eventList中删除掉
	assert(eventList.size() > 0);
	vector<EventElement>::iterator iter = eventList.begin(); 
	EventElement *newElement = new EventElement(*iter);
	eventList.erase(iter);
	return *newElement;
}

Timer::Timer(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, int timeSlotLen): staticCustomerSet(staticCustomerSet), 
	dynamicCustomerSet(dynamicCustomerSet), timeSlotLen(timeSlotLen){   // 构造函数，输入参数为所有顾客，以及各时间段开始值

	EventElement *newEvent;
	int timeSlotNum = (int)staticCustomerSet[0]->timeProb.size();   // 时间段数目
	int i;
	for(i=0; i<timeSlotNum; i++) {  // 增加“时间段到达”事件
		newEvent = new EventElement(i*timeSlotLen, newTimeSlot, -1, -1);
		eventList.push_back(*newEvent);
	}
	vector<Customer*>::iterator iter = dynamicCustomerSet.begin();
	for(iter; iter< dynamicCustomerSet.end(); iter++) {  // 增加“新顾客到达事件”
		newEvent = new EventElement((*iter)->startTime, newCustomer, -1, (*iter)->id);
		eventList.push_back(*newEvent);
	}
	sort(eventList.begin(), eventList.end(), ascendSort);
}

void Timer::addEventElement(EventElement &newEvent){  // 往事件表中增加事件){  // 往事件表中增加事件
	eventList.push_back(newEvent);
	sort(eventList.begin(), eventList.end(), ascendSort);
}

void Timer::deleteEventElement(int carIndex){     // 删除与carIndex相关的事件
	vector<EventElement>::iterator iter = eventList.begin();
	for(iter; iter<eventList.end(); ){
		if((*iter).carIndex == carIndex) {
			iter = eventList.erase(iter);
		}
		iter++;
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

void Timer::updateEventElement(EventElement &newEvent){  // 更新事件
	vector<EventElement>::iterator iter = eventList.begin();
	for(iter; iter<eventList.end(); iter++){
		if((*iter).carIndex == newEvent.carIndex) {
			// 判断更新事件的位置，并更新之
			// 注意对于每一辆车，在事件表中仅存放一个事件
			(*iter).time = newEvent.time;
			(*iter).eventType = newEvent.eventType;
		}
	}
}

// enum EventType{newCustomer, carArrived, finishedService, carDepature, newTimeSlot, carOffWork};
void Timer::run() {
	Dispatcher disp(staticCustomerSet, dynamicCustomerSet, depot, capacity, timeSlotLen);  // 调度中心初始化
	int slotIndex = 0;  // 第0个时间段
	while(eventList.size() != 0) {
		EventElement currentEvent = pop();  // 弹出最近事件
		switch(currentEvent.eventType) {
		case newCustomer: {   // 新顾客到达
			EventElement newEvent = disp.handleNewCustomer(slotIndex, searchCustomer(currentEvent.customerId, dynamicCustomerSet));
			if(newEvent.time != -1) {   // 时间为-1表示无效事件
				deleteEventElement(newEvent.carIndex);   // 删除该货车原来的事件
				addEventElement(newEvent);
			}
			break;
						  }
		case carArrived: {
			EventElement newEvent = disp.handleCarArrived(currentEvent.time, currentEvent.carIndex);
			addEventElement(newEvent);
			break;
						 }
		case finishedService: {
			EventElement newEvent = disp.handleFinishedService(currentEvent.time, currentEvent.carIndex);
			if(newEvent.time != -1) {  // 时间为-1表示无效事件
				addEventElement(newEvent);
			}
			break;
							  }
		case newTimeSlot: {
			slotIndex++;
			vector<EventElement> newEventList = disp.handleNewTimeSlot(slotIndex);
			vector<EventElement>::iterator eventIter;
			for(eventIter = newEventList.begin(); eventIter < newEventList.end(); eventIter++) {
				// 逐个加入事件列表中
				deleteEventElement((*eventIter).carIndex);
				addEventElement(*eventIter);
			}
			break;
						  }
		case carOffWork: {
			// do nothing now
			break;
						 }
		}
	}
}