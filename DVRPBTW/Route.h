#ifndef _ROUTE_H
#define _ROUTE_H

#include<iostream>
#include<vector>
#include "Customer.h"

using namespace std;


class Route{
public:
	Route(Customer &headNode, Customer &rearNode, float capacity);      // 构造函数
	~Route();   // 析构函数
	Route(const Route &L);  // 复制构造函数
	Route& operator= (const Route &L);  // 重载赋值运算符 
	Customer& operator[] (int k);     // 得到链表中的第k个元素
	const Customer& operator[] (int k) const; // 静态函数
	bool isEmpty();   // 判断链表是否为空
	bool insertAfter(Customer &item1, Customer &item2); // 在链表中与item1相同的节点后面插入节点item2
	void insertAtHead(Customer &item);   // 在表头插入item
	void insertAtRear(Customer &item);    // 在表尾插入item
	bool deleteNode(Customer &item);   // 在链表中删除与item相同的节点
	bool next();  // current指针往前走，若返回false，表示已经走到头
	void printRoute();   // 打印路径
	void clear();     // 清空链表
	void assignCar(int carIndex); // 为route分配车辆
	void replaceRoute(const Route &route);  // 以route替换掉current指针后的路径
	Route& capture();  // 抓取current指针后的路径
	Customer currentPos();   // 返回当前位置
	Customer nextPos() {return *(current->next);}      // 下一个节点的位置
	bool moveForward();      // 向前进
	int getSize();     // 得到当前链表的大小
	vector<Customer*> getAllCustomer();  // 得到路径中所有的顾客节点
	vector<float> computeReducedCost(float DTpara[], bool artificial = false);  // 计算所有节点的移除代价
	void computeInsertCost(Customer item, float &minValue, Customer &customer1, float &secondValue, Customer &customer2, 
		float noiseAmount = 0.0f, bool noiseAdd = false, float penaltyPara = 0.0f, bool adativeNoise = false);  
	// 计算item节点在路径中的最小插入代价和次小插入代价
	// 返回其最佳/次佳插入点前面的顾客节点
	// penaltyPara为惩罚系数，若不需要惩罚则penaltyPara = 0
	bool timeWindowJudge(Customer *pre, int pos, Customer item);
	// 计算把item插入到pre后面是否会违反时间窗约束
	void refreshArrivedTime();   // 更新一下各个节点的到达时刻
	void changeCarIndex(int newIndex);  // 更新车辆编号
	float getLen(float DTpara[], bool artificial = false);   // 得到路径长度
	Customer getHeadNode();    // 得到头结点
	Customer getRearNode();    // 得到尾节点
	vector<float> getArrivedTime();     // 得到本车所有节点的arrivedTime，注意第一个元素是0
	float getQuantity() {return this->quantity;}       // 得到本车已使用的容量(装载顾客)
	float getLeftQuantity() {return this->leftQuantity;}    // 得到本车剩余容量（与车辆行驶情况相关）
	float getCapacity() {return this->capacity;}       // 返回本车的车载量
	void decreaseLeftQuantity(float amount) {leftQuantity -= amount;}   // 减少剩余的车容量（服务了新的顾客）
private:
	Customer *head, *current, *rear;  // 表头，表尾和当前指针，当前指针指向货车当前的驻地
	int size;         // 链表的长度
	int carIndex;     // 所属货车
	float quantity;   // 当前已使用的容量(与装载的顾客数量有关)
	float leftQuantity;   // 剩余容量（与车辆行驶情况有关）
	float capacity;       // 车载量，在这里也存一下
	void copy(const Route& L);  // 复制链表，供复制构造函数和重载“=”函数使用
	vector<float> arrivedTime;  // 在各个节点的到达时刻
};


#endif