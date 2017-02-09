#include "LNS_rel.h"
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<numeric>
#include<sstream>
#include<limits>
#include<map>
#include<cmath>
#include<ctime>
#include<string>
#include<functional>
#include<cstddef>

void getAllCustomerInOrder(vector<Car*> originCarSet, vector<int> &customerNum, vector<Customer*> &allCustomerInOrder){
	// 获取Car集合中所有的顾客
	// customerNum: 各辆车服务的顾客数量
	// allCustomer: 所有的顾客节点
	int i=0;
	for(vector<Car*>::iterator it1 = originCarSet.begin(); it1 < originCarSet.end(); it1++){
		if(i==0){  // 如果customerNum中没有元素，则不需要累加
			customerNum.push_back((*it1)->getCustomerNum()); 
		}else{     // 后面的数需要累加
			customerNum.push_back((*it1)->getCustomerNum() + customerNum[i-1]);
		}
		i++;
		vector<Customer*> customerSet = (*it1)->getAllCustomer(); // 每辆货车所负责的顾客
		for(vector<Customer*>::iterator it2=customerSet.begin(); it2<customerSet.end(); it2++){
			allCustomerInOrder.push_back(*it2);   // 逐个顾客节点插入
			//???? 是否会复制customer?如果是则需要delete掉customerSet
		}
	}
}

void deleteCustomer(vector<int> removedIndexset, vector<int> customerNum, vector<Customer*> allCustomerInOrder, 
					vector<Car*> &originCarSet, vector<Customer*> &removedCustomer){
	// 现在将removedIndexset对应的customer给拿出来
	// 同时对所有car的路径进行更改（移除节点）
	int indexRemovedLen = removedIndexset.end() - removedIndexset.begin();
	for(int i=0; i<indexRemovedLen; i++){
		int carIndex;
		int currentIndex = removedIndexset[i];
		vector<int>::iterator iter;
		iter = upper_bound(customerNum.begin(), customerNum.end(), currentIndex);  // 第一个比currentIndex大的元素
		carIndex = iter - customerNum.begin();
		bool mark = originCarSet[carIndex]->deleteCustomer(*allCustomerInOrder[currentIndex]);
		if(mark == false) {
			cout << "what??@@" << endl;
		}
		Customer *temp = new Customer;
		*temp = *allCustomerInOrder[currentIndex];
		removedCustomer.push_back(temp);
	}
}

void computeReducedCost(vector<Car*> originCarSet, vector<int> indexsetInRoute, vector<int> removedIndexset, 
						vector<pair<float, int>> &reducedCost, float DTpara[]){
	// indexsetInRoute: 尚在路径中的节点编号
	// removedIndexSet: 已被移除的节点编号
	int i;
	int carNum = originCarSet.end() - originCarSet.begin();
	vector<float> reducedCostInRoute(0); // 尚在路径中的各个节点的移除代价
	for(i=0; i<carNum; i++){
		vector<float> tempReducedCost = originCarSet[i]->computeReducedCost(DTpara);
		reducedCostInRoute.insert(reducedCostInRoute.end(), tempReducedCost.begin(), tempReducedCost.end());
	}
	for(i=0; i<indexsetInRoute.end()-indexsetInRoute.begin(); i++){
		int index = indexsetInRoute[i];
		reducedCost[index].first = reducedCostInRoute[i];
		reducedCost[index].second = index;
	}
	for(i=0; i<removedIndexset.end() - removedIndexset.begin(); i++){
		int index = removedIndexset[i];
		reducedCost[index].first = MAX_FLOAT;  // 已经移除掉的节点，不作考虑
		reducedCost[index].second = index;	
	}
}

