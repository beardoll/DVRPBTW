% 读取xml文件
fileName = 'RC103_100.xml';
filePath = 'C:\Users\cfinsbear\Documents\DVRPBTW\DVRPBTW\ALNS_Result\';
try
    xDoc = xmlread([filePath, fileName]);
catch
    error('Failed to read XML file %s', infilename);
end

routeLen = xDoc.getElementsByTagName('RouteLen');  
% 路径长度
% 返回的是一个elementList, 即使只有一个节点，也需要routeLen.item(0)来获取该节点

routeSet = [];

allRoute = xDoc.getElementsByTagName('Route');
for i = 0:allRoute.getLength-1
    thisRoute = allRoute.item(i);
    routeNode.id = str2num(char(thisRoute.getAttribute('index')));
    routeNode.nodeSet = [];
    Node = thisRoute.getFirstChild;
    while ~isempty(Node)
        if Node.getNodeType == Node.ELEMENT_NODE
            % Assume that each element has a single org.w3c.dom.Text child
            % 有的childnode不是dom类型的
            % xmlread会认为空格也算是一个节点，因此需要进行判断
            nodeElem.id = str2num(char(Node.getAttribute('id')));
            nodeElem.type = char(Node.getAttribute('type'));
            coordinate = Node.getFirstChild;
            while ~isempty(coordinate)
                % 读取x, y坐标，用childNode的方式读取
                if coordinate.getNodeType == coordinate.ELEMENT_NODE
                    s = char(coordinate.getFirstChild.getData);
                    switch char(coordinate.getTagName)
                        case 'cx'
                            nodeElem.cx = str2num(s);
                        case 'cy'
                            nodeElem.cy = str2num(s);
                    end
                end
                coordinate = coordinate.getNextSibling;
            end
            routeNode.nodeSet = [routeNode.nodeSet, nodeElem];
        end
        Node = Node.getNextSibling;
    end
    routeSet = [routeSet, routeNode];
end

len = str2num(char(routeLen.item(0).getFirstChild.getData));

drawRoute(routeSet);
