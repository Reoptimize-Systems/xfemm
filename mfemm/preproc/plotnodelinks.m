function plotnodelinks(nodes, links, varargin)
% plots a set of nodes and links between them
%
% Syntax
%
% plotnodelinks(nodes, links)
%
% Input
%
% nodes - (n x 2) or (n x 3) set of coordinates of node locations to be
%   plotted
%
% links - (n x 2) set of links between nodes. these are a set of integers
%  which index the nodes in the list.  
%

    Inputs.ZeroBased = true;
    
    Inputs = parse_pv_pairs(Inputs, varargin);
    
    if Inputs.ZeroBased
        links = links + 1;
    end
    
    if size(nodes, 2) == 2
        is2d = true;
        nodes(:,3) = 0;
    elseif size(nodes, 2) == 3
        is2d = false;
    else
        error('Nodes matrix must be a (n x 2) or (n x 3) set of coordinates.')
    end
    
    hold all
    
    for i = 1:size(links, 1)
        line([nodes(links(i,1),1), nodes(links(i,2),1)], [nodes(links(i,1),2), nodes(links(i,2),2)], [nodes(links(i,1),3), nodes(links(i,2),3)]);
    end
    
    hold off
    
    hold on
    
    % plot all the nodes too
    scatter3(nodes(:,1), nodes(:,2), nodes(:,3), 'xr');
    
    if ~is2d
        view(3);
    end
    
    hold off
    
    Xlim = get(gca, 'Xlim');
    Ylim = get(gca, 'Ylim');
    
    set(gca, 'Xlim', Xlim + [-0.1, 0.1]*(max(nodes(:,1)) - min(nodes(:,1))) )
    
    set(gca, 'Ylim', Ylim + [-0.1, 0.1]*(max(nodes(:,2)) - min(nodes(:,2))) )

end