function [rules,vars] = MMakefile_fmesher (varargin)

    options.DoCrossBuildWin64 = false;
    options.Verbose = false;
    options.Debug = false;
    
    options = mfemmdeps.parse_pv_pairs (options, varargin);

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
    
    vars.LDFLAGS = '${LDFLAGS} -lstdc++';

    % flags that will be passed direct to mex
    vars.MEXFLAGS = ['${MEXFLAGS} -I"../cfemm/fmesher" -I"../cfemm/fmesher/triangle" -I"../cfemm/libfemm" -I"../cfemm/libfemm/liblua" ', trilibraryflag];
    
    if options.Verbose
        vars.MEXFLAGS = [vars.MEXFLAGS, ' -v'];
    end
    
    if isunix && ~mfemmdeps.isoctave () 
        if options.Debug == false
            vars.OPTIMFLAGS = ['-O2'];
            vars.MEXFLAGS = [vars.MEXFLAGS, ' CXXOPTIMFLAGS="-O2 -DNDEBUG"'];
        else
            vars.OPTIMFLAGS = ['-O0'];
            vars.MEXFLAGS = [vars.MEXFLAGS, ' CXXOPTIMFLAGS="-O0 -DDEBUG"'];
        end
    end
    
    if ~ismscompiler
        vars.CXXFLAGS = '${CXXFLAGS} -fpermissive';
        vars.CFLAGS = '${CFLAGS} -fpermissive';
    end
%     vars.
    
    cfemmpath = [thisfilepath, '/..', '/cfemm'];
    fmesherpath = [cfemmpath, '/fmesher']; 
    trianglepath = [fmesherpath, '/triangle']; 
    libfemmpath = [cfemmpath, '/libfemm'];
    libluapath = [libfemmpath, '/liblua'];

    vars.OBJS = { ...
      ... % liblua
      [libluapath, '/femmcomplex.${OBJ_EXT}'], ...
      [libluapath, '/lapi.${OBJ_EXT}'], ...  
      [libluapath, '/lcode.${OBJ_EXT}'], ...  
      [libluapath, '/ldo.${OBJ_EXT}'], ...  
      [libluapath, '/liolib.${OBJ_EXT}'], ...  
      [libluapath, '/lmem.${OBJ_EXT}'], ...  
      [libluapath, '/lstate.${OBJ_EXT}'], ...  
      [libluapath, '/ltable.${OBJ_EXT}'], ... 
      [libluapath, '/lundump.${OBJ_EXT}'], ...
      [libluapath, '/lauxlib.${OBJ_EXT}'], ... 
      [libluapath, '/ldblib.${OBJ_EXT}'], ...
      [libluapath, '/lfunc.${OBJ_EXT}'], ...
      [libluapath, '/llex.${OBJ_EXT}'], ...    
      [libluapath, '/lobject.${OBJ_EXT}'], ...
      [libluapath, '/lstring.${OBJ_EXT}'], ...
      [libluapath, '/ltests.${OBJ_EXT}'], ...
      [libluapath, '/lvm.${OBJ_EXT}'], ...
      [libluapath, '/lbaselib.${OBJ_EXT}'], ...
      [libluapath, '/ldebug.${OBJ_EXT}'], ...
      [libluapath, '/lgc.${OBJ_EXT}'], ...  
      [libluapath, '/lmathlib.${OBJ_EXT}'], ... 
      [libluapath, '/lparser.${OBJ_EXT}'], ... 
      [libluapath, '/lstrlib.${OBJ_EXT}'], ...
      [libluapath, '/ltm.${OBJ_EXT}'], ...   
      [libluapath, '/lzio.${OBJ_EXT}'], ...
      ... % libfemm
      [libfemmpath, '/cspars.${OBJ_EXT}'], ...
      [libfemmpath, '/cuthill.${OBJ_EXT}'], ...
      [libfemmpath, '/feasolver.${OBJ_EXT}'], ...
      [libfemmpath, '/FemmProblem.${OBJ_EXT}'], ...
      [libfemmpath, '/FemmReader.${OBJ_EXT}'], ...
      [libfemmpath, '/fparse.${OBJ_EXT}'], ...
      [libfemmpath, '/fullmatrix.${OBJ_EXT}'], ...
      [libfemmpath, '/spars.${OBJ_EXT}'], ...
      [libfemmpath, '/CArcSegment.${OBJ_EXT}'], ...
      [libfemmpath, '/CBlockLabel.${OBJ_EXT}'], ...
      [libfemmpath, '/CBoundaryProp.${OBJ_EXT}'], ...
      [libfemmpath, '/CCircuit.${OBJ_EXT}'], ...
      [libfemmpath, '/CCommonPoint.${OBJ_EXT}'], ...
      [libfemmpath, '/CMaterialProp.${OBJ_EXT}'], ...
      [libfemmpath, '/CNode.${OBJ_EXT}'], ...
      [libfemmpath, '/CPointProp.${OBJ_EXT}'], ...
      [libfemmpath, '/CSegment.${OBJ_EXT}'], ...
      [libfemmpath, '/IntPoint.${OBJ_EXT}'], ... 
      ... % fmesher
      [fmesherpath, '/fmesher.${OBJ_EXT}'], ... 
      [fmesherpath, '/nosebl.${OBJ_EXT}'], ...  
      [fmesherpath, '/writepoly.${OBJ_EXT}'], ...
      [trianglepath, '/triangle.${OBJ_EXT}'], ...  
      ... % mexfunction
      'mexfmesher.cpp' };

    % mexfmesher.${MEX_EXT}: ${OBJS}
    %     mex $^ -output $@
    rules(1).target = {'mexfmesher.${MEX_EXT}'};
    rules(1).deps = vars.OBJS;
    rules(1).commands = 'mex ${MEXFLAGS} $^ dummy.cpp -output $@';
    
    % created the following using:
    % clc
    % for i = 1:numel (vars.OBJS)
    %     fprintf ('rules(end+1).target = ''%s.${OBJ_EXT}'';\nrules(end).deps = ''%s.h'';\n\n', vars.OBJS{i}(1:end-11), vars.OBJS{i}(1:end-11));
    % end

    rules(end+1).target = [libluapath, '/femmcomplex.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/femmcomplex.h'];

    rules(end+1).target = [libluapath, '/lapi.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lapi.h'];

    rules(end+1).target = [libluapath, '/lcode.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lcode.h'];

    rules(end+1).target = [libluapath, '/ldo.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/ldo.h'];

