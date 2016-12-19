#include "LoadFile.h"
#include "tinystr.h"
#include "tinyxml.h"

bool LoadFile::getData(vector<Customer*> &allCustomer, Customer &depot, float &capacity){
	// 读取xml内容于allCustomers, depot, capacity中
	string file = filePath + fileName;   // 文件路径+文件名
	TiXmlDocument doc(file.c_str());     // 读入XML文件
	if(!doc.LoadFile()) return false;    // 如果无法读取文件，则返回
	TiXmlHandle hDoc(&doc);         // hDoc是&doc指向的对象
	TiXmlElement* pElem;            // 指向元素的指针
	pElem = hDoc.FirstChildElement().Element(); //指向根节点
	TiXmlHandle hRoot(pElem);       // hRoot是根节点
	Customer* customer;
	int tempINT;
	float tempFLOAT;

	// 读取x,y，它们放在network->nodes->node节点中
	TiXmlElement* nodeElem = hRoot.FirstChild("network").FirstChild("nodes").FirstChild("node").Element(); //当前指向了node节点
	for(nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement()) { // 挨个读取node节点的信息
		customer = new Customer;
		TiXmlHandle node(nodeElem);  // nodeElem所指向的节点
		TiXmlElement* xElem = node.FirstChild("cx").Element();  // cx节点
		TiXmlElement* yElem = node.FirstChild("cy").Element();  // cy节点
		nodeElem->QueryIntAttribute("id", &tempINT);  //把id放到temp1中，属性值读法
		if(tempINT == 0){  // depot节点
			depot.id = tempINT;
			tempFLOAT = (float)atof(xElem->GetText());    // char转float
			depot.x = tempFLOAT;
			tempFLOAT = (float)atof(yElem->GetText());
			depot.y = tempFLOAT;
			depot.type = 'D';
			depot.serviceTime = 0;
			depot.arrivedTime = 0;
			depot.startTime = 0;
		} else {
			customer = new Customer;
			customer->id = tempINT;        
			tempFLOAT = (float)atof(xElem->GetText());    // char转float
			customer->x = tempFLOAT;
			tempFLOAT = (float)atof(yElem->GetText());
			customer->y = tempFLOAT;
			customer->type = 'L';
			allCustomer.push_back(customer);
		}
	}

	 // 读取其余信息
	TiXmlElement* requestElem = hRoot.FirstChild("requests").FirstChild("request").Element(); // 指向了request节点
	int count = 0;
	for(requestElem; requestElem; requestElem = requestElem->NextSiblingElement()) {
		customer = allCustomer[count];     // 当前顾客节点，注意不能赋值给一个新的对象，否则会调用复制构造函数
		TiXmlHandle request(requestElem);  // 指针指向的对象
		TiXmlElement* startTimeElem = request.FirstChild("tw").FirstChild("start").Element(); // start time
		TiXmlElement* endTimeElem = request.FirstChild("tw").FirstChild("end").Element();     // end time
		TiXmlElement* quantityElem = request.FirstChild("quantity").Element();                // quantity
		TiXmlElement* serviceTimeElem = request.FirstChild("service_time").Element();         // service time
		// 分别读取各项数据
		tempFLOAT = (float)atof(startTimeElem->GetText());
		customer->startTime = tempFLOAT;  
		tempFLOAT = (float)atof(endTimeElem->GetText());
		customer->endTime = tempFLOAT;
		tempFLOAT = (float)atof(quantityElem->GetText());
		customer->quantity = tempFLOAT;
		tempFLOAT = (float)atof(serviceTimeElem->GetText());
		customer->serviceTime = tempFLOAT;
		count++;
	}

	// 读取capacity
	TiXmlElement* capacityElem = hRoot.FirstChild("fleet").FirstChild("vehicle_profile").FirstChild("capacity").Element();
	tempFLOAT = (float)atof(capacityElem->GetText());
	capacity = tempFLOAT;
	return true;
}