function links = getarclinks_mfemm(FemmProblem, varargin)
% gets all the arc segments from an mfemm problem structure
%
% Syntax
%
% links = getarclinks_mfemm(FemmProblem)
%
% Description
%
% Obtains all the arc segment links from a FemmProblem structure in a
% compact array form. getarclinks_mfemm returns an array of the form
%
%  links = [ n0, n1, arc length, max segment degrees; 
%            n0, n1, arc length, max segment degrees; 
%            n0, n1, arc length, max segment degrees; ]
%
% See also: getnodelinks_mfemm
%

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

    Inputs.Groups = [];
    
    Inputs = mfemmdeps.parseoptions (Inputs, varargin);
    
    if isfield(FemmProblem, 'ArcSegments') && ~isempty(FemmProblem.ArcSegments)
        
        links = [cell2mat({FemmProblem.ArcSegments(:).n0}'), ...
                 cell2mat({FemmProblem.ArcSegments(:).n1}'), ...
                 cell2mat({FemmProblem.ArcSegments(:).ArcLength}'), ...
                 cell2mat({FemmProblem.ArcSegments(:).MaxSegDegrees}')];
             
        if ~isempty(Inputs.Groups)
            
            rmlinks = [];
            for ind = 1:numel (FemmProblem.ArcSegments)
                
                if ~(any(Inputs.Groups == FemmProblem.ArcSegments(ind).InGroup))
                    rmlinks = [ rmlinks, ind ];
                end
                
            end
            % delete links not in the specified groups
            links(rmlinks,:) = [];

        end
        
    else
        links = [];
    end

end