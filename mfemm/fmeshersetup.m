function fmeshersetup(dodebug)
% compiles the fmesher mexfunction 

    if nargin < 1
        dodebug = false;
    end
    
    if ~isoctave
        
        % First check a C++ compiler is present
        cc = mex.getCompilerConfigurations('C++', 'Selected');

        if isempty(cc)
            error(['You must have a C++ compiler setup. Run mex -setup to choose ', ...
                   'an appropriate compiler, you may need to install one on your system.']);
        end

    end

    % store the current directory
    origdir = pwd;
    
    % change to the mfemm directory (the directory this file is in)
    cd(fileparts(which('fmeshersetup.m')));

    % set some common compiler flags, we replace all calls to printf to
    % calls to mexPrintf
    if dodebug
        common_compiler_flags = '-g -D"printf=mexPrintf"';
    else
        common_compiler_flags = '-D"printf=mexPrintf"';
    end
    
    % add an appropriate flag for the trilibrary C external depending on
    % computer architecture
    if ispc
        trilibraryflag = '-DCPU86';
    else
        trilibraryflag = '-DLINUX';
    end

    % call mex with the appropriately constructed string
    if isoctave
        
        mkoctfile('--mex', ...
                  common_compiler_flags, ...
                  trilibraryflag, ...
                  'mexfmesher.cpp', ...
                  'fmesher/fmesher.cpp', ...
                  'fmesher/triangle.c', ...
                  'fmesher/complex.cpp', ...
                  'fmesher/intpoint.cpp', ...
                  'fmesher/nosebl.cpp', ...
                  'fmesher/writepoly.cpp');
             
    else
        
        mex(common_compiler_flags, ...
            trilibraryflag, ...
            'mexfmesher.cpp', ...
            'fmesher/fmesher.cpp', ...
            'fmesher/triangle.c', ...
            'fmesher/complex.cpp', ...
            'fmesher/intpoint.cpp', ...
            'fmesher/nosebl.cpp', ...
            'fmesher/writepoly.cpp');
        
    end
     
     
     % return to original directory
     cd(origdir);
    
end