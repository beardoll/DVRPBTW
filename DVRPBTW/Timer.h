#ifndef _TIMER_H
#define _TIMER_H

#include<vector>
#include "EventList.h"

class Timer{  // 事件触发器
public:
	Timer(vector<Customer*>, float* timeDivision, int size=0);   // 构造函数，输入参数为所有顾客，以及各时间段开始值
	~Timer();  // 析构函数
	EventList& pop();   // 把事件表第一个元素弹出
	void addEventList(vector<EventList*> newEventList);  // 往事件表中增加事件
	void deleteEventList(vector<string*> eventType);     // 删除eventType类型的事件
	void run();   // 调度中心进行运作，需要初始化Dispatcher
private:
	vector<EventList*> eventList;  // 事件表
	int size;     // 事件表的长度
	
};

#endif