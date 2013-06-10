function hfig = plotfemmproblem(FemmProblem)
% plotfemmproblem: plots a simple visualisation of an mfemm problem
% structure
% 
% Syntax
%
% hfig = plotfemmproblem(FemmProblem)
%
% Input
%
%  FemmProblem - mfemm problem structure
%
% Output
%
%  hfig - hadle to the created figure
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

    hfig = figure;
    
    nodes = getnodecoords_mfemm(FemmProblem);
    
    links = getnodelinks_mfemm(FemmProblem);
    
    % plot the segments as lines
    plotnodelinks(nodes, links);
    
    arclinks = getarclinks_mfemm(FemmProblem);

    % plot the arc segments as lines 
    hold all
    plotarclinks(nodes, arclinks(:,1:2), arclinks(:,3), arclinks(:,4));
    hold off
    
    % get the extent of the problem region
    [x,y,w,h] = extent_mfemm(FemmProblem);
    
    minlabelrad = 0.025 * magn([w,h]);
    
    maxlabelrad = 4 * minlabelrad;
    
    labelradrange = maxlabelrad - minlabelrad;
    
    if isfield(FemmProblem, 'BlockLabels')
        
        hold on
        
        maxtriarea = max(cell2mat({FemmProblem.BlockLabels(:).MaxArea}'));
    
        for i = 1:numel(FemmProblem.BlockLabels)

            if FemmProblem.BlockLabels(i).MaxArea == -1

                labeld = minlabelrad;

                labelcolour = 'm';

            else

                labeld = minlabelrad + labelradrange * (FemmProblem.BlockLabels(i).MaxArea / maxtriarea);

                labelcolour = 'k';

            end

            rectpos = [FemmProblem.BlockLabels(i).Coords(1) - labeld/2, ...
                       FemmProblem.BlockLabels(i).Coords(2) - labeld/2, ...
                       labeld, ...
                       labeld];

            rectangle('Position', rectpos, 'Curvature', [1,1], 'EdgeColor', labelcolour);

            plotcross(FemmProblem.BlockLabels(i).Coords(1), ...
                      FemmProblem.BlockLabels(i).Coords(2), ...
                      labeld,labeld, 'Color', labelcolour)

            % Draw the label a little to the right and above the 
            text(FemmProblem.BlockLabels(i).Coords(1) + 1.025*labeld, ...
                 FemmProblem.BlockLabels(i).Coords(2) + 1.025*labeld, ...
                 FemmProblem.BlockLabels(i).BlockType, ...
                 'Color', [0 0 0.5]);

        end

        hold off
        
    end
    
%     hold all
    
    % plot the mesh if it's present, would be nice to cycle through colours
    % for regions here
    if isfield(FemmProblem, 'Mesh')

        triplot(FemmProblem.Mesh.Triangles(:,1:3) + 1, ...
            FemmProblem.Mesh.Vertices(:,1), ...
            FemmProblem.Mesh.Vertices(:,2));

    end
    
%     hold off
    
    axis equal
    
%     addlistener(cah, 'YLim', 'PreSet', @mfemm_plot_chngylim);
%     addlistener(cah, 'YLim', 'PreSet', @mfemm_plot_chngxlim);

end


function plotcross(x,y,w,h,varargin)
% plotcross: plots a cross of a given height and width

    line([x - w/2, ...
          x + w/2], ...
         [y - h/2, ...
          y + h/2], varargin{:});

    line([x + w/2, ...
          x - w/2], ...
         [y - h/2, ...
          y + h/2], varargin{:});

end

    
    
    