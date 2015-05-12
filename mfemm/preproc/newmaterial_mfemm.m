function Material = newmaterial_mfemm(Name, varargin)
% creates a new material structure suitable for an mfemm FemmProblem
% structure
%
% Syntax
%
% [FemmProblem, matname, matind] = newmaterial_mfemm(Name, 'Parameter', Value)
%
% Description
%
% creates a new material structure suitable for an mfemm FemmProblem
% structure. The material properties are specified in through sets of
% parameter-value pairs. The possible parameters are:
% 
%   Name        - name of the material
%   Mu_x        - relative permeability in x-direction
%   Mu_y        - relative permeability in y-direction
%   H_c         - coercivity (A/m)
%   H_cAngle    - ? always zero
%
%   J_re        - real part of source current density (current at DC)(MA/m2)
%   J_im        - imaginary part of source current density (current at DC)(MA/m2)
%   Sigma       - electrical conductivity (MS/m)
%   d_lam       - lamination thickness (of iron, not insulation) (mm)
%
%   Phi_h       - hysterisis lag angle for non-linear problems (degrees)
%   Phi_hx      - hysterisis lag angle in x-direction for linear problems (degrees)
%   Phi_hy      - hysterisis lag angle in y-direction for linear problems (degrees)
%
%   LamType     - type of lamination (0 = none/inplane, 1 = parallel to x, 2 = parallel to y)
%   LamFill     - fill factor, fraction of core filled with iron
%   NStrands    - number of wire strands
%   WireD       - wire diameter
%   BHPoints    - a table of points defining the BH curve (can be empty)
%   Density     - material density (kg/m3)
%
%

% Copyright 2013-2014 Richard Crozier
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
        
    % create a struct array of materials of length 0
    Material = emptymaterial_mfemm ();
    
    % set default values
    
    % Magnetics properties
    Material(1).Mu_x = 1;
    Material(1).Mu_y = 1;
    Material(1).H_c = 0;
    Material(1).H_cAngle = 0;
    Material(1).J_re = 0;
    Material(1).J_im = 0;
    Material(1).Sigma = 0;
    Material(1).d_lam = 0;
    Material(1).Phi_h = 0;
    Material(1).Phi_hx = 0;
    Material(1).Phi_hy = 0;
    Material(1).LamType = 0;
    Material(1).LamFill = 1;
    Material(1).NStrands = 0;
    Material(1).WireD = 0;
    Material(1).BHPoints = [];
    Material(1).Density = [];
    
    % Thermal material properties
    Material(1).Kx = nan;
    Material(1).Ky = nan;
    Material(1).Kt = nan;
    Material(1).qv = nan;
    Material(1).TKPoints = [];

    Material(1) = mfemmdeps.parseoptions(Material(1), varargin);
    
    Material(1).Name = Name;
    
end