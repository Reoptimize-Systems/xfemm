function [FemmProblem, groupno] = addgroup_mfemm(FemmProblem, name, groupno)
% adds a new group number to the list of groups in a FemmProblem structure
%
% Syntax
%
% [FemmProblem, groupno] = addgroup_mfemm(FemmProblem, name)
% [FemmProblem, groupno] = addgroup_mfemm(FemmProblem, name, groupno)
%
% Input
%
%  FemmProblem - an existing mfemm FemmProblem Structure. If the field
%    'Groups' is not present in the structure it will be created
%
%  name - a string containing a name to be associated with the group
%    number. This string must follow the same rules as matlab variable
%    names, i.e. no spaces, leading numbers etc. 
%
%  groupno - optional, number to assign to the group. If groupno is not
%    supplied, a group number is generated that has a high likelyhood of
%    being unique. This will not be a small number that is easy for a human
%    to recognise and use.
%
% Output
%
%  FemmProblem - the modified FemmProblem structure. If the field 'Groups'
%   was not present previously it will have been added. The provided name
%   will be added as a field to FemmProblem.Groups, whose contents is the
%   new group number.
%
%  

    if nargin < 3
        % generate a group number, incorporating the date and time to
        % encourage uniqueness
        groupno = ceil ( 1000000 + rand (1) * (9999999 - 1000000) );
    else
        checkforexisting (FemmProblem, groupno);
    end
    
    if ~isfield (FemmProblem, 'Groups')
        FemmProblem.Groups = struct ();
    end
    
    if ~isvarname (name)
        error ( 'MFEMM::badgroupname', ...
                '%s is not a valid group name, it must follow the same rules as matlab variable names.', ...
                name );
    end
    
    if isfield (FemmProblem.Groups, name)
        error ( 'MFEMM::badgroupname', ...
                'A group with name %s already exists.', ...
                name );
    end
    	
    FemmProblem.Groups.(name) = groupno;

end

function checkforexisting (FemmProblem, groupno)
% tests if group numbers are already present in the FemmProblem

    % check the group number is unique in the groups
    if isfield (FemmProblem, 'Groups')
        for fname = fieldnames (FemmProblem.Groups)
            
            if FemmProblem.Groups.(fname) == groupno
                error ( 'MFEMM::badgroupno', ...
                        'Group number %f already exists in the list of groups, for group with name %s', ...
                        groupno, fname );
            end
            
        end
    end
    
    % TODO: check the segments etc group numbers to test is this is a
    % number in use, and in this case issue a warning
    

end