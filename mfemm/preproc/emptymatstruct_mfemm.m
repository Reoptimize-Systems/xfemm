function Material = emptymatstruct_mfemm()
% mfemm_emptymatstruct: Creates a structure containing all the required
% material fields with some default values, i.e. 
%
% Name = '';
% Mu_x = 1;
% Mu_y = 1;
% H_c = 0;
% H_cAngle = 0;
% J_re = 0;
% J_im = 0;
% Sigma = 0;
% d_lam = 0;
% Phi_h = 0;
% Phi_hx = 0;
% Phi_hy = 0;
% LamType = 0;
% LamFill = 1;
% NStrands = 0;
% WireD = 0;
% BHPoints = 0;
%
% These are effectively the material properties for Air without the 'Air'
% name.
    
    Material.Name = '';
    Material.Mu_x = [];
    Material.Mu_y = [];
    Material.H_c = [];
    Material.H_cAngle = [];
    Material.J_re = [];
    Material.J_im = [];
    Material.Sigma = [];
    Material.d_lam = [];
    Material.Phi_h = [];
    Material.Phi_hx = [];
    Material.Phi_hy = [];
    Material.LamType = [];
    Material.LamFill = [];
    Material.NStrands = [];
    Material.WireD = [];
    Material.BHPoints = [];

end
