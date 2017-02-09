#include "Simulator.h"
#include "ALNS.h"
#include "Matrix.h"
#include "PublicFunction.h"
#include "SSALNS.h"
#include<algorithm>
#include "TxtRecorder.h"

Simulator::Simulator(int samplingRate, int timeSlotLen, int timeSlotNum, int slotIndex, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
					 vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan):samplingRate(samplingRate), timeSlotLen(timeSlotLen), 
					 timeSlotNum(timeSlotNum), slotIndex(slotIndex){ // 构造函数
	vector<Customer*>::iterator custIter;
	this->promiseCustomerSet.reserve(promiseCustomerSet.end() - promiseCustomerSet.begin());
	this->promiseCustomerSet = copyCustomerSet(promiseCustomerSet);
	
	this->waitCustomerSet.reserve(waitCustomerSet.end() - waitCustomerSet.begin());
	this->waitCustomerSet = copyCustomerSet(waitCustomerSet);

	this->dynamicCustomerSet.reserve(dynamicCustomerSet.end() - dynamicCustomerSet.begin());
	this->dynamicCustomerSet = copyCustomerSet(dynamicCustomerSet);
	
	this->currentPlan.reserve(currentPlan.size());
	this->currentPlan = copyPlan(currentPlan);
}

Simulator::~Simulator(){  // 析构函数
	//clearCarSet();
	//clearCustomerSet();
}

void clearPlanSet(vector<vector<Car*>> planSet) {
	// 清除planSet
	vector<vector<Car*>>::iterator iter;
	for(iter = planSet.begin(); iter < planSet.end(); iter++) {
		withdrawPlan((*iter));
	}
}

vector<Customer*> Simulator::generateScenario(){
	// 产生情景
	// 根据动态顾客的随机信息产生其时间窗
	// 注意动态顾客只可能出现在slotIndex之后
	vector<Customer*> tempCustomer = copyCustomerSet(dynamicCustomerSet);
	vector<Customer*>::iterator iter = tempCustomer.begin();
	for(iter; iter<tempCustomer.end(); iter++){
		float randFloat = random(0,1);  // 产生随机数选择顾客可能提出需求的时间
		float sumation = 0;
		int count = roulette((*iter)->timeProb + slotIndex, timeSlotNum - slotIndex);  // 时间段计数
		float t1 = (count+slotIndex) * timeSlotLen;
		float t2 = (count+slotIndex+1) * timeSlotLen;
		float tempt = random(t1, t2);
		float maxTime = timeSlotNum * timeSlotLen;  // 最大允许时间
		(*iter)->startTime = min(tempt, maxTime - 2*(*iter)->serviceTime);  
		float timeWindowLen = random(2 * (*iter)->serviceTime, maxTime - (*iter)->startTime - (*iter)->serviceTime);  // 时间窗长度
		(*iter)->endTime = (*iter)->startTime + timeWindowLen;
	}
	return tempCustomer;
}

vector<int> getID(vector<Customer*> customerSet){ // 得到customerSet的所有Id
	vector<int> ids(0);
	ids.reserve(customerSet.end() - customerSet.begin());
	vector<Customer*>::iterator iter = customerSet.begin();
	for(iter; iter<customerSet.end(); iter++){
		ids.push_back((*iter)->id);
	}
	return ids;
}

vector<Car*> Simulator::initialPlan(Customer depot, float capacity){     // 利用采样制定初始计划
	ostringstream ostr;
	vector<int>::iterator intIter;
	vector<Car*>::iterator carIter;
	int i,j;
	sort(promiseCustomerSet.begin(), promiseCustomerSet.end());  // 顾客集按照id大小进行排序
	vector<int> validId;
	validId.push_back(0);   // 第一个节点是仓库节点
	vector<int> tempId = getID(promiseCustomerSet);    // 所有在计划开始前已知的顾客id(属于必须服务的顾客)
	for(intIter = tempId.begin(); intIter < tempId.end(); intIter++) {
		validId.push_back(*intIter);
	}
	// 初始化transformMatrix
	Matrix<int> transformMatrix(validId.size(), validId.size());
	for(i=0; i<validId.size(); i++) {
		for(j=0; j<validId.size(); j++) {
			transformMatrix.setValue(i,j,0);
		}
	}
	vector<vector<Car*>> planSet(samplingRate);   // 所有采样得到的计划
	vector<vector<Car*>>::iterator planIter = planSet.begin();
	// 对所有的情景运行ALNS算法，并且把解放入planSet中
	// 在此过程中将根据validId对所有的解，仅保留id在validId中的顾客节点
	vector<Customer*>::iterator iter;
	ostr.str("");
	ostr << "----Sampling begins!" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	for(planIter; planIter<planSet.end(); planIter++) {
		vector<Customer*> allCustomer = copyCustomerSet(promiseCustomerSet);   // 本次执行ALNS算法的所有顾客节点
		vector<Customer*> currentDynamicCust = generateScenario();  // 采样
		iter = currentDynamicCust.begin();
		for(iter; iter<currentDynamicCust.end(); iter++){
			allCustomer.push_back(*iter);
		}
		int num = planIter - planSet.begin();
		ostr.str("");
		ostr << "Now is the " << num+1 << "th simulation" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		ALNS alg(allCustomer, depot, capacity);
		vector<Car*> solution(0);
		float cost = 0;
		alg.run(solution, cost);
		for(carIter = solution.begin(); carIter < solution.end(); carIter++) {
			(*carIter)->removeInvalidCustomer(validId);
			(*carIter)->updateTranformMatrix(transformMatrix);
		}
		*planIter = solution;
		deleteCustomerSet(allCustomer);   
	}

	// 然后对所有情景下的计划进行评分，取得分最高者作为初始路径计划
	ostr.str("");
	ostr << "----Now assessing the performance of each scenario" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	vector<pair<int, int>> scoreForPlan(samplingRate);    // 每个计划的得分
	for(planIter = planSet.begin(); planIter < planSet.end(); planIter++) {
		int pos = planIter - planSet.begin();             // 在采样得到的计划中的位置
		int score = 0;
		for(carIter = planIter->begin(); carIter < planIter->end(); carIter++) {
			score += (*carIter)->computeScore(transformMatrix);
		}
		scoreForPlan.push_back(make_pair(score, pos));
	}
	sort(scoreForPlan.begin(), scoreForPlan.end(), descendSort<int, int>);
	vector<Car*> outputPlan = copyPlan(planSet[scoreForPlan[0].second]);
	clearPlanSet(planSet);
	deleteCustomerSet(waitCustomerSet);
	deleteCustomerSet(promiseCustomerSet);
	withdrawPlan(currentPlan);
	return outputPlan;
}



