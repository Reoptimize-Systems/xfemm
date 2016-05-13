function [xycoords] = reflect2d(xycoords, varargin)
% reflect2d: mirror 2d coordinates about a reference line using a
% reflection matrix
%
% Syntax
%
% [xycoordsMirrored] = reflect2d(xycoords, 'Parameter', 'Value');
%
% Input
%
% xycoords: (n x 2) matrix of x and y coordinates to be reflected
%
% The method of reflection is then specified through a parameter value
% pair. The options and expected input are as follows:
%
%   LineEq       In this case the line of reflection is specified using a
%                line equation of the form y = mx + c. The value supplied
%                in this case must be a vector of two values, the first of
%                which is 'm', the gradient of the line, and the second of
%                which is 'c', the intercept with the y-axis.
%
%                              line of reflection
%                                  .                                        
%                                 .                                         
%                      |         .
%                      |        .
%                      |       . :
%                      |      .  :dy     m = dy / dx             
%                      |     .----                                              
%                      |    .   dx
%                 _____|___._____________________                           
%                    : |  .                                                 
%                  c : | .                                                  
%                    : |.                                                   
%                    v x                                                    
%                     .    
%
% 	TwoPoints    The line of reflection is specified as a line passing
% 	             through two coordinates. In this case the value is
% 	             expected to be a vector of four values, the first two
% 	             values are the x and y coordinate of the first point, and
% 	             the second two values the x and y coordinate of the secod
% 	             point.
%
%                                 line of reflection
%                                  .                                        
%                                 .                                         
%                      |         x  point 2                                       
%                      |        .                                      
%                      |       .                                         
%               ..............x.......................                      
%                      |     . point 1                                            
%                      |    .                                             
%                 _____|___._____________________                           
%                      |  .                                                 
%                      | .                                                  
%                      |.                                                   
%                      .                                                    
%                     .   
%
%   AnglePoint   The line of reflection is specified as a single point and
%                an angle from a line parallel to the x-axis (i.e.
%                horizontal) passing through the point at which the line of
%                relflection passes through the point. In this case a 
%                vector containing three values is expected as input, the 
%                first value is the angle in radians, and the next two
%                values are the x and y coordinates of the point through
%                which the line passes.
%
%                              line of reflection
%                                  .                                        
%                                 .                                         
%                      |         ._                                         
%                      |        .   \  angle                                     
%                      |       .     \                                     
%               ..............x......|.................                      
%                      |     . point                                             
%                      |    .                                              
%                 _____|___._____________________                           
%                      |  .                                                 
%                      | .                                                  
%                      |.                                                   
%                      .                                                    
%                     .                                                     
%                               
%
% Example
%
% xycoords = [-1 2;-3 1;-2 5;-1 2];
% refangle = 45*pi/180;
% refpoint = [0 1];
%
% [xycoordsMirrored] = reflect2d(xycoords, 'AnglePoint', [refangle, refpoint]);
%
% x = -4:4;
% y = x+1;
%
% figure
% plot(xycoords(:,1),xycoords(:,2),x,y,xycoordsMirrored(:,1),xycoordsMirrored(:,2))
% axis equal
% grid

    Inputs.LineEq = [];
    Inputs.TwoPoints = [];
    Inputs.AnglePoint = [];
    Inputs.Axis = 'x';
    
    Inputs = parse_pv_pairs(Inputs, varargin);
    
    if ~isempty(Inputs.LineEq)
        
        m = Inputs.LineEq(1);
        c = Inputs.LineEq(2);
        
        refangle = atan2(1,m);

        refpoint = [0, c];
            
        
    elseif ~isempty(Inputs.TwoPoints)
        
        refpoint = Inputs.TwoPoints(1:2);
        
        refangle = atan2(Inputs.TwoPoints(4) - Inputs.TwoPoints(2), ...
                         Inputs.TwoPoints(3) - Inputs.TwoPoints(1));
        
    elseif ~isempty(Inputs.AnglePoint)
        
        refangle = Inputs.AnglePoint(1);
        
        refpoint = Inputs.AnglePoint(2:3);
        
    elseif ~isempty(Inputs.Axis)
        
        if strcmp(Inputs.Axis, 'x')
            
            refangle = 0;

            refpoint = [0, 0];
            
        elseif strcmp(Inputs.Axis, 'y')
            
            refangle = pi/2;

            refpoint = [0, 0];
            
        else
            error('Invalid axis specification, please use ''x'' or ''y''')
        end
        
    end
    
    % shift the points such that the reference point lies on the origin
    xycoords = bsxfun(@plus, xycoords, -refpoint);
    
    % transpose for matrix multiplication
    xycoords = xycoords'; 

    % <http://www.udayton.edu/~cps/cps460/notes/2dtrans/2dtrans.html>
    % for reflection
    
    % Now rotate the shifted 2d coordinates about the origin
    matrixCoeff = [cos(refangle*2)  sin(refangle*2); 
                   sin(refangle*2) -cos(refangle*2)]; 
    
    xycoords = matrixCoeff * xycoords;

    % back to original vector format
    xycoords = xycoords'; 
    
    % transform xycoords back to original x-y coordinate
    xycoords = bsxfun(@plus, xycoords, refpoint); 
        
end

