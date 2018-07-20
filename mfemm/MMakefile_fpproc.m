function [rules,vars] = MMakefile_fpproc (varargin)

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
%     vars.MEXFLAGS = ['${MEXFLAGS} -I"postproc" -I"../cfemm/fpproc" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ', trilibraryflag];
    vars.MEXFLAGS = '${MEXFLAGS} -I"postproc" -I"../cfemm/fpproc" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ';
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
    
    fpproc_sources = { ...
        'fpproc.cpp', ...
        'makemask.cpp', ...
        'CMPointVals.cpp', ... 
    };

    [ libluacomplex_objs,  libluacomplex_rules ] = ... 
         mmake.sources2rules ( libluacomplex_sources, ...
                               'SourceDir', fullfile('..', 'cfemm', 'libfemm', 'liblua') );
                                            
    [ libfemm_objs, libfemm_rules ] = ...
         mmake.sources2rules ( libfemm_sources, ...
                               'SourceDir', fullfile('..', 'cfemm', 'libfemm') );
    
    [ fpproc_objs, fpproc_rules ] = ...
         mmake.sources2rules ( fpproc_sources, ...
                               'SourceDir', fullfile('..', 'cfemm', 'fpproc') );
                                      
    vars.OBJS = [ libluacomplex_objs, ...
                  libfemm_objs, ...
                  fpproc_objs, ...
                  {'postproc/fpproc_interface.${OBJ_EXT}', 'postproc/fpproc_interface_mex.${OBJ_EXT}' } 
                ];

    % mexfmesher.${MEX_EXT}: ${OBJS}
    %     mex $^ -output $@
    rules(1).target = {'fpproc_interface_mex.${MEX_EXT}'};
    rules(1).deps = vars.OBJS;
    rules(1).commands = 'mex ${MEXFLAGS} $^ dummy.cpp -output $@';
    
    rules = [ rules, libluacomplex_rules, libfemm_rules, fpproc_rules ];

    rules(end+1).target = 'postproc/fpproc_interface.${OBJ_EXT}';
    rules(end).deps = 'postproc/fpproc_interface.h';

    rules(end+1).target = 'postproc/fpproc_interface_mex.${OBJ_EXT}';
    rules(end).deps = 'postproc/class_handle.hpp';
    
    
    rules(end+1).target = 'tidy';
    rules(end).commands = { 'try; delete(''../cfemm/libfemm/liblua/*.${OBJ_EXT}''); catch; end;', ...
                            'try; delete(''../cfemm/libfemm/*.${OBJ_EXT}''); catch; end;', ...
                            'try; delete(''../cfemm/fpproc/*.${OBJ_EXT}''); catch; end;', ...
                            'try; delete(''postproc/*.${OBJ_EXT}''); catch; end;', ...
                            'try; delete(''*.${OBJ_EXT}''); catch; end;'};
    tidyruleind = numel (rules);
    
    rules(end+1).target = 'clean';
    rules(end).commands = [ rules(tidyruleind).commands, ...
                            {'try; delete(''*.${MEX_EXT}''); catch; end;'}];

end


