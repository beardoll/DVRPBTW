#ifndef _MATRIX_H
#define _MATRIX_H
#include<cassert>
#include<limits>
#include<iostream>
using namespace std;


template <class T>
class Matrix{
public:
	Matrix(int rows=2, int cols=2);    // 构造函数
	~Matrix();   // 析构函数
	Matrix(const Matrix<T> &M);   // 复制构造函数
	Matrix<T>& operator= (const Matrix<T> &M);  // 重载等号运算符
	void resize(int newrows, int newcols);  // 重新定义矩阵大小 

	// 行操作
	T getMaxAtRow(int row, int &pos);  // 得到第row行的最大值，返回其位置pos以及值
	T getMinAtRow(int row, int &pos);  // 得到第row行的最小值，返回其位置pos以及值
	Matrix<T> getElemAtRows(int row1, int row2);   // 得到第row1-row2行元素
	void setMatrixAtRow(int row, T* elements);     // 设置第row行元素
	void addOneRow();  // 给矩阵增加一行
	void printMatrixAtRow(int row);

	// 列操作
	T getMaxAtCol(int col, int &pos);  // 得到第col列的最大值，返回其位置pos以及值
	T getMinAtCol(int col, int &pos);  // 得到第col列的最小值，返回其位置pos以及值
	Matrix<T> getElemAtCols(int col1, int col2);   // 得到第col1-col2列元素
	void setMatrixAtCol(int col, T* elements);     // 设置第col列元素
	void printMatrixAtCol(int col);

	// 矩阵操作
	void setMatrix(const T* values);  // 设置矩阵元素
	T getMaxValue(int &i, int &j); // 得到当前矩阵的最大元素及其位置（其中一个）
	T getMinValue(int &i, int &j); // 得到当前矩阵的最小元素及其位置（其中一个）
	Matrix<T> getElemAtRowsByCols(int row1, int row2, int col1, int col2);  // 得到row1-row2, col1-col2的子矩阵
	void setValue(int i, int j, const T& newValue); // 修改元素(i,j)为newValue
	T getElement(int i, int j);  // 得到元素(i,j)
	void printMatrix();
private:
	int rows, cols;    // 矩阵的大小
	T *elements; // 矩阵元素
}; // 矩阵类


//=================== 基础操作 ===================//
template <class T>
Matrix<T>::Matrix(int rows, int cols):rows(rows), cols(cols){   // 构造函数
	assert(rows>=0 && cols>=0);
	elements = new T[rows*cols];
}

template <class T>
Matrix<T>::~Matrix(){    // 析构函数
	delete [] elements;
}

template <class T>
Matrix<T>::Matrix(const Matrix<T> &M){    // 复制构造函数
	rows = M.rows;
	cols = M.cols;
	elements = new T[rows*cols];
	for(int i=0; i<rows*cols; i++){
		elements[i] = M.elements[i];
	}
}

template <class T>
Matrix<T>& Matrix<T>:: operator=(const Matrix<T> &M){   // 重载“=”运算符
	if (rows!=M.rows || cols!=M.cols){
		delete [] elements;
		rows = M.rows;
		cols = M.cols;
		elements = new T[rows*cols];
	}
	for(int i=0; i<rows*cols; i++){
		elements[i] = M.elements[i];
	}
	return *this;
}

template <class T>
void Matrix<T>::resize(int newrows, int newcols){  // 重新定义矩阵大小
	assert(newrows>0 && newcols>0);
	delete [] elements;
	elements = new T[newrows * newcols];
	rows = newrows;
	cols = newcols;
}


//=================== 行操作 ===================//
template <class T>
T Matrix<T>::getMaxAtRow(int row, int &pos){ // 得到第row行的最大值，返回其位置pos以及值
	assert(row>=0 && row<rows);
	pos = 0;
	T maxValue = -numeric_limits<T>::max();  // 定义最大值为无穷小
	for(int i=row*cols; i<(row+1)*cols; i++) {
		if(elements[i] > maxValue) {
			maxValue = elements[i];
			pos = i - row*cols;
		}
	}
	return maxValue;
}  

template <class T>
T Matrix<T>::getMinAtRow(int row, int &pos){ // 得到第row行的最小值，返回其位置pos以及值
	assert(row>=0 && row<rows);
	pos = 0;
	T minValue = numeric_limits<T>::max();  // 定义最小值为无穷大
	for(int i=row*cols; i<(row+1)*cols; i++) {
		if(elements[i] < minValue) {
			minValue = elements[i];
			pos = i - row*cols;
		}
	}
	return minValue;
}  

template <class T> 
Matrix<T> Matrix<T>::getElemAtRows(int row1, int row2){   // 得到第row1-row2行元素
	assert(row1>=0 && row1<rows && row2>=0 && row2<rows && row1<=row2);
	Matrix<T> tempMat(row2-row1+1, cols);
	for(int i=row1; i<=row2; i++){
		for(int j=0; j<=cols; j++){
			tempMat.setValue(i-row1, j, elements[i*cols+j]);
		}
	}
	return tempMat;
}

template <class T>
void Matrix<T>::setMatrixAtRow(int row, T* elements){     // 设置第row行元素
	for(int j=0; j<cols; j++){
		this->setValue(row, j, elements[j]);
	}
}

