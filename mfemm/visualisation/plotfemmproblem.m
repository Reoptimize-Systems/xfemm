function plotfemmproblem(FemmProblem)
% plotfemmproblem: plots a simple visualisation of an mfemm problem
% structure NB: Arc Segments not yet supported

    figure;
    
    nodes = getnodecoords_mfemm(FemmProblem);
    
    links = getnodelinks_mfemm(FemmProblem);
    
    % plot the segments as lines
    plotnodelinks(nodes, links);
    
    maxtriarea = max(cell2mat({FemmProblem.BlockLabels(:).MaxArea}'));
    
    % get the extent of the problem region
    [x,y,w,h] = extent_mfemm(FemmProblem);
    
    minlabelrad = 0.025 * magn([w,h]);
    
    maxlabelrad = 4 * minlabelrad;
    
    labelradrange = maxlabelrad - minlabelrad;
    
    hold on
    
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

    
    
    