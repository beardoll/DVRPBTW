#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "Customer.h"
#include "Car.h"
#include<vector>

class Simulator{  // 仿真器类
public:
	Simulator(int samplingRate, int timeSlotLen, int timeSlotNum, int slotIndex, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
		vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan); // 构造函数
	~Simulator();  // 析构函数 
	vector<Car*> initialPlan(Customer depot, float capacity);     // 利用采样制定初始计划
	vector<Car*> replan(vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId, vector<int> &delayCustomerId, float capacity);
	vector<Customer*> generateScenario();  // 产生动态顾客的情景
	bool Simulator::checkFeasible(vector<Car*> carSet);
private:
	int samplingRate;   // 采样率
	int timeSlotLen;    // 时间段长度
	int timeSlotNum;    // timeSlot的个数
	int slotIndex;      // 当前是第几个slot
	vector<Customer*> promiseCustomerSet;     // 必须服务的顾客（即一定要在计划集中）
	vector<Customer*> waitCustomerSet;        // 可能要服务的顾客（可以不出现在计划集中，但是必须在tolerantTime之前给予回复是否能服务）   
	vector<Customer*> dynamicCustomerSet;     // 未知的顾客集
	vector<Car*> currentPlan;  
	// 当前计划
	// 在初始化的时候，并没有当前计划，这时currentPlan可以是一辆空车，我们在其中提取capacity, depot信息
};

#endif