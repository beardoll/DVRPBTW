% 批量读取数据
list=dir(['C:\Users\beardollPC\Documents\DDVRPBTW\Solomon-Benchmark\solomon-1987-rc1\','*.xml']);
fileAmount = length(list);
for fileindex = 1:fileAmount
    % 读取xml文件
    infilename =  list(fileindex).name;
    % infilename = 'RC101_025.xml';   % 要读取文件名
    try
        xDoc = xmlread(infilename);
    catch
        error('Failed to read XML file %s', infilename);
    end
    cx = []; % 存放x坐标
    cy = []; % 存放y坐标
    start_time = []; % 存放时间窗开始时间
    end_time = [];   % 存放时间窗结束时间
    quantity = [];   % 存放货物需求量
    service_time = [];  % 存放服务时间

    allXcoordItems = xDoc.getElementsByTagName('cx');  % x坐标
    allYcoordItems = xDoc.getElementsByTagName('cy');  % y坐标
    capacityNode = xDoc.getElementsByTagName('capacity');  % 车容量
    max_travel_timeNode = xDoc.getElementsByTagName('max_travel_time');  % 车辆的最长运行时间
    allStarttimeItems = xDoc.getElementsByTagName('start');  % 时间窗的起始时间
    allEndtimeItems = xDoc.getElementsByTagName('end');  % 时间窗的终止时间
    allQuantityItems = xDoc.getElementsByTagName('quantity');  % 货物需求量
    allServiceTimeItems = xDoc.getElementsByTagName('service_time');  % 服务时间

    % 读取货车容量
    data = char(capacityNode.item(0).getFirstChild.getData);
    capacity = str2num(data);

    % 读取车辆的最长运行时间
    data = char(max_travel_timeNode.item(0).getFirstChild.getData);
    max_travel_time = str2num(data);

    % 读取x坐标
    for i = 0 : allXcoordItems.getLength - 1
        data = char(allXcoordItems.item(i).getFirstChild.getData);
        if i == 0  % 仓库坐标
            depotx = str2num(data);
        else
            cx = [cx, str2num(data)];
        end
    end

    % 读取y坐标
    for i = 0 : allYcoordItems.getLength - 1
        data = char(allYcoordItems.item(i).getFirstChild.getData);
        if i == 0  % 仓库坐标
            depoty = str2num(data);
        else
            cy = [cy, str2num(data)];
        end
    end

    % 读取时间窗开始时间
    for i = 0 : allStarttimeItems.getLength - 1
        data = char(allStarttimeItems.item(i).getFirstChild.getData);
        start_time = [start_time, str2num(data)];
    end

    % 读取时间窗结束时间
    for i = 0 : allEndtimeItems.getLength - 1
        data = char(allEndtimeItems.item(i).getFirstChild.getData);
        end_time = [end_time, str2num(data)];
    end

    % 读取顾客点货物需求量
    for i = 0 : allQuantityItems.getLength - 1
        data = char(allQuantityItems.item(i).getFirstChild.getData);
        quantity = [quantity, str2num(data)];
    end

    % 读取顾客点服务时间
    for i = 0 : allServiceTimeItems.getLength - 1
        data = char(allServiceTimeItems.item(i).getFirstChild.getData);
        service_time = [service_time, str2num(data)];
    end

    % 保存为.mat格式
    S = regexp(infilename, '\.', 'split');  % 去掉.xml后缀
    savefilename = char(S(1)); % 取出xml文件的名字作为mat文件的名称
    save(strcat('C:\Users\beardollPC\Documents\DDVRPBTW\Solomon-Benchmark\solomon-mat\', savefilename), 'cx', 'cy', 'depotx', 'depoty', 'start_time', 'end_time', 'quantity', 'service_time', 'capacity', 'max_travel_time');
end





%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 示例代码 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% allCoordinatesItems = xDoc.getElementsByTagName_r('node'); % 把坐标的数据给读取出来
% for i = 0 : allCoordinatesItems.getLength - 1 % 每个node挨个读取
%     thisItem = allCoordinatesItems(i);    % 当前node
%     childNode = thisItem.getFirstChild;   % node的第一个子节点（x坐标）
%     k = 0;  % 0表示x坐标，1表示y坐标
%     while ~empty(childNode)
%         if k == 0
%             cx = [cx childNode.getFirstChild.getData];
%         else
%             cy = [cy childNode.getFirstChild.getData];
%         end
%         k = k + 1;
%         childNode = childNode.getNextSibling;  % node的下一个子节点（y坐标）
%     end    
% end
% allRequestItems = xDoc.getElementsByTagName_r('request');  % 读取时间窗、需求信息
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


