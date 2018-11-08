function FemmProblem = translateblocklabels_mfemm(FemmProblem, x, y, blockids)
% translates block labels which are
%
% Syntax
%
% FemmProblem = translateblocklabels_mfemm(FemmProblem, x, y)
% FemmProblem = translateblocklabels_mfemm(FemmProblem, x, y, nodeids)
%
% 

% Copyright 2012-2015 Richard Crozier
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


    if nargin < 4
        blockinds = 1:numel(FemmProblem.BlockLabels);
    else
        blockinds = blockids + 1;
    end
    
    for ind = blockinds
        FemmProblem.BlockLabels(ind).Coords = FemmProblem.BlockLabels(ind).Coords + [x, y];
    end

end