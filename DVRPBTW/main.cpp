#include<iostream>
#include<set>
#include<algorithm> 
#include<map>
#include<vector>
#include "LoadFile.h"
#include "Algorithm.h"
#include "OutputFile.h"

using namespace std;

static const string FILE_PATH = "C:/Users/cfinsbear/Documents/DVRPBTW/DVRPBTW/solomon-1987-rc1/";
static const string FILE_NAME = "RC103_100.xml";
static const string FILE_PATH2 = "C:/Users/cfinsbear/Documents/DVRPBTW/DVRPBTW/ALNS_Result/";
static const string FILE_NAME2 = "RC103_100.xml";
static const int NUM_OF_CUSTOMER = 100;

int main(){
	LoadFile lf(FILE_PATH, FILE_NAME, NUM_OF_CUSTOMER);
	vector<Customer*> allCustomer(0);
	Customer depot;
	float capacity = 0;
	lf.getData(allCustomer, depot, capacity);
	Algorithm ag(allCustomer, depot, capacity);
	vector<Car*> finalCarSet(0);
	float finalCost=0;
	ag.run(finalCarSet, finalCost);
	OutputFile of(finalCarSet, FILE_PATH2, FILE_NAME2, depot,finalCost);
	of.exportData();
	system("pause");
	return 0;
}