function filename = openprobleminfemm_mfemm(FemmProblem, filename)

    if nargin < 2
        filename = [tempname, '.fem'];
    end
    
    writefemmfile(filename, FemmProblem);
    % open femm if we have not already done so
    openfemm;
    % open the problem file
    opendocument(filename);

end
