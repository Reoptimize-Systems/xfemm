function fpprocsetup(dodebug, verbose)
% compiles the fpproc mexfunction

    if nargin < 1
        dodebug = false;
    end
    
    if nargin < 2
        verbose = false;
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

    % return to original dir on interruption or completion
    OC = onCleanup (@() cd(origdir));
    
    % change to the mfemm directory (the directory this file is in)
    cd(fileparts(which('fpprocsetup.m')));
    
    % make architecture specific mex directory if it doesn't already exist
    warning off MATLAB:MKDIR:DirectoryExists
    mexdir = ['xfemm_mex_files_for_' computer('arch')];
    mkdir (mexdir);
    warning on MATLAB:MKDIR:DirectoryExists

    cd (mexdir);
    
    % set some common compiler flags, we replace all calls to printf to
    % calls to mexPrintf
    if dodebug
        common_compiler_flags = {'-g', '-D"_MEX_DEBUG"'};
    else
        common_compiler_flags={};% common_compiler_flags = '-D"printf=mexPrintf"';
    end
    
    if verbose
        common_compiler_flags = [common_compiler_flags, {'-v'}];
    end

    libcommands = {'-I"../cfemm/fpproc"', ...
                   '-I"../cfemm/libfemm"', ...
                   '-I"../cfemm/libfemm/liblua"', ...
                   '-I"../postproc"', ...
                   '../cfemm/fpproc/libfpproc.a' };

    % put all the compiler commands in a cell array
    mexcommands = [ common_compiler_flags, ...
                    { ...
                      '../postproc/fpproc_interface_mex.cpp', ...
                      '../postproc/fpproc_interface.cpp', ...
                    }, ...
                    libcommands ...
                  ];

    if isoctave
        mkoctfile('--mex', mexcommands{:});
    else
        % call mex with the appropriately constructed commands
        mex(mexcommands{:});
    end

end
