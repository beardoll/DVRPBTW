#include "Route.h"
#include "Customer.h"
#include<iostream>
#include<cassert>
#include<vector>
#include<cmath>
#include<limits>
#include<ctime>
#include<cstdlib>

const float MAX_FLOAT = numeric_limits<float>::max();

using namespace std;
Route::Route(Customer &headNode, Customer &rearNode, float capacity):capacity(capacity), quantityL(0), quantityB(0){ // 构造函数
	head = new Customer;
	*head = headNode;  // 复制节点
	rear = new Customer;
	*rear = rearNode; 
	head->front = NULL;
	head->next = rear;
	rear->front = head;
	rear->next = NULL;
	current = head;  // 初始化current指针指向head节点
	size = 0;
	carIndex = -1;
	arrivedTime.push_back(0);
}

Route::~Route(){ // 析构函数
	this->clear();
}

void Route::copy(const Route &L){
	// 应当对L的head节点和rear节点都复制
	this->size = L.size;
	this->carIndex = L.carIndex;
	this->capacity = L.capacity;
	this->quantityL = L.quantityL;
	this->quantityB = L.quantityB;
	Customer* originPtr = L.head;
	Customer* copyPtr = head;
	Customer* temp;
	while(originPtr!=NULL){
		// 从头节点一直复制到尾节点
		if(originPtr == L.head){  // 正在复制第一个节点
			copyPtr = new Customer;
			copyPtr->front = NULL;
			head = copyPtr;
			*copyPtr = *L.head;
		} else{
			temp = new Customer;
			*temp = *originPtr;
			temp->front = copyPtr;
			copyPtr->next = temp;
			copyPtr = temp;
		}
		if(L.current == originPtr){
			current = copyPtr;
		}
		originPtr = originPtr->next;
	}
	temp->next = NULL;
	rear = temp;
}

Customer& Route::operator[] (int k){
	assert(k>=0 && k<size);
	Customer* temp = head->next;
	for(int i=0; i<k; i++){
		temp = temp->next;
	}
	return *temp;
}

const Customer& Route::operator[] (int k) const{
	assert(k>=0 && k<size);
	Customer* temp = head->next;
	for(int i=0; i<k; i++){
		temp = temp->next;
	}
	return *temp;
}

Route::Route(const Route &L){ // 复制构造函数	
	this->copy(L);
}


Route& Route::operator= (const Route &L){ 
	// 重载"="运算符，用以配合深复制
	this->clear();  // 清空当前链表	
	this->copy(L);
	return *this;
}

bool Route::isEmpty(){ //判断链表是否为空
	return (size==0);
}

bool Route::insertAfter(Customer &item1, Customer &item2){
	// 在链表中与item1相同的节点后面插入节点item2
	Customer* temp = new Customer;
	*temp = item2;
	Customer* ptr = head;
	while(ptr!=rear){
		if (ptr->id == item1.id){  // 根据id来判断两个节点是否相同
			break;
		}
		ptr = ptr->next;
	}
	if(ptr == rear) {
		// 没有找到，返回false
		return false;
	} 
	switch (item2.type){
	case 'L':
		quantityL = quantityL + item2.quantity;
		break;
	case 'B':
		quantityB = quantityB + item2.quantity;
		break;
	}
	temp->next = ptr->next;
	ptr->next->front = temp;
	temp->front = ptr;
	ptr->next = temp;
	size++;
	return true;
}

void Route::insertAtFront(Customer &item){ // 在表头插入item
	Customer *temp = new Customer;
	*temp = item;
	if(size == 0){
		// 如果是空链表
		head->next = temp;
		temp->front = head;
		temp->next = rear;
		rear->front = temp;
		current = head;
	}else{  // 否则front指向了一个Customer节点，拥有属性next
		temp->next = head->next;
		head->next->front = temp;
		head->next = temp;
		temp->front = head;
	}
	switch (item.type){
	case 'L':
		quantityL = quantityL + item.quantity;
		break;
	case 'B':
		quantityB = quantityB + item.quantity;
		break;
	}
	size++;
}

