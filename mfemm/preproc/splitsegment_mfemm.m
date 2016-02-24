function [FemmProblem, newsegids, newnodeids] = splitsegment_mfemm (FemmProblem, segid, lenfrac, varargin)
% splits a segment into two new segments, adding a new nodes as necessary
%
% Syntax
%
% [FemmProblem, segids, newnodeids] = splitsegment_mfemm (FemmProblem, segid, lenfrac)
%
% Input 
%
%  FemmProblem - mfemm Femmproblem structure
%
%  segid - zero-based id of the segment to be split along it's length into
%    multiple smaller segments
%
%  lenfrac - vector of one or more values. These should be fractions of the
%    length of the original segment. New nodes will be inserted at these
%    locations and segments likning the nodes. lenfrac must therefore lie
%    in the bounds 0 < lenfrac < 1. A negtive value may also be specified
%    for lenfrac, in this case, the value is a fraction of the length from
%    the end of the segment, and must lie in the range -1 < lenfrac < 0.
%
% Output
%
% FemmProblem - mfemm Femmproblem structure with the newly added segments
%
% segids - zero-based ids of the newly added segments
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

%     options.CopyGroupNumber = true;
%     options.CopyBoundary = 0;
%     
%     options = mfemmdeps.parse_pv_pairs (options, varargin);
    
    % check lenfrac is a numeric vector
    assert (isvector (lenfrac) & isnumeric (lenfrac), 'MFEMM:splitsegment:badlenfrac', ...
        'legfrac must e a numeric vector');
    
    if any(lenfrac == 0) || any(abs(lenfrac) >= 1)
        error ('MFEMM:splitsegment:badlenfrac', ...
               'lenfrac must satisfy -1 < abs(lenfrac) < 1 and lenfrac != 1');
    end
    
    lenfrac(lenfrac < 0) = 1 - abs (lenfrac(lenfrac < 0));
    
    lenfrac = sort (lenfrac);
        
    % get info about existing segment nodes
    n0 = FemmProblem.Segments(segid+1).n0;
    n2 = FemmProblem.Segments(segid+1).n1;
    n0n2Coords = [ FemmProblem.Nodes(n0+1).Coords; FemmProblem.Nodes(n2+1).Coords ];
    
    % find vector pointing along segment from first node to second node
    segvec = n0n2Coords(2,:) - n0n2Coords(1,:);
    
    % multiply vector by lenfrac
    newsegnodes = bsxfun(@plus, n0n2Coords(1,:), ...
                    bsxfun (@times, lenfrac(:), segvec));
    
    % add new nodes
    [FemmProblem, ~, newnodeids] = addnodes_mfemm (FemmProblem, newsegnodes(:,1), newsegnodes(:,2));
    
    % link first node of original segment to first new node
    FemmProblem.Segments(segid+1).n1 = newnodeids(1);
    
    % make new segments linking new node to old n2 of original segment
    [FemmProblem, seginds] = addsegments_mfemm (FemmProblem, newnodeids, [newnodeids(2:end), n2]);
    
    newsegids = seginds - 1;
    
    % TODO: copy over info as requested

end