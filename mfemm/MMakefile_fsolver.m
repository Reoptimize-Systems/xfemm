function [rules,vars] = MMakefile_fsolver (varargin)

%     mfemmdeps.getmfilepath (mfilename);

%     if ispc
%         trilibraryflag = '-DCPU86';
%     else
%         trilibraryflag = '-DLINUX';
%     end

    % flags that will be passed direct to mex
%     vars.MEXFLAGS = ['${MEXFLAGS} -I"../cfemm/fsolver" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ', trilibraryflag];
    vars.MEXFLAGS = '${MEXFLAGS} -I"../cfemm/fsolver" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ';
    if isunix && ~mfemmdeps.isoctave ()
        vars.OPTIMFLAGS = ['-O2'];
        vars.MEXFLAGS = [vars.MEXFLAGS, ' CXXOPTIMFLAGS="-O2 -DNDEBUG"'];
    end
    
    vars.LDFLAGS = '${LDFLAGS} -lstdc++';
    
    vars.OBJS = { ...
      ... % liblua
      '../cfemm/libfemm/liblua/lapi.${OBJ_EXT}', ...  
      '../cfemm/libfemm/liblua/lcode.${OBJ_EXT}', ...  
      '../cfemm/libfemm/liblua/ldo.${OBJ_EXT}', ...  
      '../cfemm/libfemm/liblua/liolib.${OBJ_EXT}', ...  
      '../cfemm/libfemm/liblua/lmem.${OBJ_EXT}', ...  
      '../cfemm/libfemm/liblua/lstate.${OBJ_EXT}', ...  
      '../cfemm/libfemm/liblua/ltable.${OBJ_EXT}', ... 
      '../cfemm/libfemm/liblua/lundump.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/lauxlib.${OBJ_EXT}', ... 
      '../cfemm/libfemm/liblua/ldblib.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/lfunc.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/llex.${OBJ_EXT}', ...    
      '../cfemm/libfemm/liblua/lobject.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/lstring.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/ltests.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/lvm.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/lbaselib.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/ldebug.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/lgc.${OBJ_EXT}', ...  
      '../cfemm/libfemm/liblua/lmathlib.${OBJ_EXT}', ... 
      '../cfemm/libfemm/liblua/lparser.${OBJ_EXT}', ... 
      '../cfemm/libfemm/liblua/lstrlib.${OBJ_EXT}', ...
      '../cfemm/libfemm/liblua/ltm.${OBJ_EXT}', ...   
      '../cfemm/libfemm/liblua/lzio.${OBJ_EXT}', ...
      ... % libfemm
      '../cfemm/libfemm/cspars.${OBJ_EXT}', ...
      '../cfemm/libfemm/cuthill.${OBJ_EXT}', ...
      '../cfemm/libfemm/feasolver.${OBJ_EXT}', ...
      '../cfemm/libfemm/femmcomplex.${OBJ_EXT}', ...
      '../cfemm/libfemm/fparse.${OBJ_EXT}', ...
      '../cfemm/libfemm/fullmatrix.${OBJ_EXT}', ...
      '../cfemm/libfemm/spars.${OBJ_EXT}', ...
      '../cfemm/libfemm/CNode.${OBJ_EXT}', ...
      '../cfemm/libfemm/CBlockLabel.${OBJ_EXT}', ...
      ... % fsolver
      '../cfemm/fsolver/fsolver.${OBJ_EXT}', ... 
      '../cfemm/fsolver/harmonic2d.${OBJ_EXT}', ... 
      '../cfemm/fsolver/harmonicaxi.${OBJ_EXT}', ...  
      '../cfemm/fsolver/matprop.${OBJ_EXT}', ...  
      '../cfemm/fsolver/static2d.${OBJ_EXT}', ...
      '../cfemm/fsolver/staticaxi.${OBJ_EXT}', ...
      ... % mexfunction
      'mexfsolver.cpp' };

    % mexfmesher.${MEX_EXT}: ${OBJS}
    %     mex $^ -output $@
    rules(1).target = {'mexfsolver.${MEX_EXT}'};
    rules(1).deps = vars.OBJS;
    rules(1).commands = 'mex ${MEXFLAGS} $^ dummy.cpp -output $@';
    
    % created the following using:
    % clc
    % for i = 1:numel (vars.OBJS)
    %     fprintf ('rules(end+1).target = ''%s.${OBJ_EXT}'';\nrules(end).deps = ''%s.h'';\n\n', vars.OBJS{i}(1:end-11), vars.OBJS{i}(1:end-11));
    % end

    rules(end+1).target = '../cfemm/libfemm/liblua/lapi.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lapi.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lcode.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lcode.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/ldo.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/ldo.h';

%     rules(end+1).target = '../cfemm/libfemm/liblua/liolib.${OBJ_EXT}';
%     rules(end).deps = '../cfemm/libfemm/liblua/liolib.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lmem.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lmem.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lstate.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lstate.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/ltable.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/ltable.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lundump.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lundump.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lauxlib.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lauxlib.h';

%     rules(end+1).target = '../cfemm/libfemm/liblua/ldblib.${OBJ_EXT}';
%     rules(end).deps = '../cfemm/libfemm/liblua/ldblib.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lfunc.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lfunc.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/llex.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/llex.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lobject.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lobject.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lstring.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lstring.h';

%     rules(end+1).target = '../cfemm/libfemm/liblua/ltests.${OBJ_EXT}';
%     rules(end).deps = '../cfemm/libfemm/liblua/ltests.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lvm.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lvm.h';

%     rules(end+1).target = '../cfemm/libfemm/liblua/lbaselib.${OBJ_EXT}';
%     rules(end).deps = '../cfemm/libfemm/liblua/lbaselib.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/ldebug.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/ldebug.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lgc.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lgc.h';

%     rules(end+1).target = '../cfemm/libfemm/liblua/lmathlib.${OBJ_EXT}';
%     rules(end).deps = '../cfemm/libfemm/liblua/lmathlib.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lparser.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lparser.h';

%     rules(end+1).target = '../cfemm/libfemm/liblua/lstrlib.${OBJ_EXT}';
%     rules(end).deps = '../cfemm/libfemm/liblua/lstrlib.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/ltm.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/ltm.h';

    rules(end+1).target = '../cfemm/libfemm/liblua/lzio.${OBJ_EXT}';
    rules(end).deps = '../cfemm/libfemm/liblua/lzio.h';

    rules(end+1).target = '../cfemm/fsolver/fsolver.${OBJ_EXT}';
    rules(end).deps = '../cfemm/fsolver/fsolver.h';

    rules(end+1).target = '../cfemm/fsolver/mmesh.${OBJ_EXT}';
    rules(end).deps = '../cfemm/fsolver/mmesh.h';


    rules(3).target = 'tidy';
    rules(3).commands = {'try; delete(''../cfemm/libfemm/liblua/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/libfemm/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/fsolver/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''*.${OBJ_EXT}''); catch; end;'};
    
    rules(4).target = 'clean';
    rules(4).commands = [ rules(3).commands, ...
                         {'try; delete(''*.${MEX_EXT}''); catch; end;'}];

end
