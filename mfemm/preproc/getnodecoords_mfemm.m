function nodecoords = getnodecoords_mfemm(FemmProblem)
% getnodes_femm: gets all the node locations from an mfemm problem
% structure

    nodecoords = cell2mat({FemmProblem.Nodes(:).Coords}');

end