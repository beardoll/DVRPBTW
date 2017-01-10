#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "Car.h"
#include "Customer.h"
#include<vector>

class Simulator{  // 仿真器类
public:
	Simulator(int samplingRate, int timeSlotLen, Customer depot, float capacity); // 构造函数
	~Simulator();  // 析构函数
	void updateCustomerSet(vector<Customer*> mustServedCustomerSet, vector<Customer*> mayServedCustomerSet, vector<Customer*> dynamicCustomerSet);
	void clearCustomerSet();    // 清空本地顾客集
	void clearCarSet();         // 清空本地货车集合
	void updateCars(vector<Car*>newCars);         // 更新车辆
	vector<Customer*> generateScenario();         // 产生情景
	void updateSamplingRate(int newSamplingRate); // 更新采样率
	vector<Car*>& initialPlan();     // 利用采样制定初始计划
	vector<Car*> run();   
	// 运行仿真器，在此处调用Algorithm类
private:
	int samplingRate;   // 采样率
	int timeSlotLen;    // 时间段长度
	Customer depot;     // 仓库节点
	float capacity;     // 车容量
	vector<Customer*> mustServedCustomerSet;  // 必须服务的顾客（即一定要在计划集中）
	vector<Customer*> mayServedCustomerSet;   // 可能要服务的顾客（可以不出现在计划集中，但是必须在tolerantTime之前给予回复是否能服务）   
	vector<Customer*> dynamicCustomerSet;     // 未知的顾客集
	vector<Car*> carSet;   // 所有车辆
};

#endif