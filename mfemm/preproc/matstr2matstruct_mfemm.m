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
% Output
%
% materials - a structure array, one element per specified material name
%   string, containing the following fields:
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

    if ischar(matstr)
        matstr = {matstr};
    elseif ~iscellstr(matstr)
        error('MFEMM:matstr2matstruct_mfemm:badmatstr', ...
            'matstr must be a string or cell array of strings containing material names.')
    end
    
    if nargin < 2
        if exist('matlib.mat', 'file')
            % Load materials library from disk
            load ('matlib.mat')
        else
            matlibdat2mat();
            load ('matlib.mat')
        end
    end
    
    for j = 1:numel(matstr)
        
        notfoundmat = true;
        
        % Check for material in the material library
        for i = 1:numel(matlib)

           % Look for string in the materials library, once found break loop
           if strcmpi(matlib(i).Name, matstr{j})
               materials(j) = matlib(i);
               notfoundmat = false;
               break;
           end
           
        end
        
        if notfoundmat
            error('MFEMM:matstr2matstruct_mfemm:matnotfound', ...
                     'Material %s not found in library\n', matstr{i});
        end
        

        % Error check, if density not specified
%         if isempty(material.Density)
%             error('MFEMM:matstr2matstruct_mfemm', 'No density specified for material %s\n', material.Name)
%         end
    
    end
    
    if ~every(isfield(materials, 'Density'))
        materials(end).Density = [];
    end
    
end