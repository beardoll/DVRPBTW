#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "Car.h"
#include "Customer.h"
#include<vector>

class Simulator{  // 仿真器类
public:
	Simulator(int samplingRate, int timeSlotLen, Customer depot, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
		vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan); // 构造函数
	~Simulator();  // 析构函数 
	void clearCustomerSet();  // 清空Simulator中的顾客集
	void clearCarSet();       // 清空Simulator中的车辆集合
	vector<Car*>& initialPlan();     // 利用采样制定初始计划
	vector<Car*> run();   
	// 运行仿真器，在此处调用Algorithm类
private:
	int samplingRate;   // 采样率
	int timeSlotLen;    // 时间段长度
	Customer depot;     // 仓库节点
	vector<Customer*> promiseCustomerSet;     // 必须服务的顾客（即一定要在计划集中）
	vector<Customer*> waitCustomerSet;        // 可能要服务的顾客（可以不出现在计划集中，但是必须在tolerantTime之前给予回复是否能服务）   
	vector<Customer*> dynamicCustomerSet;     // 未知的顾客集
	vector<Car*> currentPlan;   // 当前计划
};

#endif