function [FemmProblem, seginds, nodeinds, nodeids] = addcircle_mfemm(FemmProblem, x, y, R, varargin)
% adds a circular region 
%
% Syntax
%
% [FemmProblem, seginds, nodeinds, nodeids] = addcircle_mfemm(FemmProblem, x, y, R, varargin)
%
% 

% Copyright 2015 Richard Crozier
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

    % add two nodes for the circle
    [FemmProblem, nodeinds, nodeids] = addnodes_mfemm (FemmProblem, [-R; R] + x, [0; 0] + y);
    
    % add arc segments
    [FemmProblem, seginds] = addarcsegments_mfemm(FemmProblem, nodeids, fliplr(nodeids), [180, 180], varargin{:});
    
end