bool Simulator::checkFeasible(vector<Car*> carSet){
	// 判断promiseCustomerSet中的顾客是否都在carSet中
	vector<int> tempId = getID(promiseCustomerSet);  // 得到了promise的顾客id
	sort(tempId.begin(), tempId.end());
	vector<Car*>::iterator carIter;
	for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
		vector<Customer*> tempCust = (*carIter)->getRoute().getAllCustomer();
		vector<Customer*>::iterator custIter;
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			vector<int>::iterator intIter = find(tempId.begin(), tempId.end(), (*custIter)->id);
			if(intIter < tempId.end()) {
				// 如果找到了，就删掉
				tempId.erase(intIter);
			}
		}
	}
	if(tempId.size() != 0) {
		return false;
	} else {
		return true;
	}
}

void validPromise(vector<Car*>Plan, vector<Customer*> hurryCustomer, vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId){
	// 对hurry customer确认promise
	vector<Car*>::iterator carIter;
	vector<Customer*>::iterator custIter;
	vector<int> hurryCustomerId = getID(hurryCustomer);   // hurry customer的id
	int i;
	for(carIter = Plan.begin(); carIter < Plan.end(); carIter++){
		vector<Customer*> tempCust = (*carIter)->getAllCustomer();
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			int tempId = (*custIter)->id;
			vector<int>::iterator tempIter = find(hurryCustomerId.begin(), hurryCustomerId.end(), tempId);
			if(tempIter < hurryCustomerId.end()) {
				// 如果tempId在hurryCustomerId中
				newServedCustomerId.push_back(tempId);
				hurryCustomerId.erase(tempIter);  // 删除掉被服务的顾客剩下未被服务的顾客
			}
		}
	}
	sort(hurryCustomerId.begin(), hurryCustomerId.end());
	// 得到放弃的顾客id
	vector<int>::iterator intIter;
	for(intIter = hurryCustomerId.begin(); intIter < hurryCustomerId.end(); intIter++) {
		newAbandonedCustomerId.push_back(*intIter);
	}
}

