function [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, x, y, varargin)
% adds one or more nodes to an mfemm FemmProblem structure
%
% Syntax
%
% [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, x, y, varargin)
%
%

    nodeinds = repmat(-1, 1, numel(x));
    
    if ~isfield(FemmProblem, 'Nodes') || isempty(FemmProblem.Nodes)
        
        FemmProblem.Nodes = newnode_mfemm(x(1), y(1), varargin{:});
        nodeinds(1) = 1;
        
    else
        
        nodeinds(1) = numel(FemmProblem.Nodes) + 1;
        FemmProblem.Nodes(nodeinds(1)) = newnode_mfemm(x(1), y(1), varargin{:});
        
    end
    
    for i = 2:numel(x)
        
        nodeinds(i) = nodeinds(i-1) + 1;
        
        FemmProblem.Nodes(nodeinds(i)) = newnode_mfemm(x(i), y(i), varargin{:});      
        
    end

    nodeids = nodeinds - 1;
    
end