#ifndef _TIMER_H
#define _TIMER_H

#include<vector>
#include "EventElement.h"
#include "Customer.h"


class Timer{  // 事件触发器
public:
	Timer(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, int timeSlotLen, int timeSlotNum, float capacity, Customer depot);   // 构造函数，输入参数为所有顾客，以及各时间段开始值
	~Timer(){};  // 析构函数
	EventElement& pop();    // 把事件表的第一个元素弹出来
	void addEventElement(EventElement &newEvent);     // 往事件表中增加事件
	void updateEventElement(EventElement &newEvent);  // 更新事件
	void deleteEventElement(int carIndex);              // 删除eventType类型的事件
	void run();   // 调度中心进行运作，需要初始化Dispatcher
private:
	vector<EventElement> eventList;     // 事件队列
	vector<Customer*> staticCustomerSet;   // 提前已知的顾客（静态顾客）
	vector<Customer*> dynamicCustomerSet;  // 动态到达的顾客
	float capacity;
	Customer depot;
	int timeSlotLen;    // 时间段的长度
	int timeSlotNum;    // 时间段的个数
};

#endif