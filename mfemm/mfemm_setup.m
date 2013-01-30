function mfemm_setup()
% mfemm_setup
%
% Setup script for mfemm. This script add relevant directories to the path
% and generates the mex files required for full mfemm use.
%
% A large proportion of mfemm's functionality depends on interfaces to C++
% code. This code must be compiled into mex functions to be accessibly from
% the matlab or octave command line. Before running this script you must
% first compile some library files for use by the mex compiler. Several
% project and workspace files for the free Code::Blocks editor are provided
% to ease this prcess. Instructions are provided below for creating the
% required libraries using Code::Blocks.
%
% mfemm_setup requires requires that you ave a C++ compiler set up for use
% by Matlab or Octave (of course Octave will not have aproblem thanks to
% its seamless integration with gcc). In Matlab you must have run 
%
% mex -setup
%
% And selected a valid C++ compiler before attempting to run this
% mfemm_setup. For linux the system gcc compiler will be perfect. The
% free windows compilers supported by Matlab vary over time, but often
% there is a free version of Microsoft Visual C++ which will work.
%
%
% ===  Instructions for building required libraries with Code::Blocks   ===
%
% Code::Blocks can be obtained from here: www.codeblocks.org
%
% To build the required library files, open the file mfemm_libs.workspace
% in the same directory as this funtion. This Code::Blocks workspace
% contains several projects all of which must be compiled before running
% the rest of the setup. You will need to manually ensure that those
% projects which have a 'Library' build target have this selected. The
% projects which have a 'Library' target are fmesher, fsolver and fpproc.
% The other projects are always libraries. Once this has been done simply
% select 'build workspace' from the build menu and Code::Blocks will build
% all of the projects. Once this is done rerun this setup file to create
% the mex functions.
%
%

    % add the required directories to the path
    addpath(genpath(fileparts(which('mfemm_setup.m'))));

    if exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'libfemm', 'libfemm.a'), 'file') ...
        && exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'liblua', 'liblua.a'), 'file') ...
        && exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'fpproc', 'libfpproc.a'), 'file')

        fmeshersetup;
        fsolversetup;
        fpprocsetup;

    else

        if ~exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'libfemm', 'libfemm.a'), 'file') 
            fprintf(1, 'mfemm_setup can''t find the libfemm library (libfemm.a), have you built it?')
        end

        if ~exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'liblua', 'liblua.a'), 'file') 
            fprintf(1, 'mfemm_setup can''t find the liblua library (liblua.a), have you built it?')
        end

        if ~exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'fpproc', 'libfpproc.a'), 'file')
            fprintf(1, 'mfemm_setup can''t find the libfpproc library (libfpproc.a), have you built the fpproc project as a library?')
        end

        error('mfemm setup failed. Type help mfemm_setup for help setting up mfemm.')

    end


end

