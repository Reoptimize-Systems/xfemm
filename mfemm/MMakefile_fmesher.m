function [rules,vars] = MMakefile_fmesher (varargin)

    options.DoCrossBuildWin64 = false;
    options.W64CrossBuildMexLibsDir = '';
    options.Verbose = false;
    options.Debug = false;
    
    options = mmake.parse_pv_pairs (options, varargin);

    if isunix || mfemmdeps.isoctave
        ismscompiler = false;
    else
        cc = mex.getCompilerConfigurations ('C');
        if strncmpi (cc.Manufacturer, 'Microsoft', 9)
            ismscompiler = true;
        else
            ismscompiler = false;
        end
    end
    
    thisfilepath = mfemmdeps.getmfilepath (mfilename);
    
    thisfilepath = strrep (thisfilepath, '\', '/');

    if ispc || options.DoCrossBuildWin64
        trilibraryflag = '-DCPU86';
    else
        trilibraryflag = '-DLINUX';
    end
    
    vars.LDFLAGS = '${LDFLAGS} -lstdc++  -Wl,--no-undefined';

    % flags that will be passed direct to mex
    vars.MEXFLAGS = ['${MEXFLAGS} -D_GLIBCXX_USE_CXX11_ABI=1 -I"../cfemm/fmesher" -I"../cfemm/fmesher/triangle" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ', trilibraryflag];
    
    if options.Verbose
        vars.MEXFLAGS = [vars.MEXFLAGS, ' -v'];
    end
    
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

    if mfemmdeps.isoctave ()
        setenv('CFLAGS','-std=c++11'); %vars.CXXFLAGS = [vars.CXXFLAGS, ' -std=c++11'];
        setenv('CXXFLAGS','-std=c++11');
    end
%     vars.CXXFLAGS = [vars.CXXFLAGS, ' -std=c++14'];
    
    if ~ismscompiler
        vars.CXXFLAGS = [vars.CXXFLAGS, ' -fpermissive'];
        vars.CFLAGS = '${CFLAGS} -fpermissive';
    end
%     vars.
    
    cfemmpath = fullfile (thisfilepath, '..', 'cfemm');
    fmesherpath = fullfile (cfemmpath, 'fmesher'); 
    trianglepath = fullfile (fmesherpath, 'triangle'); 
    libfemmpath = fullfile (cfemmpath, 'libfemm');
    libluacomplexpath = fullfile (libfemmpath, 'liblua');
    
    [libluacomplex_sources, libluacomplex_headers] = getlibluasources ();
    
    libfemm_sources = getlibfemmsources ();

    fmesher_sources = { ...
        'fmesher.cpp', ...
        'main.cpp', ...
        'nosebl.cpp', ...
        'writepoly.cpp', ...
    };

    triangle_sources = {'triangle.c'};

    [ libluacomplex_objs,  libluacomplex_rules ] = ... 
         mmake.sources2rules ( libluacomplex_sources, ...
                               'SourceDir', libluacomplexpath );
                                            
    [ libfemm_objs, libfemm_rules ] = ...
         mmake.sources2rules ( libfemm_sources, ...
                               'SourceDir', libfemmpath );
                           
    [ triangle_objs, triangle_rules ] = ...
         mmake.sources2rules ( triangle_sources, ...
                               'SourceDir', trianglepath );
                           
    [ fmesher_objs, fmesher_rules ] = ...
         mmake.sources2rules ( fmesher_sources, ...
                               'SourceDir', fmesherpath );
          
    vars.OBJS = [ libluacomplex_objs, ...
                  libfemm_objs, ...
                  triangle_objs, ...
                  fmesher_objs, ...
                  {'mexfmesher.cpp'}, ...
                ];
            
    if options.DoCrossBuildWin64 

        vars = mfemmdeps.mmake_check_cross (options.W64CrossBuildMexLibsDir, vars);
        
    end
    
    rules = mfemmdeps.mmake_rule_1 ( 'mexfmesher', ...
                                     'DoCrossBuildWin64', options.DoCrossBuildWin64 );
    rules(1).deps = vars.OBJS;

    
    
    rules = [ rules, libluacomplex_rules, libfemm_rules, triangle_rules, fmesher_rules ];

    rules(end+1).target = 'tidy';
    rules(end).commands = { 'try; delete(''../cfemm/libfemm/liblua/*.${OBJ_EXT}''); catch; end;', ...
                            'try; delete(''../cfemm/libfemm/*.${OBJ_EXT}''); catch; end;', ...
                            'try; delete(''../cfemm/fmesher/*.${OBJ_EXT}''); catch; end;', ...
                            'try; delete(''*.${OBJ_EXT}''); catch; end;' };
    tidyruleind = numel (rules);
    
    rules(end+1).target = 'clean';
    rules(end).commands = [ rules(tidyruleind).commands, ...
                            {'try; delete(''*.${MEX_EXT}''); catch; end;'} ];
                     
    % mexfmesher.${MEX_EXT}: ${OBJS}
    %     mex $^ -output $@
%     rules(1).target = {'crossw64'};
%     rules(1).deps = vars.OBJS;
%     rules(1).commands = 'mex ${MEXFLAGS} $^ -output mexfmesher.mexw64';

end
