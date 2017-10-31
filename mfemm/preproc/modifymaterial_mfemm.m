function FemmProblem = modifymaterial_mfemm (FemmProblem, matname, propname, newval)

    foundmat = false;
    
    for ind = 1:numel (FemmProblem.Materials)
        
        if strcmp (matname, FemmProblem.Materials(ind).Name)
            foundmat = true;
            
            FemmProblem.Materials(ind).(propname) = newval;
            
        end
        
    end
    
    if foundmat == false
        error ('Maerial not found in FemmProblem');
    end

end