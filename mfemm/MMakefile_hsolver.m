function [rules,vars] = MMakefile_hsolver (varargin)

    options.Verbose = false;
    options.Debug = false;
    options.DoCrossBuildWin64 = false;
    options.W64CrossBuildMexLibsDir = '';
    
    options = mmake.parse_pv_pairs (options, varargin);
    
%     mfemmdeps.getmfilepath (mfilename);

%     if ispc
%         trilibraryflag = '-DCPU86';
%     else
%         trilibraryflag = '-DLINUX';
%     end

    % flags that will be passed direct to mex
%     vars.MEXFLAGS = ['${MEXFLAGS} -I"../cfemm/hsolver" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ', trilibraryflag];
    vars.MEXFLAGS = '${MEXFLAGS} -I"../cfemm/hsolver" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ';
    if isunix && ~mfemmdeps.isoctave ()
        if options.Debug
            vars.OPTIMFLAGS = '-OO';
            vars.MEXFLAGS = [vars.MEXFLAGS, ' CXXOPTIMFLAGS="-O0 -DDEBUG"'];
        else
            vars.OPTIMFLAGS = '-O2';
            vars.MEXFLAGS = [vars.MEXFLAGS, ' CXXOPTIMFLAGS="-O2 -DNDEBUG"'];
        end
    end
    
    vars.CXXFLAGS = '${CXXFLAGS}';
    
    if mfemmdeps.isoctave
        setenv('CFLAGS','-std=c++11'); %vars.CXXFLAGS = [vars.CXXFLAGS, ' -std=c++11'];
        setenv('CXXFLAGS','-std=c++11');
    end
    
%     vars.CXXFLAGS = [vars.CXXFLAGS, ' -std=c++14'];
    
    vars.LDFLAGS = '${LDFLAGS} -lstdc++';
    
    [libluacomplex_sources, libluacomplex_headers] = getlibluasources ();
    
    libfemm_sources = getlibfemmsources ();
    
    hsolver_sources = { ...
        'hsolver.cpp', ...
    };

    [ libluacomplex_objs,  libluacomplex_rules ] = ... 
         mmake.sources2rules ( libluacomplex_sources, ...
                               'SourceDir', fullfile('..', 'cfemm', 'libfemm', 'liblua') );
                                            
    [ libfemm_objs, libfemm_rules ] = ...
         mmake.sources2rules ( libfemm_sources, ...
                               'SourceDir', fullfile('..', 'cfemm', 'libfemm') );
    
    [ hsolver_objs, hsolver_rules ] = ...
         mmake.sources2rules ( hsolver_sources, ...
                               'SourceDir', fullfile('..', 'cfemm', 'hsolver') );
                                      
    vars.OBJS = [ libluacomplex_objs, ...
                  libfemm_objs, ...
                  hsolver_objs, ...
                  {'mexhsolver.cpp'}, ...
                ];
            
    if options.DoCrossBuildWin64 
        
        vars = mfemmdeps.mmake_check_cross (options.W64CrossBuildMexLibsDir, vars);
        
    end

    rules = mfemmdeps.mmake_rule_1 ( 'mexhsolver', ...
                                     'DoCrossBuildWin64', options.DoCrossBuildWin64 );
    rules(1).deps = vars.OBJS;
    
    
    rules = [ rules, libluacomplex_rules, libfemm_rules, hsolver_rules ];

    rules(end+1).target = 'tidy';
    rules(end).commands = {'try; delete(''../cfemm/libfemm/liblua/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/libfemm/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/hsolver/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''*.${OBJ_EXT}''); catch; end;'};
	tidyruleind = numel (rules);
    
    rules(end+1).target = 'clean';
    rules(end).commands = [ rules(tidyruleind).commands, ...
                         {'try; delete(''*.${MEX_EXT}''); catch; end;'}];

end
