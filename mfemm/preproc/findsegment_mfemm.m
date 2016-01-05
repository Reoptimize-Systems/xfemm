function id = findsegment_mfemm(FemmProblem, loc)
% finds the segment with mid point nearest a given location
%
% Syntax
%
% [id, xycoords] = findsegment_mfemm(FemmProblem, loc)
%
% 
% Input
%
%  FemmProblem - an mfemm problem structure containing at least one segment
%
%  loc - (n x 2) matrix of sets of x and y positions. The segment midpoint
%    closest to each location will be found.
%
% Output
%
%  id - zero-based id of segments found, add 1 to get the index in the
%    FemmProblem structure
%
%  xycoords - the x and y coordinates of the segments which where found
%


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

%     segcoords = getsegmidpointcoords_mfemm(FemmProblem);
%     
%     % find the nearest node to the location
%     % mfemmdeps.ipdm returns a structure with fields named 'rowindex',
%     % 'columnindex', and 'distance'.
%     result = mfemmdeps.ipdm(loc, segcoords, 'Result', 'Structure', 'Subset', 'NearestNeighbor');
%     
%     % get the indices of the nodes and subtract 1 to make zero based
%     id = result.columnindex - 1;

    if size (loc, 1) > 1
        id = ones (size(loc,1), 1) * nan;
        for ind = 1: size (loc, 1)
            id(ind) = findsegment_mfemm(FemmProblem, loc(ind,:));
        end
        return;
    end

    segdists = ones (numel(FemmProblem.Segments), 1) * nan;
    for ind = 1:numel(FemmProblem.Segments)
        segdists(ind) = disttoseg (loc, ...
            FemmProblem.Nodes(FemmProblem.Segments(ind).n0+1).Coords, ...
            FemmProblem.Nodes(FemmProblem.Segments(ind).n1+1).Coords);
    end
    
    [~, I] = min (segdists);

    id = I - 1;
    
%     % return the actual coordinates of the segment mid-points
%     xycoords = cat(1, segcoords(id+1,:));

end


function dist = disttoseg (x, a, b)

    d_ab = norm(a-b);
    d_ax = norm(a-x);
    d_bx = norm(b-x);

    if dot(a-b,x-b)*dot(b-a,x-a)>=0
        A = [a,1;b,1;x,1];
        dist = abs(det(A))/d_ab;        
    else
        dist = min(d_ax, d_bx);
    end

end