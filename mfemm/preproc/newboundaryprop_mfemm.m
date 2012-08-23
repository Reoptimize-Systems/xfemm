function BoundaryProp = newboundaryprop_mfemm(Name, BdryType, Arg3, varargin)
% newblocklabel_mfemm:  generates a block label structure for an mfemm
% FemmProblem
% 
% Syntax
% 
% Boundary = newboundary_mfemm(x, y, NBoundaryProps)
% Boundary = newboundary_mfemm(x, y, FemmProblem)
% Boundary = newboundary_mfemm(..., 'Parameter', 'Value')
%
% Description
%
    
    if isstruct(Arg3)
       
        FemmProblem = Arg3;
        
        % get the number of existing nodes, segments, boundaries etc. if any
        elcount = elementcount_mfemm(FemmProblem);
        
        NBoundaryProps = elcount.NBoundaryProps;
        
    elseif isscalar(Arg3)
        
        NBoundaryProps = Arg3;
        
    else
        error('MFEMM:newboundaryprop_mfemm:incorrectarg', ...
            ['Third argument to newboundaryprop_mfemm must be a ', ...
             'FemmProblem Structure or the number of existing ', ...
             'boundaries in the problem.']);
    end
    
    % Create a new boundary structure with default/empty fields
    BoundaryProp = emptyboundaryprops_mfemm();
    
    % Parse the optional arguments
    BoundaryProp = parse_pv_pairs(BoundaryProp, varargin);
    
    % Give it the correct name, with a unique id
    BoundaryProp.Name = sprintf('ID: %d - %s', NBoundaryProps + 1, Name);
    
    BoundaryProp.BdryType = BdryType;
    
end