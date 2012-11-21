function FemmProblem = mirrorsegments_mfemm(FemmProblem, seginds, disttol, varargin)
% mirros one or more segments along a line
%
% Syntax
%
% FemmProblem = mirrorsegments(FemmProblem, seginds, disttol, varargin)
%
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
    mirrorednodeids = [-1, -1];
    
    for i = 1:numel(seginds)
       
        % mirror the nodes in the segment
        segnodecoords = [ FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n0 + 1).Coords; 
                          FemmProblem.Nodes(FemmProblem.Segments(seginds(i)).n1 + 1).Coords ];
                          
        newsegcoords = reflect2d(segnodecoords, varargin{:});
        
        % get all the existing node coordinates
%         existingnodecoords = getnodecoords_mfemm(FemmProblem);
        
        % get the distances between the nodes of the segment being
        % mirrored, and those of the new segment which would be created by
        % the mirroring
        nodedists = diag(ipdm(segnodecoords, newsegcoords));
        
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
                NodeProps = FemmProblem.Nodes(mirrorednodeids(Locb,2)+1);
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
            
            [Lia,Locb] = ismember(FemmProblem.Segments(seginds(i)).n1, mirrorednodeids(:,1));
            
            if any(Lia)
                nodeids = mirrorednodeids(Locb,2);
            else
                % add the mirrored nodes to the problem if they are separated from
                % the existing node by at least tol
                NodeProps = FemmProblem.Nodes(mirrorednodeids(Locb,2)+1);
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
                NodeProps = FemmProblem.Nodes(mirrorednodeids(Locb,2)+1);
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
                NodeProps = FemmProblem.Nodes(mirrorednodeids(Locb,2)+1);
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