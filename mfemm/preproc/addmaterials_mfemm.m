function [FemmProblem, matinds] = addmaterials_mfemm(FemmProblem, Materials, varargin)
% adds one or more materials to an mfemm FemmProblem structure
%
% Syntax
% 
% [FemmProblem, matinds] = addmaterials_mfemm(FemmProblem, Materials)
%
%
% Input
%
%  FemmProblem - an mfemm FemmProblem structure
%
%  Materials - either a single string, a cell array of strings a
%    structure, array of structures, or a cell array where each element is
%    either a scalar structure or a string. If a string or cell array of strings,
%    these are the names of materials from the materials library to be
%    added to the model. If a structure or array of structures these must
%    be Materials structures in the same format as produced by the function
%    matstr2matstruct_mfemm, or newmaterial_mfemm.m. 
%
%
% Output
%
%  FemmProblem - the input FemmProblem structure with the desired materials
%    added to the 'Materials' field
%
%  matinds - a vector of indices in the Materials array containing the
%    newly added materials
%
% 
% See also: matstr2matstruct_mfemm.m, newmaterial_mfemm.m
%

% Copyright 2012-2014 Richard Crozier
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

    Inputs.MaterialsLibrary = fullfile(fileparts (which ('matstr2matstruct_mfemm')), '..', 'matlib.mat');
    
    Inputs = mfemmdeps.parseoptions (Inputs, varargin);
    
    if isfield(FemmProblem, 'Materials')
        lastmatind = numel(FemmProblem.Materials);
    else
        lastmatind = 0;
        FemmProblem.Materials = [];
    end
    
    if iscellstr(Materials) 

        matinds = lastmatind + (1:numel(Materials));
        
        FemmProblem.Materials = [FemmProblem.Materials, matstr2matstruct_mfemm( Materials, Inputs.MaterialsLibrary )];
        
    elseif ischar(Materials)
    
        Materials = {Materials};
        
        matinds = lastmatind + (1:numel(Materials));
        
        FemmProblem.Materials = [FemmProblem.Materials, matstr2matstruct_mfemm( Materials, Inputs.MaterialsLibrary )];
        
    elseif isstruct(Materials)
        
        matinds = lastmatind + (1:numel(Materials));
        
        FemmProblem.Materials = [FemmProblem.Materials, Materials];
        
    elseif iscell (Materials)
        % could be a mixture of specifications (strings and structures, so go
        % through them one by one)
        matinds = [];
        
        for ind = 1:numel(Materials)
            [FemmProblem, tempmatinds] = addmaterials_mfemm(FemmProblem, Materials{ind}, varargin{:});
            matinds = [matinds, tempmatinds];
        end
    
    else
        error('MFEMM:addmaterials_mfemm:badmaterials', ...
            'Materials must be a string, cell array of strings, or an array of materials structures.');
    end


end