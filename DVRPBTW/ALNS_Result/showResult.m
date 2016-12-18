% 读取xml文件
fileName = 'RC103_100.xml';
filePath = 'C:\Users\cfinsbear\Documents\DVRPBTW\DVRPBTW\ALNS_Result\';
try
    xDoc = xmlread([filePath, fileName]);
catch
    error('Failed to read XML file %s', infilename);
end

Node = xDoc.getElementsByTagName('node');          % 节点
routeLen = xDoc.getElementsByTagName('RouteLen');  % 路径长度

routeSet = [];

allRoute = xDoc.getElementsByTagName('Route');
for i = 0:allRoute.getLength-1
    thisRoute = allRoute.item(i);
    routeNode.id = thisRoute.getAttribute('index');
    routeNode.nodeSet = [];
    Node = thisRoute.getFirstChild;
    while ~isempty(Node)
        nodeElem.id = Node.getAttribute('id');
        cx = Node.getFirstChild;
        data = char(cx.getFirstChild.getData);
        nodeElem.cx = str2num(data);
        cy = cx.getNextSibling;
        data = char(cy.getFirstChild.getData)
        nodeElem.cy = str2num(data);
        routeNode.nodeSet = [routeNode.nodeSet, nodeElem];
    end
    routeSet = [routeSet, routeNode];
end

data = char(Node.getFirstChild.getData);
len = str2num(data);
% 读取货车容量
data = char(capacityNode.item(0).getFirstChild.getData);
capacity = str2num(data);