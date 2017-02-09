#ifndef _EVENTELEMENT_H
#define _EVENTELEMENT_H

#include<string>
#include<iostream>
#include "Customer.h"
using namespace std;

enum EventType{newCustomer, carArrived, finishedService, carDepature, newTimeSlot, carOffWork};
// 事件  
// 1.新顾客到达
// 2.货车到达目的地
// 3.货车完成服务
// 4.货车出发
// 5.新时间段到达（清空2-4类的事件）

struct EventElement{
	float time;   // 事件发生的时间
	EventType eventType;  
	int carIndex;      // 事件关联的车辆编号
	int customerId;    // 事件关联的顾客节点id
	EventElement(const float time = -1, const EventType eventType = carArrived, const int carIndex = -1, const int customerId = -1){ // 构造函数
		this->time = time;
		this->eventType = eventType;
		this->carIndex = carIndex;
		this->customerId = customerId;
	}  
	EventElement(const EventElement &item){  // 复制构造函数
		this->time = item.time;
		this->eventType = item.eventType;
		this->carIndex = item.carIndex;
		this->customerId = item.customerId;
	}
	EventElement& operator= (const EventElement& item) {
		this->time = item.time;
		this->carIndex = item.carIndex;
		this->eventType = item.eventType;
		this->customerId = item.customerId;
		return *this;
	}
};

#endif