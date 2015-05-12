function materials = matstr2matstruct_mfemm(matstr, matlib)
% matstr2matstruct: get the material structure from the materials library
% associated with an input string.
%
% Syntax
%
% materials = matstr2matstruct_mfemm(matstr)
%
% Input
%
% matstr - a string, or cell array of strings containing the names of the
%   materials to be extracted from the materials library.
%
% matlib - either an array of structures, or a string representing a file path
%   to a materials library saved in a .
%
% Output
%
% materials - a structure array, one element per specified material name
%   string, containing the following fields:
%
% Magnetic material properties
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
%   LamType     - type of lamination (0 = none/inplane, 1 = parallel to x, 
%                 2 = parallel to y)
%   LamFill     - fill factor, fraction of core filled with iron
%   NStrands    - number of wire strands
%   WireD       - wire diameter
%   BHPoints    - a table of points defining the BH curve (can be empty)
%   Density     - material density (kg/m3)
%
% Thermal material properties
%
%   Kx          - 
%   Ky          -
%   Kt          -
%   qv          -
%   TKPoints    - a table of points defining the conductivity with respect
%                 to temperature

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

    if ischar(matstr)
        matstr = {matstr};
    elseif ~iscellstr(matstr)
        error('MFEMM:matstr2matstruct_mfemm:badmatstr', ...
            'matstr must be a string or cell array of strings containing material names.')
    end
    
    if nargin < 2 || isempty (matlib)
    
        matlib = fullfile(fileparts (which ('matstr2matstruct_mfemm')), '..', 'matlib.mat');

    end

    if ischar (matlib)
        matlibfile = matlib;
    
        if exist(matlibfile, 'file')
        
            [~, ~, ext] = fileparts (matlibfile);
            
            if strcmpi(ext, '.mat')
            
                % Load materials library from disk
                load (matlibfile);
            
            elseif strcmpi (ext, '.dat')
                % convert to matlib structures
                matlib = parsematlib_mfemm (matlibfile);
            else
                error ('MFEMM:matstr2matstruct_mfemm:badfile', ...
                    'Unrecognised materials library file type, must be .mat or .dat');
            end
            
        else
            matlibdat2mat();
            load (matlibfile);
        end
    elseif ~isstruct (matlib)
        error ('MFEMM:matstr2matstruct:badinput', ...
               'Second input must be a file location of array of material structures.');
    end
    
    for matnameind = 1:numel(matstr)
        
        notfoundmat = true;
        
        % Check for material in the material library
        for libind = 1:numel(matlib)

           % Look for string in the materials library, once found break loop
           if strcmpi(matlib(libind).Name, matstr{matnameind})
               materials(matnameind) = matlib(libind);
               notfoundmat = false;
               break;
           end
           
        end
        
        if notfoundmat
            error('MFEMM:matstr2matstruct:matnotfound', ...
                     'Material %s not found in library\n', matstr{matnameind});
        end
    
    end
    
    if ~mfemmdeps.every(isfield(materials, 'Density'))
        materials(end).Density = [];
    end
    
end