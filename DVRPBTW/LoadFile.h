#ifndef _LOADFILES_H
#define _LOADFILES_H

#include "Customer.h"
#include<string>
#include<vector>

using namespace std;

class LoadFile{  // 读取xml文件的类
public:
	LoadFile(string filePath, string fileName, int customerNum):filePath(filePath), fileName(fileName), customerNum(customerNum){};
	~LoadFile(){};
	bool getData(vector<Customer*> &allCustomer, Customer &depot, float &capacity);
private:
	string filePath;
	string fileName;
	int customerNum;
};

#endif