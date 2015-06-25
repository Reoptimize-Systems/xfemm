function [rules,vars] = MMakefile_fmesher ()

    thisfilepath = mfemmdeps.getmfilepath (mfilename);

    if ispc
        trilibraryflag = '-DCPU86';
    else
        trilibraryflag = '-DLINUX';
    end

    % flags that will be passed direct to mex
    vars.MEXFLAGS = ['${MEXFLAGS} -I"../cfemm/fmesher" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ', trilibraryflag];
    
    cfemmpath = fullfile (thisfilepath, '..', 'cfemm');
    libfemmpath = fullfile (cfemmpath, 'libfemm');
    libluapath = fullfile (libfemmpath, 'liblua');

    vars.OBJS = { ...
      ... % liblua
      fullfile(libluapath, 'lapi.${OBJ_EXT}'), ...  
      fullfile(libluapath, 'lcode.${OBJ_EXT}'), ...  
      fullfile(libluapath, 'ldo.${OBJ_EXT}'), ...  
      fullfile(libluapath, 'liolib.${OBJ_EXT}'), ...  
      fullfile(libluapath, 'lmem.${OBJ_EXT}'), ...  
      fullfile(libluapath, 'lstate.${OBJ_EXT}'), ...  
      fullfile(libluapath, 'ltable.${OBJ_EXT}'), ... 
      fullfile(libluapath, 'lundump.${OBJ_EXT}'), ...
      fullfile(libluapath, 'lauxlib.${OBJ_EXT}'), ... 
      fullfile(libluapath, 'ldblib.${OBJ_EXT}'), ...
      fullfile(libluapath, 'lfunc.${OBJ_EXT}'), ...
      fullfile(libluapath, 'llex.${OBJ_EXT}'), ...    
      fullfile(libluapath, 'lobject.${OBJ_EXT}'), ...
      fullfile(libluapath, 'lstring.${OBJ_EXT}'), ...
      fullfile(libluapath, 'ltests.${OBJ_EXT}'), ...
      fullfile(libluapath, 'lvm.${OBJ_EXT}'), ...
      fullfile(libluapath, 'lbaselib.${OBJ_EXT}'), ...
      fullfile(libluapath, 'ldebug.${OBJ_EXT}'), ...
      fullfile(libluapath, 'lgc.${OBJ_EXT}'), ...  
      fullfile(libluapath, 'lmathlib.${OBJ_EXT}'), ... 
      fullfile(libluapath, 'lparser.${OBJ_EXT}'), ... 
      fullfile(libluapath, 'lstrlib.${OBJ_EXT}'), ...
      fullfile(libluapath, 'ltm.${OBJ_EXT}'), ...   
      fullfile(libluapath, 'lzio.${OBJ_EXT}'), ...
      ... % libfemm
      fullfile(libfemmpath, 'cspars.${OBJ_EXT}'), ...
      fullfile(libfemmpath, 'cuthill.${OBJ_EXT}'), ...
      fullfile(libfemmpath, 'feasolver.${OBJ_EXT}'), ...
      fullfile(libfemmpath, 'femmcomplex.${OBJ_EXT}'), ...
      fullfile(libfemmpath, 'fparse.${OBJ_EXT}'), ...
      fullfile(libfemmpath, 'fullmatrix.${OBJ_EXT}'), ...
      fullfile(libfemmpath, 'spars.${OBJ_EXT}'), ...
      ... % fmesher
      fullfile(cfemmpath, 'fmesher', 'fmesher.${OBJ_EXT}'), ... 
      fullfile(cfemmpath, 'fmesher', 'intpoint.${OBJ_EXT}'), ... 
      fullfile(cfemmpath, 'fmesher', 'nosebl.${OBJ_EXT}'), ...  
      fullfile(cfemmpath, 'fmesher', 'triangle.${OBJ_EXT}'), ...  
      fullfile(cfemmpath, 'fmesher', 'writepoly.${OBJ_EXT}'), ...
      ... % mexfunction
      'mexfmesher.cpp' };

    % mexfmesher.${MEX_EXT}: ${OBJS}
    %     mex $^ -output $@
    rules(1).target = {'mexfmesher.${MEX_EXT}'};
    rules(1).deps = vars.OBJS;
    rules(1).commands = 'mex ${MEXFLAGS} $^ -output $@';
    
    % created the following using:
    % clc
    % for i = 1:numel (vars.OBJS)
    %     fprintf ('rules(end+1).target = ''%s.${OBJ_EXT}'';\nrules(end).deps = ''%s.h'';\n\n', vars.OBJS{i}(1:end-11), vars.OBJS{i}(1:end-11));
    % end

    rules(end+1).target = fullfile (libluapath, 'lapi.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lapi.h');

    rules(end+1).target = fullfile (libluapath, 'lcode.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lcode.h');

    rules(end+1).target = fullfile (libluapath, 'ldo.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'ldo.h');

%     rules(end+1).target = fullfile (libluapath, 'liolib.${OBJ_EXT}');
%     rules(end).deps = fullfile (libluapath, 'liolib.h');

    rules(end+1).target = fullfile (libluapath, 'lmem.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lmem.h');

    rules(end+1).target = fullfile (libluapath, 'lstate.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lstate.h');

    rules(end+1).target = fullfile (libluapath, 'ltable.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'ltable.h');

    rules(end+1).target = fullfile (libluapath, 'lundump.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lundump.h');

    rules(end+1).target = fullfile (libluapath, 'lauxlib.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lauxlib.h');

%     rules(end+1).target = fullfile (libluapath, 'ldblib.${OBJ_EXT}');
%     rules(end).deps = fullfile (libluapath, 'ldblib.h');

    rules(end+1).target = fullfile (libluapath, 'lfunc.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lfunc.h');

    rules(end+1).target = fullfile (libluapath, 'llex.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'llex.h');

    rules(end+1).target = fullfile (libluapath, 'lobject.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lobject.h');

    rules(end+1).target = fullfile (libluapath, 'lstring.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lstring.h');

%     rules(end+1).target = fullfile (libluapath, 'ltests.${OBJ_EXT}');
%     rules(end).deps = fullfile (libluapath, 'ltests.h');

    rules(end+1).target = fullfile (libluapath, 'lvm.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lvm.h');

%     rules(end+1).target = fullfile (libluapath, 'lbaselib.${OBJ_EXT}');
%     rules(end).deps = fullfile (libluapath, 'lbaselib.h');

    rules(end+1).target = fullfile (libluapath, 'ldebug.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'ldebug.h');

    rules(end+1).target = fullfile (libluapath, 'lgc.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lgc.h');

%     rules(end+1).target = fullfile (libluapath, 'lmathlib.${OBJ_EXT}');
%     rules(end).deps = fullfile (libluapath, 'lmathlib.h');

    rules(end+1).target = fullfile (libluapath, 'lparser.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lparser.h');

%     rules(end+1).target = fullfile (libluapath, 'lstrlib.${OBJ_EXT}');
%     rules(end).deps = fullfile (libluapath, 'lstrlib.h');

    rules(end+1).target = fullfile (libluapath, 'ltm.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'ltm.h');

    rules(end+1).target = fullfile (libluapath, 'lzio.${OBJ_EXT}');
    rules(end).deps = fullfile (libluapath, 'lzio.h');

%     rules(end+1).target = fullfile(libfemmpath, 'cspars.${OBJ_EXT}');
%     rules(end).deps = fullfile(libfemmpath, 'cspars.h');

%     rules(end+1).target = fullfile(libfemmpath, 'cuthill.${OBJ_EXT}');
%     rules(end).deps = fullfile(libfemmpath, 'cuthill.h');

    rules(end+1).target = fullfile(libfemmpath, 'feasolver.${OBJ_EXT}');
    rules(end).deps = fullfile(libfemmpath, 'feasolver.h');

    rules(end+1).target = fullfile(libfemmpath, 'femmcomplex.${OBJ_EXT}');
    rules(end).deps = fullfile(libfemmpath, 'femmcomplex.h');

    rules(end+1).target = fullfile(libfemmpath, 'fparse.${OBJ_EXT}');
    rules(end).deps = fullfile(libfemmpath, 'fparse.h');

    rules(end+1).target = fullfile(libfemmpath, 'fullmatrix.${OBJ_EXT}');
    rules(end).deps = fullfile(libfemmpath, 'fullmatrix.h');

    rules(end+1).target = fullfile(libfemmpath, 'spars.${OBJ_EXT}');
    rules(end).deps = fullfile(libfemmpath, 'spars.h');

    rules(end+1).target = fullfile(cfemmpath, 'fmesher/fmesher.${OBJ_EXT}');
    rules(end).deps = fullfile(cfemmpath, 'fmesher/fmesher.h');

    rules(end+1).target = fullfile(cfemmpath, 'fmesher/intpoint.${OBJ_EXT}');
    rules(end).deps = fullfile(cfemmpath, 'fmesher/intpoint.h');

    rules(end+1).target = fullfile(cfemmpath, 'fmesher/nosebl.${OBJ_EXT}');
    rules(end).deps = fullfile(cfemmpath, 'fmesher/nosebl.h');

    rules(end+1).target = fullfile(cfemmpath, 'fmesher/triangle.${OBJ_EXT}');
    rules(end).deps = fullfile(cfemmpath, 'fmesher/triangle.h');
% 
%     rules(end+1).target = fullfile(cfemmpath, 'fmesher/writepoly.${OBJ_EXT}');
%     rules(end).deps = fullfile(cfemmpath, 'fmesher/writepoly.h');

    rules(3).target = 'tidy';
    rules(3).commands = {'try; delete(''../cfemm/libfemm/liblua/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/libfemm/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/fmesher/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''*.${OBJ_EXT}''); catch; end;'};
    
    rules(4).target = 'clean';
    rules(4).commands = [ rules(3).commands, ...
                         {'try; delete(''*.${MEX_EXT}''); catch; end;'}];

end