void generateMatrix(vector<int> &allIndex, vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, Matrix<float> &minInsertPerRoute, 
					Matrix<Customer> &minInsertPos, Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, float noiseAmount, bool noiseAdd,
					float DTpara[], bool regularization){
	// regularization: 为true表示调用ALNS算法，不需要施加惩罚项
	//                 为false表示调用SSALNS算法，需要施加惩罚项
	float DTH1, DTH2, DTL1, DTL2;
	float *DTIter = DTpara;
	DTH1 = *(DTIter++);
	DTH2 = *(DTIter++);
	DTL1 = *(DTIter++);
	DTL2 = *(DTIter++);
	int removedCustomerNum = removedCustomer.size();
	int carNum = removedCarSet.size();
	for(int i=0; i<carNum; i++){
		// removedCarSet[i]->getRoute().refreshArrivedTime();  // 先更新一下各条路径的arrivedTime
		for(int j=0; j<removedCustomerNum; j++){
			if(i==0){
				allIndex.push_back(j);
			}
			float minValue, secondValue;
			Customer customer1, customer2;
			float penaltyPara = 0;
			if(regularization = false) {
				if(removedCarSet[i]->judgeArtificial() == false){  // 如果不是虚拟车
					switch(removedCustomer[j]->priority) {  // 如果是虚拟车
					case 1:
						penaltyPara = -DTH1;
						break;
					case 2:
						penaltyPara = -DTL1;
						break;
					}
				} else {   // 如果是虚拟车，则需要进行惩罚
					switch(removedCustomer[j]->priority) {  // 如果是虚拟车
					case 1:
						penaltyPara = DTH2;
						break;
					case 2:
						penaltyPara = DTL2;
						break;
					}
				}
			}
			removedCarSet[i]->computeInsertCost(*removedCustomer[j], minValue, customer1, secondValue, customer2, noiseAmount, noiseAdd, penaltyPara, regularization);
			minInsertPerRoute.setValue(i, j, minValue);
			minInsertPos.setValue(i, j, customer1);
			secondInsertPerRoute.setValue(i, j, secondValue);
			secondInsertPos.setValue(i, j, customer2);
		}
	}
}

void updateMatrix(vector<int> restCustomerIndex, Matrix<float> &minInsertPerRoute, Matrix<Customer> &minInsertPos, 
				  Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, int selectedCarPos, vector<Car*> &removedCarSet,
				  vector<Customer*>removedCustomer, float noiseAmount, bool noiseAdd, float DTpara[], bool regularization){
	// regularization: 为true表示调用ALNS算法，不需要施加惩罚项
	// 为false表示调用SSALNS算法，需要施加惩罚项
	// 提取DTpara中的元素
	float DTH1, DTH2, DTL1, DTL2;
	float *DTIter = DTpara;
	DTH1 = *(DTIter++);
	DTH2 = *(DTIter++);
	DTL1 = *(DTIter++);
	DTL2 = *(DTIter++);

	// 更新四个矩阵
	// removedCarSet[selectedCarPos]->getRoute().refreshArrivedTime();
	for(int i=0; i<(int)restCustomerIndex.size();i++) {
		int index = restCustomerIndex[i];   // 顾客下标
		float minValue, secondValue;
		Customer customer1, customer2;
		float penaltyPara = 0;
		if(regularization = false) {
			if(removedCarSet[selectedCarPos]->judgeArtificial() == false) { // 如果不是虚构的车辆
				switch(removedCustomer[index]->priority) {  // 根据不同的顾客优先级，赋予不同的惩罚系数（当插入到artificial vehicle时）
				case 1:
					penaltyPara = -DTH1;
					break;
				case 2:
					penaltyPara = -DTL1;
					break;
				}		
			} else {   // 是虚构的车辆
				switch(removedCustomer[index]->priority) {  // 根据不同的顾客优先级，赋予不同的惩罚系数（当插入到artificial vehicle时）
				case 1:
					penaltyPara = DTH2;
					break;
				case 2:
					penaltyPara = DTL2;
					break;
				}		
			}
		}
		removedCarSet[selectedCarPos]->computeInsertCost(*removedCustomer[index], minValue, customer1, secondValue, customer2, noiseAmount, noiseAdd, penaltyPara, regularization);
		minInsertPerRoute.setValue(selectedCarPos, index, minValue);
		minInsertPos.setValue(selectedCarPos, index, customer1);
		secondInsertPerRoute.setValue(selectedCarPos, index, secondValue);
		secondInsertPos.setValue(selectedCarPos, index, customer2);
	}
}

void reallocateCarIndex(vector<Car*> &originCarSet){  // 重新为货车编号
	int count = 0;
	for(int i=0; i<(int)originCarSet.size(); i++){
		if(originCarSet[i]->judgeArtificial() == false) {  // 若为真实的车辆，则不需要重新编号
			count++;
		} else{  // 若为虚假的车辆，则重新编号
			originCarSet[i]->changeCarIndex(count);
			count++;
		}
	}
}

