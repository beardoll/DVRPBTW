#ifndef _PUBLICFUNCTION_H
#define _PUBLICFUNCTION_H

#include "Car.h"

using namespace std;

float random(float start, float end);
template<class T1, class T2> bool ascendSort(pair<T1, T2> x, pair<T1, T2> y) {
	// 递增排序
	// 第二个元素包含该元素在原序列中的位置
	return x.first < y.first;
}
template<class T1, class T2>  bool descendSort(pair<T1, T2> x, pair<T1, T2> y) {
	// 递减排序
	// 第二个元素包含该元素在原序列中的位置
	return x.first > y.first;
}
vector<float> randomVec(int num);
vector<int> getRandom(int lb, int ub, int m, vector<int> &restData);
	// 获得范围从lb到ub的m个不重复的数字
	// 剩余数字置于restData中
int roulette(vector<float> probability);
	// 根据probability，应用轮盘算法得到这次随机仿真出现的离散值
	// 内嵌将probability进行归一化的函数
int roulette(float *probability, int num);
	// 根据probability，应用轮盘算法得到这次随机仿真出现的离散值
	// 传入的是概率数组的头指针以及总共的概率分布数量
	// 内嵌将probability进行归一化的函数
inline void withdrawPlan(vector<Car*> &Plan);    // 销毁计划
inline vector<Car*> copyPlan(vector<Car*> Plan); // 复制计划
inline void deleteCustomerSet(vector<Customer*> &customerSet);            // 删除customerSet
inline vector<Customer*> copyCustomerSet(vector<Customer*> customerSet);  // 复制customerSet


// 模板函数和内联函数的实现
inline void withdrawPlan(vector<Car*> &Plan){  
	// 销毁计划
	vector<Car*>::iterator carIter;
	for(carIter = Plan.begin(); carIter < Plan.end(); carIter++) {
		delete(*carIter);
	}
	Plan.resize(0);
}

inline vector<Car*> copyPlan(vector<Car*> Plan) {
	// 复制计划
	vector<Car*>::iterator carIter;
	vector<Car*> outputPlan;
	for(carIter = Plan.begin(); carIter < Plan.end(); carIter++) {
		Car* newCar = new Car(**carIter);
		outputPlan.push_back(newCar);
	}
	return outputPlan;
}

inline void deleteCustomerSet(vector<Customer*> &customerSet){   // 删除CustomerSet
	vector<Customer*>::iterator iter;
	for(iter = customerSet.begin(); iter < customerSet.end(); iter++) {
		delete(*iter);
	}
}

inline vector<Customer*> copyCustomerSet(vector<Customer*> customerSet){  // 复制customerSet
	vector<Customer*> outputCust;
	vector<Customer*>::iterator custIter;
	for(custIter = customerSet.begin(); custIter < customerSet.end(); custIter++) {
		Customer *newCust = new Customer(**custIter);
		outputCust.push_back(newCust);
	}
	return outputCust;
}

#endif