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
	void insertAtFront(Customer &item);   // 在表头插入item
	bool deleteNode(Customer &item);   // 在链表中删除与item相同的节点
	bool next();  // current指针往前走，若返回false，表示已经走到头
	void printRoute();   // 打印路径
	void clear();     // 清空链表
	void assignCar(int carIndex); // 为route分配车辆
	void replaceRoute(const Route &route);  // 以route替换掉current指针后的路径
	Route& capture();  // 抓取current指针后的路径
	Customer& currentPos();   // 返回当前位置
	int getSize();     // 得到当前链表的大小
	vector<Customer*> getAllCustomer();  // 得到路径中所有的顾客节点
	vector<float> computeReducedCost();  // 计算所有节点的移除代价
	void computeInsertCost(Customer item, float &minValue, Customer &customer1, float &secondValue, Customer &customer2, 
		float noiseAmount, bool noiseAdd);  
	// 计算item节点在路径中的最小插入代价和次小插入代价
	// 返回其最佳/次佳插入点前面的顾客节点
	bool timeWindowJudge(Customer *pre, int pos, Customer item);
	// 计算把item插入到pre后面是否会违反时间窗约束
	void refreshArrivedTime();   // 更新一下各个节点的到达时刻
	void changeCarIndex(int newIndex);  // 更新车辆编号
	float getLen();   // 得到路径长度
	vector<float> getArrivedTime();     // 得到本车所有节点的arrivedTime，注意第一个元素是0
private:
	Customer *head, *current, *rear;  // 表头，表尾和当前指针，当前指针指向货车当前的驻地
	int size;         // 链表的长度
	int carIndex;     // 所属货车
	float quantityL, quantityB;  // linehaul和backhaul节点分别的重量
	float capacity;   // 车载量，在这里也存一下
	void copy(const Route& L);  // 复制链表，供复制构造函数和重载“=”函数使用
	vector<float> arrivedTime;  // 在各个节点的到达时刻
};


#endif