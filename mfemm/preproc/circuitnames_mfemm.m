function cnames = circuitnames_mfemm (FemmProblem)
% returns the names of all circuits in a FemmProblem structure

    cnames = {};
    
    for ind = 1:numel(FemmProblem.Circuits)
        cnames = [ cnames, {FemmProblem.Circuits(ind).Name} ];
    end

end