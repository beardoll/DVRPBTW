#include "Dispatcher.h"

Dispatcher::Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, int samplingRate):
	dynamicCustomerSet(dynamicCustomerSet), samplingRate(samplingRate){
	// 构造函数
	// 在此处应该生成初始计划集
	mustServedCustomerSet.resize(staticCustomerSet.size());
	copy(staticCustomerSet.begin(), staticCustomerSet.end(), mustServedCustomerSet.begin());  // 初始已知的顾客都是must served类型
	mayServedCustomerSet.resize(0);    // 初始化may served类型的集合为空
	currentPlan.resize(0);
}

void Dispatcher::setSamplingRate(int samplingRate){   // 设置采样率
	this->samplingRate = samplingRate;
}


