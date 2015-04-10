function [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, x, y, varargin)
% adds one or more nodes to an mfemm FemmProblem structure at the specified
% locations
%
% Syntax
%
% [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, x, y, varargin)
%
% Description
%
% 
% Input
%
% FemmProblem - A FemmProblem structure as created by newproblem_mfemm.m
%
% x - A matrix of x (or r for axisymmetric problems)coordinates of the
%   nodes to be added to the problem structure
%
% y - A matrix of y (or z for axisymmetric problems) coordinates of the
%   nodes to be added to the problem structure. Must be the same size as x.
%
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

    nodeinds = repmat(-1, 1, numel(x));
    
    if ~isfield(FemmProblem, 'Nodes') || isempty(FemmProblem.Nodes)
        
        FemmProblem.Nodes = newnode_mfemm(x(1), y(1), varargin{:});
        nodeinds(1) = 1;
        
    else
        
        nodeinds(1) = numel(FemmProblem.Nodes) + 1;
        FemmProblem.Nodes(nodeinds(1)) = newnode_mfemm(x(1), y(1), varargin{:});
        
    end
    
    for i = 2:numel(x)
        
        nodeinds(i) = nodeinds(i-1) + 1;
        
        FemmProblem.Nodes(nodeinds(i)) = newnode_mfemm(x(i), y(i), varargin{:});      
        
    end

    nodeids = nodeinds - 1;
    
end