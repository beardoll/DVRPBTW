#include "Route.h"
#include<vector>
#ifndef _CAR_H
#define _CAR_H

enum State{wait, depature, arrived, startservice};

class Car{
public:
	Car(Customer &headNode, Customer &rearNode, float capacity, int index, bool artificial = true);  // 构造函数
	~Car();          // 析构函数
	Car(Car &item);  //复制构造函数
	Car& operator= (Car &item);       // 重载赋值操作
	void updateState(char newState);  // 更新状态
	char getState();                  // 得到当前的状态
	void updateRoute(Route &L);       // 更新current指针后的路径
	void moveForword();         // 向前进
	void getLeftCapacity();     // 得到剩余的车容量
	void getServedCustomers();  // 得到已经服务过的顾客点集
	Route& getRoute();    // 得到本车路径
	void changeCarIndex(int newIndex);    // 更改车辆编号
	int getCarIndex() {return carIndex;}  // 得到车辆编号
	bool judgeArtificial() {return artificial;} // 返回车辆性质
private:
	State state;    // 货车状态（服务？行进？等待？）
	Route route;    // 计划要走的路径
	bool artificial;  // 为true表示是虚构的车辆，false表示真实的车辆
	int carIndex;     // 货车编号
};

#endif