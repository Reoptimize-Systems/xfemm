function varargout = mfemm_setup(varargin)
% mfemm_setup: set up the matlab interface to xfemm
%
% Syntax
%
% mfemm_setup ()
% mfemm_setup ('Parameter', Value)
% 
% Description
%
% mfemm_setup performs various operations to set up mfemm. At it's
% simplest, it will just add some directories to your Matlab/Octave path.
% At it's most complicated it will compile from scratch a set of libraries
% and matlab mex files. What it does, or attempts to do, will depend on
% what has already been done before, or supplied with the package, or what
% you tell it to do using various options.
%
% mfemm_setup has several optional arguments that may be supplied as
% Parameter-Value pairs. The possible options are shown below. Some may
% require further reading of the help below to fully understand.
%
% 'RunTests' - true or false flag. If true, a set of test runs of the mex
%   files will be performed. Defaults to false.
%
% 'ForceMexRecompile' - true or false flag. If true, forces the mex files
%   all be recompiled. Library files will not be recompiled unless they are
%   not present. Defaults to false.
%
% 'ForceAllRecompile' - true or false flag. If true all mex files will be
%   recompiled, and the libraries. If any required libraries are missing,
%   an attempt will be made to build them also. Defaults to false if not
%   supplied.
%
% 'ForceCmake' - true or false flag. If true, forces cmake to be rerun,
%   even if the makefiles are already present (i.e. cmake has already been
%   run). Defaults to false.
%
% 'DoDebug' - true or false flag. Mex files will be built with debugging
%   symbols. Defaults to false if not supplied.
%
% 'Verbose' - true or false flag. Verbose build output if true. Defaults to
%   false if not supplied.
% 
% These may be supplied in any order. An example call the mfemm_setup might
% be:
%
%   mfemm_setup ('DoDebug', false, 'ForceAllRecompile', true)
%
% A large proportion of mfemm's functionality depends on mex interfaces to
% C++ code. This code must be compiled into mex functions to be accessible
% from the matlab or octave command line. If you're using a released
% version of mfemm for your platform, you won't have to worry too much
% about this, and can just run mfemm_setup. Compiled mex files for your
% platform may be provided for your convenience with this package, in which
% case mfemm_setup will skip the compilation step (unless you force it not
% to). If you do need/want to recompile, in Matlab therefore, you must have
% previously run:
%
% mex -setup
%
% And selected a valid C++ compiler before attempting to run this
% mfemm_setup function. For Linux, or Octave on Windows, the default gcc
% compiler will be ideal. For Matlab on Windows, the situation is more
% complicated. 
%
%
% If you are not using a release of mfemm_setup, or you are not using a
% platform for which a release has been supplied, before running this
% script it may also first be required to compile some library files for
% use by the mex compiler. mfemm uses cmake as it's build system, and any
% platform supported by cmake therefore should work.
%
% ------------------------   Linux Systems    ----------------------------- 
%
% On linux type systems the process is quite smoothly automated and, if
% necessary, mfemm_setup.m will build the libraries using cmake and the
% system 'make' program, you will not need to do anything.
%
%                            ** cmake **
%
% If there are problems, you can try invoking the cmake and make command
% yourself. Just change directory to the mfemm/cfemm directory, and run the
% following commands:
%
% cmake . -DCMAKE_BUILD_TYPE=Release
% make
%
% ------------------------- Windows Systems ------------------------------- 
%
%                           ** mingw-w64 **
%
% Unfortunately Windows lacks somewhat the development environment of Linux
% systems. On Windows, you will need to install mingw-w64. At the time of
% writing, there are no official mingw-w64 toolchains available, but
% several stable 'unofficial' toolchains. Choose one targeted at your
% platform.
%
%                            ** Gnumex **
%
% For Windows, the compilers supported by Matlab vary over time. However,
% you will need to build the mex files using the same compiler with which
% you (or the mfemm developers, or whoever) created the libraries. Often
% there is a free version of Microsoft Visual C++ which will work, but no
% project files are provided for this. The gnumex project provides a way to
% use gcc on Windows with Matlab (this will work with the compiler which
% comes with code::blocks, mingw). gnumex can be found on sourceforge,
% here:
%
%                 http://gnumex.sourceforge.net/
%
%
%                            ** cmake **
%
% If you need to rebuild *everything* from scratch (even the makefiles)
% including the libraries, you will need cmake (version 2.8.8 or newer).
% You will usually not need to do this.
%
% mfemm uses cmake for its build system, if you're familiar with this, you
% can create visual studio solution files (or nmake files) for the library
% compilation, and use the microsoft compilers with matlab. You will then
% need to use the same compiler in Matlab to build the mex files.
% mfemm_setup will not call cmake correctly to create Visual Studio
% solution files and build them, you will have to invoke this manually.
%

    if nargin < 1
        
        if nargout > 0
            % return the subfunctions for testing/use
            varargout {1} = { @makelibs };
            return;
        end
    end
    
    % add the required directories to the path
    thisfilepath = which('mfemm_setup.m');
    
    if mfemmdeps.isoctave
        thisfilepath = canonicalize_file_name(thisfilepath);
    end
    
    thisfilepath = fileparts (thisfilepath);
    
    % add mfile paths
    addpath(thisfilepath);
    addpath (fullfile (thisfilepath, 'preproc'));
    addpath (fullfile (thisfilepath, 'postproc'));
    addpath (fullfile (thisfilepath, 'examples'));
    addpath (fullfile (thisfilepath, 'depends'));
    addpath (fullfile (thisfilepath, 'visualisation'));
    
    % parse inputs
    Inputs.ForceAllRecompile = false;
    Inputs.ForceMexRecompile = false;
    Inputs.ForceCmake = false;
    Inputs.DoDebug = false;
    Inputs.Verbose = false;
    Inputs.RunTests = false;
    
    Inputs = mfemmdeps.parseoptions (Inputs, varargin);

    % make architecture specific directory for mex files if it doesn't
    % already exist
    warning off MATLAB:MKDIR:DirectoryExists
    mexdir = fullfile(thisfilepath, ['xfemm_mex_files_for_' computer('arch')]);
    mkdir (mexdir);
    warning on MATLAB:MKDIR:DirectoryExists
    
    % add it to the path
    addpath (mexdir);
    
    if ~(exist('mexfsolver', 'file') == 3) ...
            || ~(exist('mexfmesher', 'file') == 3) ...
            || ~(exist('fpproc_interface_mex', 'file') == 3) ...
            || Inputs.ForceAllRecompile ...
            || Inputs.ForceMexRecompile

        fprintf('Compiling mex functions for mfemm.\n');
        
        CC = onCleanup(@() cd(pwd));
        cd (thisfilepath);
        
        if ~ ( all( [ exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libfmesher.a'), 'file'), ... 
                   exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libfsolver.a'), 'file'), ...
                   exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libfpproc.a'), 'file'), ...
                   exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libhsolver.a'), 'file'), ...
                   exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libhpproc.a'), 'file') ] ) ...
               || all( [ exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libfmesher.lib'), 'file'), ... 
                   exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libfsolver.lib'), 'file'), ...
                   exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libfpproc.lib'), 'file'), ...
                   exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libhsolver.lib'), 'file'), ...
                   exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libhpproc.lib'), 'file') ] ) ...
             ) ...
             || Inputs.ForceAllRecompile
          
            % attempt to make the libs if not found, or we're recompiling
            % everything
            makelibs (thisfilepath, Inputs);
            
        end
        
        if exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libfmesher.a'), 'file') 
            fmeshersetup (Inputs.DoDebug, Inputs.Verbose);
        else
            % try building 
            error('MFEMM:setup', 'mfemm_setup can''t find the libfmesher library (libfmesher.a), run ''help mfemm_setup'' for more info.\n')
        end

        if exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libfsolver.a'), 'file') 
            fsolversetup (Inputs.DoDebug, Inputs.Verbose);
        else
            error('MFEMM:setup', 'mfemm_setup can''t find the libfsolver library (libfsolver.a), run ''help mfemm_setup'' for more info.\n')
        end

        if exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libfpproc.a'), 'file')
            fpprocsetup (Inputs.DoDebug, Inputs.Verbose);
        else
            error('MFEMM:setup', 'mfemm_setup can''t find the libfpproc library (libfpproc.a), run ''help mfemm_setup'' for more info.\n')
        end
        
        if exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libhsolver.a'), 'file') 
            hsolversetup (Inputs.DoDebug, Inputs.Verbose);
        else
            error('MFEMM:setup', 'mfemm_setup can''t find the libhsolver library (libhsolver.a), run ''help mfemm_setup'' for more info.\n')
        end

        if exist(fullfile(thisfilepath, 'cfemm', 'lib', 'libhpproc.a'), 'file')
            hpprocsetup (Inputs.DoDebug, Inputs.Verbose);
        else
            error('MFEMM:setup', 'mfemm_setup can''t find the libhpproc library (libhpproc.a), run ''help mfemm_setup'' for more info.\n')
        end

    else
        
        fprintf('Not compiling mex functions as they already exist.\n');
    
    end
    
    if Inputs.RunTests
        
        fmesher_test_file = fullfile (thisfilepath, 'cfemm', 'fmesher', 'test', 'Temp.fem');
        
        fprintf ('Running mexfmesher on file:\n%s\n', fmesher_test_file);
        mexfmesher (fmesher_test_file);
        
        fsolver_test_file = fullfile (thisfilepath, 'cfemm', 'fmesher', 'test', 'Temp');
        fprintf ('Running mexfsolver on file:\n%s\n', fsolver_test_file);
        mexfsolver (fsolver_test_file, double(true));
        
        fprintf ('Running fmesher on file:\n%s\n', fmesher_test_file);
        fmesher (fmesher_test_file);
        
        fsolver_test_file = fullfile (thisfilepath, 'cfemm', 'fmesher', 'test', 'Temp.fem');
        fprintf ('Running fsolver on file:\n%s\n', fsolver_test_file);
        fsolver (fmesher_test_file);
        
        fpproc_test_file = fullfile (thisfilepath, 'cfemm', 'fmesher', 'test', 'Temp.ans');
        fprintf ('Loading solution using fpproc from file:\n%s\n', fpproc_test_file);
        solution = fpproc (fpproc_test_file);
        vals = solution.getpointvalues(0.038, 0.207)
        
    end


end

function makelibs (thisfilepath, Inputs)
% build the libs required for mfemm, invoking cmake if necessary

    cd (fullfile(thisfilepath, 'cfemm'));
    
    if isunix
        if (exist (fullfile(pwd, 'Makefile'), 'file') == 0) || Inputs.ForceCmake
            system('cmake . -DCMAKE_BUILD_TYPE=Release');
        end
        system('make');
    else
        if (exist (fullfile(pwd, 'Makefile'), 'file') == 0)  || Inputs.ForceCmake
            system('cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .')
        end
        system('mingw32-make.exe');
    end

end

function t = mfemmdeps.isoctave()
% ISOCTAVE.M
% ISOCTAVE  True if the operating environment is octave.
%    Usage: t=mfemmdeps.isoctave();
% 
%    Returns 1 if the operating environment is octave, otherwise
%    0 (Matlab)
% 
% ---------------------------------------------------------------
%
% COPYRIGHT : (c) NUHAG, Dept.Math., University of Vienna, AUSTRIA
%             http://nuhag.eu/
%             Permission is granted to modify and re-distribute this
%             code in any manner as long as this notice is preserved.
%             All standard disclaimers apply.

    if exist('OCTAVE_VERSION')
        % Only Octave has this variable.
        t=1;
    else
        t=0;
    end

end