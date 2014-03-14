function mfemm_setup(forceallcompile)
% mfemm_setup
%
% A large proportion of mfemm's functionality depends on interfaces to C++
% code. This code must be compiled into mex functions to be accessibly from
% the matlab or octave command line. Compiled mex files for your platform
% may be provided for your with this package, in which case mfemm_setup
% will skip the compilation step (unless you call it as mfemm_setup(true)
% to force recompilation). 
%
% If you do need/want to create the mex files, before running this script
% you must first compile some library files for use by the mex compiler. 
%
% Linux: On linux type systems the process is automated, and mfemm_setup.m
% will build the libraries using the system 'make' program, you will not
% need to do anything. If there are problems, you can try invoking the make
% command yourself, see the instructions for building the libraries using a
% makefile below. 
%
% Windows: Unfortunately Windows lacks somewhat the development environment
% of Linux systems. On windows, it is probably easiest to use the
% Code::Blocks C++ IDE. Several project and workspace files for the free
% Code::Blocks IDE are provided to ease this process. Instructions are
% provided below for creating the required libraries using Code::Blocks.
%
% In both cases mfemm_setup requires requires that you ave a C++ compiler
% set up for use by Matlab or Octave (Octave will not have a problem thanks
% to its seamless integration with gcc). In Matlab you must have run
%
% mex -setup
%
% And selected a valid C++ compiler before attempting to run this
% mfemm_setup function. For linux the system gcc compiler will be ideal.
% The free windows compilers supported by Matlab vary over time. However,
% you will need to build the mex files using the same compiler with which
% you created the libraries earlier. Often there is a free version of
% Microsoft Visual C++ which will work, but no project files are provided
% for this. The gnumex project provides a way to use gcc on Windows with
% Matlab if desired (this will work with the compiler which comes with
% code::blocks, mingw).
%
% 
% ===   Instructions for building required libraries using Makefile     ===
%
% On your linux system change directory to the mfemm top level directory.
% In this directory run the following command:
%
% make
%
% That should be it! Now you can rerun mfemm_setup to complete the process.
%
%
% ===  Instructions for building required libraries with Code::Blocks   ===
%
% Code::Blocks can be obtained from here: www.codeblocks.org
%
% To build the required library files, open the file mfemm_libs.workspace
% file in the same directory as this funtion in Code::Blocks. This
% Code::Blocks workspace contains several projects all of which must be
% compiled before running the rest of the setup. You will need to manually
% ensure that those projects which have a 'Library' build target have this
% selected. The projects which have a 'Library' target are fmesher, fsolver
% and fpproc. The other projects are always libraries and you will want to
% build the "Release" versions of these. Once this has been done simply
% select 'build workspace' from the build menu and Code::Blocks will build
% all of the projects. Once this is done rerun mfemm_setup to create the
% mex functions. 
%
% Note that you are building for Matlab will want your Matlab mex compiler
% to be the same as your Code::Blocks compiler. The gnumex project
% (available elsewhere) can help you use the gcc with Matlab.
%
%

    if nargin < 1
        forceallcompile = false;
    end

    % add the required directories to the path
    thisfilepath = which('mfemm_setup.m');
    
    if isoctave
        thisfilepath = canonicalize_file_name(thisfilepath);
    end
    
    thisfilepath = fileparts (thisfilepath);
    
    addpath(genpath(thisfilepath));
    
    if ~(exist('mexfsolver', 'file') == 3) ...
            || ~(exist('mexfmesher', 'file') == 3) ...
            || ~(exist('fpproc_interface_mex', 'file') == 3) ...
            || forceallcompile

        fprintf('Compiling mex functions for mfemm.\n');
        
        if isunix
            CC = onCleanup(@() cd(pwd));
            cd (thisfilepath);
            % if we are on a unixy computer we can make the necessary
            % libraries using make
            system(sprintf('make -f %s', fullfile(thisfilepath, 'Makefile')));
        end
        
        if exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'libfemm', 'libfemm.a'), 'file') ...
            && exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'liblua', 'liblua.a'), 'file') ...
            && exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'fpproc', 'libfpproc.a'), 'file')

            fmeshersetup;
            fsolversetup;
            fpprocsetup;

        else

            if ~exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'libfemm', 'libfemm.a'), 'file') 
                fprintf(1, 'mfemm_setup can''t find the libfemm library (libfemm.a), have you built it?\nType help mfemm_setup for more info.\n')
            end

            if ~exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'liblua', 'liblua.a'), 'file') 
                fprintf(1, 'mfemm_setup can''t find the liblua library (liblua.a), have you built it?\nType help mfemm_setup for more info.\n')
            end

            if ~exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'fpproc', 'libfpproc.a'), 'file')
                fprintf(1, 'mfemm_setup can''t find the libfpproc library (libfpproc.a), have you built the fpproc project as a library?\nType help mfemm_setup for more info.\n')
            end

            error('mfemm setup failed. Type help mfemm_setup for help setting up mfemm.')

        end
        
    else
        
        fprintf('Not compiling mex functions as they already exist.\n');
    
    end


end

function t = isoctave()
% ISOCTAVE.M
% ISOCTAVE  True if the operating environment is octave.
%    Usage: t=isoctave();
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