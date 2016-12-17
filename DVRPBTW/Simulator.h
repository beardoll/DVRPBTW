#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "Car.h"
#include "Customer.h"
#include "ProbInfo.h"
#include<vector>

class Simulator{  // 仿真器类
public:
	Simulator(int samplingRate, vector<Customer*> unservedCustomers, vector<Car*> cars, ProbInfo probinfo); // 构造函数
	~Simulator();  // 析构函数
	void updateUnservedCustomer(vector<Customer*> newUnservedCustomers); // 更新未服务的顾客  
	void updateSamplingRate(int newSamplingRate);     // 更新采样率
	void updateCars(vector<Car*>newCars);  // 更新车辆
	vector<Route*> run();   
	// 运行仿真器，在此处调用Algorithm类
private:
	int samplingRate;   // 采样率
	vector<Customer*> unservedCustomers;  // 未服务的顾客
	vector<Car*> Cars;   // 所有车辆
	ProbInfo probInfo;   // 顾客历史信息
};

#endif