#ifndef _TXTRECORDER_H
#define _TXTRECORDER_H

#include<iostream>
#include<sstream>
#include<string>
#include<fstream>

using namespace std;

class TxtRecorder{
public:
	//FileOperator(string path) {
	//	outfile = ofstream(path, ofstream::app);
	//}
	static ofstream outfile;
	static void addLine(string str) {
		outfile << str;
	}
	static void closeFile(){
		outfile.close();
	}
};

#endif