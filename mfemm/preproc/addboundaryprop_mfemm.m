function [FemmProblem, ind] = addboundaryprop_mfemm(FemmProblem, Name, BdryType, varargin)
% creates and adds a new boundary to an mfemm FemmProblem structure
% 
% Syntax
% 
% FemmProblem = addboundaryprop_mfemm(FemmProblem, Name, BdryType)
% FemmProblem = addboundaryprop_mfemm(..., 'Parameter', 'Value')
%
% Description
%

    
    % get the number of existing boundaries etc. if any
    elcount = elementcount_mfemm(FemmProblem);

    NBoundaryProps = elcount.NBoundaryProps;
    
    % Create a new boundary with a unique id if possible
    BoundaryProp = newboundaryprop_mfemm(Name, BdryType, NBoundaryProps, varargin{:});
    
    if ~isfield(FemmProblem, 'BoundaryProps') || NBoundaryProps == 0
        
        FemmProblem.BoundaryProps = BoundaryProp;
        
        ind = 1;
        
    else
        
        ind = numel(FemmProblem.BoundaryProps) + 1;
        
        FemmProblem.BoundaryProps(ind) = BoundaryProp;
        
    end
    
end