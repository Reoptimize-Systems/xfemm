function fsolver(filename)
% solves a finite element problem described by a .fem file and which has
% already been meshed using fmesher
%

    % if present, strip the .fem extension to get the base file name for
    % mexfsolver
    if strcmpi(filename(end-4:end), '.fem')
        filename = filename(1:end-4);
    end
    
    if exist(filename, 'file') ~= 2
        error('The supplied filename location cannot be found.')
    end
    
    exts = {'.ele', '.node', '.pbc', '.poly', '.edge'};
    
    missingfilestr = '';
    anymissing = false;
    % check for the existance of missing files
    for i = 1:numel(exts)
        if ~exist([filename, exts{i}], 'file');
            anymissing = true;
            missingfilestr = sprintf('%s%s\n', missingfilestr, [filename, exts{i}])
        end
    end
    
    if anymissing
        error('The following required files were missing\n%sPerhaps you need to rerun fmesher?');
    end

    mexfsolver(filename)

end