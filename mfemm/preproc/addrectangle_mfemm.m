function [FemmProblem, seginds, nodeinds, nodeids, centre] = addrectangle_mfemm(FemmProblem, x, y, w, h, varargin)
% adds a rectangular region to FemmProblem structure
%
% Syntax
%
% [FemmProblem, seginds, nodeinds, nodeids, centre] = addrectangle_mfemm(FemmProblem, x, y, w, h)
% [...] = addrectangle_mfemm(..., )
%
% Description
%
% Creates a rectangular region made up of four nodes and four segments. The
% nodes are created anti-clockwise from the bottom left corner, and the new
% segments are added in the same order.
%
% Input
%
%  FemmProblem - existing FemmProblem structure to which new nodes and
%    segments will be added
%
%  x - x position of lower left hand corner of rectangle
%
%  y - y position of lower left hand corner of rectangle
%
%  w - width of the rectangle (length in x direction)
%
%  h - height of the rectangle (length in y direction)
%
%  Segment properties can be supplied as additional arguments, either as a
%  structure containing the appropriate fields, or as Parameter-Value
%  pairs. The syntax is the same as that described in the help for
%  addsegments_mfemm.
%
% Output
%
%  FemmProblem - modified femmproblem with new nodes and segments added
%
%  seginds - indices of new segments
%
%  nodeinds - indices of new nodes
%
%  nodeids - zero-based IDs of new nodes
%
%  centre - 2 element vector containing the x and y position of the
%   rectangle centre
%
%
%
% See also: addpolygon_mfemm, addsegments_mfemm

% Copyright 2012-2016 Richard Crozier
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

    % construct the coordinates of the vertices in clockwise order
    coords = [x, y; 
              x + w, y;
              x + w, y + h;
              x, y + h ];
          
    % use addpolygon to make the rectangle
    [FemmProblem, seginds, nodeinds, nodeids] = addpolygon_mfemm(FemmProblem, coords, varargin{:});

    % locate the centre for convenience
    centre = mfemmdeps.rectcentre(coords(1,:), coords(3,:));
    
end