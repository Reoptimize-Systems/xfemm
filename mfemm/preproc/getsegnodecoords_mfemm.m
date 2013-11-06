function segcoords = getsegnodecoords_mfemm(FemmProblem)
% getblocklabelcoords_mfemm: gets the coordinates of the nodes making up
% each segment

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

    temp = {FemmProblem.Nodes( cell2mat({FemmProblem.Segments(:).n0})+1 ).Coords};
    segcoords = cell2mat(temp(:));
    
    temp = {FemmProblem.Nodes( cell2mat({FemmProblem.Segments(:).n1})+1 ).Coords};
    segcoords = [ segcoords, cell2mat(temp(:))];
    
end