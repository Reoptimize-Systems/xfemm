function name = getgroupname_mfemm (FemmProblem, groupno)
% get the anem of the group with a given number

    name = '';
    
    if isfield (FemmProblem, 'Groups')
        for fname = fieldnames (FemmProblem.Groups)'
            if FemmProblem.Groups.(fname{1}) == groupno
                name = fname{1};
            end
        end
    end

end