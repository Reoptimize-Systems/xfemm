function [FemmProblem, blockind] = addblocklabel_mfemm(FemmProblem, x, y, varargin)
% adds a new block label to an mfemm FemmProblem structure
%
% Syntax
%
% [FemmProblem, blockind] = addblocklabel_mfemm(FemmProblem, x, y, 'Parameter', 'Value')
% 
% Descriptions
%
% addblocklabel_mfemm add a structure containng the appropriate fields for
% a BlockLabel to an existing mfemm FemmProblem Structure. The output
% structure will contain the following fields:
% 
%   x              x-coordinate of label
%   y              y-coordinate of label
%   BlockType      Name of material property associated with block
%   MaxArea        Maximum desired triangle area throughout block
%   InCircuit      Name of circuit properties associated with block
%   MagDir         Magnetization direction
%   InGroup        Group number of the block
%   Turns          Number of turns associated with block when in circuit
%   IsExternal     Specifies if block label lies in an external region
%
% x and y are the coordinates of the block label. Values of the other
% structure fields can be supplied by passing in the field name and value
% as parameter-value pairs. If not supplied the fields will have the
% following default values.
%
%    BlockType = ''     No material (default wil be used)
%    MaxArea = -1       Mesh size chosen automatically
%    InCircuit = ''     Empty string (no cicuits)
%    MagDir = 0         magnet direction pointing to right
%    InGroup = 0        block group number
%    Turns = 1          one turn
%    IsExternal = 0     Block not in external region
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

    BlockLabel = newblocklabel_mfemm(x, y, varargin{:});
    
    if ~isfield(FemmProblem, 'BlockLabels') || isempty(FemmProblem.BlockLabels)
        
        FemmProblem.BlockLabels = BlockLabel;
        
        blockind = 1;
        
    else
        
        blockind = numel(FemmProblem.BlockLabels) + 1;
        
        FemmProblem.BlockLabels(blockind) = BlockLabel;
        
    end
    
end