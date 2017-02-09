#include "SSALNS.h"
#include "LNS_rel.h"
#include "PublicFunction.h"
#include<algorithm>
#include<cassert>
#include "Matrix.h"
#include<functional>
#include<ctime>
#include "TxtRecorder.h"


using namespace std;

SSALNS::SSALNS(vector<Customer*> waitCustomer, vector<Car*> originPlan, float capacity){
	// 给各顾客赋予优先级
	this->capacity = capacity;
	depot = originPlan[0]->getRearNode();
	vector<Customer*>::iterator custPtr;
	for(custPtr = waitCustomer.begin(); custPtr < waitCustomer.end(); custPtr++){
		Customer* newCust = new Customer(**custPtr);
		newCust->priority = 2;
		this->waitCustomer.push_back(newCust);
	}
	vector<Car*>::iterator carPtr;
	for(carPtr = originPlan.begin(); carPtr < originPlan.end(); carPtr++){
		Customer headNode = (*carPtr)->getHeadNode();
		Customer rearNode = (*carPtr)->getRearNode();
		int carIndex = (*carPtr)->getCarIndex();
		float capacity = (*carPtr)->getCapacity();
		Car* newCar = new Car(headNode, rearNode, capacity, carIndex);
		vector<Customer*> custVec = (*carPtr)->getAllCustomer();
		for(custPtr = custVec.begin(); custPtr < custVec.end(); custPtr++) {
			(*custPtr)->priority = 1;
			newCar->insertAtRear(**custPtr);
		}
		this->originPlan.push_back(newCar);
	}
};

SSALNS::~SSALNS() {}