bool Route::deleteNode(Customer &item){
	// 删除链表中与item相同的节点
	Customer* temp1 = head->next;
	while(temp1!=rear) {
		if(temp1->id == item.id) {  // 如果找到，temp1指向当前节点，temp2->next=temp1;
			break;
		}
		temp1 = temp1->next;
	}
	if(temp1 == rear) {  // 没有找到
		return false;
	} else {
		Customer* nextNode = temp1->next;
		Customer* frontNode = temp1->front;
		frontNode->next = nextNode;
		nextNode->front = frontNode;
		delete temp1;
		size--;
		switch (item.type){
		case 'L':
			quantityL = quantityL - item.quantity;
			break;
		case 'B':
			quantityB = quantityB - item.quantity;
			break;
		}
		return true;
	}
}

bool Route::next(){ // current指针往前走
	current = current->next;
	if(current == rear) {  // 走到了尽头
		return false;
	} else {
		return true;
	}
}

Customer& Route::currentPos(){ // 返回当前位置
	return *current;
}

void Route::clear(){  // 清空链表，不清空head节点和rear节点?
	Customer* ptr1 = head;
	Customer* ptr2;
	while(ptr1!=NULL){
		ptr2 = ptr1->next;
		delete ptr1;
		ptr1 = ptr2;
	}
	head = NULL;
	rear = NULL;
	current = NULL;
	size = 0;
	carIndex = -1;  
}

void Route::assignCar(int carIndex){ // 为route分配车辆
	this->carIndex = carIndex;
}

Route& Route::capture(){ 
	// 抓取current指针后的路径
	// current指针当前节点将作为head节点
	Route* ptr1 = new Route(*current, *rear, capacity);
	if(current->next == rear) { // current指针后已经没有路径
		return *ptr1;
	}
	Customer* ptr2 = current->next;
	Customer *ptr3, *ptr4;
	ptr4 = ptr1->head;
	while(ptr2 != rear) {
		ptr3 = new Customer;
		*ptr3 = *ptr2;
		ptr4->next = ptr3;
		ptr3->front = ptr4;
		ptr4 = ptr3;
		switch (ptr2->type){
		case 'L':
			ptr1->quantityL = ptr1->quantityL + ptr2->quantity;
			break;
		case 'B':
			ptr1->quantityB = ptr1->quantityB + ptr2->quantity;
			break;
		}
		ptr2 = ptr2->next;
		ptr1->size++;
	}
	ptr3->next = ptr1->rear;
	ptr1->rear->front = ptr3;
	return *ptr1;
}

void Route::replaceRoute(const Route &route) {  // 以route替换掉current指针后的路径
	if(current == head) { // 空路径，直接复制即可
		this->clear();   
		this->copy(route);
		return;
	}
	Customer* ptr1 = route.head->next;
	Customer *ptr2, *ptr3;
	ptr2 = current->next;
	// 清除原路径中current指针后面的元素
	// 不包括对rear节点的清除
	while(ptr2 != rear) { 
		switch (ptr2->type){
		case 'L':
			quantityL = quantityL - ptr2->quantity;
			break;
		case 'B':
			quantityB = quantityB - ptr2->quantity;
			break;
		}
		ptr3 = ptr2->next;
		delete ptr2;
		ptr2 = ptr3;
		size--;
	}
	// 将route中除head和rear外的节点都复制到current指针后
	ptr3 = current;
	while(ptr1 != route.rear){  
		switch (ptr1->type){
		case 'L':
			quantityL = quantityL + ptr1->quantity;
			break;
		case 'B':
			quantityB = quantityB + ptr1->quantity;
			break;
		}
		ptr2 = new Customer;
		*ptr2 = *ptr1;
		ptr3->next = ptr2;
		ptr2->front = ptr3;
		ptr3 = ptr2;
		ptr1 = ptr1->next;
		size++;
	}
	ptr3->next = rear;
	rear->front = ptr3;
	return;
}

