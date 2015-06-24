function Segment = newsegment_mfemm(n0, n1, varargin)
% newsegment_mfemm: generates a segment structure for an mfemm FemmProblem
% 
% Syntax
% 
% Segment = newsegment_mfemm(n0, n1, 'Parameter', 'Value', ...)
% 
% Descriptions
%
% newsegment_mfemm creates a structure containng the appropriate fields for
% a Segment for an mfemm FemmProblem Structure. The output structure will
% contain the following fields:
% 
%   n0                  node number at end 0
%   n1                  node number at end 1
%   MaxSideLength       max length of triangles along the segment
%   Hidden              determines the visibility in the femm post-processor
%   InGroup             group number of segment
%   BoundaryMarker      Name of boundary assigned to segment
%
% n0 and n1 are the nodes at either end of the Segment. Values of the other
% structure fields can be supplied by passing in the field name and value
% as parameter-value pairs. If not supplied the fields will have the
% following default values.
%
%   MaxSideLength   -1 (no set length)
%   Hidden           0 (not hidden)
%   InGroup          0 
%   BoundaryMarker   '' (empty char, not a boundary segment)
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
    Segment.MaxSideLength = -1;
    Segment.Hidden = 0;
    Segment.InGroup = 0;
    Segment.BoundaryMarker = '';
    Segment.InConductor = 0;
    
    % parse any options for the segment supplied by the user
    Segment = mfemmdeps.parseoptions(Segment, varargin);
    
    Segment.n0 = n0;
    Segment.n1 = n1;
    
end

