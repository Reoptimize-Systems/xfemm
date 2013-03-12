function segcoords = getsegmidpointcoords_mfemm(FemmProblem)
% getblocklabelcoords_mfemm: gets all the block label locations from an
% mfemm problem structure

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

    segn0coords = {FemmProblem.Nodes( cell2mat({FemmProblem.Segments(:).n0})+1 ).Coords};
    segn0coords = cell2mat(segn0coords(:));
    
    segn1coords = {FemmProblem.Nodes( cell2mat({FemmProblem.Segments(:).n1})+1 ).Coords};
    segn1coords = cell2mat(segn1coords(:));
    
    % mid-point is mean value of x and y points
    segcoords = [mean([segn0coords(:,1), segn1coords(:,1)], 2), mean([segn0coords(:,2), segn1coords(:,2)], 2)];

end