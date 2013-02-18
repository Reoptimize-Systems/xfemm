function [FemmProblem, seginds] = addsegments_mfemm(FemmProblem, n0, n1, varargin)
% adds a segment to an mfemm FemmProblem structure
%
% Syntax
% 
% [FemmProblem, seginds] = addsegments_mfemm(FemmProblem, n0, n1)
% [FemmProblem, seginds] = addsegments_mfemm(FemmProblem, n0, n1, 'Parameter', 'Value')
% [FemmProblem, seginds] = addsegments_mfemm(FemmProblem, n0, n1, options)
% 
% Description
%
% addsegments_mfemm(FemmProblem, n0, n1) adds a segment to the FemmProblem
% structure between nodes with the pairs of ids in n0 and n1.  n0 and n1
% can be matrices of node ids of the same size, with segments being added
% between each corresponding n0 and n1 id.
%
% addsegments_mfemm(FemmProblem, n0, n1, 'Parameter', 'Value') performs the
% same operation, but applies further optional parameters to the segments,
% as specified in one or more paramter-value pairs. Possible p-v pairs are:
%
%   'MaxSideLength'  - scalar value of the maximum length of triangle sides
%                      along the segment. Defaults to -1, which means no
%                      maximum length is set.
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
% addsegments_mfemm(FemmProblem, n0, n1, options) performs the
% same operation, but instead of supplying the optional arguments as p-v
% pairs they arer supplied as fields in an options structure. The fields
% must have the same names as specified in the description of the p-v pairs
% above.
%
%
% See also: newsegments_mfemm.m
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

    seginds = repmat(-1, 1, numel(n0));
    
    if (numel(varargin) == 1) && isstruct(varargin{1}) && (numel(varargin{1}) == numel(n0))
        
        if ~isfield(FemmProblem, 'Segments') || isempty(FemmProblem.Segments)
            seginds(1) = 1;
            FemmProblem.Segments = newsegment_mfemm(n0(1), n1(1), varargin{1}(1));
        else
            seginds(1) = numel(FemmProblem.Segments) + 1;
            FemmProblem.Segments(seginds(1)) = newsegment_mfemm(n0(1), n1(1), varargin{1}(1));
        end

        for i = 2:numel(n0)

            seginds(i) = seginds(i-1) + 1;

            FemmProblem.Segments(seginds(i)) = newsegment_mfemm(n0(i), n1(i), varargin{1}(i));      

        end
        
    else
        
        if ~isfield(FemmProblem, 'Segments') || isempty(FemmProblem.Segments)
            seginds(1) = 1;
            FemmProblem.Segments = newsegment_mfemm(n0(1), n1(1), varargin{:});
        else
            seginds(1) = numel(FemmProblem.Segments) + 1;
            FemmProblem.Segments(seginds(1)) = newsegment_mfemm(n0(1), n1(1), varargin{:});
        end

        for i = 2:numel(n0)

            seginds(i) = seginds(i-1) + 1;

            FemmProblem.Segments(seginds(i)) = newsegment_mfemm(n0(i), n1(i), varargin{:});      

        end
        
    end
end