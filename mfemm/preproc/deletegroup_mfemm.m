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
    for indi = 1:numel(FemmProblem.Nodes)
        
        if FemmProblem.Nodes(ind).GroupNo == groupno
            
            rminds = [ rminds, indii ];
            
        end
        
    end
    
    for ind = 1:numel(rminds)
        FemmProblem = deletenode_mfemm(FemmProblem, rmind(ind));
    end
    
    % delete segments in the group
    rminds = [];
    for indi = 1:numel(FemmProblem.Segments)
        
        if FemmProblem.Segments(ind).GroupNo == groupno
            
            rminds = [ rminds, indii ];
            
        end
        
    end
    
    FemmProblem.Segments(rminds) = [];
    
    % delete segments in the group
    rminds = [];
    for indi = 1:numel(FemmProblem.Segments)
        
        if FemmProblem.Segments(ind).GroupNo == groupno
            
            rminds = [ rminds, indii ];
            
        end
        
    end
    
    FemmProblem.Segments(rminds) = [];
    
    % delete arc segments in the group
    rminds = [];
    for indi = 1:numel(FemmProblem.ArcSegments)
        
        if FemmProblem.ArcSegments(ind).GroupNo == groupno
            
            rminds = [ rminds, indii ];
            
        end
        
    end
    
    FemmProblem.ArcSegments(rminds) = [];
    
    % delete labels in the group
    rminds = [];
    for indi = 1:numel(FemmProblem.BlockLabels)
        
        if FemmProblem.BlockLabels(ind).GroupNo == groupno
            
            rminds = [ rminds, indii ];
            
        end
        
    end
    
    FemmProblem.BlockLabels(rminds) = [];

end