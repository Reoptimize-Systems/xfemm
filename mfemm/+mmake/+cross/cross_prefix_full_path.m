function path = cross_prefix_full_path (cross_prefix)
% get full cross-compiler prefix path

    % do some backflips to get the full path to the mxe cross-compiler
    % as otherwise it isn't in the path when matlab tries to call it,
    % at least in R2018a. R2018 seems to wipe out user paths in whatever
    % environment it calls the cross compiler executable, so the full path
    % is needed if it is installed in a non-standard location

    cross_gcc = [cross_prefix, '-gcc'];

    [~, cross_gcc_full_path] = system (['which ', cross_gcc]);

    cross_gcc_full_path = strtrim (cross_gcc_full_path);

    cross_containing_dir = fileparts (cross_gcc_full_path);

    path = fullfile (cross_containing_dir, cross_prefix);
    
end