%     rules(end+1).target = [libluapath, '/liolib.${OBJ_EXT}'];
%     rules(end).deps = [libluapath, '/liolib.h'];

    rules(end+1).target = [libluapath, '/lmem.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lmem.h'];

    rules(end+1).target = [libluapath, '/lstate.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lstate.h'];

    rules(end+1).target = [libluapath, '/ltable.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/ltable.h'];

    rules(end+1).target = [libluapath, '/lundump.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lundump.h'];

    rules(end+1).target = [libluapath, '/lauxlib.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lauxlib.h'];

%     rules(end+1).target = [libluapath, '/ldblib.${OBJ_EXT}'];
%     rules(end).deps = [libluapath, '/ldblib.h'];

    rules(end+1).target = [libluapath, '/lfunc.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lfunc.h'];

    rules(end+1).target = [libluapath, '/llex.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/llex.h'];

    rules(end+1).target = [libluapath, '/lobject.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lobject.h'];

    rules(end+1).target = [libluapath, '/lstring.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lstring.h'];

%     rules(end+1).target = [libluapath, '/ltests.${OBJ_EXT}'];
%     rules(end).deps = [libluapath, '/ltests.h'];

    rules(end+1).target = [libluapath, '/lvm.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lvm.h'];

%     rules(end+1).target = [libluapath, '/lbaselib.${OBJ_EXT}'];
%     rules(end).deps = [libluapath, '/lbaselib.h'];

    rules(end+1).target = [libluapath, '/ldebug.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/ldebug.h'];

    rules(end+1).target = [libluapath, '/lgc.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lgc.h'];

