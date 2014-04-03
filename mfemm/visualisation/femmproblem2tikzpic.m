function tikzstrs = femmproblem2tikzpic (FemmProblem, width_mm)
% Converts a FemmProblem structure to TIKZ picture suitible for inclusion
% in a LaTeX document
%
% Syntax
%
% tikzstrs = femmproblem2tikzpic (FemmProblem)
% tikzstrs = femmproblem2tikzpic (FemmProblem, width_mm)
%
% Input
%
%   FemmProblem - structure containing mfemm problem
%
%   width_mm - the approximate width the final picture will be in mm, if
%     not supplied, the picture will be approximately 150 mm wide
%
% Output
%
%   tikzstrs - cell array of strings containing the tizpicture, one line
%     per row.
%

    [~,~,w,~] = extent_mfemm (FemmProblem);
    
    w = length2metres_mfemm (w, FemmProblem.ProbInfo.LengthUnits);
    
    if nargin > 1
        width = width_mm / 1000;
    else
        % default real width in m
        width = 150e-3;
    end
    
    tikzstart = { ...
sprintf('\\begin{tikzpicture}[x=100cm,y=100cm,scale=%f,', width / w);
...sprintf('\\begin{tikzpicture}[scale=1,');
'  lines/.style={draw=black}]';
    };

    % draw segments
    tikzbody = {'  % The segments.'};
    for ind = 1:numel (FemmProblem.Segments)
        tikzbody = [tikzbody; {['  ', drawseg(ind, FemmProblem)]}];
    end
    
    % draw arc segments
    tikzbody = [tikzbody; {'  % The arc segments.'}];
    for ind = 1:numel (FemmProblem.ArcSegments)
        tikzbody = [tikzbody; {['  ', drawarcseg(ind, FemmProblem)]}];
    end

    tikzend = { ...
'\end{tikzpicture}';
    };

    tikzstrs = [ tikzstart; tikzbody; tikzend ];

end

function segstr = drawseg (seg, FemmProblem)
% generates a tikz command to draw a line

    segstr = sprintf ('\\draw [lines] (%f,%f) -- (%f,%f);', ...
                        FemmProblem.Nodes(FemmProblem.Segments(seg).n0+1).Coords(1), ...
                        FemmProblem.Nodes(FemmProblem.Segments(seg).n0+1).Coords(2), ...
                        FemmProblem.Nodes(FemmProblem.Segments(seg).n1+1).Coords(1), ...
                        FemmProblem.Nodes(FemmProblem.Segments(seg).n1+1).Coords(2));
                    
                    
end


function asegstr = drawarcseg (arcseg, FemmProblem)
% function to convert the arc segments in the femm problem to tikz arcs

    [centre, r] = circcentre ( FemmProblem.Nodes(FemmProblem.ArcSegments(arcseg).n0+1).Coords, ...
                               FemmProblem.Nodes(FemmProblem.ArcSegments(arcseg).n1+1).Coords, ...
                               deg2rad(FemmProblem.ArcSegments(arcseg).ArcLength) );
                          
    startcoords = FemmProblem.Nodes(FemmProblem.ArcSegments(arcseg).n0+1).Coords - centre;
    endcoords = FemmProblem.Nodes(FemmProblem.ArcSegments(arcseg).n1+1).Coords - centre;
    
    [angs, ~] = cart2pol ( [startcoords(1), endcoords(1)], [startcoords(2), endcoords(2)] );

    angs = rad2deg (angs);
    
    asegstr = sprintf ( '\\draw [lines] (%f,%f) arc (%f:%f:%f);', ...
                        startcoords(1)+centre(1), startcoords(2)+centre(2), ...
                        angs(1), angs(2), r );

end


function [centre, r] = circcentre (A, B, angle)
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

