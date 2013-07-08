function FemmProblem = deletenode_mfemm(FemmProblem, index)
% deletes a node from an mfemm FemmProblem structure, also deleting any
% connected segments and arc segments

    % First delete all segments connected to the deleted node

    rminds = [];

    for indi = 1:numel(FemmProblem.Segments)

        if FemmProblem.Segments(indi).n0 == indi-1 ...
                || FemmProblem.Segments(indi).n1 == indi-1

            rminds = [ rminds, indi ];

        end

    end

    FemmProblem.Segments(rminds) = [];

    % delete all arc segments connected to the deleted node
    rminds = [];
    
    for indi = 1:numel(FemmProblem.ArcSegments)
        
        if FemmProblem.ArcSegments(indi).n0 == indi-1 ...
                || FemmProblem.ArcSegments(indi).n1 == indi-1
            
            rminds = [ rminds, indi ];
            
        end
        
    end
    
    FemmProblem.ArcSegments(rminds) = [];
    
    % now delete the node 
    FemmProblem.Nodes(index) = [];
    
    % and renumber any remaining segments and arc segments if necessary
    for indi = 1:numel(FemmProblem.Segments)
        
        if FemmProblem.Segments(indi).n0 > index
            
            FemmProblem.Segments(indi).n0 = FemmProblem.Segments(indi).n0 - 1;
            
        end
        
        if FemmProblem.Segments(indi).n1 > index
            
            FemmProblem.Segments(indi).n1 = FemmProblem.Segments(indi).n1 - 1;
            
        end
        
    end
    
    for indi = 1:numel(FemmProblem.ArcSegments)
        
        if FemmProblem.ArcSegments(indi).n0 > index
            
            FemmProblem.ArcSegments(indi).n0 = FemmProblem.ArcSegments(indi).n0 - 1;
            
        end
        
        if FemmProblem.ArcSegments(indi).n1 > index
            
            FemmProblem.ArcSegments(indi).n1 = FemmProblem.ArcSegments(indi).n1 - 1;
            
        end
        
    end
    

end