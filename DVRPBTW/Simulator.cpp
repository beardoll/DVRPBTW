#include "Simulator.h"
#include "ALNS.h"
#include "Matrix.h"
#include "PublicFunction.h"
#include "SSALNS.h"
#include<algorithm>

Simulator::Simulator(int samplingRate, int timeSlotLen, int slotIndex, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
					 vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan):samplingRate(samplingRate), timeSlotLen(timeSlotLen), slotIndex(slotIndex){ // 构造函数
	vector<Customer*>::iterator custIter;
	this->promiseCustomerSet.reserve(promiseCustomerSet.end() - promiseCustomerSet.begin());
	for(custIter = promiseCustomerSet.begin(); custIter < promiseCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		this->promiseCustomerSet.push_back(newCust);
	}
	this->waitCustomerSet.reserve(waitCustomerSet.end() - waitCustomerSet.begin());
	for(custIter = waitCustomerSet.begin(); custIter < waitCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		this->waitCustomerSet.push_back(newCust);
	}
	this->dynamicCustomerSet.reserve(dynamicCustomerSet.end() - dynamicCustomerSet.begin());
	for(custIter = dynamicCustomerSet.begin(); custIter < dynamicCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		this->dynamicCustomerSet.push_back(newCust);
	}
	vector<Car*>::iterator carIter;
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		Car* newCar = new Car(**carIter);
		this->currentPlan.push_back(newCar);
	}
}

Simulator::~Simulator(){  // 析构函数
	//clearCarSet();
	//clearCustomerSet();
}

void Simulator::clearCustomerSet(){    // 清空本地顾客集
	vector<Customer*>::iterator iter = promiseCustomerSet.begin();
	for(iter; iter < promiseCustomerSet.end(); iter++){  
		delete *iter;
	}
	iter = waitCustomerSet.begin();
	for(iter; iter < waitCustomerSet.end(); iter++){
		delete *iter;
	}
	iter = dynamicCustomerSet.begin();
	for(iter; iter < dynamicCustomerSet.end(); iter++){
		delete *iter;
	}
	promiseCustomerSet.resize(0);
	waitCustomerSet.resize(0);
	dynamicCustomerSet.resize(0);
}

void Simulator::clearCarSet(){  // 清空货车集合
	vector<Car*>::iterator iter = currentPlan.begin();
	for(iter; iter<currentPlan.end(); iter++) {
		delete *iter;
	}
	currentPlan.resize(0);
}

vector<Customer*> Simulator::generateScenario(){
	// 产生情景
	// 根据动态顾客的随机信息产生其时间窗
	vector<Customer*>::iterator iter = dynamicCustomerSet.begin();
	for(iter; iter<dynamicCustomerSet.end(); iter++){
		float randFloat = random(0,1);  // 产生随机数选择顾客可能提出需求的时间
		float sumation = 0;
		vector<float>::iterator iter2 = (*iter)->timeProb.begin();
		int count = roulette((*iter)->timeProb);  // 时间段计数
		float t1 = count * timeSlotLen;
		float t2 = (count+1) * timeSlotLen;
		float tempt = random(t1, t2);
		float maxTime = (int)(*iter)->timeProb.size() * timeSlotLen;  // 最大允许时间
		(*iter)->startTime = floor(min(tempt, maxTime - 2*(*iter)->tolerantTime));  
		float timeWindowLen = floor(random(2 * (*iter)->serviceTime, maxTime - (*iter)->startTime));  // 时间窗长度
		(*iter)->endTime = (*iter)->startTime + timeWindowLen;
	}
}

vector<int> getID(vector<Customer*> customerSet){ // 得到customerSet的所有Id
	vector<int> ids(0);
	ids.reserve(customerSet.end() - customerSet.begin());
	vector<Customer*>::iterator iter = customerSet.begin();
	for(iter; iter<customerSet.end(); iter++){
		ids.push_back((*iter)->id);
	}
}

