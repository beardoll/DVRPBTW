#ifndef _ALGORITHM_H
#define _ALGORITHM_H
#include "Car.h"
#include<vector>

class Algorithm{  // 算法类
public:
	Algorithm(vector<Customer*> allCustomer, Customer depot, float capacity):allCustomer(allCustomer), depot(depot), capacity(capacity){};
	~Algorithm(){};
	void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float maxd, float maxt, float maxquantity);
	void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
	void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p);  
	void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void reallocateCarIndex(vector<Car*> &originCarSet); // 重新编号车辆
	void removeNullRoute(vector<Car*> &originCarSet);    // 清除空车辆 
	void run(vector<Car*> &finalCarSet, float &finalCost);  // 运行算法，相当于算法的main()函数
private:
	vector<Customer*> allCustomer;
	Customer depot;
	float capacity;
};

#endif