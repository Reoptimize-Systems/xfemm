function segcoords = getsegmidpointcoords_mfemm(FemmProblem)
% getblocklabelcoords_mfemm: gets all the block label locations from an
% mfemm problem structure

    segn0coords = cell2mat({FemmProblem.Nodes(cell2mat({FemmProblem.Segments(:).n0})).Coords});
    
    segn1coords = cell2mat({FemmProblem.Nodes(cell2mat({FemmProblem.Segments(:).n1})).Coords});
    
    % mid-point is mean value of x and y points
    segcoords = [mean([segn0coords(:,1), segn1coords(:,1)], 1), mean([segn0coords(:,2), segn1coords(:,2)], 1)];

end