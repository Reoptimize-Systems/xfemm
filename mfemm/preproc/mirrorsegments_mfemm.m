function [FemmProblem, newseginds] = mirrorsegments_mfemm(FemmProblem, seginds, disttol, varargin)
% mirros one or more segments along a line
%
% Syntax
%
% FemmProblem = mirrorsegments_mfemm(FemmProblem, seginds, disttol, 'Parameter', Value)
%
% Description
%
% mirrorsegments_mfemm mirrors the segments specified in 'seginds' along a
% line. disttol is a tolerance value if the distance of a segments nodes
% from the line of reflection are less than this tolerance then they are
% not reflected. The exact behaviour depends on whether one or both
% segments lie on the line.
%
% The line and method of reflection are then specified through a parameter
% value pair. The options and expected input are as follows:
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


% Copyright 2012-2013 Richard Crozier
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
    mirrorednodeids = [-1, -1];
    
    for i = 1:numel(seginds)
       
        % mirror the nodes in the segment
        segnodecoords = [ FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n0 + 1).Coords; 
                          FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n1 + 1).Coords ];
                          
        newsegcoords = mfemmdeps.reflect2d(segnodecoords, varargin{:});
        
        % get all the existing node coordinates
%         existingnodecoords = getnodecoords_mfemm(FemmProblem);
        
        % get the distances between the nodes of the segment being
        % mirrored, and those of the new segment which would be created by
        % the mirroring
        nodedists = diag(mfemmdeps.ipdm(segnodecoords, newsegcoords));
        
%         if any(nodedists > disttol)
%             
%                 
%                 % add the mirrored nodes to the problem if they are separated from
%                 % the existing node by at least tol
%                 [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, ...
%                                                                   newsegcoords(nodedists > disttol, 1), ...
%                                                                   newsegcoords(nodedists > disttol, 2));
% 
%                 mirrorednodeids = [mirrorednodeids, nodeids];
%             
%             
%         end
        
        SegProps = FemmProblem.Segments(seginds(i));
        SegProps = rmfield(SegProps, 'n0');
        SegProps = rmfield(SegProps, 'n1');
        
        % join the relevant nodes to make a new segment with the same
        % properties as the original segment
        if (nodedists(1) <= disttol) && (nodedists(2) <= disttol)
            % do nothing the segment lies along the line of reflection
            nodedists;
            
        elseif (nodedists(1) <= disttol) && (nodedists(2) > disttol)
            % node n0 of the existing segment lies on the line of
            % reflection, we only need to reflect node n1 of the segment
            % we first check we have not already mirroed node n1 of this
            % segment when processing another segment
            [Lia,Locb] = ismember(FemmProblem.Segments(seginds(i)).n1, mirrorednodeids(:,1));
            
            if any(Lia)
                % we've already mirred the node for another segment, so the
                % node ids for the new segment connection are taken from
                % the list of mirrored nodes
                nodeids = mirrorednodeids(Locb,2);
            else
                % add the mirrored nodes to the problem if they are separated from
                % the existing node by at least tol
                NodeProps = FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n1+1);
                NodeProps = rmfield(NodeProps, 'Coords');
                [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, ...
                                                                  newsegcoords(2,1), ...
                                                                  newsegcoords(2,2), ...
                                                                  NodeProps);
                                                              
                mirrorednodeids = [mirrorednodeids; ...
                                   FemmProblem.Segments(seginds(i)).n1, nodeids]; 
                
            end
            
            % link existing node n0, to the mirred version of node n1 of
            % the existing segment
            [FemmProblem, newsegind] = addsegments_mfemm(FemmProblem, ...
                                                         FemmProblem.Segments(seginds(i)).n0, ...
                                                         nodeids, ...
                                                         SegProps);
                                   
        elseif (nodedists(2) <= disttol) && (nodedists(1) > disttol)
            
            [Lia,Locb] = ismember(FemmProblem.Segments(seginds(i)).n0, mirrorednodeids(:,1));
            
            if any(Lia)
                nodeids = mirrorednodeids(Locb,2);
            else
                % add the mirrored nodes to the problem if they are separated from
                % the existing node by at least tol
                NodeProps = FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n0+1);
                NodeProps = rmfield(NodeProps, 'Coords');
                [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, ...
                                                                  newsegcoords(1,1), ...
                                                                  newsegcoords(1,2));
                                                              
                mirrorednodeids = [mirrorednodeids; ...
                                   FemmProblem.Segments(seginds(i)).n0, nodeids]; 
                
            end
            
            [FemmProblem, newsegind] = addsegments_mfemm(FemmProblem, ...
                                       nodeids, ...
                                       FemmProblem.Segments(seginds(i)).n1, ...
                                       SegProps);
            
        else
            
            [Lia,Locb] = ismember(FemmProblem.Segments(seginds(i)).n0, mirrorednodeids(:,1));
            
            if any(Lia)
                nodeids = mirrorednodeids(Locb,2);
            else
                % add the mirrored nodes to the problem if they are separated from
                % the existing node by at least tol
                NodeProps = FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n0+1);
                NodeProps = rmfield(NodeProps, 'Coords');
                [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, ...
                                                                  newsegcoords(1,1), ...
                                                                  newsegcoords(1,2), ...
                                                                  NodeProps);
                                                              
                mirrorednodeids = [mirrorednodeids; ...
                                   FemmProblem.Segments(seginds(i)).n0, nodeids]; 
                
            end
            
            [Lia,Locb] = ismember(FemmProblem.Segments(seginds(i)).n1, mirrorednodeids(:,1));
            
            if any(Lia)
                nodeids = [nodeids, mirrorednodeids(Locb,2)];
            else
                % add the mirrored nodes to the problem if they are separated from
                % the existing node by at least tol
                NodeProps = FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n1+1);
                NodeProps = rmfield(NodeProps, 'Coords');
                [FemmProblem, nodeinds, nodeids(2)] = addnodes_mfemm(FemmProblem, ...
                                                                     newsegcoords(2,1), ...
                                                                     newsegcoords(2,2), ...
                                                                     NodeProps);
                                                              
                mirrorednodeids = [mirrorednodeids; ...
                                   FemmProblem.Segments(seginds(i)).n1, nodeids(2)]; 
                
            end
            
            [FemmProblem, newsegind] = addsegments_mfemm(FemmProblem, ...
                                                         nodeids(1), ...
                                                         nodeids(2), ...
                                                         SegProps);
            
        end
        
        newseginds = [newseginds, newsegind];
        
    end

end 

function mergenodes_mfemm(FemmProblem, tol)

    nodelocs = getnodecoords_mfemm(FemmProblem);
    
    

end