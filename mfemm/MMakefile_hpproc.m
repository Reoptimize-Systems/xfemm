function [rules,vars] = MMakefile_hpproc (varargin)

	options.DoCrossBuildWin64 = false;
    options.W64CrossBuildMexLibsDir = '';
    options.Verbose = false;
    options.Debug = false;
    
    options = mmake.parse_pv_pairs (options, varargin);
    
%     mfemmdeps.getmfilepath (mfilename);

%     if ispc
%         trilibraryflag = '-DCPU86';
%     else
%         trilibraryflag = '-DLINUX';
%     end

    % flags that will be passed direct to mex
%     vars.MEXFLAGS = ['${MEXFLAGS} -I"postproc" -I"../cfemm/hpproc" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ', trilibraryflag];
    vars.MEXFLAGS = '${MEXFLAGS} -I"postproc" -I"../cfemm/hpproc" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ';
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
    
    hpproc_sources = { ...
        'hpproc.cpp', ...
        'CHPointVals.cpp', ...
    };

    [ libluacomplex_objs,  libluacomplex_rules ] = ... 
         mmake.sources2rules ( libluacomplex_sources, ...
                               'SourceDir', fullfile('..', 'cfemm', 'libfemm', 'liblua') );
                                            
    [ libfemm_objs, libfemm_rules ] = ...
         mmake.sources2rules ( libfemm_sources, ...
                               'SourceDir', fullfile('..', 'cfemm', 'libfemm') );
    
    [ hpproc_objs, hpproc_rules ] = ...
         mmake.sources2rules ( hpproc_sources, ...
                               'SourceDir', fullfile('..', 'cfemm', 'hpproc') );
                           
    vars.OBJS = [ libluacomplex_objs, ...
                  libfemm_objs, ...
                  hpproc_objs, ...
                  {'postproc/hpproc_interface.${OBJ_EXT}', ...
                   'postproc/hpproc_interface_mex.${OBJ_EXT}' } 
                ];

    % mexfmesher.${MEX_EXT}: ${OBJS}
    %     mex $^ -output $@
%     rules(1).target = 'hpproc_interface_mex.${MEX_EXT}';
%     rules(1).deps = vars.OBJS;
%     rules(1).commands = 'mex ${MEXFLAGS} ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $^ dummy.cpp -output $@';

    if options.DoCrossBuildWin64 
        
        vars = mfemmdeps.mmake_check_cross (options.W64CrossBuildMexLibsDir, vars);
        
    end
    
    if options.Verbose
        extra_mex_args = [extra_mex_args, ' -v'];
    end

    rules = mfemmdeps.mmake_rule_1 ( 'hpproc_interface_mex', ...
                                     'DoCrossBuildWin64', options.DoCrossBuildWin64 );
    rules(1).deps = vars.OBJS;
    
    rules = [ rules, libluacomplex_rules, libfemm_rules, hpproc_rules ];
    
    rules(end+1).target = 'postproc/hpproc_interface.${OBJ_EXT}';
    rules(end).deps = 'postproc/hpproc_interface.h';

    rules(end+1).target = 'postproc/hpproc_interface_mex.${OBJ_EXT}';
    rules(end).deps = 'postproc/class_handle.hpp';
    
    rules(end+1).target = 'tidy';
    rules(end).commands = {'try; delete(''../cfemm/libfemm/liblua/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/libfemm/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/hpproc/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''postproc/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''*.${OBJ_EXT}''); catch; end;'};
    tidyruleind = numel (rules);
    
    rules(end+1).target = 'clean';
    rules(end).commands = [ rules(tidyruleind).commands, ...
                         {'try; delete(''*.${MEX_EXT}''); catch; end;'}];

end
