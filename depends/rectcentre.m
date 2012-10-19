function xycent = rectcentre(varargin)
% rectcenter, simple function which finds the centre of a rectangle
%
% Syntax
%
% xycent = rectcentre(x1, y1, x2, y2)
% xycent = rectcentre(N1, N2)
%
% Input
%
% Either four inputs:
%
%   x1 - vector of x-coordinates of bottom left corners
%
%   y1 - vector of y-coordinate of bottom left corners
%
%   x2 - vector of x-coordinate of top right corners
%
%   y2 - vector of y-coordinate of top right corners
%
% Or two:
%
%   N1 = [x1,y1]
%
%   N2 = [x2,y2] 
%

% Author: Richard Crozier

    if nargin == 2
       
        if size(varargin{1}, 2) == 2 && size(varargin{1}, 2) == 2
            x1 = varargin{1}(:,1);
            y1 = varargin{1}(:,2);
            x2 = varargin{2}(:,1);
            y2 = varargin{2}(:,2);
        else
            error('If supplying two arguments, each must be (n x 2) matrices of node coordinates, one coordinate per row')
        end 
        
    elseif nargin == 4
        
        x1 = varargin{1};
        y1 = varargin{2};
        x2 = varargin{3};
        y2 = varargin{4};
        
    else
        error('Incorrect number of input arguments')
    end

    xycent(:,1) = x1 + (x2 - x1)./2;
    
    xycent(:,2) = y1 + (y2 - y1)./2;

end