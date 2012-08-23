function labelcoords = getblocklabelcoords_mfemm(FemmProblem)
% getblocklabelcoords_mfemm: gets all the block label locations from an
% mfemm problem structure

    labelcoords = cell2mat({FemmProblem.BlockLabels(:).Coords}');

end