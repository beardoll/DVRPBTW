#include "SetBench.h"
#include "PublicFunction.h"
#include<ctime>
#include<cstdlib>
#include<algorithm>


SetBench::SetBench(vector<Customer*> originCustomerSet, int timeSlotLen, int timeSlotNum):
	originCustomerSet(originCustomerSet), timeSlotLen(timeSlotLen), timeSlotNum(timeSlotNum){}// 构造函数


vector<int> getRandom(int lb, int ub, int m, vector<int> &restData){
	// 产生m个不同的，范围为[lb, ub)的随机数
	// restData, 除了返回值外剩余的数值
	restData.resize(0);
	for(int i=0; i<ub-lb; i++) {
		restData.push_back(i+lb);
	}
	int total = m;
	vector<int> outputData(0);
	for(int j=0; j<m; j++) {
		vector<int>::iterator iter = restData.begin();
		int num = rand() % total; // 0-total-1
		iter += num;
		int temp = *(iter);
		outputData.push_back(num);
		restData.erase(iter);
		total--;
	}
	return outputData;
}

void SetBench::constructProbInfo(){ 
	// 设置各个节点的概率信息
	vector<int> BHsPos(0);                     // BHs的位置
	int i, j;
	vector<Customer*>::iterator iter = originCustomerSet.begin();
	for(iter; iter < originCustomerSet.end(); iter++) {
		vector<float> dist = randomVec(timeSlotNum);   // 在各个slot提出需求的概率
		(*iter)->timeProb = dist;
	}
}

void SetBench::construct(vector<Customer*> &staticCustomerSet, vector<Customer*> &dynamicCustomerSet){
	// 根据概率情况构造样本
	int customerAmount = originCustomerSet.end() - originCustomerSet.begin();
	int i;
	int dynamicNum = (int)floor(customerAmount*dynamicism);  // 动态到达的顾客数量
	vector<int> staticPos;           // 静态到达的顾客节点在originCustomerSet中的定位
	vector<int> dynamicPos = getRandom(0, customerAmount, dynamicNum, staticPos);   // 动态到达的BHs在BHs集合下的坐标
	vector<Customer*>::iterator iter = originCustomerSet.begin();
	staticCustomerSet.resize(0);
	dynamicCustomerSet.resize(0);
	for(iter; iter<originCustomerSet.end(); iter++) {
		int count = iter - originCustomerSet.begin();  // 当前顾客节点于originCustomerSet中的定位
		vector<int>::iterator iter2 = find(dynamicPos.begin(), dynamicPos.end(), count); // 寻找count是否是dynamicPos中的元素
		if(iter2 != dynamicPos.end()) {   // 在dynamicPos集合中
			dynamicCustomerSet.push_back(*iter);
		} else {  
			staticCustomerSet.push_back(*iter);
		}
		float randFloat = random(0, 1);
		float accumulation = 0;   // 轮盘算法
		int timeSlot = 0;         // 时间段
		vector<float>::iterator iter2 = (*iter)->timeProb.begin();
		i=1;  // 选中的时间段
		while(accumulation < randFloat) {
			accumulation += *(iter2++);	
			i++;
		}
		float t1 = (i-1) * timeSlotLen;  // 时间段的开始
		float t2 = i * timeSlotLen;      // 时间段的结束
		float tempt = random(t1, t2);
		float maxActiveTime = timeSlotNum * timeSlotNum;  // 货车可工作的最晚时间
		(*iter)->startTime =  min(tempt, maxActiveTime - 2 * (*iter)->serviceTime); // 至少宽限2倍的serviceTime
		float timeWindowLen = random(2 * (*iter)->serviceTime, maxActiveTime - (*iter)->startTime);  // 时间窗长度
		(*iter)->endTime = (*iter)->startTime + timeWindowLen;
		(*iter)->tolerantTime = (*iter)->startTime + random(0.3, 0.6) * timeWindowLen;   // 可容忍的最晚得到回复的时间，为0.3-0.6倍的时间窗长度+startTime
	}
}