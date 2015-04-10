function FemmProblem = setcircuitcurrent (FemmProblem, cname, current)
% sets the current in an existing circuit in a FemmProblem structure
%
% Syntax
%
% FemmProblem = setcircuitcurrent (FemmProblem, cname, current)
%
%

    cnames = circuitnames_mfemm (FemmProblem);
    
    loc = find (strcmp (cnames,cname));
    
    if numel (loc) < 1
        error ('MFEMM:setcircuitcurrent:notfound', ...
            'circuit with name %s was not found in FemmProblem.', ...
            cname);
    elseif numel (loc) > 1
        error ('MFEMM:setcircuitcurrent:multiplefound', ...
            'Multiple circuits with name %s were found in FemmProblem.', ...
            cname);
    end
    
    FemmProblem.Circuits(loc).TotalAmps_re = real (current);
    FemmProblem.Circuits(loc).TotalAmps_im = imag (current);

end