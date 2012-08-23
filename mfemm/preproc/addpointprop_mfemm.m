function [FemmProblem, ppropind] = addpointprop_mfemm(FemmProblem, PPropName, varargin)
% adds a new point property to an mfemm FemmProblem structure

    PointProp = newpointprop_mfemm(PPropName, varargin{:});
    
    if ~isfield(FemmProblem, 'PointProps') || isempty(FemmProblem.PointProps)
        
        FemmProblem.PointProps = PointProp;
        
        ppropind = 1;
        
    else
        
        ppropind = numel(FemmProblem.PointProps) + 1;
        
        FemmProblem.PointProps(ppropind) = PointProp;
        
    end
    
end