Matrix<int> computeTransform(vector<vector<pair<int,int>>> ids, int customernum){
	// 所有的scenario中计算节点之间的转移频率
	// customernum: ids中顾客节点的数目
	Matrix<int> transformRate(customernum+1, customernum+1); 
	for(int i=0; i<customernum+1; i++){  // transfromRate中所有元素置零
		for(int j=0; j<customernum+1; j++){
			transformRate.setValue(i, j, 0);
		}
	}
	// 第一行/第一列对应id为0
	// 在Matrix中，id由小到大进行排序
	vector<vector<pair<int, int>>>::iterator iter = ids.begin();
	for(iter; iter<ids.end(); iter++){
		vector<pair<int, int>>::iterator iter2 = iter->begin();
		for(iter2; iter2<iter->end()-1;){
			int frontpos = iter2->second;   // 前一个节点在staticIds中的位置
			int backpos = (++iter2)->second; // 后一个节点在staticIds中的位置
			int originValue = transformRate.getElement(frontpos+1, backpos+1);  // 第一个元素是depot，要顺延1
			transformRate.setValue(frontpos+1, backpos+1, ++originValue);
		}
	}
}

vector<pair<int, int>> getIdSetInPlan(vector<Car*> planSet, vector<int> validId){
	// 将planSet中id于ValidId中的节点转化为IdSet（存储排列顺序信息）
	// 返回的向量的第一个元素，以0表示仓库节点，每辆车的路径以0隔开
	// 第二个元素，表示节点id于ValidId的位置，对于仓库节点0，其位置记为-1
	vector<pair<int, int>> output(0);
	vector<Car*>::iterator carIter = planSet.begin();
	for(carIter; carIter<planSet.end(); carIter++){
		vector<Customer*> customerThisRoute = (*carIter)->getRoute().getAllCustomer(); // 该车服务的所有顾客节点
		vector<Customer*>::iterator custIter = customerThisRoute.begin();
		output.push_back(make_pair(0,-1));  // 从仓库开始，位置为-1是因为顾客点于staticId位置在allPlan中从0开始
		for(custIter; custIter<customerThisRoute.end(); custIter++){
			int currentId = (*custIter)->id;
			vector<int>::iterator idIter = find(validId.begin(), validId.end(), currentId); // 搜索当前顾客的id是否在动态顾客集合中
			if(idIter != validId.end()){  // 必须要服务的顾客
				output.push_back(make_pair(currentId, (int)(idIter - validId.begin())));
			}
		}
		output.push_back(make_pair(0,-1));  // 从仓库结束
	}
	return output;
}

int computeScore(vector<pair<int, int>> idInPlan, Matrix<int> transformMatrix){
	// 计算idInPlan的得分
	vector<pair<int, int>>::iterator iter = idInPlan.begin();
	int score = 0;
	for(iter; iter<idInPlan.end()-1;){
		int frontpos = iter->second;
		int backpos = (++iter)->second;
		score += transformMatrix.getElement(frontpos, backpos);
	}
	return score;
}

vector<Car*> assessment(Matrix<int> transformMat, vector<vector<pair<int, int>>> allScenarioId, vector<Customer*> allServedCustomer, 
	vector<Car*> originPlan, bool mark){
	// 根据transfromMat对allScenarioId进行评价，选出最佳路径计划
	// allScenarioId与allServedCustomer对应
	// originPlan: 进行采样之前的路径计划，从中提取关于起点，终点以及载货量等信息
	// mark: false表示没有originPlan, true表示有originPlan
	vector<vector<pair<int, int>>>::iterator planIter = allScenarioId.begin();
	vector<int, int> scoreForPlan(0);  // 各个plan的得分，第二个元素是plan的位置，因为要排序
	int count = 0;
	for(planIter; planIter<allScenarioId.end(); planIter++){
		scoreForPlan.push_back(make_pair(computeScore(*planIter, transformMat), count++));
	}
	sort(scoreForPlan.begin(), scoreForPlan.end(), descendSort<int, int>);   // 降序排列
	int planIndex = scoreForPlan[0].second;   // 选中的plan的位置
	vector<pair<int, int>> idInPlan = allScenarioId[planIndex];
	vector<pair<int, int>>::iterator iter2 = idInPlan.begin()+1; // 第一个元素是仓库，跳过
	vector<Car*> outputPlan(0);   // 输出初始计划
	count = 0;  // 车辆编号计数

	// 将得分最高的计划（id顺序）所对应的customer一一对应到路径中作为输出
	for(iter2; iter2<idInPlan.end(); ){
		Car currentCar  = *originPlan[0];
		if(mark == true) {
			currentCar = *originPlan[count];
		}
		Customer headNode = currentCar.getRoute().getHeadNode();
		Customer rearNode = currentCar.getRoute().getRearNode();
		float capacity = currentCar.getRoute().getCapacity();
		float quantity = currentCar.getRoute().getQuantity();
		Car *tempCar = new Car(headNode, rearNode, capacity, count++, false, quantity);
		while((*iter2).first != 0){ // 以仓库为断点
			Customer item = *allServedCustomer[(*iter2).second];
			tempCar->getRoute().insertAtRear(item);
			iter2++;
		}
		outputPlan.push_back(tempCar);
		iter2++;
	}
	return outputPlan;
}

