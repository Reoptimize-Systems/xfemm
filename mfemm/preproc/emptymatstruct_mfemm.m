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

%     Material.Name = '';
%     Material.Mu_x = [];
%     Material.Mu_y = [];
%     Material.H_c = [];
%     Material.H_cAngle = [];
%     Material.J_re = [];
%     Material.J_im = [];
%     Material.Sigma = [];
%     Material.d_lam = [];
%     Material.Phi_h = [];
%     Material.Phi_hx = [];
%     Material.Phi_hy = [];
%     Material.LamType = [];
%     Material.LamFill = [];
%     Material.NStrands = [];
%     Material.WireD = [];
%     Material.BHPoints = [];

    Material = newmaterial_mfemm ('');

end
