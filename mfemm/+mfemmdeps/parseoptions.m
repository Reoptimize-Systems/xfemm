function defaults = parseoptions(defaults, args)
% parses options supplied either as a set of parameter-value pairs or as an
% structure
%
% Syntax
%
% s = parseoptions(defaults, args)
%
% Input
%
%  defaults - structure, with one field for every potential
%   option to be parsed. Each field will contain the default value for that
%   property. If no default is supplied for a given property, then that
%   field must be empty.
%
%  args - this is either a cell array or structure in the same format as
%   defaults.
%
%   If a cell array, this must be a set of property/value pairs. Case is
%   ignored when comparing properties to the list of field names in
%   default. Also, any unambiguous shortening of a field/property name is
%   allowed.
%
%   If a structure, this is converted to a cell array of p-v pairs
%   identical to that described above, where the structure field names are
%   the parameter names, and their contents the values. This is then
%   treated identically to the cell array p-v pairs input.
%
%   args can als be a scalar cell array, which contains only a sturcture as
%   described above, to facilitate accepting the contents of varargin as
%   input.
%
% Output
%
%  s - parameter struct that reflects any updated property/value pairs in
%   the supplied input.
% 
% See also STRUCT2PVPAIRS, PARSE_PV_PAIRS


    if isstruct(args) && numel(args) == 1
        
        % input is a scalar structure array
        defaults = mfemmdeps.parse_pv_pairs(defaults, mfemmdeps.struct2pvpairs(args));
        
    elseif iscell(args)
        
        % check args is not a single cell containing an options structure
        if numel(args) == 1 && isstruct(args{1})
            
            % call this function again with the structure as input, normal
            % error hadling can be done for the structure case then
            defaults = mfemmdeps.parseoptions(defaults, args{1});
            
        else
            
            if numel(args) == 1 && iscell(args{1})
                % input must be a cell containing another cell with a set of
                % p-v pairs, parse_pv_pairs will do appropriate checking for
                % this
                defaults = mfemmdeps.parse_pv_pairs(defaults, args{:});
            else
                % input must be a set of p-v pairs, parse_pv_pairs will do
                % appropriate checking for this
                defaults = mfemmdeps.parse_pv_pairs(defaults, args); 
            end
            
        end
        
    else
        error(['parseoptions expects either a scalar structure, a ', ...
               'single cell containing a scalar structure, or a cell ', ...
               'array of p-v pairs']);
    end

end