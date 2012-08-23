function [FemmProblem, seginds, nodeinds, blockind, nodeids, labelloc] = addrectregion_mfemm(FemmProblem, x, y, w, h, BlockProps, SegProps)
% add a rectangular region with a block label and optional segment
% properties
%
% Syntax
%
% [FemmProblem, seginds, nodeinds, blockind, nodeids, labelloc] = ...
%         addrectregion_mfemm(FemmProblem, x, y, w, h, BlockProps, SegProps)
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

    [FemmProblem, seginds, nodeinds, nodeids, labelloc] = ...
                            addrectangle_mfemm(FemmProblem, x, y, w, h);
    
    BlockPropsPVPairs = struct2pvpairs(BlockProps);
    
    [FemmProblem, blockind] = addblocklabel_mfemm(FemmProblem, labelloc(1), labelloc(2), BlockPropsPVPairs{:});
    
    if nargin > 6
        
        if numel(SegProps) == 1

            SegProps(1:4) = SegProps;

        elseif numel(SegProps) ~= 4

            error('MFEMM:addrectregion_mfemm:wrongnumsegprops', ...
                'Segprops must be a structure array of size 1 or 4');

        end

        for i = 1:4

            FemmProblem.Segments(seginds(i)) = parse_pv_pairs(FemmProblem.Segments(seginds(i)), struct2pvpairs(SegProps(i)));

        end

    end


end