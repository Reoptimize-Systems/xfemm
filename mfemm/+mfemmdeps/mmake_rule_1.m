function [rules, vars] = mmake_rule_1 (mexname, vars, varargin)

    options.DoCrossBuildWin64 = false;
    options.AddStdCppLib = false;
    
    options = mmake.parse_pv_pairs (options, varargin);

    if options.DoCrossBuildWin64
        rules(1).target = {sprintf('%s.mexw64', mexname)};
        rules(1).commands = sprintf ('mex ${MEXFLAGS} ${COMPILERKEY}="${COMPILER}" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $^ EXE="%s.mexw64"', mexname);
    else
        rules(1).target = {sprintf('%s.${MEX_EXT}', mexname)};
        if mfemmdeps.isoctave ()
            rules(1).commands = sprintf ('mex ${MEXFLAGS} $^ --output $@');
        else
            if options.AddStdCppLib
                rules(1).commands = sprintf ('mex ${MEXFLAGS} ${COMPILERKEY}="${COMPILER}" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" LINKLIBS="${LINKLIBS}" $^ -output $@');
            else
                rules(1).commands = sprintf ('mex ${MEXFLAGS} ${COMPILERKEY}="${COMPILER}" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $^ -output $@');
            end
        end
    end
    
    if ~mfemmdeps.isoctave () && options.AddStdCppLib
        if ispc
            vars.LINKLIBS = sprintf(' -L"''%s''" -llibmx -llibmex -llibmat -lm -llibmwlapack -llibmwblas -lstdc++ ' , fullfile (matlabroot, 'extern', 'lib', 'win64', 'mingw64'));
        else
            vars.LINKLIBS = sprintf(' -L''%s'' -lmx -lmex -lmat -lm -lstdc++ ', fullfile (matlabroot, 'bin', 'glnxa64'));
        end
    end
    
end