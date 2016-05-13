function [nodecoords, originds] = getnodecoords_mfemm(FemmProblem, varargin)
% getnodes_femm: gets all the node locations from an mfemm problem
% structure
%
% Syntax
%
% nodecoords = getnodecoords_mfemm(FemmProblem)
%
% Input 
%
%  FemmProblem - mfemm FemmProblem structure from which to extract the node
%    coordinates
%
% Output
%
%  nodecoords - (n x 2) matrix containing the node coordinates
%
%
% See also: getnodelinks_mfemm.m, getseglinks_mfemm,
%           getsegnodecoords_mfemm, getsegmidpointcoords_mfemm
%           

% Copyright 2012-2014 Richard Crozier
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
    
    if isfield(FemmProblem, 'Nodes') && ~isempty (FemmProblem.Nodes)
        
        nodecoords = cell2mat({FemmProblem.Nodes(:).Coords}');
        originds = 1:size (nodecoords, 1);
        
        if ~isempty(Inputs.Groups)
            
            rmnodes = [];
            for ind = 1:numel (FemmProblem.Nodes)

                if ~(any(Inputs.Groups == FemmProblem.Nodes(ind).InGroup))
                    rmnodes = [ rmnodes, ind ];
                end

            end
            
            % delete nodes not in the specified groups
            nodecoords(rmnodes,:) = [];
            originds(rmnodes) = [];

        end
        
    else
        nodecoords = [];
        originds = [];
    end

end