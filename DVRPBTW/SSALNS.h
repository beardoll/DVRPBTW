#ifndef _SSLANS_H
#define _SSALNS_H
#include<vector>
#include "Car.h"

class SSALNS{  // SSALNS算法
public:
	SSALNS(vector<Customer*> waitCustomer, vector<Car*> originPlan, float capacity);
	~SSALNS();
	void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float maxd, float maxt, float maxquantity);
	void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
	void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float DTpara[]);  
	void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd, float DTpara[]);
	void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd, float DTpara[]);
	void run(vector<Car*> &finalCarSet, float &finalCost);  // 运行算法，相当于算法的main()函数
private:
	vector<Customer*> waitCustomer;     // 待插入的顾客
	vector<Car*> originPlan;   // 初始计划
	Customer depot;
	float capacity;
};

#endif