#ifndef _SSLANS_H
#define _SSALNS_H
#include "Car.h"
#include "Matrix.h"
#include<vector>

class SSALNS{  // SSALNS算法
public:
	SSALNS(vector<Customer*> PR2, vector<Customer*> PR3, vector<Car*> originPlan, Customer depot, float capacity):
		PR2(PR2), PR3(PR3), originPlan(originPlan), depot(depot), capacity(capacity){
	};
	~SSALNS(){};
	void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float maxd, float maxt, float maxquantity);
	void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
	void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p);  
	void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd, vector<float> DTpara);
	void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd, vector<float> DTpara);
	void run(vector<Car*> &finalCarSet, float &finalCost);  // 运行算法，相当于算法的main()函数
private:
	vector<Customer*> PR2;     // 第二优先级顾客
	vector<Customer*> PR3;     // 第三优先级顾客
	vector<Car*> originPlan;   // 初始计划
	Customer depot;
	float capacity;
};

#endif