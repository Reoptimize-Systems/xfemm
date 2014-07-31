function filename = openprobleminfemm_mfemm(FemmProblem, filename)
% writes a FemmProblem structure to disk in the appropriate file format and 
% opens the file in FEMM
%
% Syntax
%
%  filename = openprobleminfemm_mfemm(FemmProblem, filename)
%
% Input
%
%   FemmProblem - mfemm FemmProblem structure
%
%   filename - optional file name to be used for the output file_in_loadpath
%
% Output
%
%   filename - the name of the file written to disk
%
    
    if nargin < 2
        if strncmpi (FemmProblem.ProbInfo.Domain, 'm', 1)
            filename = [tempname, '.fem'];
        elseif strncmpi (FemmProblem.ProbInfo.Domain, 'h', 1)
            filename = [tempname, '.feh'];
        end
    end
    
    writefemmfile(filename, FemmProblem);
    % open femm if we have not already done so
    openfemm;
    % open the problem file
    opendocument(filename);

end
