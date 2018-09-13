function rules = mmake_rule_1 (mexname, varargin)

    options.DoCrossBuildWin64 = false;
    options.ExtraMexArgs = '';
    
    options = parse_pv_pairs (options, varargin);

    if options.DoCrossBuildWin64
        rules(1).target = {sprintf('%s.mexw64', mexname)};
        rules(1).commands = sprintf ('mex ${MEXFLAGS} ${COMPILERKEY}="${COMPILER}" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" %s $^ EXE="%s.mexw64"', options.ExtraMexArgs, mexname);
    else
        rules(1).target = {sprintf('%s.${MEX_EXT}', mexname)};
        if mfemmdeps.isoctave ()
            rules(1).commands = sprintf ('mex ${MEXFLAGS} %s $^ --output $@', options.ExtraMexArgs);
        else
            rules(1).commands = sprintf ('mex ${MEXFLAGS} ${COMPILERKEY}="${COMPILER}" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" %s $^ -output $@', options.ExtraMexArgs);
        end
    end
    
end