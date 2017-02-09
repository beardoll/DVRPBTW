#ifndef _ALNS_H
#define _ALNS_H
#include "Car.h"
#include<vector>

class ALNS{  // 算法类
public:
	ALNS(vector<Customer*> allCustomer, Customer depot, float capacity);
	~ALNS(){};
	void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float maxd, float maxt, float maxquantity);
	void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
	void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p);  
	void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void run(vector<Car*> &finalCarSet, float &finalCost);  // 运行算法，相当于算法的main()函数
private:
	vector<Customer*> allCustomer;
	Customer depot;
	float capacity;
};

#endif