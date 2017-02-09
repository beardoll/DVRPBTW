#include "PublicFunction.h"
#include "LNS_rel.h"
#include "ALNS.h"
#include<algorithm>
#include<cassert>
#include<functional>

using namespace std;

ALNS::ALNS(vector<Customer*> allCustomer, Customer depot, float capacity):depot(depot), capacity(capacity){
	this->allCustomer = copyCustomerSet(allCustomer);
}

void ALNS::shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer,
							int q, int p, float maxd, float maxt, float maxquantity){
	// originCarSet: 未执行remove操作前的货车集合
	// removedCarSet: 执行remove操作后的货车集合
	// removedCustomer: 被移除的顾客节点
	// q: 本次需要remove的顾客数量
	// p: 增加remove操作的随机性
	// maxd, maxt, maxquantity: 归一化值
	// 每次循环移除 y^p*|L|个顾客，L为路径中剩余节点，y是0-1之间的随机数
	ascendSort<int, int>(make_pair(1,2), make_pair(3,4));						
	int phi = 9;
	int kai = 3;
	int psi = 2;
	int carAmount = originCarSet.end()-originCarSet.begin();  // 货车数量
	vector<int> customerNum(0);       // 各辆车所负责的顾客节点数目
	vector<Customer*> allCustomerInOrder(0);
	int i,j;
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);   
	// 获取所有的Customer, customerNum存放各辆车所拥有的顾客节点数目， allCustomer存放所有顾客节点
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();  // 顾客数量
	vector<pair<float, int>> R(customerAmount*customerAmount);     // 相似矩阵
	float temp1;
	vector<int> allIndex(customerAmount);  // 0~customerAmount-1
	for(i=0; i<(int)originCarSet.size(); i++){
		originCarSet[i]->getRoute().refreshArrivedTime();
	}
	for(i=0; i<customerAmount; i++){
		allIndex[i] = i;
		for(j=0; j<customerAmount; j++){
			if(i==j) { 
				R[i*customerAmount+j].first = MAX_FLOAT;
				R[i*customerAmount+j].second = j;
			}
			else{
				temp1 = phi*sqrt(pow(allCustomerInOrder[i]->x-allCustomerInOrder[j]->x,2) + pow(allCustomerInOrder[i]->y-allCustomerInOrder[j]->y, 2))/maxd +
					kai * abs(allCustomerInOrder[i]->arrivedTime - allCustomerInOrder[j]->arrivedTime)/maxt +
					psi * abs(allCustomerInOrder[i]->quantity - allCustomerInOrder[j]->quantity)/maxquantity;
				R[i*customerAmount+j].first = temp1;   // i行j列
				R[i*customerAmount+j].second = j;
				R[j*customerAmount+i].first = temp1;
				R[j*customerAmount+i].second = i;      // j行i列
			}
		}
	}
	int selectedIndex;           // 被选中的节点在allCustomer中的下标
	vector<int> removedIndexset; // 所有被移除的节点的下标集合
	selectedIndex = int(random(0,customerAmount));   // 随机选取一个节点
	removedIndexset.push_back(selectedIndex);
	vector<int> indexsetInRoute(customerAmount-1);     // 在路径中的节点的下标集合
	set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
	while((int)removedIndexset.size() < q){  // 要移除掉一共q个节点
		vector<pair<float, int>> currentR(0);        // 当前要进行排序的相似矩阵（向量），只包含尚在路径中的节点
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin(); // 在路径中的节点的个数
		vector<pair<float, int>>::iterator iter1;
		for(i=0; i<indexInRouteLen; i++){
			int index = indexsetInRoute[i];
			currentR.push_back(R[selectedIndex*customerAmount + index]);
		}
		sort(currentR.begin(), currentR.end(), ascendSort<float, int>);  // 相似性按小到大进行排序
		float y = random(0,1);  // 产生0-1之间的随机数
		int indexsetInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();  // indexsetInRoute的长度
		int removeNum = max((int)floor(pow(y,p)*indexsetInRouteLen), 1);             // 本次移除的节点数目
		for(i=0; i<removeNum ; i++){
			removedIndexset.push_back(currentR[i].second);
		}
		int indexRemovedLen = removedIndexset.end() - removedIndexset.begin();  // 当前removedIndexset的长度
		int randint = (int)random(0,indexRemovedLen);  // 产生一个0-indexRemovedLen的随机数
		selectedIndex = removedIndexset[randint];
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void ALNS::randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q){
	// originCarSet: 未执行remove操作前的货车集合
	// removedCarSet: 执行remove操作后的货车集合
	// removedCustomer: 被移除的顾客节点
	// q: 本次需要remove的顾客数量
	int i;
	vector<int> customerNum(0);       // 各辆货车顾客节点数目 
	vector<Customer*> allCustomerInOrder(0);  // 所有顾客节点
	vector<int> allIndex;             // 0~customerAmount-1
	vector<int> indexsetInRoute(0);
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();
	vector<int> removedIndexset(0); 
	for(i=0; i<customerAmount; i++){
		allIndex.push_back(i);
	}
	indexsetInRoute = allIndex;
	for(i=0; i<q; i++){
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();  // 尚在路径中的节点个数
		int selectedIndex = int(random(0, indexInRouteLen));  // 在indexsetInRoute中的索引
		removedIndexset.push_back(indexsetInRoute[selectedIndex]);
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void ALNS::worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p){
	// originCarSet: 未执行remove操作前的货车集合
	// removedCarSet: 执行remove操作后的货车集合
	// removedCustomer: 被移除的顾客节点
	// q: 本次需要remove的顾客数量
	// p: 增加remove操作的随机性
	int i;
	vector<int> customerNum(0);       // 各辆货车顾客节点数目 
	vector<Customer*> allCustomerInOrder(0);  // 所有顾客节点
	vector<int> allIndex(0);             // 0~customerAmount-1
	vector<int> indexsetInRoute(0);	  // 尚在路径中的节点下标
	vector<int> removedIndexset(0);    // 被移除的节点下标
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	int customerAmount = allCustomer.end() - allCustomer.begin();
	for(i=0; i<customerAmount; i++){
		allIndex.push_back(i);
	}
	indexsetInRoute = allIndex;
	while((int)removedIndexset.size() < q){
		vector<pair<float, int>> reducedCost(customerAmount);  // 各节点的移除代价
		float penaltyPara[4] = {0, 0 , 0, 0};
		computeReducedCost(originCarSet, indexsetInRoute, removedIndexset, reducedCost, penaltyPara);
		sort(reducedCost.begin(), reducedCost.end(), ascendSort<float, int>);   // 递增排序
		float y = rand()/(RAND_MAX+1.0f);  // 产生0-1之间的随机数
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();
		int removedNum = static_cast<int>(max(floor(pow(y,p)*indexInRouteLen), 1.0f));
		assert(removedNum <= indexInRouteLen);
		for(i=0; i<removedNum; i++){
			removedIndexset.push_back(reducedCost[i].second);
		}
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void ALNS::greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd){
	// 把removedCustomer插入到removedCarSet中
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // 需要插入到路径中的节点数目
	int carNum = removedCarSet.end() - removedCarSet.begin();  // 车辆数目
	int i;
	vector<int> alreadyInsertIndex(0);		   // 已经插入到路径中的节点下标，相对于allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // 在每条路径中的最小插入代价矩阵（行坐标：车辆，列坐标：顾客）
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // 在每条路径中的最小插入代价所对应的节点
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // 在每条路径中的次小插入代价矩阵
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // 在每条路径中的次小插入代价所对应的节点
	vector<int> allIndex(0);   // 对removedCustomer进行编号
	float penaltyPara[4] = {0, 0, 0, 0};
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, noiseAmount, noiseAdd, penaltyPara);
	vector<int> restCustomerIndex = allIndex;  // 剩下没有插入到路径中的节点下标，相对于allIndex
	vector<pair<float, pair<int,int>>> minInsertPerRestCust(0);  // 各个removedcustomer的最小插入代价
	                                                             // 只包含没有插入到路径中的节点
	                                                             // 第一个整数是节点下标，第二个节点是车辆下标
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		minInsertPerRestCust.clear();  // 每次使用之前先清空
		for(i=0; i<(int)restCustomerIndex.size(); i++){               // 只计算尚在路径中的节点
			int index = restCustomerIndex[i];
			int pos;
			float minValue;
			minValue = minInsertPerRoute.getMinAtCol(index, pos);
			minInsertPerRestCust.push_back(make_pair(minValue, make_pair(index, pos)));
		}	
		sort(minInsertPerRestCust.begin(), minInsertPerRestCust.end(), ascendSort<float, pair<int, int>>);
		int selectedCustIndex = minInsertPerRestCust[0].second.first;  // 被选中的顾客节点编号
		if(minInsertPerRestCust[0].first != MAX_FLOAT){  // 如果找到了可行插入位置
			int selectedCarIndex = minInsertPerRestCust[0].second.second;  // 被选中的车辆编号
			removedCarSet[selectedCarIndex]->insertAfter(minInsertPos.getElement(selectedCarIndex, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			alreadyInsertIndex.push_back(selectedCustIndex);
			vector<int>::iterator iterINT;
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_difference要求先排序
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // 更新restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarIndex, removedCarSet, removedCustomer, noiseAmount, noiseAdd, penaltyPara);
		} else {  // 没有可行插入位置，则再新开一辆货车
			int selectedCarIndex = carNum++;  // 被选中的车辆编号
			Car *newCar = new Car(depot, depot, capacity, selectedCarIndex);
			newCar->insertAtHead(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // 添加到货车集合中
			alreadyInsertIndex.push_back(selectedCustIndex);             // 更新selectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_difference要求先排序
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // 更新restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();   // 增加一行
			minInsertPos.addOneRow();
			secondInsertPerRoute.addOneRow();
			secondInsertPos.addOneRow();
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarIndex, removedCarSet, removedCustomer, noiseAmount, noiseAdd, penaltyPara);
		}
	}
}

void ALNS::regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd){
	// 把removedCustomer插入到removedCarSet中
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // 需要插入到路径中的节点数目
	int carNum = removedCarSet.end() - removedCarSet.begin();  // 车辆数目
	int i;
	vector<int> alreadyInsertIndex(0);		   // 已经插入到路径中的节点下标，相对于allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // 在每条路径中的最小插入代价矩阵（行坐标：车辆，列坐标：顾客）
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // 在每条路径中的最小插入代价所对应的节点
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // 在每条路径中的次小插入代价矩阵
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // 在每条路径中的次小插入代价所对应的节点
	vector<int> allIndex(0);   // 对removedCustomer进行编号
	float penaltyPara[4] = {0, 0, 0, 0};
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, noiseAmount, noiseAdd, penaltyPara);
	vector<int> restCustomerIndex = allIndex;  // 剩下没有插入到路径中的节点下标，相对于allIndex
	vector<pair<float, pair<int,int>>> regretdiffPerRestCust(0);  // 各个removedcustomer的最小插入代价与次小插入代价之差
	                                                              // 只包含没有插入到路径中的节点
	                                                              // 第一个整数是节点下标，第二个节点是车辆下标
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		int selectedCustIndex;   // 选中的顾客节点编号
		int selectedCarIndex;    // 选中的货车编号
		regretdiffPerRestCust.clear();
		for(i=0; i<(int)restCustomerIndex.size(); i++){
			int index = restCustomerIndex[i];        // 顾客节点下标
			float minValue, secondValue1, secondValue2;
			int pos1, pos2, pos3;
			minValue = minInsertPerRoute.getMinAtCol(index, pos1);          // 最小插入代价
			minInsertPerRoute.setValue(pos1, index, MAX_FLOAT);
			secondValue1 = minInsertPerRoute.getMinAtCol(index, pos2);      // 候选次小插入代价
			minInsertPerRoute.setValue(pos1, index, minValue);              // 恢复数据
			secondValue2 = secondInsertPerRoute.getMinAtCol(index, pos3);   // 候选次小插入代价
			if(minValue == MAX_FLOAT){  
				// 如果发现某个节点已经没有可行插入点，则优先安排之
				regretdiffPerRestCust.push_back(make_pair(MAX_FLOAT, make_pair(index, pos1)));
			} else if(minValue != MAX_FLOAT && secondValue1==MAX_FLOAT && secondValue2==MAX_FLOAT){
				// 如果只有一个可行插入点，则应该优先安排
				// 按照minValue的值，最小者应该率先被安排
				// 因此diff = LARGE_FLOAT - minValue
				regretdiffPerRestCust.push_back(make_pair(LARGE_FLOAT-minValue, make_pair(index, pos1)));
			} else{
				if(secondValue1 <= secondValue2){
					regretdiffPerRestCust.push_back(make_pair(abs(minValue-secondValue1), make_pair(index, pos1)));
				} else{
					regretdiffPerRestCust.push_back(make_pair(abs(minValue-secondValue2), make_pair(index, pos1)));
				}
			}
		}
		sort(regretdiffPerRestCust.begin(), regretdiffPerRestCust.end(), descendSort<float, pair<int, int>>);  // 应该由大到小进行排列
		if(regretdiffPerRestCust[0].first == MAX_FLOAT) {
			// 如果所有的节点都没有可行插入点，则开辟新车
			selectedCarIndex = carNum++;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			Car *newCar = new Car(depot, depot, capacity, selectedCarIndex);
			newCar->insertAtHead(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // 添加到货车集合中
			alreadyInsertIndex.push_back(selectedCustIndex); // 更新selectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // 更新restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();   // 增加一行
			minInsertPos.addOneRow();
			secondInsertPerRoute.addOneRow();
			secondInsertPos.addOneRow();
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarIndex, removedCarSet, removedCustomer, noiseAmount, noiseAdd, penaltyPara);	
		} else {
			// 否则，不需要开辟新车
			selectedCarIndex = regretdiffPerRestCust[0].second.second;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			alreadyInsertIndex.push_back(selectedCustIndex);
			removedCarSet[selectedCarIndex]->insertAfter(minInsertPos.getElement(selectedCarIndex, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin());
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarIndex, removedCarSet, removedCustomer, noiseAmount, noiseAdd, penaltyPara);
		}
	}
}

