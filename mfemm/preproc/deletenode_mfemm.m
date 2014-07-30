function FemmProblem = deletenode_mfemm(FemmProblem, ids)
% deletes a node from an mfemm FemmProblem structure, also deleting any
% connected segments and arc segments
%
% Syntax
%
% FemmProblem = deletenode_mfemm(FemmProblem, index)
%
% Input
%
%  FemmProblem - mfemm FemmProblem structure
%
%  ids - matrix of one or more nodes to be deleted
%
%

    % if more than one node is to be deleted, call this function for each
    if numel (ids) > 1
        for ind = 1:numel (ids)
            % reduce the node ids in the list bigger than the one we're about 
            % to remove by one so they are numbered correctly
            ids(ids > ids(ind)) = ids(ids > ids(ind)) - 1;
            FemmProblem = deletenode_mfemm (FemmProblem, ids(ind));
        end
        return;
    end
    
    % First delete all segments connected to the deleted node

    rminds = [];

    for indi = 1:numel(FemmProblem.Segments)

        if FemmProblem.Segments(indi).n0 == ids ...
                || FemmProblem.Segments(indi).n1 == ids

            rminds = [ rminds, indi ];

        end

    end

    FemmProblem.Segments(rminds) = [];

    % delete all arc segments connected to the deleted node
    rminds = [];
    
    for indi = 1:numel(FemmProblem.ArcSegments)
        
        if FemmProblem.ArcSegments(indi).n0 == ids ...
                || FemmProblem.ArcSegments(indi).n1 == ids
            
            rminds = [ rminds, indi ];
            
        end
        
    end
    
    FemmProblem.ArcSegments(rminds) = [];
    
    % now delete the node 
    FemmProblem.Nodes(ids+1) = [];
    
    % and renumber any remaining segments and arc segments if necessary
    for indi = 1:numel(FemmProblem.Segments)
        
        if FemmProblem.Segments(indi).n0 > ids
            
            FemmProblem.Segments(indi).n0 = FemmProblem.Segments(indi).n0 - 1;
            
        end
        
        if FemmProblem.Segments(indi).n1 > ids
            
            FemmProblem.Segments(indi).n1 = FemmProblem.Segments(indi).n1 - 1;
            
        end
        
    end
    
    for indi = 1:numel(FemmProblem.ArcSegments)
        
        if FemmProblem.ArcSegments(indi).n0 > ids
            
            FemmProblem.ArcSegments(indi).n0 = FemmProblem.ArcSegments(indi).n0 - 1;
            
        end
        
        if FemmProblem.ArcSegments(indi).n1 > ids
            
            FemmProblem.ArcSegments(indi).n1 = FemmProblem.ArcSegments(indi).n1 - 1;
            
        end
        
    end
    

end