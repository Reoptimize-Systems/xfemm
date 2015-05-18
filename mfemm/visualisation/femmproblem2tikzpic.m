function tikzstrs = femmproblem2tikzpic (FemmProblem, varargin)
% Converts a FemmProblem structure to TIKZ picture suitible for inclusion
% in a LaTeX document
%
% Syntax
%
% tikzstrs = femmproblem2tikzpic (FemmProblem)
% tikzstrs = femmproblem2tikzpic (FemmProblem, 'Paramter', Value)
%
% Input
%
%   FemmProblem - structure containing mfemm problem
%
% femmproblem2tikzpic can then take a number of arguments supplied as
% Parameter-Value pairs, i.e. a string containing the parameter name, and
% the value. The possible parameters are:
%
%   'width_mm' - the approximate width the final picture will be in mm, if
%     not supplied, the picture will be approximately 150 mm wide
%
%   'groups' - a vector of groups in the problem. Only these groups will be
%     drawn in the output. All segments will be added to the picture, but
%     those not in these groups will be commented out.
%
% Output
%
%   tikzstrs - cell array of strings containing the tikzpicture, one line
%     per row.
%
% Example
%
% % to draw the only the groups 1 and 5 from the FemmProblem 
% tikzstrs = femmproblem2tikzpic (FemmProblem, 'groups', [1, 5])
%
% See also: cellstr2txtfile
%

% Created by Richard Crozier (C) 2014

    opts.width_mm = 150;
    opts.groups = [];
    
    opts = mfemmdeps.parseoptions (opts, varargin);
    
    [~,~,w,~] = extent_mfemm (FemmProblem);
    
    w = length2metres_mfemm (w, FemmProblem.ProbInfo.LengthUnits);

    width = opts.width_mm / 1000;
    
    tikzstart = { ...
sprintf('\\begin{tikzpicture}[x=100cm,y=100cm,scale=%f,', width / w);
...sprintf('\\begin{tikzpicture}[scale=1,');
'  lines/.style={draw=black}]';
    };

    if isfield(FemmProblem, 'Groups')
        % write out the group names
        tikzstart = [tikzstart; {'% Named groups in problem structure:'}];

        for fname = fieldnames(FemmProblem.Groups)'
            tikzstart = [ tikzstart; { ['% ', fname{1}, ' : ', num2str(FemmProblem.Groups.(fname{1}))] } ];
        end
    
    end
    
    % draw segments
    tikzbody = {'  % The segments.'};
    for ind = 1:numel (FemmProblem.Segments)
        tikzbody = [tikzbody; {drawseg(ind, FemmProblem, opts.groups)}];
    end
    
    % draw arc segments
    tikzbody = [tikzbody; {'  % The arc segments.'}];
    for ind = 1:numel (FemmProblem.ArcSegments)
        % write, but comment it out
        tikzbody = [tikzbody; {drawarcseg(ind, FemmProblem, opts.groups)}];
    end

    tikzend = { ...
'\end{tikzpicture}';
    };

    tikzstrs = [ tikzstart; tikzbody; tikzend ];

end

function segstr = drawseg (seg, FemmProblem, groups)
% generates a tikz command to draw a line

    if isempty (groups) || any(groups == FemmProblem.Segments(seg).InGroup)
        segstr = sprintf ('  %% In group: %s -- %f\n  \\draw [lines] (%f,%f) -- (%f,%f);', ...
                            getgroupname_mfemm (FemmProblem, FemmProblem.Segments(seg).InGroup), ...
                            FemmProblem.Segments(seg).InGroup, ...
                            FemmProblem.Nodes(FemmProblem.Segments(seg).n0+1).Coords(1), ...
                            FemmProblem.Nodes(FemmProblem.Segments(seg).n0+1).Coords(2), ...
                            FemmProblem.Nodes(FemmProblem.Segments(seg).n1+1).Coords(1), ...
                            FemmProblem.Nodes(FemmProblem.Segments(seg).n1+1).Coords(2));   
    else
        segstr = sprintf ('  %% In group: %s -- %f\n%%  \\draw [lines] (%f,%f) -- (%f,%f);', ...
                            getgroupname_mfemm (FemmProblem, FemmProblem.Segments(seg).InGroup), ...
                            FemmProblem.Segments(seg).InGroup, ...
                            FemmProblem.Nodes(FemmProblem.Segments(seg).n0+1).Coords(1), ...
                            FemmProblem.Nodes(FemmProblem.Segments(seg).n0+1).Coords(2), ...
                            FemmProblem.Nodes(FemmProblem.Segments(seg).n1+1).Coords(1), ...
                            FemmProblem.Nodes(FemmProblem.Segments(seg).n1+1).Coords(2));
    end
                    
                    
end


function asegstr = drawarcseg (arcseg, FemmProblem, groups)
% function to convert the arc segments in the femm problem to tikz arcs
    
    [centre, r] = circcentre ( FemmProblem.Nodes(FemmProblem.ArcSegments(arcseg).n0+1).Coords, ...
                               FemmProblem.Nodes(FemmProblem.ArcSegments(arcseg).n1+1).Coords, ...
                               mfemmdeps.deg2rad(FemmProblem.ArcSegments(arcseg).ArcLength) );

    startcoords = FemmProblem.Nodes(FemmProblem.ArcSegments(arcseg).n0+1).Coords - centre;
    endcoords = FemmProblem.Nodes(FemmProblem.ArcSegments(arcseg).n1+1).Coords - centre;

    [angs, ~] = cart2pol ( [startcoords(1), endcoords(1)], [startcoords(2), endcoords(2)] );

    angs = rad2deg (angs);
        
    if isempty (groups) || any(groups == FemmProblem.ArcSegments(arcseg).InGroup)
        asegstr = sprintf ( '  %% In group: %s -- %f\n  \\draw [lines] (%f,%f) arc (%f:%f:%f);', ...
                            getgroupname_mfemm (FemmProblem, FemmProblem.ArcSegments(arcseg).InGroup), ...
                            FemmProblem.ArcSegments(arcseg).InGroup, ...
                            startcoords(1)+centre(1), startcoords(2)+centre(2), ...
                            angs(1), angs(2), r );
    else
        asegstr = sprintf ( '  %% In group: %s -- %f\n%%  \\draw [lines] (%f,%f) arc (%f:%f:%f);', ...
                            getgroupname_mfemm (FemmProblem, FemmProblem.ArcSegments(arcseg).InGroup), ...
                            FemmProblem.ArcSegments(arcseg).InGroup, ...
                            startcoords(1)+centre(1), startcoords(2)+centre(2), ...
                            angs(1), angs(2), r );
    end
                        

end


function [centre, r] = circcentre (A, B, angle)
% calculates the centre and radius of a circle given two points and an arc
% angle between them. The position of the circle is determined by the
% order of the supplied points.
%
% Syntax
%
% [centre, r] = mfemmdeps.circcentre(A, B, angle)
%
%
    
    % get vector pointing from A to B
    AB = B - A;
    
    % find perpendicular vector to AB
    V = [ -AB(:,2), AB(:,1) ];
    
    % find mid point of AB
    M = A + AB .* 0.5;
    
    % find length of AB and divide by two to get triangle base
    b = 0.5 * mfemmdeps.magn(AB);
    
    % find triangle height
    h = b ./ tan(angle ./ 2);
    
    % find circle centre
    centre = M + h * mfemmdeps.unit(V);
    
    % find radius
    r = sqrt(h.^2 + b.^2);
    
end

