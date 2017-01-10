#include "Simulator.h"
#include "Algorithm.h"
#include "Matrix.h"
#include "PublicFunction.h"
#include<algorithm>

Simulator::Simulator(int samplingRate, int timeSlotLen, Customer depot, float capacity):samplingRate(samplingRate), timeSlotLen(timeSlotLen),
	depot(depot), capacity(capacity){ // 构造函数
	mustServedCustomerSet.resize(0);
	mayServedCustomerSet.resize(0);
	dynamicCustomerSet.resize(0);
	carSet.resize(0);
}

Simulator::~Simulator(){  // 析构函数
	clearCarSet();
	clearCustomerSet();
}

void Simulator::updateCustomerSet(vector<Customer*> mustServedCustomerSet, vector<Customer*> mayServedCustomerSet, vector<Customer*> dynamicCustomerSet){
	clearCarSet();
	vector<Customer*>::iterator iter;
	Customer* newCustomer;
	for(iter=mustServedCustomerSet.begin(); iter<mustServedCustomerSet.end(); iter++){
		newCustomer = new Customer;
		*newCustomer = **iter;
		this->mustServedCustomerSet.push_back(newCustomer);
	}
	for(iter=mayServedCustomerSet.begin(); iter<mayServedCustomerSet.end(); iter++){
		newCustomer = new Customer;
		*newCustomer = **iter;
		this->mayServedCustomerSet.push_back(newCustomer);
	}
	for(iter=dynamicCustomerSet.begin(); iter<dynamicCustomerSet.end(); iter++){
		newCustomer = new Customer;
		*newCustomer = **iter;
		this->dynamicCustomerSet.push_back(newCustomer);
	}
}

void Simulator::clearCustomerSet(){    // 清空本地顾客集
	vector<Customer*>::iterator iter = mustServedCustomerSet.begin();
	for(iter; iter<mustServedCustomerSet.end(); iter++){  
		delete *iter;
	}
	iter = mayServedCustomerSet.begin();
	for(iter; iter<mayServedCustomerSet.end(); iter++){
		delete *iter;
	}
	iter = dynamicCustomerSet.begin();
	for(iter; iter<dynamicCustomerSet.end(); iter++){
		delete *iter;
	}
	mustServedCustomerSet.resize(0);
	mayServedCustomerSet.resize(0);
	dynamicCustomerSet.resize(0);
}

void Simulator::clearCarSet(){  // 清空货车集合
	vector<Car*>::iterator iter = carSet.begin();
	for(iter; iter<carSet.end(); iter++) {
		delete *iter;
	}
	carSet.resize(0);
}

vector<Customer*> Simulator::generateScenario(){
	// 产生情景
	// 根据动态顾客的随机信息产生其时间窗
	vector<Customer*>::iterator iter = dynamicCustomerSet.begin();
	for(iter; iter<dynamicCustomerSet.end(); iter++){
		float randFloat = random(0,1);  // 产生随机数选择顾客可能提出需求的时间
		float sumation = 0;
		vector<float>::iterator iter2 = (*iter)->timeProb.begin();
		int count = 1;  // 时间段计数
		while(sumation < randFloat) {
			sumation += *iter2;
			count++;
			iter2++;
		}
		float t1 = (count-1) * timeSlotLen;
		float t2 = count * timeSlotLen;
		float tempt = random(t1, t2);
		float maxTime = (int)(*iter)->timeProb.size() * timeSlotLen;  // 最大允许时间
		(*iter)->startTime = min(tempt, maxTime - 2*(*iter)->tolerantTime);
		float timeWindowLen = random(2 * (*iter)->serviceTime, maxTime - (*iter)->startTime);  // 时间窗长度
		(*iter)->endTime = (*iter)->startTime + timeWindowLen;
	}
}

vector<int> getID(vector<Customer*> customerSet){ // 得到customerSet的所有Id
	vector<int> ids(0);
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
			int originValue = transformRate.getElement(frontpos+1, backpos+1);
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
			if(idIter != validId.end()){  // Must served类型的顾客
				output.push_back(make_pair(currentId, (int)(idIter - validId.begin())));
			}
		}
	}
	output.push_back(make_pair(0,-1));  // 从仓库结束
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

vector<Car*>& Simulator::initialPlan(){     // 利用采样制定初始计划
	sort(mustServedCustomerSet.begin(), mustServedCustomerSet.end());  // 顾客集按照id大小进行排序
	vector<int> staticId = getID(mustServedCustomerSet);    // 所有在计划开始前已知的顾客id(属于必须服务的顾客)
	vector<vector<pair<int, int>>> allIdSet(samplingRate);  // 所有采样得到的计划
	                                                        // 第一个数是id，第二个数是id在staticid中的位置
	vector<vector<pair<int, int>>>::iterator planIter = allIdSet.begin();

	// 对所有的情景运行ALNS算法，并且把解对应的id顺序放在allIdSet中
	for(planIter; planIter<allIdSet.end(); planIter++) {
		vector<Customer*> allCustomer(0);   // 本次执行ALNS算法的所有顾客节点
		vector<Customer*>::iterator iter = mustServedCustomerSet.begin();
		for(iter; iter<mustServedCustomerSet.end(); iter++){
			allCustomer.push_back(*iter);
		}
		iter = dynamicCustomerSet.begin();
		for(iter; iter<dynamicCustomerSet.end(); iter++){
			allCustomer.push_back(*iter);
		}
		Algorithm alg(allCustomer, depot, capacity, 2000);
		vector<Car*> solution(0);
		float cost = 0;
		alg.run(solution, cost);
		*planIter = getIdSetInPlan(solution, staticId);
	}

	// 对所有节点之间的转移关系进行记录，得到评分矩阵
	// 然后对所有情景下的计划进行评分，取得分最高者作为初始路径计划
	Matrix<int> transformMatrix = computeTransform(allIdSet, (int)staticId.size());  // 转移矩阵
	planIter = allIdSet.begin();
	vector<int, int> scoreForPlan(0);  // 各个plan的得分，第二个元素是plan的位置，因为要排序
	int count = 0;
	for(planIter; planIter<allIdSet.end(); planIter++){
		scoreForPlan.push_back(make_pair(computeScore(*planIter, transformMatrix), count++));
	}
	sort(scoreForPlan.begin(), scoreForPlan.end(), ascendSort<int, int>);
	int planIndex = scoreForPlan[0].second;   // 选中的plan的位置
	vector<pair<int, int>> idInPlan = allIdSet[planIndex];
	vector<pair<int, int>>::iterator iter2 = idInPlan.begin()+1; // 第一个元素是仓库，跳过
	vector<Car*> outputPlan(0);   // 输出初始计划
	count = 0;

	// 将得分最高的计划（id顺序）所对应的customer一一对应到路径中作为输出
	for(iter2; iter2<idInPlan.end(); ){
		Car *tempCar = new Car(depot, depot, capacity, count++);
		while((*iter2).first != 0){ // 以仓库为断点
			Customer item = *mustServedCustomerSet[(*iter2).second];
			tempCar->getRoute().insertAtRear(item);
			iter2++;
		}
		outputPlan.push_back(tempCar);
		iter2++;
	}
	return outputPlan;
}