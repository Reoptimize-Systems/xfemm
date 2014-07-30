function Material = emptymaterial_mfemm ()
% creates an empty material structure fwith appropriate field names for
% materials
%
% Syntax
%
% Material = emptymaterial_mfemm ()
%
% Output
%
%  Material - (0 x 0) struct array with fields:
%    Name
%    Mu_x
%    Mu_y
%    H_c
%    H_cAngle
%    J_re
%    J_im
%    Sigma
%    d_lam
%    Phi_h
%    Phi_hx
%    Phi_hy
%    LamType
%    LamFill
%    NStrands
%    WireD
%    BHPoints
%    Density
%    Kx 
%    Ky
%    Kt
%    qv
%    TKPoints
%
%
%

% Copyright 2014 Richard Crozier
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



    % Magnetics properties
    Material = struct ('Name', {}, ...
                       'Mu_x', {}, ...
                       'Mu_y', {}, ...
                       'H_c', {}, ...
                       'H_cAngle', {}, ...
                       'J_re', {}, ...
                       'J_im', {}, ...
                       'Sigma', {}, ...
                       'd_lam', {}, ...
                       'Phi_h', {}, ...
                       'Phi_hx', {}, ...
                       'Phi_hy', {}, ...
                       'LamType', {}, ...
                       'LamFill', {}, ...
                       'NStrands', {}, ...
                       'WireD', {}, ...
                       'BHPoints', {}, ...
                       'Density', {}, ...
                       'Kx', {}, ... % thermal proerties start here
                       'Ky', {}, ...
                       'Kt', {}, ...
                       'qv', {}, ...
                       'TKPoints', {} );

end