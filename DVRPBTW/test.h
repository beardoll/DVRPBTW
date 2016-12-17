////////////////// test Matrix.h /////////////////////
//Matrix<int> intM(3,3);
//intM.resize(3,4);
//int a[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
//intM.setMatrix(a);
//intM.printMatrix();
//int b[3] = {11, 13, 15};
//intM.setMatrixAtCol(1, b);
//intM.printMatrix();
//int k;
//int maxvalue = intM.getMaxAtRow(1,k);
//cout << "maxvalue:" << maxvalue << ' ' << "pos:" << k << endl;
//Matrix<int> temp = intM.getElemAtRowsByCols(2,3,2,3);
//temp.printMatrix();
//int l1,l2;
//int temp1 = intM.getMaxValue(l1,l2);
//cout << "max:" << temp1 << ' ' << "pos:" << l1 << ' ' << l2 << endl;
//cout << "getElements" << endl;
//cout << intM.getElement(2,4) << endl;
////////////////////////////////////////////////////////

/////////////////// test Route.h ///////////////////////
//Route r1;
//Customer c1, c2, c3, c4;
//c1.id = 1;
//c1.type = 'D';
//c2.id = 2;
//c2.type = 'L';
//c3.id = 3;
//c3.type = 'B';
//r1.insertAtFront(c1);
//r1.insertAfter(c1, c3);
//r1.insertAtFront(c2);
//r1.printRoute();
//cout << r1.getSize() << endl;
//cout << "..." << endl;
//Route r2(r1);
////r1.deleteNode(c3);
////r1.printRoute();
////cout << r1.getSize() << endl;
////cout << "..." << endl;
//Customer ctemp= r1[1];
//cout << "id:" << ctemp.id << ' ' << "type:" << ' ' << ctemp.type << endl;
//cout << "..." << endl;
//r1.replaceRoute(r2);
//r1.printRoute();
//cout << r1.getSize() << endl;
//cout << "..." << endl;
//Route r3 = r1.capture();
//r3.printRoute();
//cout << r3.getSize() << endl;
////////////////////////////////////////////////////////

///////////////////移除节点后计算对其邻节点移除代价造成的影响///////////////////
//bool Route::deleteNode(Customer &item, float &frontRemoveCost, float &nextRemoveCost){
//	// 删除链表中与item相同的节点
//	// 然后返回删除后其前后节点的remove cost
//	Customer* temp1 = head->next;
//	while(temp1!=rear) {
//		if(temp1->id == item.id) {  // 如果找到，temp1指向当前节点，temp2->next=temp1;
//			break;
//		}
//		temp1 = temp1->next;
//	}
//	if(temp1 == rear) {  // 没有找到, frontRemoveCost和nextRemoveCost都没有意义
//		frontRemoveCost = 0;
//		nextRemoveCost = 0;
//		return false;
//	}
//	if(size == 1) {  // 路径中只有一个节点，那么frontRemoveCost和nextRemoveCost都没有意义
//		head->next = rear;
//		rear->front = head;
//		frontRemoveCost = 0;
//		nextRemoveCost = 0;
//	} else {
//		if(temp1 == head->next) {  // 如果是头结点
//			frontRemoveCost = 0;
//			Customer* nextNode = temp1->next;
//			Customer* nnextNode = nextNode->next;
//			nextRemoveCost = sqrt(pow(head->x - nextNode->x, 2) + pow(head->y - nextNode->y, 2)) + 
//				sqrt(pow(nextNode->x - nnextNode->x, 2) + pow(nextNode->y - nnextNode->y, 2)) -
//				sqrt(pow(head->x - nnextNode->x, 2) + pow(head->y - nnextNode->y, 2));
//			head->next = nextNode;
//			nextNode->front = head;
//		} else if(temp1 == rear->front) {  // 如果是尾节点
//			nextRemoveCost = 0;
//			Customer* frontNode = temp1->front;
//			Customer* ffrontNode = frontNode->front;
//			frontRemoveCost = sqrt(pow(rear->x - frontNode->x, 2) + pow(rear->y - frontNode->y, 2)) + 
//				sqrt(pow(frontNode->x - ffrontNode->x, 2) + pow(frontNode->y - ffrontNode->y, 2)) -
//				sqrt(pow(rear->x - ffrontNode->x, 2) + pow(rear->y - ffrontNode->y, 2));
//			rear->front = frontNode;
//			frontNode->next = rear;
//		} else{
//			Customer* nextNode = temp1->next;
//			Customer* nnextNode = nextNode->next;
//			Customer* frontNode = temp1->front;
//			Customer* ffrontNode = frontNode->front;
//			frontRemoveCost = sqrt(pow(temp1->x - frontNode->x, 2) + pow(temp1->y - frontNode->y, 2)) + 
//				sqrt(pow(frontNode->x - ffrontNode->x, 2) + pow(frontNode->y - ffrontNode->y, 2)) -
//				sqrt(pow(temp1->x - ffrontNode->x, 2) + pow(temp1->y - ffrontNode->y, 2));
//			nextRemoveCost = sqrt(pow(temp1->x - nextNode->x, 2) + pow(temp1->y - nextNode->y, 2)) + 
//				sqrt(pow(nextNode->x - nnextNode->x, 2) + pow(nextNode->y - nnextNode->y, 2)) -
//				sqrt(pow(temp1->x - nnextNode->x, 2) + pow(temp1->y - nnextNode->y, 2));
//			frontNode->next = nextNode;
//			nextNode->front = frontNode;
//		}
//	}
//	delete temp1;
//	size--;
//	return true;
//}
///////////////////////////////////////////////////////////////////////////////