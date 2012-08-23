function FemmProblem = translategroups_mfemm(FemmProblem, groupnos, x, y)
% translates all nodes, segments, arc segments and block labels which are
% members of the specified group numbers
%
% Syntax
%
% FemmProblem = translategroups_mfemm(FemmProblem, groupnos, x, y)
%
% 

    if nargin < 4
        if numel(x) == 2
            shift = x(:)';
        else
            error('if only 3 arguments are supplied, the third must be a two-element vector');
        end
    else
        if isscalar(x) && isscalar(y)
            shift = [x, y];
        else
            error('If x and y must be scalar values of the desired translation in each axis');
        end
    end
    
    excludenodes = [];
    
    for i = 1:numel(groupnos)
        
        if isfield(FemmProblem, 'Segments')
            
            for ii = 1:numel(FemmProblem.Segments)
                
                if FemmProblem.Segments(ii).InGroup == groupnos(i) && ...
                        ~any(FemmProblem.Segments(ii).n0+1 == excludenodes) && ...
                        ~any(FemmProblem.Segments(ii).n1+1 == excludenodes)
                    
                    FemmProblem.Nodes(FemmProblem.Segments(ii).n0+1).Coords = ...
                        FemmProblem.Nodes(FemmProblem.Segments(ii).n0+1).Coords + shift;
                    
                    FemmProblem.Nodes(FemmProblem.Segments(ii).n1+1).Coords = ...
                        FemmProblem.Nodes(FemmProblem.Segments(ii).n1+1).Coords + shift;
                    
                    excludenodes = [excludenodes, FemmProblem.Segments(ii).n0+1, FemmProblem.Segments(ii).n1+1];
                    
                end
                
            end
            
        end
        
        if isfield(FemmProblem, 'ArcSegments')
            for ii = 1:numel(FemmProblem.ArcSegments)
                
                if FemmProblem.ArcSegments(ii).InGroup == groupnos(i) && ...
                        ~any(FemmProblem.ArcSegments(ii).n0+1 == excludenodes) && ...
                        ~any(FemmProblem.ArcSegments(ii).n1+1 == excludenodes)
                    
                    FemmProblem.Nodes(FemmProblem.ArcSegments(ii).n0+1).Coords = ...
                        FemmProblem.Nodes(FemmProblem.ArcSegments(ii).n0+1).Coords + shift;
                    
                    FemmProblem.Nodes(FemmProblem.ArcSegments(ii).n1+1).Coords = ...
                        FemmProblem.Nodes(FemmProblem.ArcSegments(ii).n1+1).Coords + shift;
                    
                    excludenodes = [excludenodes, FemmProblem.ArcSegments(ii).n0+1, FemmProblem.ArcSegments(ii).n1+1];
                    
                end
                
            end
        end
        
        if isfield(FemmProblem, 'BlockLabels')
            
            for ii = 1:numel(FemmProblem.BlockLabels)
                
                if FemmProblem.BlockLabels(ii).InGroup == groupnos(i)
                    FemmProblem.BlockLabels(ii).Coords = FemmProblem.BlockLabels(ii).Coords + shift;
                end
                
            end
            
        end
    end

    excludenodes = unique(excludenodes);
    
    if isfield(FemmProblem, 'Nodes')
        % now shift any nodes in the groups which have not already been moved
        % when the segments and arc segments were moved
        for i = 1:numel(groupnos)

            for ii = 1:numel(FemmProblem.Nodes)
                
                if FemmProblem.Nodes(ii).InGroup == groupnos(i) && ...
                        ~any(excludenodes == ii)

                    FemmProblem.Nodes(ii).Coords = FemmProblem.Nodes(ii).Coords + shift;

                end
                
            end

        end    
    end
    
end