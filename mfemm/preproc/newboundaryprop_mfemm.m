function BoundaryProp = newboundaryprop_mfemm(Name, BdryType, varargin)
% newblocklabel_mfemm:  generates a boundary property structure for an
% mfemm FemmProblem structure
% 
% Syntax
% 
% Boundary = newboundaryprop_mfemm(Name, BdryType);
% Boundary = newboundaryprop_mfemm(Name, BdryType, NBoundaryProps)
% Boundary = newboundaryprop_mfemm(Name, BdryType, FemmProblem)
% Boundary = newboundaryprop_mfemm(..., 'Parameter', 'Value')
%
% Description
%
% newboundaryprop_mfemm() creates a new boundary structure for an mfemm
% FemmProblem structure. newboundaryprop_mfemm(Name, BdryType) creates a
% structure containing the following fields:
%
%   Name        - Replaced with contents of 'Name', a string supplied by
%                 the user
%   BdryType    - Scalar value determining the boundary type. This can have 
%                 the values  
%   A_0 = 0;
%   A_1 = 0;
%   A_2 = 0;
%   Phi = 0;
%   c0 = 0;
%   c0i = 0;
%   c1 = 0;
%   c1i = 0;
%   Mu_ssd      - Small skin depth relative permeability
%   Sigma_ssd   - Small skin depth conductivity
%
%
% For a Prescribed A type boundary condition, set the A0, A1, A2 and Phi
% parameters as required. Set all other parameters to zero.
%
% For a Small Skin Depth type boundary condtion, set Mu_ssd to the desired
% relative permeability and Sigma_ssd to the desired conductivity in MS/m.
% Set BdryFormat to 1 and all other parameters to zero.
%
% To obtain a Mixedtype boundary condition, set C1 and C0 as required and
% BdryFormat to 2. Set all other parameters to zero (the defaults).
%
% For a Strategic dual imageboundary, set BdryFormat to 3 and set all other
% parameters to zero, or supply no p-v pairs to have newboundaryprop_mfemm
% do this for you.
%
% For a Periodicboundary condition, set BdryFormat to 4 and set all other
% parameters to zero, or supply no p-v pairs to have newboundaryprop_mfemm
% do this for you.
%
% For an Anti-Perodic set BdryFormat to 5 and set all other
% parameters to zero, or supply no p-v pairs to have newboundaryprop_mfemm
% do this for you.
%
% See also: addboundaryprop_mfemm.m, emptyboundaryprops_mfemm.m
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


    Arg3 = varargin{1};
    
    if isstruct(Arg3)
       
        FemmProblem = Arg3;
        
        % get the number of existing nodes, segments, boundaries etc. if any
        elcount = elementcount_mfemm(FemmProblem);
        
        NBoundaryProps = elcount.NBoundaryProps;
        makeid = true;
        varargin = varargin(2:end);
        
    elseif ~ischar(Arg3) && isscalar(Arg3)
        
        NBoundaryProps = Arg3;
        makeid = true;
        varargin = varargin(2:end);
        
    else
        makeid = false;
    end
    
    % Create a new boundary structure with default/empty fields
    BoundaryProp = emptyboundaryprops_mfemm();
    
    switch BdryType
        
        case 3
            BoundaryProp.BdryType = 3;
            
        case 4
            BoundaryProp.BdryType = 4;
            
        case 5
            BoundaryProp.BdryType = 5;
            
        otherwise
            
            % Parse the optional arguments
            BoundaryProp = parse_pv_pairs(BoundaryProp, varargin);
            
    end
    
    
    if makeid
        % Give it the correct name, with a unique id
        BoundaryProp.Name = sprintf('ID: %d - %s', NBoundaryProps + 1, Name);
    else
        % Use the name passed in, withut making it unique
        BoundaryProp.Name = Name;
    end
    
    BoundaryProp.BdryType = BdryType;
    
end
