function [FemmProblem, matinds] = addmaterials_mfemm(FemmProblem, Materials)
% adds one or more materials to an mfemm FemmProblem structure
%
% Syntax
% 
% [FemmProblem, matinds] = addmaterials_mfemm(FemmProblem, Materials)
%
%

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