template <class T>
void Matrix<T>::printMatrixAtRow(int row){
	assert(row>=0 && row < rows);
	int startPoint = row * cols;
	int endPoint = (row+1) * cols;
	for(int i=startPoint; i<endPoint; i++) {
		cout << *elements[i] << endl;
	}
}

template <class T>
void Matrix<T>::addOneRow(){  
	// 给矩阵增加一行
	// 同时保留原来的数据
	T* newElements = new T[rows*cols + cols];
	for(int i=0; i<rows*cols; i++){
		newElements[i] = elements[i];
	}
	delete [] elements;
	elements = newElements;
	rows++;
}


//=================== 列操作 ===================//
template <class T>
T Matrix<T>::getMaxAtCol(int col, int &pos){ // 得到第col列的最大值，返回其位置pos以及值
	assert(col>=0 && cols<cols);
	T maxValue = -numeric_limits<T>::max();  // 定义最大值为无穷小
	pos = 0;
	int tpos = 0;
	for(int i=0; i<rows; i++){
		tpos = i*cols+col;    // 定位当前元素
		if(elements[tpos] > maxValue) {
			maxValue = elements[tpos];
			pos = i;
		}
	}
	return maxValue;
}

template <class T>
T Matrix<T>::getMinAtCol(int col, int &pos){ // 得到第col列的最小值，返回其位置pos以及值
	assert(col>=0 && col<cols);
	pos = 0;
	T minValue = numeric_limits<T>::max();  // 定义最小值为无穷大
	int tpos;
	for(int i=0; i<rows; i++){
		tpos = i*cols+col;    // 定位当前元素
		if(elements[tpos] < minValue) {
			minValue = elements[tpos];
			pos = i;
		}
	}
	return minValue;
}

template <class T>
Matrix<T> Matrix<T>::getElemAtCols(int col1, int col2){  // 得到第cols1-col2列元素
	assert(col1>=0 && col1<cols && col2>=0 && col2<cols && col1<=col2);
	Matrix<T> tempMat(rows, col2-col1+1);
	for(int i=0; i<rows; i++){
		for(int j=col1; j<=col2; j++){
			tempMat.setValue(i, j-col1, elements[i*cols+j]);
		}
	}
	return tempMat;
}

template <class T>
void Matrix<T>::setMatrixAtCol(int col, T* elements){     // 设置第col列元素
	for(int i=0; i<rows; i++){
		this->setValue(i, col, elements[i]);
	}
}

template <class T>
void Matrix<T>::printMatrixAtCol(int col){
	assert(col>=0 && col < cols);
	for(int i=0; i<rows; i++) {
		cout << getElement(i, col) << endl;
	}
}


//=================== 矩阵操作 ===================//
template <class T>
void Matrix<T>:: setMatrix(const T* values){
	for(int i=0; i<rows*cols; i++){
		elements[i] = values[i];
	}
}

template <class T>
void Matrix<T>::setValue(int i, int j, const T& value){
	assert(i>=0 && i<rows && j>=0 && j<cols);
	elements[i*cols+j] = value;

}

template <class T>
T Matrix<T>::getMaxValue(int &i, int &j){ // 得到当前矩阵的最大元素及其位置（其中一个）
	T maxValue = -numeric_limits<T>::max();  // 定义最大值为无穷小
	i = 0;
	j = 0;
	int pos;
	for(int k=0; k<rows; k++) {
		for(int l=0; l<cols; l++){
			pos = k*cols+l;
			if(elements[pos] > maxValue) {
				maxValue = elements[pos];
				i = k;
				j = l;
			}
		}
	}
	return maxValue;
}

template <class T>
T Matrix<T>::getMinValue(int &i, int &j){ // 得到当前矩阵的最小及其位置（其中一个）
	T minValue = numeric_limits<T>::max();  // 定义最小值为无穷大
	i = 0;
	j = 0;
	int pos;
	for(int k=0; k<rows; k++) {
		for(int l=0; l<cols; l++){
			pos = k*cols+l;
			if(elements[pos] < minValue) {
				minValue = elements[pos];
				i = k;
				j = l;
			}
		}
	}
	return minValue;
}

template <class T>
Matrix<T> Matrix<T>::getElemAtRowsByCols(int row1, int row2, int col1, int col2){  // 得到row1-row2, col1-col2的子矩阵
	assert(row1>=0 && row1<rows && row2>=0 && row2<rows && row1<=row2);
	assert(col1>=0 && col1<cols && col2>=0 && col2<cols && col1<=col2);
	Matrix<T> tempMat(row2-row1+1, col2-col1+1);
	for(int i=row1; i<=row2; i++){
		for(int j=col1; j<=col2; j++){
			tempMat.setValue(i-row1, j-col1, elements[i*cols+j]);
		}
	}
	return tempMat;
}

template <class T>
T Matrix<T>::getElement(int i, int j){  // 重载运算符()
	assert(i>=0 && i<rows && j>=0 && j<cols);
	T value = elements[i*cols+j];
	return value;
}

template <class T>
void Matrix<T>::printMatrix(){
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			cout << getElement(i,j) << '\t';
		}
		cout << endl;
	}
	cout << endl;
}

#endif