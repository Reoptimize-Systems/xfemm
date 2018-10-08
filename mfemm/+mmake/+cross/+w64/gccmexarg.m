function gcc_arg = gccmexarg (varargin)
% get the full path to cross-compiler gcc

    options.CrossPrefix = 'x86_64-w64-mingw32.static';
    
    options = mmake.parse_pv_pairs (options, varargin);

    gcc_arg = mmake.cross.gccmexarg (options.CrossPrefix);

end