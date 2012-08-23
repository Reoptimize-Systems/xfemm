function [FemmProblem, blockind] = addblocklabel_mfemm(FemmProblem, x, y, varargin)
% adds a new block label to an mfemm FemmProblem structure
%
% Syntax
%
% [FemmProblem, blockind] = addblocklabel_mfemm(FemmProblem, x, y, 'Parameter', 'Value')
% 
%

    BlockLabel = newblocklabel_mfemm(x, y, varargin{:});
    
    if ~isfield(FemmProblem, 'BlockLabels') || isempty(FemmProblem.BlockLabels)
        
        FemmProblem.BlockLabels = BlockLabel;
        
        blockind = 1;
        
    else
        
        blockind = numel(FemmProblem.BlockLabels) + 1;
        
        FemmProblem.BlockLabels(blockind) = BlockLabel;
        
    end
    
end