function [result, cinds] = hascircuit_mfemm (FemmProblem, names)
% returns true is supplied name is a circuit name in a FemmProblem
% structure
%
% Syntax
%
% result = hascircuit_mfemm (FemmProblem, names)
%
% 

    if ischar (names)
        names = {names};
    end
    
    if ~iscellstr (names)
        error ('MFEMM:hascircuit:badinput', ...
               'Input must be a string or cell array of strings.');
    end
    
    cnames = circuitnames_mfemm (FemmProblem);
    
    result = false (size (names));
    cinds = zeros (size (result));
    
    for ind = 1:numel (names)
        loc = find (strcmp (cnames,names{ind}));
        result(ind) = ~isempty (loc);
        if result(ind)
           cinds(ind) = loc(1);
        end
    end
    
end