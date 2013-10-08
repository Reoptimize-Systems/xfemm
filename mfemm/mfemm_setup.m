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
% There are two ways to do this. On linux type systems it is probably
% easiest to use the provided Makefile in the mfemm directory. On windows,
% it is probably easiest to use the Code::Blocks C++ IDE. Several project
% and workspace files for the free Code::Blocks IDE are provided to ease
% this process. Instructions are provided below for creating the required
% libraries using Code::Blocks or using the Makefile.
%
% mfemm_setup requires requires that you ave a C++ compiler set up for use
% by Matlab or Octave (of course Octave will not have a problem thanks to
% its seamless integration with gcc). In Matlab you must have run 
%
% mex -setup
%
% And selected a valid C++ compiler before attempting to run this
% mfemm_setup function. For linux the system gcc compiler will be perfect.
% The free windows compilers supported by Matlab vary over time, but often
% there is a free version of Microsoft Visual C++ which will work. The
% gnumex project provides a way to use gcc on Windows with Matlab if
% desired.
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

    % add the required directories to the path
    fprintf('Adding mfemm directories to the path.\n');
    
    addpath(genpath(fileparts(which('mfemm_setup.m'))));
    
    if ~exist('mexfsolver', 'file') ...
            || ~exist('mexfmesher', 'file') ...
            || ~exist('fpproc_interface_mex', 'file') ...
            || forceallcompile

        fprintf('Compiling mex functions for mfemm.\n');
        
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

