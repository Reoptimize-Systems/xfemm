function [links, nodes] = getnodelinks_mfemm(FemmProblem, varargin)
% getnodelinks_femm: gets all the nodes from an mfemm FemmProblem structure
% and the links between them
%
% Syntax
% 
% [links, nodes] = getnodelinks_mfemm(FemmProblem)
%
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
    
    links = getseglinks_mfemm(FemmProblem, Inputs);
    
    [nodes, originds] = getnodecoords_mfemm(FemmProblem, Inputs);
    
    if ~isempty (Inputs.Groups)
       
        % renumber the links to reflect new node indices
        temp_links = ones (size (links)) * nan;
        
        for ind = 1:numel (originds)

            temp_links (links == originds(ind)) = ind - 1;

        end
        
        links = temp_links;
        
    end

end