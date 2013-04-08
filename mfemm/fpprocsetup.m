function fpprocsetup(dodebug)
% compiles the fpproc mexfunction

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
    cd(fileparts(which('fpprocsetup.m')));

    % set some common compiler flags, we replace all calls to printf to
    % calls to mexPrintf
    if dodebug
        common_compiler_flags = {'-g', '-D"_MEX_DEBUG"'};
    else
        common_compiler_flags={};% common_compiler_flags = '-D"printf=mexPrintf"';
    end

    % TODO: build the libries?
    libcommands = {'./pfemm/fpproc/libfpproc.a', ...
                   './pfemm/liblua/liblua.a', ...
                   './pfemm/libfemm/libfemm.a', ...
                   '-I"./pfemm/fpproc"','-I"./pfemm/liblua"','-I"./pfemm/libfemm"', '-I"./postproc"'};

    % put all the compiler commands in a cell array
    mexcommands = [ common_compiler_flags, ...
                    { ...
                      '-v', ...
                      './postproc/fpproc_interface_mex.cpp', ...
                      './postproc/fpproc_interface.cpp', ...
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
