function [FemmProblem, arcseginds] = addarcsegments_mfemm(FemmProblem, n0, n1, angle, varargin)
% adds a segment to an mfemm FemmProblem structure
%
% Syntax
% 
% [FemmProblem, arcseginds] = addarcsegments_mfemm(FemmProblem, n0, n1, angle)
% [FemmProblem, arcseginds] = addarcsegments_mfemm(FemmProblem, n0, n1, angle, 'Parameter', 'Value')
% [FemmProblem, arcseginds] = addarcsegments_mfemm(FemmProblem, n0, n1, angle, options)
% 
% Description
%
% addarcsegments_mfemm(FemmProblem, n0, n1, angle) adds an arc segment to
% the FemmProblem structure between nodes with the pairs of ids in n0 and
% n1, sweeping out angle. n0 and n1 can be matrices of node ids of the same
% size, with arc segments being added between each corresponding n0 and n1
% id. angle must be a matrix or vector of the same size and n1 and n0.
%
% addarcsegments_mfemm(FemmProblem, n0, n1, angle, 'Parameter', 'Value')
% performs the same operation, but applies further optional parameters to
% the arc segments, as specified in one or more paramter-value pairs.
% Possible p-v pairs are:
%
%   'MaxSegDegrees'  - scalar value of the maximum degrees in the arc
%                      segment spanned by a single sub-segment in the
%                      discretization of the arc along the segment.
%                      Defaults to 1 degree.
%
%   'Hidden'         - Scalar value determining the visibility in the
%                      femm post-processor. If this evaluates to true the 
%                      segment will be hidden. Defaults to false.
%
%   'InGroup'        - Scalar value containing the group number of segment. 
%                      Defaults to zero.
%
%   'BoundaryMarker' - String containing the name of a boundary assigned to
%                      segment. Defaults to an empty string, i.e. no
%                      boundary property.
% 
% The values are applied to all segments created.
%
% addarcsegments_mfemm(FemmProblem, n0, n1, angle, options) performs the
% same operation, but instead of supplying the optional arguments as p-v
% pairs they arer supplied as fields in an options structure. The fields
% must have the same names as specified in the description of the p-v pairs
% above.
%
%
% See also: newarcsegment_mfemm.m
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

    arcseginds = repmat(-1, 1, numel(n0));
    
    if ~isfield(FemmProblem, 'ArcSegments') || isempty(FemmProblem.Segments)
        arcseginds(1) = 1;
        FemmProblem.ArcSegments = newarcsegment_mfemm(n0(1), n1(1), angle(1), varargin{:});
    else
        arcseginds(1) = numel(FemmProblem.Segments) + 1;
        FemmProblem.Segments(arcseginds(1)) = newarcsegment_mfemm(n0(1), n1(1), angle(1), varargin{:});
    end
    
    for i = 2:numel(n0)
        
        arcseginds(i) = arcseginds(i-1) + 1;
        
        FemmProblem.Segments(arcseginds(i)) = newarcsegment_mfemm(n0(i), n1(i), angle(i), varargin{:});      
        
    end
    
end