void SSALNS::shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer,
							int q, int p, float maxd, float maxt, float maxquantity){
	// originCarSet: 未执行remove操作前的货车集合
	// removedCarSet: 执行remove操作后的货车集合
	// removedCustomer: 被移除的顾客节点
	// q: 本次需要remove的顾客数量
	// p: 增加remove操作的随机性
	// maxd, maxt, maxquantity: 归一化值
	// 每次循环移除 y^p*|L|个顾客，L为路径中剩余节点，y是0-1之间的随机数

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
		float y = rand()/(RAND_MAX+1.0f);  // 产生0-1之间的随机数
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

void SSALNS::randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q){
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

void SSALNS::worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float DTpara[]){
	// originCarSet: 未执行remove操作前的货车集合
	// removedCarSet: 执行remove操作后的货车集合
	// removedCustomer: 被移除的顾客节点
	// q: 本次需要remove的顾客数量
	// p: 增加remove操作的随机性
	int i;
	vector<int> customerNum(0);       // 各辆货车顾客节点数目 
	vector<Customer*> allCustomerInOrder(0);  // 所有顾客节点
	vector<int> allIndex(0);             // 0~customerAmount-1，为所有的customer标记位置
	vector<int> indexsetInRoute(0);	  // 尚在路径中的节点下标
	vector<int> removedIndexset(0);    // 被移除的节点下标
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();  // originCarSet中的顾客数量
	for(i=0; i<customerAmount; i++){
		allIndex.push_back(i);
	}
	indexsetInRoute = allIndex;
	while((int)removedIndexset.size() < q){
		vector<pair<float, int>> reducedCost(customerAmount);  // 各节点的移除代价	
		computeReducedCost(originCarSet, indexsetInRoute, removedIndexset, reducedCost, DTpara);
		sort(reducedCost.begin(), reducedCost.end(), ascendSort<float, int>);   // 递增排序
		float y = rand()/(RAND_MAX+1.0f);  // 产生0-1之间的随机数
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();
		int removedNum = static_cast<int>(max(floor(pow(y,p)*indexInRouteLen), 1.0f));
		assert(removedNum <= indexInRouteLen);
		//vector<int> selectedIndex = probSelection(reducedCost, removedNum);
		//for(vector<int>::iterator intIter = selectedIndex.begin(); intIter < selectedIndex.end(); intIter++){
		//	removedIndexset.push_back(reducedCost[*intIter].second);
		//}
		for(i=0; i<removedNum; i++) {
			removedIndexset.push_back(reducedCost[i].second);
		}
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;   // 整数向量迭代器
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void SSALNS::greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd, float DTpara[]){
	// 把removedCustomer插入到removedCarSet中
	// 若当前货车无法容纳所有的removedCustomer，则新建artificial car
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // 需要插入到路径中的节点数目
	int carNum = removedCarSet.end() - removedCarSet.begin();    // 车辆数目
	int newCarIndex = removedCarSet[carNum-1]->getCarIndex()+1;  // 新车的起始标号
	int i;
	vector<int> alreadyInsertIndex(0);		   // 已经插入到路径中的节点下标，相对于allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // 在每条路径中的最小插入代价矩阵（行坐标：车辆，列坐标：顾客）
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // 在每条路径中的最小插入代价所对应的节点
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // 在每条路径中的次小插入代价矩阵
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // 在每条路径中的次小插入代价所对应的节点
	vector<int> allIndex(0);   // 对removedCustomer进行编号,1,2,3,...
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, 
		noiseAmount, noiseAdd, DTpara, false);
	//if(removedCustomerNum > 56) {
	//	minInsertPerRoute.printMatrixAtCol(56);
	//}
	vector<int> restCustomerIndex = allIndex;  // 剩下没有插入到路径中的节点下标，相对于removedCustomer
	vector<pair<float, pair<int,int>>> minInsertPerRestCust(0);  // 各个removedcustomer的最小插入代价
	                                                             // 只包含没有插入到路径中的节点
	                                                             // 第一个整数是节点下标，第二个节点是车辆位置
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		minInsertPerRestCust.clear();  // 每次使用之前先清空
		for(i=0; i<(int)restCustomerIndex.size(); i++){               // 只计算尚在路径中的节点
			int index = restCustomerIndex[i];
			int pos;
			float minValue;
			minValue = minInsertPerRoute.getMinAtCol(index, pos);
			minInsertPerRestCust.push_back(make_pair(minValue, make_pair(index, pos)));
		}	
		sort(minInsertPerRestCust.begin(), minInsertPerRestCust.end(), ascendSort<float, pair<int,int>>);
		int selectedCustIndex = minInsertPerRestCust[0].second.first;  // 被选中的顾客节点编号
		if(minInsertPerRestCust[0].first != MAX_FLOAT){  // 如果找到了可行插入位置
			int selectedCarPos = minInsertPerRestCust[0].second.second;  // 被选中的车辆位置
			removedCarSet[selectedCarPos]->insertAfter(minInsertPos.getElement(selectedCarPos, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			alreadyInsertIndex.push_back(selectedCustIndex);
			vector<int>::iterator iterINT;
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_difference要求先排序
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // 更新restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);
		} else {  // 没有可行插入位置，则再新开一辆货车
			int selectedCarPos = carNum++;  // 被选中的车辆位置
			Car *newCar = new Car(depot, depot, capacity, newCarIndex++, true);
			newCar->insertAtHead(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // 添加到货车集合中
			alreadyInsertIndex.push_back(selectedCustIndex); // 更新selectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_difference要求先排序
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // 更新restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();   // 增加一行
			minInsertPos.addOneRow();
			secondInsertPerRoute.addOneRow();
			secondInsertPos.addOneRow();
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);
		}
	}
}

void SSALNS::regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd, float DTpara[]){
	// 把removedCustomer插入到removedCarSet中
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // 需要插入到路径中的节点数目
	int carNum = removedCarSet.end() - removedCarSet.begin();    // 车辆数目
	int newCarIndex = removedCarSet[carNum - 1]->getCarIndex();  // 新车编号
	int i;
	vector<int> alreadyInsertIndex(0);		   // 已经插入到路径中的节点下标，相对于allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // 在每条路径中的最小插入代价矩阵（行坐标：车辆，列坐标：顾客）
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // 在每条路径中的最小插入代价所对应的节点
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // 在每条路径中的次小插入代价矩阵
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // 在每条路径中的次小插入代价所对应的节点
	vector<int> allIndex(0);   // 对removedCustomer进行编号
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, 
		noiseAmount, noiseAdd, DTpara, false);
	vector<int> restCustomerIndex = allIndex;  // 剩下没有插入到路径中的节点下标，相对于removedCustomer
	vector<pair<float, pair<int,int>>> regretdiffPerRestCust(0);  // 各个removedcustomer的最小插入代价与次小插入代价之差
	                                                              // 只包含没有插入到路径中的节点
	                                                              // 第一个整数是节点下标，第二个节点是车辆下标
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		int selectedCustIndex;   // 选中的顾客节点编号
		int selectedCarPos;      // 选中的货车位置
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
			selectedCarPos= carNum++;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			Car *newCar = new Car(depot, depot, capacity, newCarIndex++, true);
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
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);	
		} else {
			// 否则，不需要开辟新车
			selectedCarPos = regretdiffPerRestCust[0].second.second;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			alreadyInsertIndex.push_back(selectedCustIndex);
			removedCarSet[selectedCarPos]->insertAfter(minInsertPos.getElement(selectedCarPos, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin());
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);
		}
	}
}