void removeNullRoute(vector<Car*> &originCarSet, bool mark){    
	// 清除空车辆
	// 若mark=true, 则只允许清除虚拟的空车
	vector<Car*>::iterator iter;
	int count = 0;
	for(iter=originCarSet.begin(); iter<originCarSet.end();){
		if ((*iter)->getRoute().getSize() == 0){
			if(mark == true) {
				if ((*iter)->judgeArtificial() == true) { // 如果是空车而且是虚拟的车
					iter = originCarSet.erase(iter);
				} else{
					(*iter)->changeCarIndex(count++);
					++iter;				
				}
			}
			else {
				iter = originCarSet.erase(iter);
			}
		} else {
			(*iter)->changeCarIndex(count++);
			++iter;
		}
	}
}

size_t codeForSolution(vector<Car*> originCarSet){  // 对每个解（多条路径）进行hash编码
	vector<int> customerNum;
	vector<Customer*> allCustomerInOrder;
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	stringstream ss;
	string allStr = "";
	for(int i=0; i<(int)allCustomerInOrder.size(); i++){
		ss.str("");  // 每次使用之前先清空ss
		int a = allCustomerInOrder[i]->id;
		ss << a;
		allStr += ss.str();
	}
	hash<string> str_hash;
	return str_hash(allStr);
}

void computeMax(vector<Customer*> allCustomer, float &maxd, float &mind, float &maxquantity){
	// 计算所有顾客之间的最大距离以及顾客的最大货物需求量
	int customerAmount = (int)allCustomer.size();
	Matrix<float> D1(customerAmount, customerAmount);
	Matrix<float> D2(customerAmount, customerAmount);
	float tempmax = -MAX_FLOAT;
	for(int i=0; i<customerAmount; i++){
		if(allCustomer[i]->quantity > tempmax){
			tempmax = allCustomer[i]->quantity;
		}
		D1.setValue(i,i, 0.0f);
		D2.setValue(i,i, MAX_FLOAT);
		for(int j=i+1; j<customerAmount; j++){
			float temp = sqrt(pow(allCustomer[i]->x - allCustomer[j]->x, 2) + pow(allCustomer[i]->y - allCustomer[j]->y, 2));
			D1.setValue(i, j, temp);
			D2.setValue(i, j, temp);
			D1.setValue(j, i, temp);
			D2.setValue(j, i, temp);
		}
	}
	int t1, t2;
	maxd = D1.getMaxValue(t1, t2);
	mind = D2.getMinValue(t1, t2);
	maxquantity = tempmax;
}

float getCost(vector<Car*> originCarSet, float DTpara[]){
	// 返回originCarSet的路长
	float totalCost = 0;
	for(int i=0; i<(int)originCarSet.size(); i++){

		float temp;
		if(originCarSet[i]->judgeArtificial() == true) {
			temp = originCarSet[i]->getRoute().getLen(DTpara, true);
		} else {
			temp = originCarSet[i]->getRoute().getLen(DTpara, false);
		}
		totalCost += temp;
	}
	return totalCost;
}

int getCustomerNum(vector<Car*> originCarSet){
	// 获得路径集中顾客节点的数目
	int customerNum = 0;
	for(int i=0; i<(int)originCarSet.size(); i++){
		customerNum += originCarSet[i]->getRoute().getSize();
	}
	return customerNum;
}

bool carSetEqual(vector<Car*> carSet1, vector<Car*> carSet2){
	// 判断carSet1和carSet2是否相等
	if(carSet1.size() != carSet2.size()) {return false;}
	bool mark = true;
	for(int i=0; i<(int)carSet1.size(); i++){
		vector<Customer*> cust1 = carSet1[i]->getRoute().getAllCustomer();
		vector<Customer*> cust2 = carSet2[i]->getRoute().getAllCustomer();
		if(cust1.size() != cust2.size()) {mark = false; break;}
		for(int j=0; j<(int)cust1.size(); j++) {
			if(cust1[j]->id != cust2[j]->id) {mark = false; break;}
		}
	}
	return mark;
}

bool customerSetEqual(vector<Customer*>c1, vector<Customer*>c2){
	if(c1.size() != c2.size()) {return false;}
	bool mark = true;
	for(int i=0; i<(int)c1.size(); i++) {
		if(c1[i]->id != c2[i]->id) {mark = false; break;}
	}
	return mark;

}