%     rules(end+1).target = [libluapath, '/lmathlib.${OBJ_EXT}'];
%     rules(end).deps = [libluapath, '/lmathlib.h'];

    rules(end+1).target = [libluapath, '/lparser.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lparser.h'];

%     rules(end+1).target = [libluapath, '/lstrlib.${OBJ_EXT}'];
%     rules(end).deps = [libluapath, '/lstrlib.h'];

    rules(end+1).target = [libluapath, '/ltm.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/ltm.h'];

    rules(end+1).target = [libluapath, '/lzio.${OBJ_EXT}'];
    rules(end).deps = [libluapath, '/lzio.h'];

%     rules(end+1).target = [libfemmpath, '/cspars.${OBJ_EXT}'];
%     rules(end).deps = [libfemmpath, '/cspars.h'];

%     rules(end+1).target = [libfemmpath, '/cuthill.${OBJ_EXT}'];
%     rules(end).deps = [libfemmpath, '/cuthill.h'];

    rules(end+1).target = [libfemmpath, '/feasolver.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/feasolver.h'];

    rules(end+1).target = [libfemmpath, '/FemmProblem.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/FemmProblem.h'];

    rules(end+1).target = [libfemmpath, '/FemmReader.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/FemmReader.h'];

    rules(end+1).target = [libfemmpath, '/fparse.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/fparse.h'];

    rules(end+1).target = [libfemmpath, '/fullmatrix.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/fullmatrix.h'];

    rules(end+1).target = [libfemmpath, '/spars.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/spars.h'];

    rules(end+1).target = [libfemmpath, '/CArcSegment.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/CArcSegment.h'];

    rules(end+1).target = [libfemmpath, '/CBlockLabel.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/CBlockLabel.h'];

    rules(end+1).target = [libfemmpath, '/CBoundaryProp.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/CBoundaryProp.h'];

    rules(end+1).target = [libfemmpath, '/CCircuit.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/CCircuit.h'];

    rules(end+1).target = [libfemmpath, '/CCommonPoint.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/CCommonPoint.h'];

    rules(end+1).target = [libfemmpath, '/CMaterialProp.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/CMaterialProp.h'];

    rules(end+1).target = [libfemmpath, '/CNode.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/CNode.h'];

    rules(end+1).target = [libfemmpath, '/CPointProp.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/CPointProp.h'];

    rules(end+1).target = [libfemmpath, '/CSegment.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/CSegment.h'];

    rules(end+1).target = [libfemmpath, '/IntPoint.${OBJ_EXT}'];
    rules(end).deps = [libfemmpath, '/IntPoint.h'];

    rules(end+1).target = [fmesherpath, '/fmesher.${OBJ_EXT}'];
    rules(end).deps = [fmesherpath, '/fmesher.h'];

    rules(end+1).target = [fmesherpath, '/nosebl.${OBJ_EXT}'];
    rules(end).deps = [fmesherpath, '/nosebl.h'];

    rules(end+1).target = [trianglepath, '/triangle.${OBJ_EXT}'];
    rules(end).deps = [trianglepath, '/triangle.h'];

% 
%     rules(end+1).target = [fmesherpath, '/writepoly.${OBJ_EXT}');
%     rules(end).deps = [fmesherpath, '/writepoly.h'];

    rules(3).target = 'tidy';
    rules(3).commands = {'try; delete(''../cfemm/libfemm/liblua/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/libfemm/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''../cfemm/fmesher/*.${OBJ_EXT}''); catch; end;', ...
                         'try; delete(''*.${OBJ_EXT}''); catch; end;'};
    
    rules(4).target = 'clean';
    rules(4).commands = [ rules(3).commands, ...
                         {'try; delete(''*.${MEX_EXT}''); catch; end;'}];
                     
    % mexfmesher.${MEX_EXT}: ${OBJS}
    %     mex $^ -output $@
%     rules(1).target = {'crossw64'};
%     rules(1).deps = vars.OBJS;
%     rules(1).commands = 'mex ${MEXFLAGS} $^ -output mexfmesher.mexw64';

end