bool judgeFeasible(vector<Car*> carSet, int &infeasibleNum) {
	// 判断carSet是否可行
	bool mark = true;
	vector<Car*>::iterator carIter;
	infeasibleNum = 0;
	for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
		if((*carIter)->judgeArtificial() == true) {
			// 判断artificial车上是否有priority为1的节点
			vector<Customer*> tempCust = (*carIter)->getAllCustomer();
			for(vector<Customer*>::iterator custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
				if((*custIter)->priority == 1) {
					mark = false;
					infeasibleNum++;
				}
			}
		}
	}
	return mark;
}


void SSALNS::run(vector<Car*> &finalCarSet, float &finalCost){  // 运行算法，相当于算法的main()函数
	int i;
	int PR1NUM = getCustomerNum(originPlan);
	int PR2NUM = (int)waitCustomer.size();
	int customerAmount = PR1NUM + PR2NUM;  // 总的顾客数
	int originCarNum = (int)originPlan.size();   // 初始拥有的货车数量
	vector<Customer*>::iterator custPtr;
	vector<Car*>::iterator carIter;
	srand(time(0));

	// 把当前拥有的所有顾客节点放到allCustomer中
	vector<Customer*> allCustomer;         // 所有的customer
	allCustomer.reserve(customerAmount);   // 为allCustomer预留空间
	for(carIter = originPlan.begin(); carIter < originPlan.end(); carIter++) {
		vector<Customer*> temp = (*carIter)->getAllCustomer();
		vector<Customer*>::iterator tempIter;
		for(tempIter = temp.begin(); tempIter < temp.end(); tempIter++) {
			allCustomer.push_back(*tempIter);
		}
	}
	for(custPtr = waitCustomer.begin(); custPtr < waitCustomer.end(); custPtr++) {
		allCustomer.push_back(*custPtr);
	}

	// 计算变量DT，依次放入vector DTpara中
	float DTH1, DTH2, DTL1, DTL2;
	float maxd, mind, maxquantity, distToDepot;    // 节点之间的最大/最小距离以及节点的最大货物需求量
	computeMax(allCustomer, maxd, mind, maxquantity);
	distToDepot = 0;    // 各个顾客节点到仓库的距离
	for(custPtr = allCustomer.begin(); custPtr < allCustomer.end(); custPtr++) {
		distToDepot += sqrt(pow((*custPtr)->x - depot.x, 2) + pow((*custPtr)->y - depot.y, 2));
	}
	DTL2 = 50;
	DTL1 = 2*maxd + 1;
	DTH2 = 80;
	float tempsigma1 = 2*maxd + DTH2;
	//float tempsigma2 = 2*(PR1NUM + PR2NUM + PR3NUM) * maxd + PR2NUM * DT22 + PR3NUM * DT32 - 
	//	(PR1NUM + PR2NUM + PR3NUM) * mind + PR2NUM * DT21 + PR3NUM * DT31 - DT12;
	float tempsigma2 = 2*distToDepot - DTH2 + PR2NUM * (DTL1 + DTL2) - (PR1NUM + PR2NUM + 1) * mind;
	DTH1 = max(tempsigma1, tempsigma2) + 1;

	float DTpara[4];
	DTpara[0] = DTH1;
	DTpara[1] = DTH2;
	DTpara[2] = DTL1;
	DTpara[3] = DTL2;

	// 这是根据priority为1的顾客插入到artificial car中的个数施加相应的惩罚
	float Delta = maxd*1.5;


	// 构造base solution
	vector<Car*> tempCarSet1;
	Car *tcar = new Car(depot, depot, capacity, 100, true);
	tempCarSet1.push_back(tcar);    // artificial carset, 存放待服务的顾客节点
	greedyInsert(tempCarSet1, waitCustomer, 0, false, DTpara); 
	vector<Car*> baseCarSet = copyPlan(originPlan);   
	for(carIter = tempCarSet1.begin(); carIter < tempCarSet1.end(); carIter++) {
		Car *tcar = new Car(**carIter);
		baseCarSet.push_back(tcar);
	}
	withdrawPlan(tempCarSet1);
	float baseCost = getCost(baseCarSet, DTpara);   // 基准代价，如果得到的解由于这个解，则一定可行
	                                                 // 一般来说比这个解更差的解是不可行的
	vector<Car*> artificialCarSet(0);
	vector<Car*> currentCarSet(0);
	for(carIter = originPlan.begin(); carIter < originPlan.end(); carIter++) {
		// 保留原有的车辆，记录其起点以及终点以及剩余容量、基准时间
		Car *newCar = new Car((*carIter)->getNullCar());
		currentCarSet.push_back(newCar);
	}
	greedyInsert(currentCarSet, allCustomer, 0, false, DTpara);  // 以当前所拥有的车辆为基础，构造初始解（完全重新构造）
	vector<Car*> globalCarSet = copyPlan(currentCarSet);         // 全局最优解，初始化与当前解相同
	float currentCost = getCost(currentCarSet, DTpara);
	float globalCost = currentCost;

	vector<size_t> hashTable(0);  // 哈希表
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
	setZero<int>(removeFreq, removeNum);
	setZero<int>(insertFreq, insertNum);
	setZero<int>(noiseFreq, 2);
	int removeScore[removeNum];     // 各个remove heuristic的得分
	int insertScore[insertNum];     // 各个insert heuristic的得分
	int noiseScore[2];              // 噪声得分
	setZero<int>(removeScore, removeNum);
	setZero<int>(insertScore, insertNum);
	setZero<int>(noiseScore, 2);
	// 三项得分设定
	int sigma1 = 33;
	int sigma2 = 9;
	int sigma3 = 13;
	float r = 0.1f;       // weight更新速率

	// 其余核心参数
	int maxIter = 2000; // 总的迭代次数
	int segment = 100;   // 每隔一个segment更新removeProb, removeWeight等参数
	float w = 0.05f;      // 初始温度设定有关参数
	float T = w * abs(currentCost) / (float)log(2);   // 初始温度
	int p = 6;           // 增加shawRemoval随机性
	int pworst = 3;      // 增加worstRemoval的随机性    
	float ksi = 0.8f;    // 每次移除的最大节点数目占总节点数的比例
	float eta = 0.025f;   // 噪声系数
	float noiseAmount = eta * maxd;   // 噪声量
	float c = 0.9998f;    // 降温速率
	vector<Customer*> removedCustomer(0);    // 被移除的节点
	vector<Car*> tempCarSet = copyPlan(currentCarSet);      // 暂时存放当前解

	pair<bool, int> removalSelectTrend = make_pair(false, 0);
	for(int iter=0; iter<maxIter; iter++){
		if(iter%segment == 0){  // 新的segment开始
			//cout << "...............Segement:" << (int)floor(iter/segment)+1 << "................" << endl;
			//cout << "base cost is: " << baseCost << endl;
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
				setZero<int>(removeFreq, removeNum);
				setZero<int>(removeScore, removeNum);
				setZero<int>(insertFreq, insertNum);
				setZero<int>(insertScore, insertNum);
				setZero<int>(noiseFreq, 2);
				setZero<int>(noiseScore, 2);
			}
		}

		// 产生随机数选取remove heuristic和insert heuristic
		// 以概率选择remove heuristic
		int removeIndex;
		float sumation;
		if(removalSelectTrend.first == false) {
			float removeSelection = random(0,1);  // 产生0-1之间的随机数
			sumation = removeProb[0];
			removeIndex = 0;    // remove heuristic编号
			while(sumation < removeSelection){
				sumation += removeProb[++removeIndex];
			}
		}
		else{
			removeIndex = removalSelectTrend.second;
		}
		removalSelectTrend.first = false;
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
		//removeIndex = 1;
		//insertIndex = 0;
		////////////////////////////////////////////

		// 相应算子使用次数加一
		removeFreq[removeIndex]++;
		insertFreq[insertIndex]++;
		noiseFreq[1-(int)noiseAdd]++;
		int maxRemoveNum = min(100, static_cast<int>(floor(ksi*customerAmount)));  // 最多移除那么多节点
		int minRemoveNum = 4;  // 最少移除那么多节点
		minRemoveNum = floor(0.4*customerAmount);
		int currentRemoveNum = (int)floor(random(minRemoveNum, maxRemoveNum));  // 当前要移除的节点数目
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
				worstRemoval(tempCarSet, removedCustomer, currentRemoveNum, pworst, DTpara);
				break;
			}
		}
		// 执行insert heuristic
		switch(insertIndex) {
		case 0:
			{
				greedyInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);
				break;
			}
		case 1:
			{
				regretInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);
				break;
			}
		}
		assert(getCustomerNum(tempCarSet) == customerAmount);
		// 移除空路径
		removeNullRoute(tempCarSet, true);

		// 使用模拟退火算法决定是否接收该解
		bool accept = false;
		float newCost = getCost(tempCarSet, DTpara);
		float acceptProb = exp(-(newCost - currentCost)/T);
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
					ksi = 0.4f;  // 如果得到了更好的解，则减少对当前解的扰动
					removeScore[removeIndex] += sigma2;
					insertScore[insertIndex] += sigma2;
					noiseScore[1-(int)noiseAdd] += sigma2;
				} else {      
					if(accept == true) {       // 情况3
						ksi = 0.8f;   // 如果得到了更差的解，则增加对当前解的扰动
						removalSelectTrend.first = true;   // 这时强制使用random removal来破坏当前的解
						removalSelectTrend.second = 1;     // random removal
						removeScore[removeIndex] += sigma3;
						insertScore[insertIndex] += sigma3;
						noiseScore[1-(int)noiseAdd] += sigma3;						
					}
				}
			}
		}
		if(accept == true) {    // 如果被接受了，则更新currentCarSet， 并且tempCarSet不变
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){
				hashTable.push_back(newRouteCode); 
			}
			currentCost = newCost;     // 如果被接收，则更新当前解
			withdrawPlan(currentCarSet);
			currentCarSet = copyPlan(tempCarSet);
		} else {    // 否则，tempCarSet恢复为currentCarSet
			withdrawPlan(tempCarSet);
			tempCarSet = copyPlan(currentCarSet);
		}
	}

	finalCarSet.clear();
	finalCarSet.reserve(originPlan.size());
	ostringstream ostr;
	ostr.str("");
	if(globalCost > baseCost) {
		// 如果搜索不到更好的解，则维持原来的解
		ostr << "SSALNS: we should use the origin plan" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		finalCarSet = copyPlan(originPlan);
	} else {
		ostr << "SSALNS: we will use the new plan" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		finalCarSet = copyPlan(globalCarSet);
	}
	//int infeasibleNum;
	//bool mark2 = judgeFeasible(finalCarSet, infeasibleNum);
	//cout << "Whether the solution is feasible? " << mark2 << endl;
	finalCost = globalCost;
	deleteCustomerSet(waitCustomer);
	withdrawPlan(originPlan);
	withdrawPlan(baseCarSet);
	withdrawPlan(tempCarSet);
}

