#ifndef _DISPATCHER_H
#define _DISPATHCER_H

#include "EventList.h"
#include "Car.h"
#include "ProbInfo.h"
#include<vector>

class Dispatcher{   // 调度中心类
public:
	Dispatcher(vector<Customer*> initKnownCustomer, ProbInfo);  // 构造函数
	~Dispatcher(); // 析构函数
	EventList& handleNewCustomer(float time, Customer& newCustomer);  // 处理新顾客到达
	EventList& handleCarArrived(float time, int carIndex);   // 处理货车到达事件
	EventList& handleFinishedService(float time, int carIndex);  // 处理货车完成服务事件
	EventList& handleDepature(float time, int carIndex);  // 处理货车出发事件
	EventList& handleNewTimeSlot(float time, vector<Customer*> newKnownCustomers); // 新时间段开始 
	Car& searchCar(int carIndex);   // 根据货车编号搜索货车
	void addKnownCustomer(vector<Customer*> newKnownCustomers); 
	// 增加已知需求的顾客集，在此处调用Simulator类
	void setSamplingRate(int samplingRate);   // 设置采样率
private:
	vector<Customer*> knownCustomer;  // 当前需求已知的顾客
	ProbInfo probInfo;                // 所有顾客的历史信息 
	int samplingRate;    // 采样率
};

#endif