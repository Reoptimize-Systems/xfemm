function [FemmProblem, matinds] = addmaterials_mfemm(FemmProblem, Materials)
% adds one or more materials to an mfemm FemmProblem structure
%
% Syntax
% 
% [FemmProblem, matinds] = addmaterials_mfemm(FemmProblem, Materials)
%
%

    if isfield(FemmProblem, 'Materials')
        lastmatind = numel(FemmProblem.Materials);
    else
        lastmatind = 0;
        FemmProblem.Materials = [];
    end
    
    if iscellstr(Materials) 

        matinds = lastmatind + (1:numel(Materials));
        
        FemmProblem.Materials = [FemmProblem.Materials, matstr2matstruct_mfemm( Materials )];
        
    elseif ischar(Materials)
        
        matinds = lastmatind + (1:numel(Materials));
        
        FemmProblem.Materials = [FemmProblem.Materials, matstr2matstruct_mfemm( Materials )];
        
    elseif isstruct(Materials)
        
        matinds = lastmatind + (1:numel(Materials));
        
        FemmProblem.Materials = [FemmProblem.Materials, Materials];
        
    else
        error('MFEMM:addmaterials_mfemm:badmaterials', ...
            'Materials must be a string, cell array of strings, or an array of materials structures.');
    end


end