void ALNS::run(vector<Car*> &finalCarSet, float &finalCost){  // 运行算法，相当于算法的main()函数
	int i;
	// 建立初始解
	int customerAmount = allCustomer.size();
	vector<Car*> currentCarSet(0);
	Car *initialCar = new Car(depot, depot, capacity, 0);  // 先新建一辆车
	currentCarSet.push_back(initialCar);
	greedyInsert(currentCarSet, allCustomer, 0, false);  // 建立初始路径
	float penaltyPara[4] = {0, 0, 0, 0}; 
	float currentCost = getCost(currentCarSet, penaltyPara);
	vector<Car*> globalCarSet = copyPlan(currentCarSet);
	float globalCost = currentCost;

	// 哈希表
	vector<size_t> hashTable(0);  
	hashTable.push_back(codeForSolution(currentCarSet));

	// 评分机制相关参数的设定
	const int removeNum = 3;    // remove heuristic的个数
	const int insertNum = 2;    // insert heuristic的个数
	float removeProb[removeNum];  // 各个remove heuristic的概率
	float insertProb[insertNum];  // 各个insert heuristic的概率
	float noiseProb[2] = {0.5, 0.5};        // 噪声使用的概率
	for(i=0; i<removeNum; i++){
		removeProb[i] = 1.0f/removeNum;
	}
	for(i=0; i<insertNum; i++){
		insertProb[i] = 1.0f/insertNum;
	}
	float removeWeight[removeNum];  // 各个remove heuristic的权重
	float insertWeight[insertNum];  // 各个insert heuristic的权重
	float noiseWeight[2];   // 加噪声/不加噪声 分别的权重
	setOne(removeWeight, removeNum);
	setOne(insertWeight, insertNum);
	setOne(noiseWeight, 2);
	int removeFreq[removeNum];      // 各个remove heuristic使用的频率
	int insertFreq[insertNum];      // 各个insert heuristic使用的频率
	int noiseFreq[2];               // 噪声使用的频率，第一个是with noise，第二个是without noise
	float haha[3];
	setZero(removeFreq, removeNum);
	setZero(insertFreq, insertNum);
	setZero(noiseFreq, 2);
	int removeScore[removeNum];     // 各个remove heuristic的得分
	int insertScore[insertNum];     // 各个insert heuristic的得分
	int noiseScore[2];              // 噪声得分
	setZero(removeScore, removeNum);
	setZero(insertScore, insertNum);
	setZero(noiseScore, 2);
	// 三项得分设定
	int sigma1 = 33;
	int sigma2 = 9;
	int sigma3 = 13;
	float r = 0.1f;       // weight更新速率

	// 其余核心参数
	int maxIter = 200; // 总的迭代次数
	int segment = 100;   // 每隔一个segment更新removeProb, removeWeight等参数
	float w = 0.05f;      // 初始温度设定有关参数
	float T = w * currentCost / (float)log(2);   // 初始温度
	int p = 6;           // 增加shawRemoval随机性
	int pworst = 3;      // 增加worstRemoval的随机性
	float ksi = 0.4f;     // 每次移除的最大节点数目占总节点数的比例
	float eta = 0.025f;   // 噪声系数
	float maxd, mind, maxquantity;    // 节点之间的最大距离以及节点的最大货物需求量
	computeMax(allCustomer, maxd, mind, maxquantity);
	float noiseAmount = eta * maxd;   // 噪声量
	float c = 0.9998f;    // 降温速率
	vector<Customer*> removedCustomer(0);                // 被移除的节点
	vector<Car*> tempCarSet = copyPlan(currentCarSet);   // 暂时存放当前解
	for(int iter=0; iter<maxIter; iter++){
		if(iter%segment == 0){  // 新的segment开始
			//cout << "...............Segement:" << (int)floor(iter/segment)+1 << "................" << endl;
			//cout << "current best cost is:" << globalCost << endl;
			//cout << "hash table length is:" << hashTable.size() << endl;
			//cout << "shaw   removal:" <<  "(score)-" << removeScore[0] << '\t' << "(freq)-" << removeFreq[0] << endl;
			//cout << "random removal:" <<  "(score)-" << removeScore[1] << '\t' << "(freq)-" << removeFreq[1] << endl;
			//cout << "worst  removal:" <<  "(score)-" << removeScore[2] << '\t' << "(freq)-" << removeFreq[2] << endl;
			//cout << "greedy  insert:" <<  "(score)-" << insertScore[0] << '\t' << "(freq)-" << insertFreq[0] << endl;
			//cout << "regret  insert:" <<  "(score)-" << insertScore[1] << '\t' << "(freq)-" << insertFreq[1] << endl;
			//cout << "noise    addIn:" <<  "(score)-" << noiseScore[0]  << '\t' << "(freq)-" << noiseFreq[0]  << endl;
			//cout << endl;
			if(iter != 0){      // 如果不是第一个segment
				// 更新权重
				updateWeight(removeFreq, removeWeight, removeScore, r, removeNum);
				updateWeight(insertFreq, insertWeight, insertScore, r, insertNum);
				updateWeight(noiseFreq, noiseWeight, noiseScore, r, 2);
				// 更新概率
				updateProb(removeProb, removeWeight, removeNum);
				updateProb(insertProb, insertWeight, insertNum);
				updateProb(noiseProb, noiseWeight, 2);
				// 将各变量置零
				setZero(removeFreq, removeNum);
				setZero(removeScore, removeNum);
				setZero(insertFreq, insertNum);
				setZero(insertScore, insertNum);
				setZero(noiseFreq, 2);
				setZero(noiseScore, 2);
			}
		}

		// 产生随机数选取remove heuristic和insert heuristic
		// 以概率选择remove heuristic
		float removeSelection = random(0,1);  // 产生0-1之间的随机数
		float sumation = removeProb[0];
		int removeIndex = 0;    // remove heuristic编号
		while(sumation < removeSelection){
			sumation += removeProb[++removeIndex];
		}
		// 以概率选择insert heurisitc
		float insertSelection = random(0,1);
		sumation = insertProb[0];
		int insertIndex = 0;
		while(sumation < insertSelection){
			sumation += insertProb[++insertIndex];
		}
		// 以概率选择是否增加噪声影响
		float noiseSelection = random(0,1);
		bool noiseAdd = false;
		if(noiseProb[0] > noiseSelection) {
			noiseAdd = true;
		}

		////@@@@@@@@@ dangerous!!!!!!!!! @@@@@@@@@//
		//removeIndex = 0;
		////////////////////////////////////////////

		// 相应算子使用次数加一
		removeFreq[removeIndex]++;
		insertFreq[insertIndex]++;
		noiseFreq[1-(int)noiseAdd]++;
		int maxRemoveNum = min(100, static_cast<int>(floor(ksi*customerAmount)));  // 最多移除那么多节点
		int minRemoveNum = 4;  // 最少移除那么多节点
		int currentRemoveNum = (int)random(minRemoveNum, maxRemoveNum);  // 当前要移除的节点数目
		removedCustomer.clear();         // 清空removedCustomer
		removedCustomer.resize(0);

		// 执行remove heuristic
		switch(removeIndex) {
		case 0: 
			{
				// 首先得到maxArrivedTime
				float maxArrivedTime = -MAX_FLOAT;
				for(i=0; i<(int)tempCarSet.size(); i++){
					tempCarSet[i]->getRoute().refreshArrivedTime();	
					vector<float> temp = tempCarSet[i]->getRoute().getArrivedTime();
					sort(temp.begin(), temp.end(), greater<float>());
					if(temp[0] > maxArrivedTime) {
						maxArrivedTime = temp[0];
					}
				}
				shawRemoval(tempCarSet, removedCustomer, currentRemoveNum, p, maxd, maxArrivedTime, maxquantity);
				break;
			}
		case 1:
			{
				randomRemoval(tempCarSet, removedCustomer, currentRemoveNum);
				break;
			}
		case 2:
			{
				worstRemoval(tempCarSet, removedCustomer, currentRemoveNum, pworst);
				break;
			}
		}
		// 执行insert heuristic
		switch(insertIndex) {
		case 0:
			{
				greedyInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd);
				break;
			}
		case 1:
			{
				regretInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd);
				break;
			}
		}
		assert(getCustomerNum(tempCarSet) == customerAmount);
		// 移除空路径
		removeNullRoute(tempCarSet);

		// 使用模拟退火算法决定是否接收该解
		float newCost = getCost(tempCarSet, penaltyPara);
		float acceptProb = exp(-(newCost - currentCost)/T);
		bool accept = false;
		if(acceptProb > rand()/(RAND_MAX+1.0f)) {
			accept = true;
		}
		T = T * c;   // 降温
		size_t newRouteCode = codeForSolution(tempCarSet);

		// 接下来判断是否需要加分
		// 加分情况如下：
		// 1. 当得到一个全局最优解时
		// 2. 当得到一个尚未被接受过的，而且更好的解时
		// 3. 当得到一个尚未被接受过的解，虽然这个解比当前解差，但是这个解被接受了
		if(newCost < globalCost){  // 情况1
			removeScore[removeIndex] += sigma1;
			insertScore[insertIndex] += sigma1;
			noiseScore[1-(int)noiseAdd] += sigma1;
			withdrawPlan(globalCarSet);
			globalCarSet = copyPlan(tempCarSet);
			globalCost = newCost;
		} else {
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){  // 该解从来没有被接受过
				if(newCost < currentCost){    // 如果比当前解要好，情况2
					removeScore[removeIndex] += sigma2;
					insertScore[insertIndex] += sigma2;
					noiseScore[1-(int)noiseAdd] += sigma2;
				} else {
					if(accept == true) {       // 情况3
						removeScore[removeIndex] += sigma3;
						insertScore[insertIndex] += sigma3;
						noiseScore[1-(int)noiseAdd] += sigma3;						
					}
				}
			}
		}
		if(accept == true) {    
			// 如果被接受了，则更新currentCarSet， 并且tempCarSet不变
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){
				hashTable.push_back(newRouteCode); 
			}
			currentCost = newCost;     // 如果被接收，则更新当前解
			withdrawPlan(currentCarSet);
			currentCarSet = copyPlan(tempCarSet);
		} else {    
			// 否则，tempCarSet恢复为currentCarSet
			withdrawPlan(tempCarSet);
			tempCarSet = copyPlan(currentCarSet);
		}
	}
	finalCarSet.clear();
	finalCarSet.resize(globalCarSet.size());
	finalCarSet = copyPlan(globalCarSet);
	withdrawPlan(globalCarSet);
	withdrawPlan(tempCarSet);
	withdrawPlan(currentCarSet);
	deleteCustomerSet(allCustomer);
	finalCost = globalCost;
}

