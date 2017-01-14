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

vector<int> getRandom(int lb, int ub, int m, vector<int> &restData){
	// 产生m个不同的，范围为[lb, ub)的随机数
	// restData, 除了返回值外剩余的数值
	restData.resize(0);
	for(int i=0; i<ub-lb; i++) {
		restData.push_back(i+lb);
	}
	int total = m;
	vector<int> outputData(0);
	for(int j=0; j<m; j++) {
		vector<int>::iterator iter = restData.begin();
		int num = rand() % total; // 0-total-1
		iter += num;
		int temp = *(iter);
		outputData.push_back(num);
		restData.erase(iter);
		total--;
	}
	return outputData;
}

int roulette(vector<float> probability) {
	// 轮盘算法
	// 随机选出一个整数k (from 0 to |probability|)。
	vector<float>::iterator floatIter;
	float sumation1 = accumulate(probability.begin(), probability.end(), 0); // 求和
	for(floatIter = probability.begin(); floatIter < probability.end(); floatIter++) {
		*floatIter /= sumation1;  // 归一化
	}
	int totalNum = probability.end() - probability.begin();  // 总数目
	int k = 0;
	float sumation = 0;
	float randFloat = rand()/(RAND_MAX + 1.0f);
	floatIter = probability.begin();
	while((sumation < randFloat) && (floatIter < probability.end())) {
		k++;
		sumation += *(floatIter++);
	}
	k = max(k-1, 0); // randFloat = 0 时 k-1 < 0
	return k;
}
#endif