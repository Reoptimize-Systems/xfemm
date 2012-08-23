function [FemmProblem] = removenodes_mfemm(FemmProblem, nodeids)
% remove nodes from an mfemm FemmProblem structure, and any segments or arc
% segments attached to those nodes
%
% Syntax
%
% [FemmProblem] = removenodes_mfemm(FemmProblem, nodeids)
%
%

    orignodeids = nodeids;

    % for each node id to be removed look for segments containing that node
    % id and remove them
    for i = 1:numel(nodeids)
        
        % Modify Segments
        if isfield(FemmProblem, 'Segments')

            FemmProblem.Segments(arrayfun(@(x)isequal(x.n0, nodeids(i)), FemmProblem.Segments)) = [];

            FemmProblem.Segments(arrayfun(@(x)isequal(x.n1, nodeids(i)), FemmProblem.Segments)) = [];

            for ii = 1:numel(FemmProblem.Segments)
                if FemmProblem.Segments(ii).n0 > nodeids(i)
                    FemmProblem.Segments(ii).n0 = FemmProblem.Segments(ii).n0 - 1;
                end
            end
            
            for ii = 1:numel(FemmProblem.Segments)
                if FemmProblem.Segments(ii).n1 > nodeids(i)
                    FemmProblem.Segments(ii).n1 = FemmProblem.Segments(ii).n1 - 1;
                end
            end

        end
        
        % Modify Arc Segments
        if isfield(FemmProblem, 'ArcSegments')

            FemmProblem.ArcSegments(arrayfun(@(x)isequal(x.n0, nodeids(i)), FemmProblem.ArcSegments)) = [];

            FemmProblem.ArcSegments(arrayfun(@(x)isequal(x.n1, nodeids(i)), FemmProblem.ArcSegments)) = [];

            for ii = 1:numel(FemmProblem.ArcSegments)
                if FemmProblem.ArcSegments(ii).n0 > nodeids(i)
                    FemmProblem.ArcSegments(ii).n0 = FemmProblem.ArcSegments(ii).n0 - 1;
                end
            end

            for ii = 1:numel(FemmProblem.ArcSegments)
                if FemmProblem.ArcSegments(ii).n1 > nodeids(i)
                    FemmProblem.ArcSegments(ii).n1 = FemmProblem.ArcSegments(ii).n1 - 1;
                end
            end

        end
        
        nodeids(nodeids > nodeids(i)) = nodeids(nodeids > nodeids(i)) - 1;
        
    end
    
    % remove the nodes
    FemmProblem.Nodes(orignodeids+1) = [];

end