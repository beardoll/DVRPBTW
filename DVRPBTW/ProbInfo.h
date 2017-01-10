#ifndef _PROBINFO_H
#define _PROBINFO_H

#include "Matrix.h"

struct ProbInfo{
	// 顾客未知信息
	Matrix<float> timeProb;    // 顾客在各时间段提出需求的概率
};


#endif