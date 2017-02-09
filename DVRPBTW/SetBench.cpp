#include "SetBench.h"
#include "PublicFunction.h"

SetBench::SetBench(vector<Customer*> originCustomerSet, int timeSlotLen, int timeSlotNum, float dynamicism):
	originCustomerSet(originCustomerSet), timeSlotLen(timeSlotLen), timeSlotNum(timeSlotNum), dynamicism(dynamicism){}// 构造函数

void SetBench::constructProbInfo(){ 
	// 设置各个节点的概率信息
	vector<int> BHsPos(0);                     // BHs的位置
	int i;
	vector<Customer*>::iterator iter = originCustomerSet.begin();
	for(iter; iter < originCustomerSet.end(); iter++) {
		vector<float> dist = randomVec(timeSlotNum);   // 在各个slot提出需求的概率
		for(i=0; i<timeSlotNum; i++) {
			(*iter)->timeProb[i] = dist[i];
		}
	}
}

void SetBench::construct(vector<Customer*> &staticCustomerSet, vector<Customer*> &dynamicCustomerSet){
	// 根据概率情况构造样本
	constructProbInfo();
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
		                                               // 这里默认originCustomerSet是按id升序排列
		vector<int>::iterator iter2 = find(dynamicPos.begin(), dynamicPos.end(), count); // 寻找count是否是dynamicPos中的元素
		if(iter2 != dynamicPos.end()) {   // 在dynamicPos集合中
			dynamicCustomerSet.push_back(*iter);
		} else {  
			staticCustomerSet.push_back(*iter);
		}
		int selectSlot = roulette((*iter)->timeProb, timeSlotNum);   // 利用轮盘算法采样得出顾客可能提出需求的时间段
		float t1 = selectSlot * timeSlotLen;  // 时间段的开始
		float t2 = (selectSlot+1) * timeSlotLen;      // 时间段的结束
		float tempt = random(t1, t2);
		float maxActiveTime = timeSlotNum * timeSlotLen;  // 货车可工作的最晚时间
		(*iter)->startTime =  min(tempt, maxActiveTime - 2 * (*iter)->serviceTime); // 至少宽限2倍的serviceTime
		float timeWindowLen = random(2 * (*iter)->serviceTime, maxActiveTime - (*iter)->startTime- (*iter)->serviceTime);  // 时间窗长度
		(*iter)->endTime = (*iter)->startTime + timeWindowLen;
		(*iter)->tolerantTime = (*iter)->startTime + random(0.3, 0.6) * timeWindowLen;   // 可容忍的最晚得到回复的时间，为0.3-0.6倍的时间窗长度+startTime
	}
}