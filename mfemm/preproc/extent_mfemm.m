function [x,y,w,h] = extent_mfemm(FemmProblem)
% extent_mfemm: gets the extent of a problem described in an mfemm
% structure

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

    nodecoords = getnodecoords_mfemm(FemmProblem);
    
    x = min(nodecoords(:,1));
    
    y = min(nodecoords(:,2));
    
    
    w = max(nodecoords(:,1)) - x;
    
    h = max(nodecoords(:,2)) - y;


end