vector<Car*>& Simulator::initialPlan(){     // 利用采样制定初始计划
	sort(promiseCustomerSet.begin(), promiseCustomerSet.end());  // 顾客集按照id大小进行排序
	vector<int> staticId = getID(promiseCustomerSet);       // 所有在计划开始前已知的顾客id(属于必须服务的顾客)
	vector<vector<pair<int, int>>> allScenarioId(samplingRate);  // 所有采样得到的计划
	                                                        // 第一个数是id，第二个数是id在staticid中的位置
															// 路径之间以0隔开
	vector<vector<pair<int, int>>>::iterator planIter = allScenarioId.begin();
	Customer depot = currentPlan[0]->getRoute().getRearNode();
	float capacity = currentPlan[0]->getRoute().getCapacity();
	// 对所有的情景运行ALNS算法，并且把解对应的id顺序放在allIdSet中
	for(planIter; planIter<allScenarioId.end(); planIter++) {
		vector<Customer*> allCustomer(0);   // 本次执行ALNS算法的所有顾客节点
		vector<Customer*>::iterator iter = promiseCustomerSet.begin();
		for(iter; iter<promiseCustomerSet.end(); iter++){
			allCustomer.push_back(*iter);
		}
		vector<Customer*> currentDynamicCust = generateScenario();  // 采样
		iter = currentDynamicCust.begin();
		for(iter; iter<currentDynamicCust.end(); iter++){
			allCustomer.push_back(*iter);
		}
		ALNS alg(allCustomer, depot, capacity);
		vector<Car*> solution(0);
		float cost = 0;
		alg.run(solution, cost);
		*planIter = getIdSetInPlan(solution, staticId);  // 只保留promise customer的id
	}

	// 对所有节点之间的转移关系进行记录，得到评分矩阵
	// 然后对所有情景下的计划进行评分，取得分最高者作为初始路径计划
	Matrix<int> transformMat = computeTransform(allScenarioId, (int)staticId.size());  // 转移矩阵
	Car *tempCar = new Car(depot, depot, capacity, 0);
	vector<Car*> originPlan;
	originPlan.push_back(tempCar);
	vector<Car*> outputPlan = assessment(transformMat, allScenarioId, promiseCustomerSet, originPlan, false);
	return outputPlan;
}

void validPromise(vector<Car*>Plan, vector<Customer*> hurryCustomer, vector<Customer*> &newServedCustomer, vector<Customer*> &newAbandonedCustomer){
	// 对hurry customer确认promise
	vector<Car*>::iterator carIter;
	vector<Customer*>::iterator custIter;
	vector<int> hurryCustomerId = getID(hurryCustomer);   // hurry customer的id
	vector<bool> validMark;  // 元素为true表示确认服务，为false表示放弃服务
	int i;
	for(i=0; i<(int)hurryCustomer.size(); i++) {
		validMark.push_back(false);
	}
	for(carIter = Plan.begin(); carIter < Plan.end(); carIter++){
		vector<Customer*> tempCust = (*carIter)->getRoute().getAllCustomer();
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			int tempId = (*custIter)->id;
			vector<int>::iterator tempIter = find(hurryCustomerId.begin(), hurryCustomerId.end(), tempId);
			if(tempIter < hurryCustomerId.end()) {
				// 如果tempId不在hurryCustomerId中
				validMark[tempIter - hurryCustomerId.begin()] = true;
			}
		}
	}
	for(i=0; i<(int)hurryCustomer.size(); i++) {
		if(validMark[i] == true) {
			newServedCustomer.push_back(hurryCustomer[i]);
		} else {
			newAbandonedCustomer.push_back(hurryCustomer[i]);
		}	
	}
}

