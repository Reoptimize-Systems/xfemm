function BlockLabel = newblocklabel_mfemm(x, y, varargin)
% newblocklabel_mfemm:  generates a block label structure for an mfemm
% FemmProblem
% 
% Syntax
% 
% BlockLabel = newblocklabel_mfemm(x, y, 'Parameter', 'Value', ...)
% 
% Descriptions
%
% newblocklabel_mfemm creates a structure containng the appropriate fields for
% a BlockLabel for an mfemm FemmProblem Structure. The output structure will
% contain the following fields:
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
%   IsDefault      Specifies if block is to be used as the default material 
%                  for blocks where no material is specified
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
%    IsDefault = 0      Block not default material
%    MagDirFctn = ''    No magnetisation function string
%    IsMagnet = false   Flag indicating if block is magnet
%
% See also: addblocklabel_mfemm.m, addrectregion_mfemm.m
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

    BlockLabel.BlockType = '';
    BlockLabel.MaxArea = -1;
    BlockLabel.InCircuit = '';
    BlockLabel.MagDir = NaN;
    BlockLabel.InGroup = 0;
    BlockLabel.Turns = 1;
    BlockLabel.IsExternal = 0;
    BlockLabel.IsDefault = 0;
    BlockLabel.MagDirFctn = '';
    BlockLabel.IsMagnet = false;
    
    BlockLabel = mfemmdeps.parseoptions(BlockLabel, varargin);

    if ischar(BlockLabel.MagDir)
        BlockLabel.MagDirFctn = BlockLabel.MagDir;
        BlockLabel.MagDir = 0;
    end
    
    BlockLabel.Coords = [x, y];
    
end