int Route::getSize() {
	return this->size;
}

void Route::printRoute(){ // 打印链表
	Customer* ptr = head;
	for(; ptr!=NULL; ptr=ptr->next) {
		cout << "id:" << ptr->id << ' ' << "type:" << ' ' << ptr->type << endl;
	}
}

vector<Customer*> Route::getAllCustomer(){  // 得到路径中所有的顾客节点
	vector<Customer*> customerSet(size);
	Customer* ptr = head->next;
	Customer* ptr2;
	for(int i=0; i<size; i++){
		ptr2 = new Customer;
		*ptr2 = *ptr;
		customerSet[i] = ptr2;
		ptr = ptr->next;
	}
	return customerSet;
}

vector<float> Route::computeReducedCost(){ 
	// 得到所有顾客节点的移除代价
	// 值越小表示移除它可以节省更多的代价
	vector<float> costArr(0);
	Customer *ptr1 = head;   // 前节点
	Customer *ptr2, *ptr3;
	for(int i=0; i<size; i++){
		ptr2 = ptr1->next;  // 当前节点
		ptr3 = ptr2->next;  // 后节点
		float temp =  -sqrt(pow(ptr1->x - ptr2->x, 2) + pow(ptr1->y - ptr2->y, 2)) - 
			sqrt(pow(ptr2->x - ptr3->x, 2) + pow(ptr2->y - ptr3->y, 2)) +
			sqrt(pow(ptr1->x - ptr3->x, 2) + pow(ptr1->y - ptr3->y, 2));
		costArr.push_back(temp);
		ptr1 = ptr1->next;
	}
	return costArr;
} 

bool Route::timeWindowJudge(Customer *pre, int pos, Customer item){  
	// 计算把item插入到pre后面是否会违反时间窗约束
	// 暂时不考虑仓库的关仓时间
	// pos是pre的位置, 0表示仓库
	float time = arrivedTime[pos];
	Customer *ptr1, *ptr2;

	// 接下来是判断插入item后会不会违反item或者其后继节点的时间窗约束
	if(time < pre->startTime){   // arrivedTime[pos]只加到了pre的arrived time，没有判断是否提前到达
		time = pre->startTime;
	}
	time += pre->serviceTime;
	time = time + sqrt(pow(pre->x - item.x, 2) + pow(pre->y - item.y, 2));
	if(time > item.endTime) {  // 违反了时间窗约束
		return false;
	} else{
		if(time < item.startTime) {
			time = item.startTime;
		}
		time = time + item.serviceTime;
		ptr2 = pre->next;
		if(ptr2 == rear){  // item后面的是终点，暂时不计算
			return true;
		} else {
			time = time + sqrt(pow(ptr2->x - item.x, 2) + pow(ptr2->y - item.y, 2));
			if(time > ptr2->endTime) {
				return false;
			} else {
				if(time < ptr2->startTime) {
					time = ptr2->startTime;
				}
				time = time + ptr2->serviceTime;
			}
		}
	}

	// 然后判断会不会违反更靠后的节点的时间窗约束
	bool mark = true;
	ptr1 = pre->next;
	ptr2 = ptr1->next;
	while(mark == true && ptr2 !=rear){ 
		time = time + sqrt(pow(ptr1->x - ptr2->x, 2) + pow(ptr1->y - ptr2->y, 2));	
		if(time > ptr2->endTime){
			mark = false;
			break;
		} else {
			if(time < ptr2->startTime){
				time = ptr2->startTime;
			}
			time = time + ptr2->serviceTime;
		}
		ptr1 = ptr1->next;
		ptr2 = ptr2->next;
	}
	return mark;
}

