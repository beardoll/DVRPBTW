#include "Timer.h"
#include<cassert>
#include<algorithm>

bool ascendSort(EventElement &a, EventElement &b){  // 递增排序
	return a.time < b.time;
}

EventElement& Timer::pop(){
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

void Timer::deleteEventList(EventType eventType){     // 删除eventType类型的事件
	vector<EventElement>::iterator iter = eventList.begin();
	for(iter; iter<eventList.end(); ){
		if((*iter).eventType == eventType) {
			iter = eventList.erase(iter);
		}
		iter++;
	}
}

void Timer::updateEventElement(EventElement &newEvent){  // 更新事件
	vector<EventElement>::iterator iter = eventList.begin();
	for(iter; iter<eventList.end(); iter++){
		if((*iter).carIndex == newEvent.carIndex && (*iter).customerId == newEvent.customerId) {
			// 判断更新事件的位置，并更新之
			// 注意对于一个顾客节点，在事件表中仅存放一个事件
			(*iter).time = newEvent.time;
			(*iter).eventType = newEvent.eventType;
		}
	}
}