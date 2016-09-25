function FemmProblem = modifyblockprop_mfemm (FemmProblem, blockname, propname, newval)

    foundblock = false;
    
    for ind = 1:numel (FemmProblem.BlockLabels)
        
        if strcmp (blockname, FemmProblem.BlockLabels(ind).BlockType)
            foundblock = true;
            
            FemmProblem.BlockLabels(ind).(propname) = newval;
            
        end
        
    end
    
    if foundblock == false
        error ('Block label not found in FemmProblem');
    end

end