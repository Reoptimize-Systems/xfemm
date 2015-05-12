function [FemmProblem, seginds, aseginds, nodeinds, blockind, nodeids, labelloc] = ...
            addcurvedtrapezoidregion_mfemm (FemmProblem, Ri, Ro, angi, ango, BlockProps, SegProps)
% add a rectangular region with a block label and optional segment
% properties
%
% Syntax
%
% [FemmProblem, seginds, nodeinds, blockind, nodeids, labelloc] = ...
%         addcurvedtrapezoidregion_mfemm (FemmProblem, Ri, Ro, angi, ango, BlockProps, SegProps)
%
% Description
%
% addcurvedtrapezoidregion_mfemm adds a curved trapezoidal region with a
% block label in its centre with the specified properties to an existing
% mfemm FemmProblem Structure.
%
% BlockProps is a structure, containing the properties of the material in
% the region. It can contain the following fields:
% 
%   BlockType      Name of material property associated with block
%   MaxArea        Maximum desired triangle area throughout block
%   InCircuit      Name of circuit properties associated with block
%   MagDir         Magnetization direction
%   InGroup        Group number of the block
%   Turns          Number of turns associated with block when in circuit
%   IsExternal     Specifies if block label lies in an external region
%
% Those fields not supplied the fields will have the following default
% values.
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

    [FemmProblem, seginds, aseginds, nodeinds, nodeids, labelloc] = ...
                            addcurvedtrapezoid_mfemm (FemmProblem, Ri, Ro, angi, ango);
    
    BlockPropsPVPairs = mfemmdeps.struct2pvpairs (BlockProps);
    
    [FemmProblem, blockind] = addblocklabel_mfemm (FemmProblem, labelloc(1), labelloc(2), BlockPropsPVPairs{:});
    
    if nargin > 6
        
        if numel (SegProps) == 1

            SegProps(1:4) = SegProps;

        elseif numel (SegProps) ~= 4

            error ('MFEMM:addcurvedtrapezoidregion_mfemm:wrongnumsegprops', ...
                'Segprops must be a structure array of size 1 or 4');

        end

        FemmProblem.Segments(seginds(1)) = mfemmdeps.parse_pv_pairs (FemmProblem.Segments(seginds(1)), mfemmdeps.struct2pvpairs (SegProps(1)));
        FemmProblem.Segments(seginds(2)) = mfemmdeps.parse_pv_pairs (FemmProblem.Segments(seginds(2)), mfemmdeps.struct2pvpairs (SegProps(2)));
        FemmProblem.ArcSegments(aseginds(1)) = mfemmdeps.parse_pv_pairs (FemmProblem.ArcSegments(aseginds(1)), mfemmdeps.struct2pvpairs (SegProps(3)));
        FemmProblem.ArcSegments(aseginds(2)) = mfemmdeps.parse_pv_pairs (FemmProblem.ArcSegments(aseginds(2)), mfemmdeps.struct2pvpairs (SegProps(4)));

    end

end