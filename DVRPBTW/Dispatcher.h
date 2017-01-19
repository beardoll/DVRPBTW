#ifndef _DISPATCHER_H
#define _DISPATHCER_H

#include "EventElement.h"
#include "Car.h"
#include "ProbInfo.h"
#include<vector>

class Dispatcher{   // 调度中心类
public:
	Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, Customer depot, float capacity, int timeSlotLen, 
		int samplingRate = 30);  // 构造函数
	~Dispatcher(){}; // 析构函数
	EventElement handleNewCustomer(int slotIndex, Customer& newCustomer);  // 处理新顾客到达
	EventElement handleCarArrived(float time, int carIndex);            // 处理货车到达事件
	EventElement handleFinishedService(float time, int carIndex);       // 处理货车完成服务事件
	// EventElement handleDepature(float time, int carIndex);              // 处理货车出发事件
	vector<EventElement> handleNewTimeSlot(int slotIndex); // 新时间段开始 
	Car& searchCar(int carIndex);   // 根据货车编号搜索货车
	void carFinishedTask(int carIndex);       // 收车 
private:
	vector<Customer*> allCustomer;    // 所有的顾客
	vector<int> dynamicCustomerId;    // 动态到达的顾客的id
	vector<int> ServedCustomerId;     // 已经服务过的顾客id
	vector<int> promisedCustomerId;   // (未服务过的)已经得到'OK' promise的顾客id
	vector<int> waitCustomerId;       // (未服务过的且已知的)还在等待'OK' promise的顾客id
	vector<int> rejectCustomerId;     // (未服务过的且已知的)得到了'NO' promise的顾客id
	vector<Car*> currentPlan;         // 当前计划
	vector<Car*> finishedPlan;        // 已完成计划    
	Customer depot;
	float capacity;
	int timeSlotLen;   // 时间段的长度
	int samplingRate;    // 采样率
};

#endif