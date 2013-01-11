function links = getarclinks_mfemm(FemmProblem)
% getnodelinks_femm: gets all the arc segments from an mfemm problem
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

    if isfield(FemmProblem, 'ArcSegments')
        links = [cell2mat({FemmProblem.ArcSegments(:).n0}'), ...
                 cell2mat({FemmProblem.ArcSegments(:).n1}'), ...
                 cell2mat({FemmProblem.ArcSegments(:).ArcLength}'), ...
                 cell2mat({FemmProblem.ArcSegments(:).MaxSegDegrees}')];
    else
        links = [];
    end

end