function xycoords = rotate2D (xycoords, rotangle, refpoint)
% rotate points in a plane about an arbitrary reference pointpoint
%
% Syntax
%
% xycoords = rotate2D (xycoords, rotangle, refpoint)
%
% Input
%
%   xycoords - (n x 2) matrix of n (x,y) coordinates to be rotated
%
%   rotangle - scalar angle by which to rotate the points. Positive angles
%     correspond to clockwise rotations
%
%   refpoint - optional x,y coordinate about which to rotate the points. If
%     not supplied, the origin (0,0) will be used.
%
% 

    if nargin < 3
        refpoint = [0,0];
    end
    
    rotangle = -rotangle;
    
    % shift the points such that the reference point lies on the origin
    xycoords = bsxfun(@minus, xycoords, refpoint);
    
    % transpose for matrix multiplication
    xycoords = xycoords.'; 
    
    % Now rotate the shifted 2d coordinates about the origin
    matrixCoeff = [ cos(rotangle), sin(rotangle); 
                   -sin(rotangle), cos(rotangle) ]; 
    
    xycoords = matrixCoeff * xycoords;

    % back to original vector format
    xycoords = xycoords'; 
    
    % transform xycoords back to original x-y coordinate
    xycoords = bsxfun(@plus, xycoords, refpoint); 
    
end