vector<Car*> Simulator::replan(vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId, vector<int> &delayCustomerId, float capacity) {
	// 重新计划，用于vehicle出发以后
	// 首先筛选出一些着急等待回复的waitCustomer以及一些不着急回复的
	// newServedCustomer:  (wait customer中)通过re-plan接受到服务的顾客
	// newAbandonedCustomer: (wait customer中)通过re-plan确认无法接受服务的顾客
	// delayCustomer: 对于patient customer，如果当前不能确认服务，则可在未来再为其安排
	ostringstream ostr;
	vector<Customer*> hurryCustomer;
	vector<Customer*> patientCustomer;
	vector<Customer*>::iterator custIter;
	float nextMoment = (slotIndex+1) * timeSlotLen; // 下一个时间段的终止时间（下下个时间段的开始时间）
	for(custIter = waitCustomerSet.begin(); custIter < waitCustomerSet.end(); custIter) {
		if((*custIter)->tolerantTime <= nextMoment) {  // 该顾客着急于下时间段前得到回复
			Customer *tempCust = new Customer(**custIter);
			hurryCustomer.push_back(tempCust);     // 加入hurryCustomer中
		} else {
			Customer *tempCust = new Customer(**custIter);
			patientCustomer.push_back(tempCust);
		}
	}
	// 然后将这些顾客尽可能地放入当前计划中
	// 若不能，则返回'No' promise
	vector<Car*> newPlan;
	vector<Car*>::iterator carIter;
	float finalCost = 0;
	if(hurryCustomer.size() == 0) {  
		// 如果没有hurryCustomer，那么不需要对其进行replan
		ostr.str("");
		ostr << "There are no hurry Customer" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		if(patientCustomer.size() != 0) {
			ostr.str("");
			ostr << "Replan for patient customer..." << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			// 如果patientCustomer为空，则不需要对其进行replan
			SSALNS SSANLS_alg2(patientCustomer, currentPlan, capacity);
			SSANLS_alg2.run(newPlan, finalCost);
			validPromise(newPlan, patientCustomer, newServedCustomerId, delayCustomerId);
		} else {
			// 如果两个customerSet都为空，则直接复制currentPlan至newPlan
			ostr.str("");
			ostr << "There are no patient customer" << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			newPlan = copyPlan(currentPlan);
		}
	} else {
		// 对hurryCustomer进行replan
		ostr.str("");
		ostr << "Replan for hurry customer..." << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		SSALNS SSALNS_alg1(hurryCustomer, currentPlan, capacity);
		SSALNS_alg1.run(newPlan, finalCost);
		validPromise(newPlan, hurryCustomer, newServedCustomerId, newAbandonedCustomerId);
		if(patientCustomer.size() != 0) {
			ostr.str("");
			ostr << "Replan for patient customer..." << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			// 如果patientCustomer不为空，则应该对其进行replan
			SSALNS SSANLS_alg2(patientCustomer, newPlan, capacity);
			SSANLS_alg2.run(newPlan, finalCost);
			validPromise(newPlan, patientCustomer, newServedCustomerId, delayCustomerId);		
		} else {
			ostr.str("");
			ostr << "There are no patient customer" << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
		}
	}

	deleteCustomerSet(hurryCustomer);
	deleteCustomerSet(patientCustomer);
	
	// 首先得到"OK promise"的顾客的id，用于求解评分矩阵
	vector<int> allServedCustomerId;
	allServedCustomerId.push_back(0);   // 仓库节点是评分矩阵中的第一个节点
	vector<int>::iterator intIter;
	vector<int> promiseCustomerId = getID(promiseCustomerSet);
	for(intIter = promiseCustomerId.begin(); intIter < promiseCustomerId.end(); intIter++) {
		allServedCustomerId.push_back(*intIter);
	}
	for(intIter = newServedCustomerId.begin(); intIter < newServedCustomerId.end(); intIter++) {
		allServedCustomerId.push_back(*intIter);
	}
	sort(allServedCustomerId.begin(), allServedCustomerId.end());
	// 然后进行采样，调用SSALNS算法计算各个采样情景下的计划
	// 并且计算评分矩阵
	// 初始化transformMatrix
	Matrix<int> transformMatrix(allServedCustomerId.size(), allServedCustomerId.size());
	for(int i=0; i<allServedCustomerId.size(); i++) {
		for(int j=0; j<allServedCustomerId.size(); j++) {
			transformMatrix.setValue(i,j,0);
		}
	}
	vector<vector<Car*>> planSet(samplingRate);
	vector<vector<Car*>>::iterator planIter;
	ostr.str("");
	ostr << "----Sampling begins!" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	for(planIter = planSet.begin(); planIter < planSet.end(); planIter++) {
		vector<Car*> tempPlan;
		vector<Customer*>sampleCustomer = generateScenario(); // 产生动态到达顾客的情景
		int num = planIter - planSet.begin();
		ostr.str("");
		ostr << "Now is the " << num+1 << "th simulation" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		SSALNS SSALNS_alg3(sampleCustomer, newPlan, capacity);
		SSALNS_alg3.run(tempPlan, finalCost);
		for(carIter = tempPlan.begin(); carIter < tempPlan.end(); carIter++) {
			(*carIter)->removeInvalidCustomer(allServedCustomerId);
			(*carIter)->updateTranformMatrix(transformMatrix);
		}
		deleteCustomerSet(sampleCustomer);
	}

	// 取评分最高的计划作为输出
	ostr.str("");
	ostr << "----Now assessing the performance of each scenario" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	vector<pair<int, int>> scoreForPlan(samplingRate);    // 每个计划的得分
	for(planIter = planSet.begin(); planIter < planSet.end(); planIter++) {
		int pos = planIter - planSet.begin();             // 在采样得到的计划中的位置
		int score = 0;
		for(carIter = planIter->begin(); carIter < planIter->end(); carIter++) {
			score += (*carIter)->computeScore(transformMatrix);
		}
		scoreForPlan.push_back(make_pair(score, pos));
	}
	sort(scoreForPlan.begin(), scoreForPlan.end(), descendSort<int, int>);
	vector<Car*> outputPlan = copyPlan(planSet[scoreForPlan[0].second]);
	clearPlanSet(planSet);
	deleteCustomerSet(waitCustomerSet);
	deleteCustomerSet(promiseCustomerSet);
	withdrawPlan(currentPlan);
	return outputPlan;
}