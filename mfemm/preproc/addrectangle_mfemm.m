function [FemmProblem, seginds, nodeinds, nodeids, centre] = addrectangle_mfemm(FemmProblem, x, y, w, h, varargin)
% adds a rectangular region 

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

    % construct the coordinates of the vertices in clockwise order
    coords = [x, y; 
              x + w, y;
              x + w, y + h;
              x, y + h ];
          
    % use addpolygon to make the rectangle
    [FemmProblem, seginds, nodeinds, nodeids] = addpolygon_mfemm(FemmProblem, coords, varargin{:});

    % locate the centre for convenience
    centre = rectcentre(coords(1,:), coords(3,:));
    
end