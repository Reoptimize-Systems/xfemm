function plotarclinks(nodes, links, angles, maxdeg, varargin)
% plots a set of nodes and arc links between them
%
% Syntax
%
% plotarclinks(nodes, links, angles, maxdeg)
%
% Input
%
% nodes - (n x 2) or (n x 3) set of coordinates of node locations to be
%   plotted
%
% links - (n x 2) set of links between nodes. these are a set of integers
%  which index the nodes in the list.  
%
% angles - 
%
% maxdeg - 
%
%

% Copyright 2012 Richard Crozier
% 
%    Licensed under the Apache License, Version 2.0 (the "License");
%    you may not use this file except in compliance with the License.
%    You may obtain a copy of the License at
% 
%        http://www.apache.org/licenses/LICENSE-2.0
% 
%    Unless required by applicable law or agreed to in writing, software
%    distributed under the License is distributed on an "AS IS" BASIS,
%    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%    See the License for the specific language governing permissions and
%    limitations under the License.

%     warning('Plotting arc segments not yet supported, they will be ignored');
    
    Inputs.ZeroBased = true;
    
    Inputs = parse_pv_pairs(Inputs, varargin);
    
    if Inputs.ZeroBased
        links = links + 1;
    end
%     
%     if size(nodes, 2) == 2
%         is2d = true;
%         nodes(:,3) = 0;
%     elseif size(nodes, 2) == 3
%         is2d = false;
%     else
%         error('Nodes matrix must be a (n x 2) or (n x 3) set of coordinates.')
%     end
%     
    hold all
    
    for i = 1:size(links, 1)
        
        [x, y] = arcpoints(nodes(links(i,1),:), nodes(links(i,2),:), angles(i), maxdeg(i));
        
        line(x, y);
        
    end
    
    hold off
    
    hold on

    % plot all the nodes
    if isoctave
        scatter3(nodes(:,1), nodes(:,2), zeros(size(nodes(:,2))), [], [], 'xr');
    else
        scatter3(nodes(:,1), nodes(:,2), zeros(size(nodes(:,2))), 'xr');
    end
%     
%     if ~is2d
%         view(3);
%     end
%     
    hold off
    
    axis equal
%     
%     Xlim = get(gca, 'Xlim');
%     Ylim = get(gca, 'Ylim');
%     
%     set(gca, 'Xlim', Xlim + [-0.1, 0.1]*(max(nodes(:,1)) - min(nodes(:,1))) )
%     
%     set(gca, 'Ylim', Ylim + [-0.1, 0.1]*(max(nodes(:,2)) - min(nodes(:,2))) )

end

function [x, y] = arcpoints(A, B, angle, maxdeg)
% get the points on an arc for plotting

    % convert the angles to radians
    angle = deg2rad(angle);
    maxdeg = deg2rad(maxdeg);

    % get centre and radius of circles
    [centre, r] = circcentre(A, B, angle);
    
    % get starting angle of arcs
    tempA = A - centre;
    
    [starttheta, rho] = cart2pol(tempA(:,1), tempA(:,2));
    
    % get arc points
    npnts = max(3, ceil(angle ./ maxdeg));
    
    pnts = linspace(starttheta, starttheta + angle, npnts);
    
    [x, y] = pol2cart(pnts, repmat(rho, size(pnts)));
    
    x = x + centre(:,1);
    y = y + centre(:,2);
    
    
end


function [centre, r] = circcentre(A, B, angle)
% calculates the centre and radius of a circle given two points and an arc
% angle between them. The position of the circle is determined by the
% order of the supplied points.
%
% Syntax
%
% [centre, r] = circcentre(A, B, angle)
%
%
    
    % get vector pointing from A to B
    AB = B - A;
    
    % find perpendicular vector to AB
    V = [ -AB(:,2), AB(:,1) ];
    
    % find mid point of AB
    M = A + AB .* 0.5;
    
    % find length of AB and divide by two to get triangle base
    b = 0.5 * magn(AB);
    
    % find triangle height
    h = b ./ tan(angle ./ 2);
    
    % find circle centre
    centre = M + h * unit(V);
    
    % find radius
    r = sqrt(h.^2 + b.^2);
    
end


