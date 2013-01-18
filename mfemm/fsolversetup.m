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
        cc.Name = 'gcc';
    else
        try
            % First check a C++ compiler is present, and selected
            cc = mex.getCompilerConfigurations('C++', 'Selected');
        catch
            % if the getCompilerConfigurations call fails, try with gcc,
            % assuming that we are on windows and perhaps using gnumex
            cc.Name = 'gcc';
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

    % TODO: build the lua library??
    
    % test for the pernickity Visual C++ compiler, when oh when will The
    % Mathworks support mingw by default?
    if strcmp('Microsoft Visual C++', cc.Name)
        libcommands = {'pfemm\liblua\liblua.lib', ...
                       'pfemm/libfemm/libfemm.lib', ...
                       '-I".\pfemm\fsolver" -I".\pfemm\liblua" -I".\pfemm\libfemm"'};
    elseif strcmp('gcc', cc.Name)
        libcommands = {'pfemm/liblua/liblua.a', ...
                       'pfemm/libfemm/libfemm.a', ...
                       '-I"./pfemm/fsolver" -I"./pfemm/liblua" -I"./pfemm/libfemm"'};
    end
    
    % put all the compiler commands in a cell array
    mexcommands = [ ...
                    { ...
                      '-v', ...
                      common_compiler_flags, ...
                      'mexfsolver.cpp', ...
                      'pfemm/fsolver/fsolver.cpp', ...
                      'pfemm/fsolver/cspars.cpp', ...
                      'pfemm/fsolver/cuthill.cpp', ...
                      'pfemm/fsolver/harmonic2d.cpp', ...
                      'pfemm/fsolver/static2d.cpp', ...
                      'pfemm/fsolver/harmonicaxi.cpp', ...
                      'pfemm/fsolver/staticaxi.cpp', ...
                      'pfemm/fsolver/matprop.cpp', ...
                      'pfemm/fsolver/spars.cpp' ...
                    }, ...
                    libcommands ...
                  ];
    
    if isoctave
        mkoctfile('--mex', mexcommands{:});
    else
        % call mex with the appropriately constructed commands
        mex(mexcommands{:}); 
    end
     
    % return to original directory
    cd(origdir);


end
