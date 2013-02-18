function [FemmProblem, matname, matind] = addmagnet_mfemm(FemmProblem, Hc, mu_x, mu_y)
% ads a magnetic material to an mfemm problem
%
% Syntax
%
% [FemmProblem, matname, matind] = addmagnet_mfemm(FemmProblem, Hc, mu_x, mu_y)
%
% 
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


    matname = ['NdFeB ', int2str(hc2mgoe(Hc)), ' MGOe'];
    
    magmaterial.Name = matname;
    magmaterial.Mu_x = 1.049;
    magmaterial.Mu_y = 1.049;
    magmaterial.H_c = 979000;
    magmaterial.H_cAngle = 0;
    magmaterial.J_re = 0;
    magmaterial.J_im = 0;
    magmaterial.Sigma = 0.667;
    magmaterial.d_lam = 0;
    magmaterial.Phi_h = 0;
    magmaterial.Phi_hx = 0;
    magmaterial.Phi_hy = 0;
    magmaterial.LamType = 0;
    magmaterial.LamFill = 1;
    magmaterial.NStrands = 0;
    magmaterial.WireD = 0;
    magmaterial.BHPoints = [];
    magmaterial.Density = 7500;
    
    if nargin >= 2
        magmaterial.H_c = Hc;
    end
    
    if nargin >= 3
        magmaterial.Mu_x = mu_x;
    end
    
    if nargin >= 4
        magmaterial.Mu_y = mu_y;
    end
    
    [FemmProblem, matind] = addmaterials_mfemm(FemmProblem, magmaterial);
	
end