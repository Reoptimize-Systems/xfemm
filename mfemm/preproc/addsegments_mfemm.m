function [FemmProblem, seginds] = addsegments_mfemm(FemmProblem, n0, n1, varargin)
% adds a segment to an mfemm FemmProblem structure
%
% Syntax
% 
% [FemmProblem, seginds] = addsegments_mfemm(FemmProblem, n0, n1, 'Parameter', 'Value')
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