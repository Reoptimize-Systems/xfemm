function len = segmentlength_mfemm (FemmProblem, segid)
% returns the length of one or more segments with the given id numbers
%
% Syntax
%
% len = segmentlength_mfemm (segid)
%
% Input
%
%  FemmProblem - mfemm prblem structure containing the segments of interest
%
%  segid - vector of one or more zero-based indices of the segments for
%    which the length is to be determined
%
% Output
%
%  len - vector of segment lengths of the same size as segid
%
%

% Copyright 2015 Richard Crozier
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

    len = ones (size (segid)) * nan;
    
    for ind = 1:numel (segid)
        
        % get info about existing segment nodes
        n0 = FemmProblem.Segments(segid(ind)+1).n0;
        n1 = FemmProblem.Segments(segid(ind)+1).n1;
        n0n1Coords = [ FemmProblem.Nodes(n0+1).Coords; FemmProblem.Nodes(n1+1).Coords ];

        % find vector pointing along segment from first node to second node
        segvec = n0n1Coords(2,:) - n0n1Coords(1,:);

        len(ind) = mfemmdeps.magn (segvec);

    end
    
end