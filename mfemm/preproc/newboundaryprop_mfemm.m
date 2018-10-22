function BoundaryProp = newboundaryprop_mfemm(Name, BdryType, varargin)
% generates a boundary property structure for an mfemm FemmProblem
% structure
% 
% Syntax
% 
% Boundary = newboundaryprop_mfemm(Name, BdryType);
% Boundary = newboundaryprop_mfemm(Name, BdryType, NBoundaryProps)
% Boundary = newboundaryprop_mfemm(Name, BdryType, FemmProblem)
% Boundary = newboundaryprop_mfemm(..., 'Parameter', Value)
%
% Description
%
% newboundaryprop_mfemm() creates a new boundary structure for an mfemm
% FemmProblem structure. newboundaryprop_mfemm(Name, BdryType) creates a
% structure containing a number of fields with default values. These will
% now be described with some explanation for each type of boundary
% condition. For more details on the use of these boundary conditions see
% the manual for the original FEMM program (obtainable from www.femm.info).
% Optional arguments for each boundary condition are supplied as
% parameter-value pairs consisting of a string and the value to be assigned
% to that parameter. 
%
%   Name     - A string denoting the name of the new boundary. If
%              'NBoundaryProps' or 'FemmProblem' is also supplied, this
%              name will be modified to ensure it is unique by adding the
%              string 'ID: X - ' in front of the supplied name string where
%              X is replaced by an iteger value. If NBoundaryProps is
%              supplied the value of this integer wil be NBoundaryProps +
%              1. If a FemmProblem is supplied, the value will be one
%              greater than the number of boundaries already present in the
%              problem. The supplied boundayr name will not be changed if
%              only Name, BdryType and parameter-value pairs are supplied.
%
%   BdryType - Scalar value determining the boundary type. This can have 
%              values between 0 and 5. For magnetics problems, these numbers
%              have the following meanings:
%               
%              0 - Prescribed A type boundary condition
%              1 - Small Skin Depth type boundary condtion
%              2 - Mixed type boundary condition
%              3 - Strategic dual image boundary condition
%              4 - Periodic boundary condition
%              5 - Anti-Perodicboundary condition
%              6 - Periodic Air gap boundary
%              7 - Anti-Periodic Air gap boundary
%
%              For heat flow problems, these numbers have the follwoing
%              meanings:
%
%              0 - Fixed temperature type boundary condition
%              1 - Heat flux type boundary condition
%              2 - Convection boundary condition
%              3 - Radiation boundary condition
%              4 - Periodic boundary condition
%              5 - Anti-Perodic boundary condition
%
%              Further discussion of these boundary conditions, and
%              appropriate parameter-value pairs in each case now follows.
%
%                     Magnetics Problem Boundaries
%                     ----------------------------
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
%     'A0'  - A0 coeffictient
%     'A1'  - A1 coefficient
%     'A2'  - A2 coefficient
%     'Phi' - Angle to segment at which the condition will be applied
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
%      'c0'  - asymtotic boundary condition c_0 coefficient (real part),
%            defaults to zero.
%      'c0i' - asymtotic boundary condition c_0 coefficient (imaginary 
%            part), defaults to zero.
%      'c1'  - asymtotic boundary condition c_1 coefficient (real part),
%            defautls to zero.
%      'c1i' - asymtotic boundary condition c_1 coefficient (imaginary
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
%   'Mu_ssd'     - Small skin depth relative permeability
%   'Sigma_ssd'  - Small skin depth conductivity
%
% Selecting a magnetics boundary
% 
% For a Small Skin Depth type boundary condtion, set Mu_ssd to the desired
% relative permeability and Sigma_ssd to the desired conductivity in MS/m.
% Set BdryFormat to 1 and all other parameters to zero.
%
% To obtain a Mixed type boundary condition, set C1 and C0 as required and
% BdryFormat to 2. Set all other parameters to zero (the defaults).
%
% For a Strategic dual image boundary, set BdryFormat to 3 and set all other
% parameters to zero, or supply no p-v pairs to have newboundaryprop_mfemm
% do this for you.
%
% For a Periodic boundary condition, set BdryFormat to 4 and set all other
% parameters to zero, or supply no p-v pairs to have newboundaryprop_mfemm
% do this for you.
%
% For an Anti-Perodic set BdryFormat to 5 and set all other
% parameters to zero, or supply no p-v pairs to have newboundaryprop_mfemm
% do this for you.
%
% 
%                     Heat flow Problem Boundaries
%                     ----------------------------
%
% Fixed Temperature Boundary
%
% Sets the temperature along the boundayr to a fixed value
%
% Heat flux boundary
%
% The heat flux q across the boundary is prescribed such that
%
%  k dT/dn + q = 0
%
% where n is a vector normal to the boundary, k is the thermal conductivity of
% the material and q is the flux crossing the boundary.
%
% Convection boundary
%
% Represents a surface cooled by a fluid flow. This is represented by the
% equation
%
%  k dT/dN + h (T - T0) = 0
% 
% where h is the heat transfer coefficient and T0 is the ambient cooling fluid
% temperature
%
% Radiation
%
% Represents heat radiated from the surface of a body. Mathmatically this is 
% described using
%
%  k dT/dN + beta ksb (T^4 - T0^4) = 0
%
% where beta is the emissivity of the surface (a dimensionless value between 0
% and 1) and ksb is the Stefan-Boltzmann constant.
%
% Selecting a heat flow boundary
%
% 
% For a Fixed Temperature boundary condtion, set Tset to the desired
% temperature. Set BdryFormat to 0 and all other parameters to zero.
%
% To obtain a Heat Flux boundary condition, set qs to the desired W/m^2 on the
% boundary and BdryFormat to 1. Set all other parameters to zero (the defaults).
%
% For a Convection boundary, set h to the desired heat flow coefficient and T0
% to the ambient temperature of the convecting fluid. Set BdryFormat to 2 and 
% set all other parameters to zero.
%
% For a Radiation boundary set beta to the desired emissivity constant and T0
% to the ambient temperature. Set BdryFormat to 3.
%
% For a Periodic boundary condition, set BdryFormat to 4 and set all other
% parameters to zero, or supply no p-v pairs to have newboundaryprop_mfemm
% do this for you.
%
% For an Anti-Perodic set BdryFormat to 5 and set all other
% parameters to zero, or supply no p-v pairs to have newboundaryprop_mfemm
% do this for you.
%
%
%
% See also: addboundaryprop_mfemm.m, emptyboundaryprops_mfemm.m
%
    
% Copyright 2012-2015 Richard Crozier
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

    if nargin > 2
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
    
    else
        makeid = false;
    end
    
    % Create a new boundary structure with default/empty fields
    BoundaryProp = emptyboundaryprop_mfemm();
    
    switch BdryType
        
        case 3
            BoundaryProp.BdryType = 3;
            
        case 4
            BoundaryProp.BdryType = 4;
            
        case 5
            BoundaryProp.BdryType = 5;
            
        otherwise
            
            % Parse the optional arguments
            BoundaryProp = mfemmdeps.parse_pv_pairs(BoundaryProp, varargin);
            
    end
    
    
    if makeid
        % Give it the correct name, with a unique id
        BoundaryProp.Name = sprintf('ID: %d - %s', NBoundaryProps + 1, Name);
    else
        % Use the name passed in, without making it unique
        BoundaryProp.Name = Name;
    end
    
    BoundaryProp.BdryType = BdryType;
    
end