void Route::computeInsertCost(Customer item, float &minValue, Customer &customer1, float &secondValue, Customer &customer2,
							  float noiseAmount, bool noiseAdd){
	// 计算item节点在路径中的最小插入代价和次小插入代价
	// 返回其最佳/次佳插入点前面的顾客节点
	Customer *pre = head;
	Customer *succ = pre->next;
	minValue = MAX_FLOAT;
	secondValue = MAX_FLOAT;
	customer1.id = -1;
	customer2.id = -1;
	for(int i=0; i<=size; i++) {  // 一共有size+1个位置可以考虑插入
		switch(item.type){
		case 'L':
			if(pre->type == 'D' || pre->type == 'L') { // 优先级约束
				if(quantityL + item.quantity <= capacity){   // 容量约束
					if(timeWindowJudge(pre, i, item) == true) { // 满足时间窗约束
						float temp = sqrt(pow(pre->x - item.x, 2) + pow(pre->y - item.y, 2)) +
								sqrt(pow(item.x - succ->x, 2) + pow(item.y - succ->y, 2)) -
								sqrt(pow(pre->x - succ->x, 2) + pow(pre->y - succ->y, 2));
						if(noiseAdd == true) { // 如果需要添加随机噪声
							float y = rand()/(RAND_MAX+1.0f);   // y in (0,1)
							float noise = -noiseAmount + 2*noiseAmount*y;
							temp = max(temp+noise, 0.0f);
						}
						if(temp <= minValue){  // 找到了更小的，更新minValue和secondValue
							secondValue = minValue;
							customer2 = customer1;
							minValue = temp;
							customer1 = *pre;
							int aa = 0;
						}
					}
				}
			}
			break;
		case 'B':
			if((pre->type == 'L' && succ->type == 'B') || (pre->type == 'L' && succ->type == 'D') || (pre->type == 'B') == true){  
				// 优先级约束
				if(quantityB + item.quantity <= capacity){    // 容量约束
					if(timeWindowJudge(pre, i, item) == true) {  // 满足时间窗约束
						float temp = sqrt(pow(pre->x - item.x, 2) + pow(pre->y - item.y, 2)) +
								sqrt(pow(item.x - succ->x, 2) + pow(item.y - succ->y, 2)) -
								sqrt(pow(pre->x - succ->x, 2) + pow(pre->y - succ->y, 2));
						if(noiseAdd == true) { // 如果需要添加随机噪声
							float y = rand()/(RAND_MAX+1.0f);   // y in (0,1)
							float noise = -noiseAmount + 2*noiseAmount*y;
							temp = max(temp+noise, 0.0f);
						}
						if(temp <= minValue){  // 找到了更小的，更新minValue和secondValue
							secondValue = minValue;
							customer2 = customer1;
							minValue = temp;
							customer1 = *pre;
						}
					
					}
				}
			}
			break;
		}
		pre = pre->next;
		if(succ != rear){
			succ = succ->next;
		}
	}
}

void Route::refreshArrivedTime(){   
	// 更新一下各个节点的到达时刻
	// 第一个值为0
	arrivedTime.clear();
	arrivedTime.push_back(0);
	Customer* tfront = head;
	Customer* tcurrent = head->next;
	float time = 0;
	while(tcurrent != rear){
		time = time + sqrt(pow(tfront->x - tcurrent->x, 2) + pow(tfront->y - tcurrent->y, 2));
		arrivedTime.push_back(time);
		tcurrent->arrivedTime = time;
		if(time < tcurrent->startTime){
			time = tcurrent->startTime;
		}
		time = time + tcurrent->serviceTime;
		tfront = tfront->next;
		tcurrent = tcurrent->next;
	}
}

void Route::changeCarIndex(int newIndex){  // 更新车辆编号
	carIndex = newIndex;
}

float Route::getLen(){   // 得到路径长度
	Customer *ptr1 = head;
	Customer *ptr2 = head->next;
	float len = 0;
	while(ptr2 != NULL){
		len = len + sqrt(pow(ptr1->x - ptr2->x, 2)+pow(ptr1->y - ptr2->y, 2));
		ptr2 = ptr2->next;
		ptr1 = ptr1->next;
	}
	return len;
}

vector<float> Route::getArrivedTime(){     // 得到本车所有节点的arrivedTime
	return arrivedTime;
}