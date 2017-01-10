#ifndef _PUBLICFUNCTION_H
#define _PUBLICFUNCTION_H
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<vector>
#include<numeric>

using namespace std;

float random(float start, float end){
	// 产生start到end之间的随机数
	return start+(end-start)*rand()/(RAND_MAX+1.0);
}

template<class T1, class T2>
bool ascendSort(pair<T1, T2> x, pair<T1, T2> y){   
	// 递增排序
	// 第二个元素包含该元素在原序列中的位置
	return x.first < y.first;
}

template<class T1, class T2>
bool descendSort(pair<T1, T2> x, pair<T1, T2> y){
	// 递减排序
	// 第二个元素包含该元素在原序列中的位置
	return x.first > y.second;
}

vector<float> randomVec(int num){  // 产生num个随机数，它们的和为1
	float rest = 1;  // 初始余量为1
	vector<float> output(0);
	for(int i=0; i<num; i++) {
		float temp = random(0, rest); // 产生随机数
		output.push_back(temp);
		rest -= temp;
	}
	return output;
}
#endif