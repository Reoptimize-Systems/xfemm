function FemmProblem = mirrorsegments_mfemm(FemmProblem, seginds, disttol, varargin)
% mirros one or more segments along a line
%
% Syntax
%
% FemmProblem = mirrorsegments(FemmProblem, seginds, disttol, 'Parameter', 'Value')
%
% 
% Input
%
% FemmProblem - An mfemm problem structure containing segments, some of
%   which are to be mirrored.
% 
% seginds - Indices of the segments in the FemmProblem Structure which are
%   to be mirrored.
%
% disttol - Distance from the line of reflection at which the segment's
%   nodes are considered to lie on the line of reflection, and will not be
%   duplicated.
%
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
%                      |       . ¦
%                      |      .  ¦dy     m = dy / dx             
%                      |     .----                                              
%                      |    .   dx
%                 _____|___._____________________                           
%                    ¦ |  .                                                 
%                  c ¦ | .                                                  
%                    ¦ |.                                                   
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
% Output
%
% FemmProblem - A modified FemmProblem structure now also containing the
%   mirrored new segments
%
%
% See also: 
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

    if isempty(disttol)
        disttol = 1e-5;
    end
    
    newseginds = [];
    
    for i = 1:numel(seginds)
       
        % mirror the nodes in the segment
        segnodecoords = [ FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n0 + 1).Coords; 
                          FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n1 + 1).Coords ];
                          
        newsegcoords = reflect2d(segnodecoords, varargin{:});
        
        % get the distances between nodes
        nodedists = diag(ipdm(segnodecoords, newsegcoords));
        
        % add the mirrored nodes to the problem if they are separated from
        % the existing node by at least tol
        [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, ...
            newsegcoords(nodedists > disttol, 1), newsegcoords(nodedists > disttol, 2));
            
        SegProps = FemmProblem.Segments(seginds(i));
        SegProps = rmfield(SegProps, 'n0');
        SegProps = rmfield(SegProps, 'n1');
        
        % join the relevant nodes to make a new segment with the same
        % properties as the original segment
        if (nodedists(1) < disttol) && (nodedists(2) < disttol)
            % do nothing the segment lies along the line of reflection
        elseif nodedists(1) < disttol
            % the first node of the segment lies on the line of reflection
            
            [FemmProblem, newsegind] = addsegments_mfemm(FemmProblem, ...
                                       FemmProblem.Segments(seginds(i)).n0, ...
                                       nodeids(1), ...
                                       SegProps);
                                   
        elseif nodedists(2) < disttol
            % the second node of the segment lies on the line of reflection
            [FemmProblem, newsegind] = addsegments_mfemm(FemmProblem, ...
                                       nodeids(1), ...
                                       FemmProblem.Segments(seginds(i)).n1, ...
                                       SegProps);
            
        else
            % neither node of the segment lies on the line of reflection
            [FemmProblem, newsegind] = addsegments_mfemm(FemmProblem, ...
                                       nodeids(1), ...
                                       nodeids(2), ...
                                       SegProps);
            
        end
        
        newseginds = [newseginds, newsegind];
        
        
    end

end 