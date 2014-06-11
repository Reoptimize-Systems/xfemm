function groupno = getgroupnumber_mfemm (FemmProblem, name)
% get the group number associated with a group name
%
% Syntax
%
% groupno = getgroupnumber_mfemm (FemmProblem, name)
%
% Input
%
%  FemmProblem - an mfemm FemmProblem structure
%
%  name - the name of the group for which the group number is to be
%    obtained
%
% Output
%
%  groupno - the group number associated with the provided group name. If
%    a group with that name is not found, groupno will be empty
%


    groupno = [];
    
    if isfield (FemmProblem, 'Groups')
        try
            groupno = FemmProblem.Groups.(name);
        end
    end

end