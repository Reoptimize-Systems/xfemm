function gcc_arg = gccmexarg (cross_prefix)
% get the full path to cross-compiler gcc

    cross_full_path = mmake.cross.cross_prefix_full_path (cross_prefix);

    gcc_arg = ['GCC="', cross_full_path, '-gcc"'];

end