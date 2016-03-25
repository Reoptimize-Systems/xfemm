function [FemmProblem] = rotatenodes_mfemm(FemmProblem, rotangle, nodeids, varargin)
% rotates nodes with the given ids
%
% Positive angle correspond to counter-clockwise rotation
%
% Syntax
%
% [FemmProblem] = rotatenodes_mfemm(FemmProblem, rotangle, nodeids)
% [FemmProblem] = rotatenodes_mfemm(..., 'Origin', [x,y])
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

    Inputs.Origin = [0,0];
    
    Inputs = mfemmdeps.parseoptions (Inputs, varargin);

    if nargin < 3
        nodeinds = 1:numel(FemmProblem.Nodes);
    else
        nodeinds = nodeids + 1;
    end
    
    for ind = nodeinds
        FemmProblem.Nodes(ind).Coords = rotate2D (FemmProblem.Nodes(ind).Coords, rotangle, Inputs.Origin);
    end

end

