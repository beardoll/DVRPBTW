#ifndef _EVENTLIST_H
#define _EVENTLIST_H

#include<string>
#include "Customer.h"
using namespace std;

struct EventList{
	float time;   // 事件发生的时间
	string eventType;  
	// 事件  
	// 1.新顾客到达
	// 2.货车到达目的地
	// 3.货车完成服务
	// 4.货车出发
	// 5.新时间段到达（清空2-4类的事件）
	int carIndex;  // 事件关联的车辆编号
	Customer customer; // 事件关联的顾客节点
}

#endif