#include "Car.h"

Car::Car(Customer &headNode, Customer &rearNode, float capacity, int index, bool artificial):
	carIndex(index), route(headNode, rearNode, capacity), artificial(artificial){
	state = wait;
}

Car::~Car(){  // 内嵌对象的析构函数会被调用，不用在此处delete route

}

Car::Car(Car &item):route(item.route), artificial(item.artificial){  //复制构造函数
	this->state = item.state;
	this->carIndex = item.carIndex;
}


Car& Car::operator= (Car &item){ // 重载赋值操作
	this->route = item.route;
	this->carIndex = item.carIndex;
	this->state = item.state;
	this->artificial = item.artificial;
	return *this;
}

Route& Car::getRoute(){    // 得到本车路径
	return this->route;
}

void Car::updateRoute(Route &L){       // 更新current指针后的路径
	route.replaceRoute(L);
}

void Car::changeCarIndex(int newIndex){
	carIndex = newIndex;
	getRoute().changeCarIndex(newIndex);
}