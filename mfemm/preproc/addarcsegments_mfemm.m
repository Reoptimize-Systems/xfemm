function [FemmProblem, arcseginds] = addarcsegments_mfemm(FemmProblem, n0, n1, angle, varargin)
% adds arc segments to an mfemm FemmProblem structure
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
% n1, sweeping out angle 'angle' in degrees. n0 and n1 can be matrices of
% node ids of the same size, with arc segments being added between each
% corresponding n0 and n1 id. angle must be a matrix or vector of the same
% size and n1 and n0.
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
%   'BoundaryMarker' - Either a string containing the name of a boundary
%                      assigned to segment, or an integer boundary number.
%                      If an integer, this must be the (1-base) index of an
%                      existing boundary condition in the FemmProblem
%                      Structure to be applied. If zero, no boundary
%                      property is applied. Defaults to an empty string,
%                      i.e. no boundary property. 
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

% set up default segment properties
    options.MaxSegDegrees = ones(size(n0));
    options.Hidden = zeros(size(n0));
    options.InGroup = zeros(size(n0));
    options.BoundaryMarker = '';
    options.InConductor = 0;
    options.MeshedSideLength = [];
    
    % parse any options for the segment supplied by the user
    options = mfemmdeps.parseoptions(options, varargin);
    
    if isempty (options.MeshedSideLength)
        options.MeshedSideLength = options.MaxSegDegrees;
    end
    
    if ischar(options.BoundaryMarker)
    	options.BoundaryMarker = {options.BoundaryMarker};
    elseif isscalar(options.BoundaryMarker)
        options.BoundaryMarker = {FemmProblem.BoundaryProps(options.BoundaryMarker).Name};
    end
       
    if numel(n0) > 1
        
       if isscalar(options.MaxSegDegrees)
           options.MaxSegDegrees = repmat(options.MaxSegDegrees, size(n0));
       end
       
       if isscalar(options.Hidden)
           options.Hidden = repmat(options.Hidden, size(n0));
       end
       
       if isscalar(options.InGroup)
           options.InGroup = repmat(options.InGroup, size(n0));
       end
       
       if iscellstr(options.BoundaryMarker) && (numel(options.BoundaryMarker) == 1)
           options.BoundaryMarker = repmat(options.BoundaryMarker, size(n0));
       elseif isnumeric(options.BoundaryMarker) && mfemmdeps.samesize(options.BoundaryMarker, n0)
           strbounds = {};
           for ind = 1:numel(options.BoundaryMarker)
               strbounds = [strbounds, {FemmProblem.BoundaryProps(options.BoundaryMarker(ind)+1).Name}];
           end
           options.BoundaryMarker = strbounds;
       elseif ~mfemmdeps.samesize(options.BoundaryMarker, n0)
           error('MFEMM:InvalidBoundary', 'Invalid boundary specification.')
       end
       
       if isscalar(options.MaxSegDegrees)
           options.MeshedSideLength = repmat(options.MeshedSideLength, size(n0));
       end
       
    end
    
    arcseginds = repmat(-1, 1, numel(n0));
    
    if ~isfield(FemmProblem, 'ArcSegments') || isempty(FemmProblem.ArcSegments)
        arcseginds(1) = 1;
        FemmProblem.ArcSegments = newarcsegment_mfemm(n0(1), n1(1), angle(1), ...
                                                    'MaxSegDegrees', options.MaxSegDegrees(1), ...
                                                    'Hidden', options.Hidden(1), ...
                                                    'InGroup', options.InGroup(1), ...
                                                    'BoundaryMarker', options.BoundaryMarker{1}, ...
                                                    'MeshedSideLength', options.MeshedSideLength(1));
    else
        arcseginds(1) = numel(FemmProblem.ArcSegments) + 1;
        FemmProblem.ArcSegments(arcseginds(1)) = newarcsegment_mfemm(n0(1), n1(1), angle(1), ...
                                                    'MaxSegDegrees', options.MaxSegDegrees(1), ...
                                                    'Hidden', options.Hidden(1), ...
                                                    'InGroup', options.InGroup(1), ...
                                                    'BoundaryMarker', options.BoundaryMarker{1}, ...
                                                    'MeshedSideLength', options.MeshedSideLength(1));
    end
    
    for i = 2:numel(n0)
        
        arcseginds(i) = arcseginds(i-1) + 1;
        
        FemmProblem.ArcSegments(arcseginds(i)) = newarcsegment_mfemm(n0(i), n1(i), angle(i), ...
                                                    'MaxSegDegrees', options.MaxSegDegrees(i), ...
                                                    'Hidden', options.Hidden(i), ...
                                                    'InGroup', options.InGroup(i), ...
                                                    'BoundaryMarker', options.BoundaryMarker{i}, ...
                                                    'MeshedSideLength', options.MeshedSideLength(i)); 
        
    end
    
end