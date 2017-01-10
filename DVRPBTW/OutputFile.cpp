#include "OutputFile.h"
#include<sstream>

OutputFile::OutputFile(vector<Car*> carSet, string filePath, string fileName, Customer depot, float result):
	carSet(carSet), fileName(fileName), filePath(filePath), depot(depot), result(result){   // 构造函数

}

void OutputFile::exportData(){         // 主函数
	TiXmlDocument doc;
	TiXmlComment *comment;
	TiXmlDeclaration *dec1 = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(dec1);

	TiXmlElement *root = new TiXmlElement("Result");
	doc.LinkEndChild(root);

	// 添加注释
	comment = new TiXmlComment();
	string s = "ALNS results for dataset" + fileName;
	comment->SetValue(s.c_str());
	root->LinkEndChild(comment);

	TiXmlElement *routeset = new TiXmlElement("RouteSet");
	root->LinkEndChild(routeset);
	TiXmlElement *route;
	vector<Car*>::iterator iter1;
	TiXmlElement *node;
	ostringstream buffer;
	string ss;
	for(iter1 = carSet.begin(); iter1 < carSet.end(); iter1++){
		// 写入Route节点
		route = new TiXmlElement("Route");
		route->SetAttribute("index", (*iter1)->getCarIndex());
		vector<Customer*> tempcust= (*iter1)->getRoute().getAllCustomer();
		vector<Customer*>::iterator iter2 = tempcust.begin();
		Customer *newdepot1 = new Customer(depot);
		Customer *newdepot2 = new Customer(depot);
		tempcust.insert(iter2, newdepot1);
		tempcust.push_back(newdepot2);
		for(iter2 = tempcust.begin(); iter2 < tempcust.end(); iter2++){
			// 写入node节点
			TiXmlElement *cx = new TiXmlElement("cx");
			TiXmlElement *cy = new TiXmlElement("cy");
			node = new TiXmlElement("Node");
			node->SetAttribute("id", (*iter2)->id);
			switch((*iter2)->type){   // 属性值是string类型而非ASCII码类型
			case 'D':
				node->SetAttribute("type", "D");
				break;
			case 'P':
				node->SetAttribute("type", "P");
				break;
			}
			buffer << (*iter2)->x;
			ss = buffer.str();
			cx->LinkEndChild(new TiXmlText(ss.c_str()));
			buffer.str("");
			buffer << (*iter2)->y;
			ss = buffer.str();
			cy->LinkEndChild(new TiXmlText(ss.c_str()));
			buffer.str("");
			node->LinkEndChild(cx);
			node->LinkEndChild(cy);
			route->LinkEndChild(node);
		}
		routeset->LinkEndChild(route);
	}
	TiXmlElement *routelen = new TiXmlElement("RouteLen");
	buffer << result;
	ss = buffer.str();
	routelen->LinkEndChild(new TiXmlText(ss.c_str()));
	root->LinkEndChild(routelen);
	string file = filePath + fileName;
	doc.SaveFile(file.c_str());
}