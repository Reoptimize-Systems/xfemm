function rules = mmake_rule_1 (mexname, varargin)

    options.DoCrossBuildWin64 = false;
    
    options = parse_pv_pairs (options, varargin);

    if options.DoCrossBuildWin64
        rules(1).target = {sprintf('%s.mexw64', mexname)};
        rules(1).commands = sprintf ('mex ${MEXFLAGS} ${COMPILERKEY}="${COMPILER}" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $^ EXE="%s.mexw64"', mexname);
    else
        rules(1).target = {sprintf('%s.${MEX_EXT}', mexname)};
        if mfemmdeps.isoctave ()
            rules(1).commands = sprintf ('mex ${MEXFLAGS} $^ --output $@');
        else
            rules(1).commands = sprintf ('mex ${MEXFLAGS} ${COMPILERKEY}="${COMPILER}" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $^ -output $@');
        end
    end
    
end