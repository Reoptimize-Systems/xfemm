function filename = fmesher(varargin)
% meshes an mfemm FemmProblem structure or existing .fem file
%
% Syntax
%
% filename = fmesher(FemmProblem)
% filename = fmesher(filename)
% filename = fmesher(FemmProblem, filename)
%
% Description
%
% fmesher.m creates a finite element mesh of an mfemm FemmProblem structure
% using fmesher, a C++ program based on the meshing routines originally
% used in the program FEMM.
%
% fmesher(FemmProblem) creates a .fem file in a temporary directory from
% the supplied FemmProblem structure and meshes it, creating a number of
% output files.
%
% fmesher(filename) meshes an existing .fem file, the location of which is
% provided in 'filename'.
%
% fmesher(FemmProblem, filename) creates a .fem file the location specified
% by filename from the supplied FemmProblem structure and meshes it,
% creating a number of output files.
%
% the actual file name of the .fem file associated with the problem is
% returned in all cases.
%
% See also:  mexfmesher, fsolver.m, fpproc.m
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

    if nargin == 1
        
        if isstruct(varargin{1})
            
            FemmProblem = varargin{1};
            % create a temporary file name location for the .fem file to be
            % created for the problem
            filename = [tempname, '.fem'];
            
            writefemmfile(filename, FemmProblem)
            
        elseif ischar(varargin{1})
            
            filename = strtrim (varargin{1});
            
            if isunix && (filename(1) == '~')
                warning ('MFEMM:fmesher:notildeexp', 'fmesher will not expand  ''~'' into your home directory, you must supply the absolute path.');
            end
            
            if exist(filename, 'file') ~= 2
               error('The supplied filename location cannot be found.') 
            end
            
        else
            error(['If supplying one input it must be either a ', ...
                   'FemmProblem structure or a filename with the location ', ...
                   'of a .fem file to be meshed.']);
        end
        
    elseif nargin == 2
        
        if ~isstruct(varargin{1}) || ~ischar(varargin{2})
            error(['If supplying two inputs the first must be a ', ...
                   'a filename with the location where the FemmProblem ', ...
                   '.fem file to be meshed wil be created, and the ', ...
                   'second a FemmProblem structure.']);
        end
        
        filename = varargin{1};
        FemmProblem = varargin{2};
        % write the .fem file
        writefemmfile(filename, FemmProblem)
        
    else
        error('Incorrect number of arguments to fmesher.')
    end

    mexfmesher(filename);

end