vector<Car*> Simulator::replan(vector<Customer*> newServedCustomer, vector<Customer*> newAbandonedCustomer, vector<Customer*> delayCustomer) {
	// 重新计划，用于vehicle出发以后
	// 首先筛选出一些着急等待回复的waitCustomer以及一些不着急回复的
	// newServedCustomer:  (wait customer中)通过re-plan接受到服务的顾客
	// newAbandonedCustomer: (wait customer中)通过re-plan确认无法接受服务的顾客
	// delayCustomer: 对于patient customer，如果当前不能确认服务，则可在未来再为其安排
	vector<Customer*> hurryCustomer;
	vector<Customer*> patientCustomer;
	vector<Customer*>::iterator custIter;
	float nextMoment = (slotIndex+1) * timeSlotLen; // 下一个时间段的终止时间（下下个时间段的开始时间）
	for(custIter = waitCustomerSet.begin(); custIter < waitCustomerSet.end(); ) {
		if((*custIter)->tolerantTime <= nextMoment) {  // 该顾客着急于下时间段前得到回复
			hurryCustomer.push_back(*custIter);    // 加入hurryCustomer中
			waitCustomerSet.erase(custIter);       // 将其从waitCustomerSet中删除
		} else {
			patientCustomer.push_back(*custIter);
			waitCustomerSet.erase(custIter);
		}
	}
	// 然后将这些顾客尽可能地放入当前计划中
	// 若不能，则返回'No' promise
	SSALNS SSALNS_alg1(hurryCustomer, currentPlan);
	vector<Car*> newPlan;
	float finalCost = 0;
	SSALNS_alg1.run(newPlan, finalCost);
	validPromise(newPlan, hurryCustomer, newServedCustomer, newAbandonedCustomer);
	SSALNS SSANLS_alg2(patientCustomer, newPlan);
	SSANLS_alg2.run(newPlan, finalCost);
	validPromise(newPlan, patientCustomer, newServedCustomer, delayCustomer);
	
	// 采样
	vector<Customer*> allServedCustomer;    // 所有接受服务的顾客
	for(custIter = promiseCustomerSet.begin(); custIter < promiseCustomerSet.end(); custIter++) {
		allServedCustomer.push_back(*custIter);
	}
	for(custIter = newServedCustomer.begin(); custIter < promiseCustomerSet.end(); custIter++) {
		allServedCustomer.push_back(*custIter);
	}
	sort(allServedCustomer.begin(), allServedCustomer.end());    // 对所有顾客按id升序排列
	vector<int> allServedCustomerId = getID(allServedCustomer);  // 所有顾客的id
	vector<vector<pair<int, int>>> allScenarioId(samplingRate);  // 所有采样得到的计划的id
	                                                             // 第一个int表示顾客的id，第二个int表示该id在allServedCustomerId中的位置
	                                                             // 每条路径以0开始，0结束，0表示仓库，其位置默认为-1
	vector<vector<pair<int, int>>>::iterator planIter;
	for(planIter = allScenarioId.begin(); planIter < allScenarioId.end(); planIter++) {
		vector<Car*> tempPlan;
		vector<Customer*>sampleCustomer = generateScenario(); // 产生动态到达顾客的情景
		SSALNS SSALNS_alg3(sampleCustomer, newPlan);
		SSALNS_alg3.run(tempPlan, finalCost);
		*planIter = getIdSetInPlan(tempPlan, allServedCustomerId);
	}

	// 评价
	Matrix<int> transfromMat = computeTransform(allScenarioId, (int)allServedCustomerId.size());  // 得到评价矩阵
	vector<Car*> outputPlan = assessment(transfromMat, allScenarioId, allServedCustomer, newPlan, true);
	return outputPlan;
}