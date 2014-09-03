function FemmProblem = rotategroups_mfemm(FemmProblem, groupnos, rotangle, origin)
% rotates all nodes, segments, arc segments and block labels which are
% members of the specified group numbers
%
% Positive angle correspond to counter-clockwise rotation
%
% Syntax
%
% FemmProblem = rotategroups_mfemm(FemmProblem, groupnos, rotangle)
% FemmProblem = rotategroups_mfemm(FemmProblem, groupnos, rotangle, origin)
%
% 

% Copyright 2014 Richard Crozier
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

    if nargin < 4
        origin = [0,0];
    end
    
    excludenodes = [];
    includenodes = [];
    
    for i = 1:numel(groupnos)
        
        if isfield(FemmProblem, 'Segments')
            
            for ii = 1:numel(FemmProblem.Segments)
                
                if FemmProblem.Segments(ii).InGroup == groupnos(i)
                    
                    includenodes = [includenodes, FemmProblem.Segments(ii).n0, FemmProblem.Segments(ii).n1];
                    
                end
                
            end
            
        end
        
        if isfield(FemmProblem, 'ArcSegments')
            for ii = 1:numel(FemmProblem.ArcSegments)
                
                if FemmProblem.ArcSegments(ii).InGroup == groupnos(i)
                    
                    includenodes = [includenodes, FemmProblem.ArcSegments(ii).n0, FemmProblem.ArcSegments(ii).n1];
                    
                end
                
            end
        end
        
        if isfield(FemmProblem, 'BlockLabels')
            
            for ii = 1:numel(FemmProblem.BlockLabels)
                
                if FemmProblem.BlockLabels(ii).InGroup == groupnos(i)
                    FemmProblem.BlockLabels(ii).Coords =  rotate2D (FemmProblem.BlockLabels(ii).Coords, rotangle, origin);
                end
                
            end
            
        end
    end
    
    if isfield(FemmProblem, 'Nodes')
        % now shift any nodes in the groups which have not already been moved
        % when the segments and arc segments were moved
        for i = 1:numel(groupnos)

            for ii = 1:numel(FemmProblem.Nodes)
                
                if FemmProblem.Nodes(ii).InGroup == groupnos(i) && ...
                        ~any(excludenodes == ii)

                    includenodes = [includenodes, ii-1];

                end
                
            end

        end    
    end
    
    includenodes = unique(includenodes);
    
    FemmProblem = rotatenodes_mfemm(FemmProblem, rotangle, includenodes, 'Origin', origin);
    
end