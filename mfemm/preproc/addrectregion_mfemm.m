function [FemmProblem, seginds, nodeinds, blockind, nodeids, labelloc] = addrectregion_mfemm(FemmProblem, x, y, w, h, BlockProps, SegProps)
% add a rectangular region with a block label and optional segment
% properties
%
% Syntax
%
% [FemmProblem, seginds, nodeinds, blockind, nodeids, labelloc] = ...
%         addrectregion_mfemm(FemmProblem, x, y, w, h, BlockProps, SegProps)
%
% Description
%
% addrectregion_mfemm add a rectangular region with a block label in its
% centre with the specified properties to an existing mfemm FemmProblem
% Structure. The nodes are created anti-clockwise from the bottom left corner,
% and the new segments are added in the same order.
%
% Input
%
%  FemmProblem - existing FemmProblem structure to which new nodes,
%    segments and a block label will be added
%
%  x - x position of lower left hand corner of rectangle
%
%  y - y position of lower left hand corner of rectangle
%
%  w - width of the rectangle (length in x direction)
%
%  h - height of the rectangle (length in y direction)
%
%  BlockProps - structure, containing the properties of the material in
%   the region. It can contain the following fields:
% 
%   BlockType      Name of material property associated with block
%   MaxArea        Maximum desired triangle area throughout block
%   InCircuit      Name of circuit properties associated with block
%   MagDir         Magnetization direction
%   InGroup        Group number of the block
%   Turns          Number of turns associated with block when in circuit
%   IsExternal     Specifies if block label lies in an external region
%
%   Those fields not supplied the fields will have the following default
%   values.
%
%    BlockType = ''     No material (default wil be used)
%    MaxArea = -1       Mesh size chosen automatically
%    InCircuit = ''     Empty string (no cicuits)
%    MagDir = 0         magnet direction pointing to right
%    InGroup = 0        block group number
%    Turns = 1          one turn
%    IsExternal = 0     Block not in external region
%
%  SegProps - optional structure, or array of four structures containing
%    the properties to be applied to the newly created segments. 
%
% Output
%
%  FemmProblem - modified femmproblem with new nodes and segments added
%
%  seginds - indices of new segments
%
%  nodeinds - indices of new nodes
%
%  blockind -  index of new block label
%
%  nodeids - zero-based IDs of new nodes
%
%  labelloc - 2 element vector containing the x and y position of the block
%    label (which is located at the centre of the rectangle).
% 
%
% See also: addrectangle_mfemm, addpolygon_mfemm, addsegments_mfemm

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

    [FemmProblem, seginds, nodeinds, nodeids, labelloc] = ...
                            addrectangle_mfemm(FemmProblem, x, y, w, h);
    
    BlockPropsPVPairs = mfemmdeps.struct2pvpairs(BlockProps);
    
    [FemmProblem, blockind] = addblocklabel_mfemm(FemmProblem, labelloc(1), labelloc(2), BlockPropsPVPairs{:});
    
    if nargin > 6
        
        if numel(SegProps) == 1

            SegProps(1:4) = SegProps;

        elseif numel(SegProps) ~= 4

            error('MFEMM:addrectregion_mfemm:wrongnumsegprops', ...
                'Segprops must be a structure array of size 1 or 4');

        end

        for i = 1:4

            FemmProblem.Segments(seginds(i)) = mfemmdeps.parse_pv_pairs(FemmProblem.Segments(seginds(i)), mfemmdeps.struct2pvpairs(SegProps(i)));

        end

    end


end