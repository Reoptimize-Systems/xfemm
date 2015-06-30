function [id, xycoords] = findnode_mfemm(FemmProblem, loc)
% findnode_mfemm: finds the node nearest a given location
%
% Syntax
%
% [id, xycoords] = findnode_mfemm(FemmProblem, loc)
%
% Input
%
%  FemmProblem - an mfemm problem structure containing at least one node
%
%  loc - (n x 2) matrix of sets of x and y positions. The node closest to
%    each location will be found.
%
% Output
%
%  id - zero-based id of nodes found, add 1 to get the index in the
%    FemmProblem structure
%
%  xycoords - the x and y coordinate of the nodes which where found
%

% Copyright 2012-2015 Richard Crozier
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


    nodecoords = getnodecoords_mfemm(FemmProblem);
    
    for ind = 1:size(loc,1)
        
        % find the nearest node to the location
        % mfemmdeps.ipdm returns a structure with fields named 'rowindex',
        % 'columnindex', and 'distance'.
        result = mfemmdeps.ipdm(loc(ind,:), nodecoords, 'Result', 'Structure', 'Subset', 'NearestNeighbor');

        % get the indices of the nodes and subtract 1 to make zero based
        id(ind) = result.columnindex - 1;

        xycoords = cat(1, FemmProblem.Nodes(id+1).Coords);
    
    end

end