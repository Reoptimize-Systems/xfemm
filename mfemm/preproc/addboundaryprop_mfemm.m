function [FemmProblem, boundind, boundname] = addboundaryprop_mfemm(FemmProblem, Name, BdryType, varargin)
% creates and adds a new boundary to an mfemm FemmProblem structure
% 
% Syntax
% 
% FemmProblem = addboundaryprop_mfemm(FemmProblem, Name, BdryType)
% FemmProblem = addboundaryprop_mfemm(..., 'Parameter', 'Value')
% [..., boundind] = addboundaryprop_mfemm(...)
% [..., boundname] = addboundaryprop_mfemm(...)
%
% Description
%
% 
% addboundaryprop_mfemm() adds a new boundary structure to an existing
% mfemm FemmProblem structure. addboundaryprop_mfemm creates a structure
% containing a number of fields with default values. These will now be
% described with some explanation for each type of boundary condition. For
% more details on the use of these boundary conditions see the manual for
% the original FEMM program (obtainable from www.femm.info). Optional
% arguments for each boundary condition are supplied as parameter-value
% pairs consisting of a string and the value to be assigned to that
% parameter.
%
%   FemmProblem - The Existing mfemm problem structure to which the new
%              boundary condition will be added.
%
%   Name     - A string denoting the name of the boundary condition for
%              later use. this name will be modified to ensure it is unique
%              by adding the string 'ID: X - ' in front of the supplied
%              name string where X is replaced by an iteger value. The
%              value will be one greater than the number of boundaries
%              already present in the problem. The new boundary name will
%              be returned in 'boundname'.
%
%   BdryType - Scalar value determining the boundary type. This can have 
%              the values:
%               
%              0 - Prescribed A type boundary condition
%              1 - Small Skin Depth type boundary condtion
%              2 - Mixed type boundary condition
%              3 - Strategic dual image boundary condition
%              4 - Periodic boundary condition
%              5 - Anti-Perodicboundary condition
%
%              Further discussion of these boundary conditions, and
%              appropriate parameter-value pairs now follows.
%
% Prescribed A Type Boundary Condition Variables
%
%  With this type of boundary condition, the vector potential, A, is
%  prescribed along a given boundary. This boundary condition can be used
%  to prescribe the flux passing normal to a boundary, since the normal
%  flux is equal to the tangential derivative of A along the boundary. The
%  form for A along the boundary is specified via the parameters A0, A1, A2
%  and Phi in the Prescribed A parameters box. If the problem is planar,
%  the parameters correspond to the formula:
%
%  A = (A0 + A1x + A2y)e^{j Phi}
%
%  If the problem type is axisymmetric, the parameters correspond to: 
%
%  A = (A0 + A1r + A2z)e^{j Phi}
%  
%  Relevant Parmater/Value Pairs:
%
%     A0  - A0 coeffictient
%     A1  - A1 coefficient
%     A2  - A2 coefficient
%     Phi - Angle to segment at which the condition will be applied
%
%   For a Prescribed A type boundary condition, set the A0, A1, A2 and Phi
%   parameters as required. Set all other parameters to zero.
%
% Mixed Type Boundary Condition Variables
%
%  This denotes a boundary condition of the form:
%
%  (1/ (mu_0 mu_r)) (dA / dn) + c_0 A + c_1 = 0
%
%  The parameters for this class of boundary condition are specified in the
%  Mixed BC parameters box in the dialog. By the choice of coefficients,
%  this boundary condition can either be a Robin or a Neumann boundary
%  condition. 
%  
%  Relevant Parmater/Value Pairs:
%
%      c0  - asymtotic boundary condition c_0 coefficient (real part),
%            defaults to zero.
%      c0i - asymtotic boundary condition c_0 coefficient (imaginary 
%            part), defaults to zero.
%      c1  - asymtotic boundary condition c_1 coefficient (real part),
%            defautls to zero.
%      c1i - asymtotic boundary condition c_1 coefficient (imaginary
%            part), defautls to zero.
%
% Small Skin Depth Type Boundary Condition Variables
%
%  This boundary condition denotes an interface with a material subject to
%  eddy currents at high enough frequencies such that the skin depth in the
%  material is very small. The result is a Robin boundary condition with
%  complex coefficients of the form:
%
%  (dA / dn) ((1 + j)/delta) A = 0
%
%  where the n denotes the direction of the outward normal to the boundary
%  and d denotes the skin depth of the material at the frequency of
%  interest. The skin depth, D is defined as:
%
%  D = sqrt( 2 / (omega mu_0 mu_r sigma) )
%
%  where mu_r and sigma are the relative permeability and conductivity of
%  the thin skin depth eddy current material. At zero frequency, this
%  boundary condition degenerates to dA/dn = 0 (because skin depth goes to
%  infinity).
%  
%  Relevant Parmater/Value Pairs:
%
%   Mu_ssd      - Small skin depth relative permeability
%   Sigma_ssd   - Small skin depth conductivity
%
% Selecting a boundary
% 
% For a Small Skin Depth type boundary condtion, set Mu_ssd to the desired
% relative permeability and Sigma_ssd to the desired conductivity in MS/m.
% Set BdryFormat to 1 and all other parameters to zero.
%
% To obtain a Mixed type boundary condition, set C1 and C0 as required and
% BdryFormat to 2. Set all other parameters to zero (the defaults).
%
% For a Strategic dual image boundary, set BdryFormat to 3 and set all
% other parameters to zero, or supply no p-v pairs to have
% addboundaryprop_mfemm do this for you.
%
% For a Periodic boundary condition, set BdryFormat to 4 and set all other
% parameters to zero, or supply no p-v pairs to have addboundaryprop_mfemm
% do this for you.
%
% For an Anti-Perodic set BdryFormat to 5 and set all other
% parameters to zero, or supply no p-v pairs to have addboundaryprop_mfemm
% do this for you.
%
%
% See also: newboundaryprop_mfemm.m, emptyboundaryprops_mfemm
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

    % get the number of existing boundaries etc. if any
    elcount = elementcount_mfemm(FemmProblem);

    NBoundaryProps = elcount.NBoundaryProps;
    
    % Create a new boundary with a unique id if possible
    BoundaryProp = newboundaryprop_mfemm(Name, BdryType, NBoundaryProps, varargin{:});
    
    if ~isfield(FemmProblem, 'BoundaryProps') || NBoundaryProps == 0
        
        FemmProblem.BoundaryProps = BoundaryProp;
        
        boundind = 1;
        
    else
        
        boundind = numel(FemmProblem.BoundaryProps) + 1;
        
        FemmProblem.BoundaryProps(boundind) = BoundaryProp;
        
    end
    
    boundname = BoundaryProp.Name;
    
end