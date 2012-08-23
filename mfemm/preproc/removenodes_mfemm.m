function [FemmProblem] = removenodes_mfemm(FemmProblem, nodeids)
% remove nodes from an mfemm FemmProblem structure, and any segments or arc
% segments attached to those nodes
%
% Syntax
%
% [FemmProblem] = removenodes_mfemm(FemmProblem, nodeids)
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

    orignodeids = nodeids;

    % for each node id to be removed look for segments containing that node
    % id and remove them
    for i = 1:numel(nodeids)
        
        % Modify Segments
        if isfield(FemmProblem, 'Segments')

            FemmProblem.Segments(arrayfun(@(x)isequal(x.n0, nodeids(i)), FemmProblem.Segments)) = [];

            FemmProblem.Segments(arrayfun(@(x)isequal(x.n1, nodeids(i)), FemmProblem.Segments)) = [];

            for ii = 1:numel(FemmProblem.Segments)
                if FemmProblem.Segments(ii).n0 > nodeids(i)
                    FemmProblem.Segments(ii).n0 = FemmProblem.Segments(ii).n0 - 1;
                end
            end
            
            for ii = 1:numel(FemmProblem.Segments)
                if FemmProblem.Segments(ii).n1 > nodeids(i)
                    FemmProblem.Segments(ii).n1 = FemmProblem.Segments(ii).n1 - 1;
                end
            end

        end
        
        % Modify Arc Segments
        if isfield(FemmProblem, 'ArcSegments')

            FemmProblem.ArcSegments(arrayfun(@(x)isequal(x.n0, nodeids(i)), FemmProblem.ArcSegments)) = [];

            FemmProblem.ArcSegments(arrayfun(@(x)isequal(x.n1, nodeids(i)), FemmProblem.ArcSegments)) = [];

            for ii = 1:numel(FemmProblem.ArcSegments)
                if FemmProblem.ArcSegments(ii).n0 > nodeids(i)
                    FemmProblem.ArcSegments(ii).n0 = FemmProblem.ArcSegments(ii).n0 - 1;
                end
            end

            for ii = 1:numel(FemmProblem.ArcSegments)
                if FemmProblem.ArcSegments(ii).n1 > nodeids(i)
                    FemmProblem.ArcSegments(ii).n1 = FemmProblem.ArcSegments(ii).n1 - 1;
                end
            end

        end
        
        nodeids(nodeids > nodeids(i)) = nodeids(nodeids > nodeids(i)) - 1;
        
    end
    
    % remove the nodes
    FemmProblem.Nodes(orignodeids+1) = [];

end