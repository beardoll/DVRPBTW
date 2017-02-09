#ifndef _LNS_REL_H
#define _LNS_REL_H

#include "Car.h"
#include<vector>
#include "Matrix.h"

const float MAX_FLOAT = numeric_limits<float>::max();
const float LARGE_FLOAT = 10000.0f;

// ALNS算法用到的共有函数

void getAllCustomerInOrder(vector<Car*> originCarSet, vector<int> &customerNum, vector<Customer*> &allCustomerInOrder);
void deleteCustomer(vector<int> removedIndexset, vector<int> customerNum, vector<Customer*> allCustomerInOrder, 
				vector<Car*> &originCarSet, vector<Customer*> &removedCustomer);
void computeReducedCost(vector<Car*> originCarSet, vector<int> indexsetInRoute, vector<int> removedIndexset, 
					vector<pair<float, int>> &reducedCost, float DTpara[]);
void generateMatrix(vector<int> &allIndex, vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, Matrix<float> &minInsertPerRoute, 
				Matrix<Customer> &minInsertPos, Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, float noiseAmount, bool noiseAdd,
				float DTpara[], bool regularization = true);
void updateMatrix(vector<int> restCustomerIndex, Matrix<float> &minInsertPerRoute, Matrix<Customer> &minInsertPos, 
				Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, int selectedCarPos, vector<Car*> &removedCarSet,
				vector<Customer*>removedCustomer, float noiseAmount, bool noiseAdd, float DTpara[], bool regularization = true);
void reallocateCarIndex(vector<Car*> &originCarSet);
void removeNullRoute(vector<Car*> &originCarSet, bool mark = false);
size_t codeForSolution(vector<Car*> originCarSet);
void computeMax(vector<Customer*> allCustomer, float &maxd, float &mind, float &maxquantity);
float getCost(vector<Car*> originCarSet, float DTpara[]);
template<class T> inline void setZero(T* p, int size);
inline void setOne(float* p, int size);
inline void updateWeight(int *freq, float *weight, int *score, float r, int num);
inline void updateProb(float *removeProb, float *removeWeight, int removeNum);
int getCustomerNum(vector<Car*> originCarSet);
bool carSetEqual(vector<Car*> carSet1, vector<Car*> carSet2);
bool customerSetEqual(vector<Customer*>c1, vector<Customer*>c2);

// 模板函数和内联函数的实现
template<class T>
inline void setZero(T* p, int size){
	for(int i=0; i<size; i++){
		*(p++) = 0;
	}
}

inline void setOne(float* p, int size){   // 将所有数组元素赋值为1
	for(int i=0; i<size; i++){
		*(p++) = 1.0f;
	}
}

inline void updateWeight(int *freq, float *weight, int *score, float r, int num) {  
	// 更新权重
	for(int i=0; i<num; i++){
		if(*freq != 0){
			*weight = *weight *(1-r) + *score / *freq*r;
		} else {    // 如果在上一个segment中没有使用过该算子，权重应当下降
			*weight = *weight*(1-r);
		}
		freq++;
		weight++;
		score++;
	}
}

inline void updateProb(float *removeProb, float *removeWeight, int removeNum){
	// 更新概率
	float accRemoveWeight = 0;  // remove权重之和
	for(int k=0; k<removeNum; k++){
		accRemoveWeight += removeWeight[k];
	}
	for(int i=0; i<removeNum; i++){
		*removeProb = *removeWeight/accRemoveWeight;
		removeProb++;
		removeWeight++;
	}
}


#endif