function varargout = mfemm_setup(forceallcompile)
% mfemm_setup
%
% A large proportion of mfemm's functionality depends on interfaces to C++
% code. This code must be compiled into mex functions to be accessibly from
% the matlab or octave command line. Compiled mex files for your platform
% may be provided for your convenience with this package, in which case
% mfemm_setup will skip the compilation step (unless you call it as
% mfemm_setup(true) to force recompilation).
%
% If you do need/want to create the mex files, before running this script
% it must first compile some library files for use by the mex compiler.
% What is required for this depends on your system type.
%
% ------------------------   Linux Systems    ----------------------------- 
%
% On linux type systems the process is smoothly automated, and
% mfemm_setup.m will build the libraries using the system 'make' program,
% you will not need to do anything. If there are problems, you can try
% invoking the make command yourself, see the instructions for building the
% libraries using a makefile below.
%
% -------------------------------------------------------------------------
%
% ------------------------- Windows Systems ------------------------------- 
%
% Unfortunately Windows lacks somewhat the development environment
% of Linux systems. On Windows, it is easiest to install the Code::Blocks
% C++ IDE and mingw compiler suite which usually comes with the
% Code::Blocks package. You will also require the gnumex project which
% allows you to use the mingw compiler as the Matlab mex compiler, there is
% further information about gnumex below.
%
% mfemm_setup will call Code::Blocks automatically if it is installed in
% the normal location to build the required libraries. You shouldn't need
% to take any manual building steps, but you will probably have had to open
% Code::Blocks at least once before running mfemm_setup doing this to
% select the default compiler. If required, there is also information on
% manually using Code::Blocks to build the libraries below.
%
% Code::Blocks can be obtained from here: www.codeblocks.org
%
% -------------------------------------------------------------------------
%
% In both cases mfemm_setup requires requires that you have a C++ compiler
% set up for use by Matlab or Octave (Octave will not have a problem thanks
% to its seamless integration with gcc). In Matlab you must have previously
% run:
%
% mex -setup
%
% And selected a valid C++ compiler before attempting to run this
% mfemm_setup function. For Linux, or Octave on Windows, the system gcc
% compiler will be ideal.
%
% For Windows, the compilers supported by Matlab vary over time. However,
% you will need to build the mex files using the same compiler with which
% you created the libraries earlier. Often there is a free version of
% Microsoft Visual C++ which will work, but no project files are provided
% for this. The gnumex project provides a way to use gcc on Windows with
% Matlab (this will work with the compiler which comes with code::blocks,
% mingw). gnumex can be found on sourceforge, here:
%
%                 http://gnumex.sourceforge.net/
% 
%
%
% = Instructions for building required libraries manually using Makefile =
%
% On your linux system, mfemm_setup will do this automaticaly without any
% action required from you, but if you want to do it manually for some
% reason you can do the following. First change directory to the mfemm top
% level directory. In this directory run the following command:
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
% If Code::Blocks has been installed in the usual directory, mfemm_setup
% should find it and call it using command line arguments to build the
% libraries without you having to do anything. However, if for some reason
% this doesn't work. to build the required library files, open the file
% mfemm_libs.workspace file in the same directory as this funtion in
% Code::Blocks. This Code::Blocks workspace contains several projects all
% of which must be compiled before running the rest of the setup. You will
% want to build the "Release" versions of these. Once this has been done
% simply select 'build workspace' from the build menu and Code::Blocks will
% build all of the projects. Next rerun mfemm_setup to create the mex
% functions.
%
% Note that you are building for Matlab will want your Matlab mex compiler
% to be the same as your Code::Blocks compiler. The gnumex project
% (available elsewhere) can help you use the gcc with Matlab.
%
%

    if nargin < 1
        forceallcompile = false;
        
        if nargout > 0
            % return the subfunctions for testing/use
            varargout {1} = { @makelibs };
            return;
        end
    end

    % add the required directories to the path
    thisfilepath = which('mfemm_setup.m');
    
    if isoctave
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
            || forceallcompile

        fprintf('Compiling mex functions for mfemm.\n');
        
        CC = onCleanup(@() cd(pwd));
        cd (thisfilepath);
        
        makelibs (thisfilepath);
        
        if exist(fullfile(thisfilepath, 'cfemm', 'fmesher', 'libfmesher.a'), 'file') 
            fmeshersetup;
        else
            % try building 
            error('MFEMM:setup', 'mfemm_setup can''t find the libfmesher library (libfmesher.a), run ''help mfemm_setup'' for more info.\n')
        end

        if exist(fullfile(thisfilepath, 'cfemm', 'fsolver', 'libfsolver.a'), 'file') 
            fsolversetup;
        else
            error('MFEMM:setup', 'mfemm_setup can''t find the libfsolver library (libfsolver.a), run ''help mfemm_setup'' for more info.\n')
        end

        if exist(fullfile(thisfilepath, 'cfemm', 'fpproc', 'libfpproc.a'), 'file')
            fpprocsetup;
        else
            error('MFEMM:setup', 'mfemm_setup can''t find the libfpproc library (libfpproc.a), run ''help mfemm_setup'' for more info.\n')
        end

    else
        
        fprintf('Not compiling mex functions as they already exist.\n');
    
    end


end

function makelibs (thisfilepath)
% build the libs required for mfemm

    if isunix
        % if we are on a unixy computer we can make the necessary
        % libraries using make
        system(sprintf('make -f %s', fullfile(thisfilepath, 'cfemm', 'Makefile')));
    else
        % user must install code::blocks, but we can run it using the
        % command line args here

        % see if code::blocks is on the windows path
        [status, result] = system ('codeblocks --help');

        if status ~= 0
            % look for it in the normal locations
            if (exist ('C:\Program Files\CodeBlocks\codeblocks.exe', 'file'))
                cbcmd = '"C:\Program Files\CodeBlocks\codeblocks.exe"';
            else
                cbcmd = '';
            end
        else
            cbcmd = 'codeblocks';
        end
        
        if isempty (cbcmd)
            error ('MFEMM:setup', 'Not all the libraries required for mfemm are present, you are on windows, and I couldn''t find a code::blocks installation to make the libraries.');
        end

        fprintf (1, 'Attempting to build required libraries for mfemm using code::blocks ...\n');
        
        [status,~] = system ([cbcmd, '--target="Release" --build "', fullfile(thisfilepath, 'mfemm_libs.workspace'), '"'], '-echo');
        
        if status ~= 0
            error ('MFEMM:setup', 'There was an error while attempting to build the mfemm libraries using code::blocks.')
        end

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