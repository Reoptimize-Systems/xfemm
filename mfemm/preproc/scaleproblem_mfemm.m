function FemmProblem = scaleproblem_mfemm(FemmProblem, x, y)
% scales an mfemm problem

    for i = 1:numel(FemmProblem.Nodes)
        FemmProblem.Nodes(i).Coords = FemmProblem.Nodes(i).Coords .* [x, y];
    end

    for i = 1:numel(FemmProblem.BlockLabels)
        FemmProblem.BlockLabels(i).Coords = FemmProblem.BlockLabels(i).Coords .* [x, y];
    end
    
end