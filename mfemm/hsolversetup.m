function hsolversetup(dodebug, verbose)
% compiles the hsolver mexfunction
%
% Syntax
%
% hsolversetup(dodebug, verbose)
%
% Input
%
%  dodebug - compile with debugging symbols
%
%  verbose - verbose compiler output to the command line
%
% See also: fpprocsetup.m, fmeshersetup.m
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

    if nargin < 1
        dodebug = false;
    end

    if nargin < 2
        verbose = false;
    end

%     if mfemmdeps.isoctave
%         cc.Name = 'gcc';
%     else
%         try
%             % First check a C++ compiler is present, and selected
%             cc = mex.getCompilerConfigurations('C++', 'Selected');
%         catch
%             % if the getCompilerConfigurations call fails, try with gcc,
%             % assuming that we are on windows and perhaps using gnumex
%             cc.Name = 'gcc';
%         end
%     end

    % store the current directory
    origdir = pwd;
    
    % return to original dir on interruption or completion
    OC = onCleanup (@() cd(origdir));

    % change to the mfemm directory (the directory this file is in)
    cd(fileparts(which('hsolversetup.m')));

    % make architecture specific mex directory if it doesn't already exist
    warning off MATLAB:MKDIR:DirectoryExists
    mexdir = ['xfemm_mex_files_for_' computer('arch')];
    mkdir (mexdir);
    warning on MATLAB:MKDIR:DirectoryExists

    cd (mexdir);
    
    % set some common compiler flags
    if dodebug
        common_compiler_flags = {'-g'};
    else
        common_compiler_flags={};% common_compiler_flags = '-D"printf=mexPrintf"';
    end

    if verbose
        common_compiler_flags = [common_compiler_flags, {'-v'}];
    end

    % TODO: build the lua library??

    % test for the pernickity Visual C++ compiler, when oh when will The
    % Mathworks support mingw by default?
    %if strcmp('Microsoft Visual C++', cc.Name)
    %    libcommands = {'pfemm\liblua\liblua.lib', ...
    %                   'pfemm/libfemm/libfemm.lib', ...
    %                   '-I".\pfemm\fsolver"', '-I".\pfemm\liblua"', '-I".\pfemm\libfemm"'};
    %elseif strcmp('gcc', cc.Name)
        libcommands = {'-I"../cfemm/hsolver"', ...
                       '-I"../cfemm/liblua"', ...
                       '-I"../cfemm/libfemm"', ...
                       '../cfemm/lib/libhsolver.a'};
    %end

    % put all the compiler commands in a cell array
    mexcommands = [ common_compiler_flags, ...
                    { '../mexhsolver.cpp' }, ...
                    libcommands ...
                  ];

    if mfemmdeps.isoctave
        mkoctfile('--mex', mexcommands{:});
    else
        % call mex with the appropriately constructed commands
        mex(mexcommands{:});
    end

end
