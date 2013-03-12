function meshsize = choosemesharea_mfemm(w, h, fraction)
% chooses a suitible mesh area constraint for a rectangular region

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

    if nargin < 3
        fraction = 0.05;
    end

    % The mesh size is the minimum of either the diagonal of a rectangular
    % region bounded by a box of width w and height h, divided by the
    % desired number of triangles, or 5 times the width or height
    % divided by the desired number of triangles
    meshsize = min([5*w*fraction, 5*h*fraction, sqrt(w^2 + h^2)*fraction]);
    
end