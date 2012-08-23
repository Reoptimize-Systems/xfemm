function fsolversetup(dodebug)
% compiles the fmesher mexfunction 

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

    if nargin < 1
        dodebug = false;
    end
    
    if isoctave
        cc.Name = 'GNU C++';
    else
        % First check a C++ compiler is present, and selected
        cc = mex.getCompilerConfigurations('C++', 'Selected');
    
        if isempty(cc)
            error(['You must have a C++ compiler setup. Run mex -setup to choose ', ...
                   'an appropriate compiler, you may need to install one on your system.']);
        end
    end

    % store the current directory
    origdir = pwd;
    
    % change to the mfemm directory (the directory this file is in)
    cd(fileparts(which('fsolversetup.m')));

    % set some common compiler flags, we replace all calls to printf to
    % calls to mexPrintf
    if dodebug
        common_compiler_flags = '-g -D"printf=mexPrintf"';
    else
        common_compiler_flags = '-D"printf=mexPrintf"';
    end

    % build the lua library??
    
    
    % test for the pernickity Visual C++ compiler, when oh when will The
    % Mathworks support mingw by default?
    if strmatch('Microsoft Visual C++', cc.Name)
        liblua = {'-I.\fsolver\liblua', ...
                  'fsolver\liblua\liblua.lib'};
    elseif strmatch('GNU C++', cc.Name)
        liblua = {'fsolver/liblua/liblua.a', '-I./fsolver/liblua'};
    end
    
    % put all the compiler commands in a cell array
    mexcommands = [{'-v', ...
        common_compiler_flags, ...
        'mexfsolver.cpp', ...
        'fsolver/fsolver.cpp', ...
        'fsolver/complex.cpp', ...
        'fsolver/cspars.cpp', ...
        'fsolver/cuthill.cpp', ...
        'fsolver/fullmatrix.cpp', ...
        'fsolver/harmonic2d.cpp', ...
        'fsolver/static2d.cpp', ...
        'fsolver/harmonicaxi.cpp', ...
        'fsolver/staticaxi.cpp', ...
        'fsolver/matprop.cpp', ...
        'fsolver/spars.cpp'}, ...
        liblua];
    
    % call mex with the appropriately constructed commands
    mex(mexcommands{:}); 
     
    % return to original directory
    cd(origdir);


end
