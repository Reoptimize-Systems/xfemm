function filepath = getmfilepath(mfile)
% getmfilepath: gets the directory containing an mfile
%
% Syntax
%
% filepath = getmfilepath(mfile)
%
% Input
%
% mfile is a string containing the name of the mfile for which the location
% is to be found, the .m extension is optional
% 
% Output
%
% filepath is the directory path containing the specified mfile
%

    if exist(mfile, 'file') == 2
        
       filepath = fileparts(which(mfile));
       
    else
        error('UTILS:nofile', 'm-file does not appear to exist')
    end
    
end