#include<iostream>
#include<set>
#include<algorithm> 
#include<map>
#include<vector>
#include "LoadFile.h"
#include "OutputFile.h"
#include "ProbInfo.h"
#include "SetBench.h"
#include "Timer.h"

using namespace std;

static const string FILE_PATH = "C:/Users/cfinsbear/Documents/DVRPBTW/DVRPBTW/solomon-1987-rc1/";
static const string FILE_NAME = "RC103_100.xml";
static const string FILE_PATH2 = "C:/Users/cfinsbear/Documents/DVRPBTW/DVRPBTW/ALNS_Result/";
static const string FILE_NAME2 = FILE_NAME;
static const int NUM_OF_CUSTOMER = 100;
static const int timeSlotLen = 60;
static const int timeSlotNum = 4;


int main(){
	LoadFile lf(FILE_PATH, FILE_NAME, NUM_OF_CUSTOMER);
	vector<Customer*> allCustomer(0);
	Customer depot;
	float capacity = 0;
	lf.getData(allCustomer, depot, capacity);
	SetBench sb(allCustomer, timeSlotLen, timeSlotNum);
	vector<Customer*> staticCustomer, dynamicCustomer;
	sb.construct(staticCustomer, dynamicCustomer);
	Timer timer(staticCustomer, dynamicCustomer, timeSlotLen);
	timer.run();
	//vector<Car*> finalCarSet;
	//OutputFile of(finalCarSet, FILE_PATH2, FILE_NAME2, depot,finalCost);
	//of.exportData();
	system("pause");
	return 0;
}