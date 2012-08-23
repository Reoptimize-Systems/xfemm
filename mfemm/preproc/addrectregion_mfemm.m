function [FemmProblem, seginds, nodeinds, blockind, nodeids, labelloc] = addrectregion_mfemm(FemmProblem, x, y, w, h, BlockProps, SegProps)
% add a rectangular region with a block label and optional segment
% properties
%
% Syntax
%
% [FemmProblem, seginds, nodeinds, blockind, nodeids, labelloc] = ...
%         addrectregion_mfemm(FemmProblem, x, y, w, h, BlockProps, SegProps)
%

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