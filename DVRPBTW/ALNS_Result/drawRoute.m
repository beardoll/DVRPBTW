function [] = drawRoute(routeSet)
    % 画出路径
    coloroption = char('r-', 'b-', 'g-', 'm-', 'k-'); % 可选颜色
    for i = 1:length(routeSet)  % 逐条路径画出来
        route = routeSet(i);
        nodeSet = route.nodeSet;
        selectcolor = mod(i,5);
        if selectcolor == 0
            selectcolor = 1;
        end
        linecolor = coloroption(selectcolor);
        for nodeindex = 1: length(nodeSet) - 1
            plot([nodeSet(nodeindex).cx nodeSet(nodeindex+1).cx], [nodeSet(nodeindex).cy nodeSet(nodeindex+1).cy], linecolor, 'LineWidth', 2);
            axis([0 100 0 100])
            hold on;
            switch nodeSet(nodeindex).type
                case 'D'
                    plot(nodeSet(nodeindex).cx, nodeSet(nodeindex).cy, 'b*', 'MarkerSize', 8);
                    hold on;
                case 'L'
                    plot(nodeSet(nodeindex).cx, nodeSet(nodeindex).cy, 'go', 'MarkerSize', 8);
                    hold on;
                case 'B'
                    plot(nodeSet(nodeindex).cx, nodeSet(nodeindex).cy, 'bd', 'MarkerSize', 8);
                    hold on;
            end
        end 
    end
    hold off;
end