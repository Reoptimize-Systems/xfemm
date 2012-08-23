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
    
% Copyright 2012 Richard Crozier
% 
%    Licensed under the Apache License, Version 2.0 (the "License");
%    you may not use this file except in compliance with the License.
%    You may obtain a copy of the License at
% 
%        http://www.apache.org/licenses/LICENSE-2.0
% 
%    Unless required by applicable law or agreed to in writing, software
%    distributed under the License is distributed on an "AS IS" BASIS,
%    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%    See the License for the specific language governing permissions and
%    limitations under the License.

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