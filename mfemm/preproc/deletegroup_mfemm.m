function FemmProblem = deletegroup_mfemm(FemmProblem, groupno)
% removes all elements from an mfemm FemmProblem structure which have a
% given group number
%
% Syntax
%
% FemmProblem = deletegroup_mfemm(FemmProblem, groupno)
%
% Description
%
%

% Created by Richard Crozier 2013

    % Delete all nodes in the group
    rminds = [];
    for ind = 1:numel(FemmProblem.Nodes)
        
        if FemmProblem.Nodes(ind).InGroup == groupno
            
            rminds = [ rminds, ind ];
            
        end
        
    end
    
    for ind = 1:numel(rminds)
        FemmProblem = deletenode_mfemm(FemmProblem, rminds(ind)-1);
        rminds(rminds > rminds(ind)) = rminds(rminds > rminds(ind)) - 1;
    end
    
    % delete segments in the group
    rminds = [];
    for ind = 1:numel(FemmProblem.Segments)
        
        if FemmProblem.Segments(ind).InGroup == groupno
            
            rminds = [ rminds, ind ];
            
        end
        
    end
    
    FemmProblem.Segments(rminds) = [];
    
    % delete arc segments in the group
    rminds = [];
    for ind = 1:numel(FemmProblem.ArcSegments)
        
        if FemmProblem.ArcSegments(ind).InGroup == groupno
            
            rminds = [ rminds, ind ];
            
        end
        
    end
    
    FemmProblem.ArcSegments(rminds) = [];
    
    % delete labels in the group
    rminds = [];
    for ind = 1:numel(FemmProblem.BlockLabels)
        
        if FemmProblem.BlockLabels(ind).InGroup == groupno
            
            rminds = [ rminds, ind ];
            
        end
        
    end
    
    FemmProblem.BlockLabels(rminds) = [];

end