function [FemmProblem, matind] = addmagnetwire_mfemm(FemmProblem, name, Dc, resistivity)
% mi_addwire: adds a wire type to the materials in a FEMM sim
%
% Input:
%
%   name - the name of the wire type to be added
%
%   Dc - optional, the conductor diameter in metres, default is 0.001 m
%        (1mm)
%
%   resistivity - optional resistivity of the material  58
%

    wirematerial = newmaterial_mfemm (name);
    
    wirematerial.Name = name;
    wirematerial.Mu_x = 1;
    wirematerial.Mu_y = 1;
    wirematerial.H_c = 0;
    wirematerial.H_cAngle = 0;
    wirematerial.J_re = 0;
    wirematerial.J_im = 0;
    wirematerial.Sigma = 58.0;
    wirematerial.d_lam = 0;
    wirematerial.Phi_h = 0;
    wirematerial.Phi_hx = 0;
    wirematerial.Phi_hy = 0;
    wirematerial.LamType = 3;
    wirematerial.LamFill = 1;
    wirematerial.NStrands = 1;
    wirematerial.WireD = 1;
    wirematerial.BHPoints = [];
    wirematerial.Density = 8600;
    
    switch nargin
        case 3
            wirematerial.WireD = Dc * 1000;
        case 4
            wirematerial.Sigma = 1/(resistivity * 1e6);
            wirematerial.WireD = Dc * 1000;
        otherwise
            error('incorrect number of input arguments, must be 1, 2 or 3')
    end
    
    [FemmProblem, matind] = addmaterials_mfemm(FemmProblem, wirematerial);

    
end