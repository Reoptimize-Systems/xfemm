function [x, y] = arcpoints(A, B, angle, maxdeg)
% get linearly spaced points on an arc
%
% Syntax
%
% [x, y] = arcpoints(A, B, angle, maxdeg)
%
% Input
%
%  A - (n x 2) matrix of arc start locations
%
%  B - (n x 2) matrix of arc end locations
%
%  angle - scalar or (n X 1) vector of angles in degrees subtended by the arc 
%    between each point A and B
%
%  maxdeg - scalar or (n X 1) vector of maximum angle in degrees subtended
%    between each point A and B
%
% Output
%
%  x,y - (n x p) vectors containing the x and y coordinates of the 'p' points on
%   each arc. Each row corresponds to points on the arc represented by each row
%   of the A and B matrices
%

    % convert the angles to radians
    angle = deg2rad (angle);
    maxdeg = deg2rad (maxdeg);

    % get centre and radius of circles
    [centre, r] = mfemmdeps.circcentre (A, B, angle);
    
    % get starting angle of arcs
    tempA = A - centre;
    
    [starttheta, rho] = cart2pol(tempA(:,1), tempA(:,2));
    
    % get arc points
    npnts = max(3, ceil(angle ./ maxdeg));
    
    pnts = ones (numel (npnts), max(npnts)) * nan;
    
    for ind = 1:size(pnts,1)
        pnts(ind,1:npnts(ind)) = linspace(starttheta(ind), starttheta(ind) + angle(ind), npnts(ind));
    end
    
    [x, y] = pol2cart(pnts, repmat(rho, 1, size(pnts,2)));
    
    x = bsxfun (@plus, x, centre(:,1));
    y = bsxfun (@plus, y, centre(:,2));
    
    
end