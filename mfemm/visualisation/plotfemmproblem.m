function [hfig, hax] = plotfemmproblem(FemmProblem, varargin)
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
%  hfig - handle to the created figure
%
%  hax - handle to the created axes

% Copyright 2012-2014 Richard Crozier
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


    options.FigureHandle = [];
    options.LabelText = {'Color', [0 0 0.5]};
    options.AddLabels = true;
    options.PlotNodes = true;
    options.InitialViewPort = [];
    
    options = parseoptions (options, varargin);
    
    if isempty (options.FigureHandle)
        hfig = figure;
    else
        hfig = options.FigureHandle;
    end
    
    hax = axes;
    
    axis equal
    
    % store the FemmProblem in the  axes user data
    set(hax, 'UserData', FemmProblem);
    
    if ~isoctave
        hZoom = zoom(gcf);
        set(hZoom, 'ActionPostCallback', {@zoomfemmplot,hax,options});
    end
    
    if ~isempty (options.InitialViewPort)
        x = options.InitialViewPort(1);
        y = options.InitialViewPort(2);
        w = options.InitialViewPort(3);
        h = options.InitialViewPort(4);
        makefemmplot(hax,options,w,h);
        set(hax, 'XLim', [x, x+w]);
        set(hax, 'YLim', [y, y+h]);
    else
        makefemmplot(hax,options);
    end
   
end

function zoomfemmplot(fig,evd,hax,options)
% makes a plot after a zoom operation, used to keep block labels the same
% size at all zoom levels

    xlim = get(hax, 'XLim');

    ylim = get(hax, 'YLim');
    
    x = xlim(1);
    y = ylim(1);
    w = xlim(2) - xlim(1);
    h = ylim(2) - ylim(1);

%     cla(hax);
    
    % string all the old mfemm stuff from the plot, it will be tagged in
    % the UserData of the plot object
    plothandles = get (hax, 'Children');
    
    for ind = 1:numel (plothandles)
        ud = get (plothandles(ind), 'UserData');
        
        if ischar (ud) && strcmp (ud, 'mfemm')
            delete (plothandles(ind));
        end
    end
    
    makefemmplot(hax,options,w,h)
    
    set(hax, 'XLim', xlim);
    
    set(hax, 'YLim', ylim);

end


function makefemmplot(hax,options,w,h)
% creates the entire problem plot

    FemmProblem = get(hax, 'UserData');  
        
    if nargin == 2
        % get the extent of the problem region
        [~,~,w,h] = extent_mfemm(FemmProblem);
    end

    nodes = getnodecoords_mfemm(FemmProblem);

    links = getnodelinks_mfemm(FemmProblem);

    % plot the segments as lines
    plotnodelinks(nodes, links, 'PlotNodes', options.PlotNodes, 'UserData', 'mfemm');

    arclinks = getarclinks_mfemm(FemmProblem);

    % plot the arc segments as lines 
    hold all
    if ~isempty(arclinks)
        plotarclinks(nodes, arclinks(:,1:2), arclinks(:,3), arclinks(:,4), ...
            'PlotNodes', options.PlotNodes, 'UserData', 'mfemm');
    end
    hold off

    if ~isempty(FemmProblem.BlockLabels)
        maxtriarea = max(cell2mat({FemmProblem.BlockLabels(:).MaxArea}'));
    end
    
    if options.AddLabels
        
        for i = 1:numel(FemmProblem.BlockLabels)

            plotblocklabel(w, h, maxtriarea, FemmProblem.BlockLabels(i), options);

        end
    
    end
    
    % plot the mesh if it's present, would be nice to cycle through colours
    % for regions here
    hold all
    if isfield(FemmProblem, 'Mesh')

        triplot(FemmProblem.Mesh.Triangles(:,1:3) + 1, ...
            FemmProblem.Mesh.Vertices(:,1), ...
            FemmProblem.Mesh.Vertices(:,2));

    end
    hold off

end


function plotblocklabel(w, h, maxtriarea, BlockLabel, options)
% adds a single block label to the problem plot
%

    minlabelrad = 0.015 * magn([w,h]);
    
    maxlabelrad = 1.5 * minlabelrad;
    
    labelradrange = maxlabelrad - minlabelrad;
    
    if BlockLabel.MaxArea == -1

        labeld = minlabelrad;

        labelcolour = 'm';

    else

        labeld = minlabelrad + labelradrange * (BlockLabel.MaxArea / maxtriarea);

        labelcolour = 'k';

    end

    rectpos = [ BlockLabel.Coords(1) - labeld/2, ...
                BlockLabel.Coords(2) - labeld/2, ...
                labeld, ...
                labeld ];
    
    hold all
    rectangle('Position', rectpos, 'Curvature', [1,1], ...
              'EdgeColor', labelcolour, ...
              'UserData', 'mfemm');

    plotcross( BlockLabel.Coords(1), ...
               BlockLabel.Coords(2), ...
               labeld,labeld, ...
               'Color', labelcolour, ...
               'UserData', 'mfemm');
           
    if isscalar (BlockLabel.MagDir) && ((~isnan (BlockLabel.MagDir)) || (BlockLabel.IsMagnet == false))
        plotmagdirarrow ( BlockLabel.Coords(1), ...
            BlockLabel.Coords(2), ...
            labeld,labeld, ...
            BlockLabel.MagDir, ...
            'Color', labelcolour, ...
            'UserData', 'mfemm' );
    end

    hold off
    
    % Draw the label a little to the right and above the block marker
    text( BlockLabel.Coords(1) + 1.025*(labeld/2), ...
          BlockLabel.Coords(2) + 1.025*(labeld/2), ...
          BlockLabel.BlockType, ...
          'UserData', 'mfemm', ...
          options.LabelText{:} );

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


function plotmagdirarrow (x,y,w,h,dir,varargin)

    arrownodes = [ x + w/2, y - h/2 ;
                   x + 2*w/2, y;
                   x + w/2, y + h/2 ;
                   x + w/2, y - h/2 ; ];
               
	arrownodes = rotate2D (arrownodes, deg2rad(dir), [x,y]);
               
    line(arrownodes(:,1), arrownodes(:,2), varargin{:});

    line(arrownodes(:,1), arrownodes(:,2), varargin{:});

end





    
    
    