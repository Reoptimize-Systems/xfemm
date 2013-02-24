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

% Copyright 2013 Richard Crozier
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
        
    Material.Mu_x = 1;
    Material.Mu_y = 1;
    Material.H_c = 0;
    Material.H_cAngle = 0;
    Material.J_re = 0;
    Material.J_im = 0;
    Material.Sigma = 0;
    Material.d_lam = 0;
    Material.Phi_h = 0;
    Material.Phi_hx = 0;
    Material.Phi_hy = 0;
    Material.LamType = 0;
    Material.LamFill = 1;
    Material.NStrands = 0;
    Material.WireD = 0;
    Material.BHPoints = [];
    Material.Density = [];

    Material = parseoptions(Material, varargin);
    
    Material.Name = Name;
    
end