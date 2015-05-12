function [centre, r] = circcentre(A, B, angle)
% calculates the centre and radius of a circle given two points and an arc
% angle between them. The position of the circle is determined by the
% order of the supplied points.
%
% Syntax
%
% [centre, r] = circcentre(A, B, angle)
%
% Input
%
%  A,B - (n x 2) matrices of points lying on each arc
%
%  angle - (n x 1) or scalar angle in radians subtended by each pair of pair of
%    points in the A,B matrices
%
    
    % get vector pointing from A to B
    AB = B - A;
    
    % find perpendicular vector to AB
    V = [ -AB(:,2), AB(:,1) ];
    
    % find mid point of AB
    M = A + AB .* 0.5;
    
    % find length of AB and divide by two to get triangle base
    b = 0.5 * magn(AB,2);
    
    % find triangle height
    h = b ./ tan(angle ./ 2);
    
    % find circle centre
    centre = M + bsxfun(@times, h, unit(V,2));
    
    % find radius
    r = sqrt(h.^2 + b.^2);
    
end