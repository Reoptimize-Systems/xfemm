function mmake(target,mmakefilename,varargin)
%MMAKE A minimal subset of GNU make, implemented in MATLAB for MATLAB.
%   GNU Make "is a tool which controls the generation of executables and 
%   other non-source files of a program from the program's source files.
%   Make gets its knowledge of how to build your program from a file called
%   the makefile, which lists each of the non-source files and how to 
%   compute it from other files." For details see: www.gnu.org/software/make
%
%   Only a minimal subset of GNU Make features are implemented. Notably:
%   - GNU-style Makefile syntax (looks for MMakefile)
%       - Immediate assignments (var := value)
%       - Variable expansion via ${var} or $(var)
%       - The basic make 'rule' syntax (target : dependencies, followed by
%         tabbed MATLAB commands)
%       - Wildcards in targets (*.x : common.h)
%       - Pattern rules (%.x : %.y)
%       - Auto variables in rule commands:
%           - $@ = the target
%           - $< = first dependency
%           - $^ = all dependencies (with duplicates removed)
%           - $+ = all dependencies (in the exact order they are listed)
%           - $* = the pattern matched by '%' in a pattern rule
%           - $& = the directory of the target
%       - MATLAB command expansion via $(eval cmd) or ${eval cmd}. The
%           string 'cmd' is evaluated directly within MATLAB. It must return a
%           value of type char or a cell array of chars. In the event that a
%           multidimensional array or cell array is returned, all elements are
%           concatenated together with a space in between.
%       - As a convenience, the following variables are always pre-set:
%           ${MEX_EXT}      -> ${eval mexext}
%           ${OBJ_EXT}      -> 'obj' on win, 'o' on unix/mac
%           ${PWD}          -> ${eval pwd}
%           ${MEXFLAGS}     -> -O (Explicitly set MEX's default)
%           ${CFLAGS}       -> MEX default
%           ${CXXFLAGS}     -> MEX default
%           ${LDFLAGS}      -> MEX default
%   - MATLAB Function Makefile syntax (Looks for MMakefile.m)
%       - functions in the form [rules, variable] = MMakefile(), where
%           rules are structure with 'target', 'deps', and 'commands' fields
%           variable is dynamic structure, with each variable a field
%   - Implicit rules
%       - %.${MEX_EXT} is automatically built with 'mex' from %.c or %.cpp
%       - %.${OBJ_EXT} is automatically built with 'mex' from %.c or %.cpp
%       - %.dlm is automatically built with rtwbuild('%')
% 
%   - KNOWN BUGS/DEFICIENCIES
%       - Multiple targets are not supported in a rule. (eg, ${OBJS}:common.h)
%       - GNU-style MMakefile parsing could be greatly improved. It does not
%           generally respect quoting or escaping. As such, files, variables 
%           and rules cannot contain ':' or '=' characters. It does, however, 
%           support quoted and escaped filenames. If you need something fancy,
%           (eg, ifs, tricky strings, etc) use the function-style MMakefile.m.
%       - Needs more tests!
%
%   When called without any arguments, MMAKE searches the current working
%   directory for a file named 'MMakefile' and builds the first target
%   listed. With one argument, it builds that target from any rules listed
%   in 'MMakefile' (if it exists in the current working directory) or the
%   implicit rules. The optional second argument may be used to specify a
%   MMakefile in another directory or saved as a different name.
%
%   Copyright (c) 2011, Matt Bauman. mbauman@gmail.com.
%   All rights reserved.

%   Redistribution and use in source and binary forms, with or without
%   modification, are permitted provided that the following conditions are
%   met:
%
%   - Redistributions of source code must retain the above copyright notice,
%     this list of conditions and the following disclaimer.
%   - Redistributions in binary form must reproduce the above copyright
%     notice, this list of conditions and the following disclaimer in the
%     documentation and/or other materials provided with the distribution.
%
%   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
%   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
%   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
%   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
%   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
%   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
%   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
%   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
%   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
%   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
%   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

%% Argument parsing and setup
if nargin < 1, target = ''; end;
if nargin < 2
    % No mmakefile specified, try to find MMakefile or MMakefile.m
    if file_exist(fullfile(pwd,'MMakefile'))
        mmakefilename = fullfile(pwd,'MMakefile');
    elseif file_exist(fullfile(pwd,'MMakefile.m'))
        mmakefilename = fullfile(pwd,'MMakefile.m');
    elseif isempty(target)
        % no target AND no makefile - not psychic!
        error('MJB:mmake:no_target','*** No targets specified and no mmakefile found.  Stop.');
    else
        mmakefilename = '';
    end
end
% if nargin < 3
    % Validate mmakefilename
    if ~is_absolute_path(mmakefilename) && ~isempty(mmakefilename)
        mmakefilename = fullfile(pwd,mmakefilename);
    end
    if ~file_exist(mmakefilename)
        error('MJB:mmake:no_mmakefile','MMakefile (%s) not found', mmakefilename)
    end
% end

% parse optinosl arguements passed as parameter-value pairs
options.DoCrossBuildWin64 = false;
options.CrossMexOptsFile = '';
options.FcnMakeFileArgs = {};

options = mfemmdeps.parse_pv_pairs (options, varargin);

if options.DoCrossBuildWin64
    if isempty (options.CrossMexOptsFile)
        error('MJB:mmake:no_crossmexoptsfile', ...
            'If DoCrossBuildWin64 == true you must supply a path to the corss mex opts shell script using ''CrossMexOptsFile''')
    end
end

% if nargin > 3, error('MJB:mmake:arguments','*** Too many arguments'); end;

%% Make the target
% Move to the correct directory
mmakefile_dir = fileparts(mmakefilename);
if ~strcmp(pwd,mmakefile_dir)
    fprintf('Entering diectory %s\n', mmakefile_dir);
    wd = cd(mmakefile_dir);
else
    wd = '';
end
% try
    % Read the makefile
    [state.implicitRules, state.vars] = implicit_mmakefile(options);
    state = read_mmakefile(state, mmakefilename, options.FcnMakeFileArgs);

    if isempty(target)
        target = state.rules(1).target{1};
    end
    result = make(target, state);
    switch result
        case -1
            error('MJB:mmake:no_rule_found','mmake: No rule found for target %s\n', target);
        case 0
            fprintf('Nothing to be done for target %s\n', target);
        case 1
            fprintf('Target %s successfully built\n', target);
    end
% catch EX
%     if ~isempty(wd), cd(wd); end
%     rethrow(EX);
% end
if ~isempty(wd)
    fprintf('Leaving directory %s\n',pwd);
    cd(wd);
end

end %function

%% Private functions %%

function [rules, vars] = implicit_mmakefile(options)
    vars.MEX_EXT = mexext;
    
    % deterine the appropriate file extension for object files
    vars.OPTIMFLAGS = '';
    if isunix () || isoctave ()
        vars.OBJ_EXT = 'o';
        ismscompiler = false;
    else
        vars.OBJ_EXT = 'obj';
        cc = mex.getCompilerConfigurations ('C');
        if strncmpi (cc.Manufacturer, 'Microsoft', 9)
            ismscompiler = true;
        else
            ismscompiler = false;
        end
    end
    vars.PWD = pwd;
    
    vars.MEXFLAGS = '-O'; % Mirror MATLAB's default, but be explicit about it
    
    if isoctave
        vars.CFLAGSKEY   = '-W';
        vars.CXXFLAGSKEY = '-W';
        vars.FFLAGSKEY   = '';
        vars.LDFLAGSKEY  = '-Wl,';
        
        vars.CFLAGS   = '';
        vars.CXXFLAGS = '';
        vars.FFLAGS   = '';
        vars.LDFLAGS  = '';
    else
        if ismscompiler
            % WHY, MATHWORKS, WHY!?
            vars.CFLAGSKEY   = 'COMPFLAGS';
            vars.CXXFLAGSKEY = 'COMPFLAGS';
            vars.FFLAGSKEY   = 'COMPFLAGS';
            vars.LDFLAGSKEY  = 'LINKFLAGS';
        else
            vars.CFLAGSKEY   = 'CFLAGS';
            vars.CXXFLAGSKEY = 'CXXFLAGS';
            vars.FFLAGSKEY   = 'FFLAGS';
            vars.LDFLAGSKEY  = 'LDFLAGS';
        end
        vars.OPTIMFLAGSKEY  = 'OPTIMFLAGS';
        
        vars.CFLAGS   = ['$' vars.CFLAGSKEY];
        vars.CXXFLAGS = ['$' vars.CXXFLAGSKEY];
        vars.FFLAGS   = ['$' vars.FFLAGSKEY];
        vars.LDFLAGS  = ['$' vars.LDFLAGSKEY];
    
    end
    

    if isunix && ~isoctave
        % Must escape $ signs in unix for the following vars:
        fields = {'CFLAGS' 'CXXFLAGS' 'FFLAGS' 'LDFLAGS', 'OPTIMFLAGS'};
        for i = 1:length(fields)
            if ~isfield(vars,fields{i}), continue; end;
            vars.(fields{i}) = strrep(vars.(fields{i}),'$','\$');
        end
    end
    
    % If no CFLAGS have been set, use the CXXFLAGS as their default
    if ~isfield(vars,'CFLAGS'), vars.CFLAGS = vars.CXXFLAGS; end;
    
    if isoctave
        idx = 1;
        rules(idx).target   = {['%.' mexext]};
        rules(idx).deps     = {'%.c'};
        rules(idx).commands = {'mex ${MEXFLAGS} "${CFLAGSKEY}${CFLAGS}" "${CXXFLAGSKEY}${CXXFLAGS}" "${LDFLAGSKEY}${LDFLAGS}" $< -output $@'};
        idx = idx+1;
        rules(idx).target   = {['%.' mexext]};
        rules(idx).deps     = {'%.cpp'};
        rules(idx).commands = {'mex ${MEXFLAGS} "${CFLAGSKEY}${CFLAGS}" "${CXXFLAGSKEY}${CXXFLAGS}" "${LDFLAGSKEY}${LDFLAGS}" $< -output $@'};
        idx = idx+1;
        rules(idx).target   = {['%.' vars.OBJ_EXT]}; % Note: in a normal function-style MMakefile.m, variable expansion is performed on targets and deps
        rules(idx).deps     = {'%.c'};
        rules(idx).commands = {'mex -c ${MEXFLAGS} "${CFLAGSKEY}${CFLAGS}" "${CXXFLAGSKEY}${CXXFLAGS}" "${LDFLAGSKEY}${LDFLAGS}" $<', ...
                               '[pathstr,name,ext] = fileparts (''$<'');', ...
                               'movefile ([name,''.'',''${OBJ_EXT}''], pathstr)' };
        idx = idx+1;
        rules(idx).target   = {['%.' vars.OBJ_EXT]};
        rules(idx).deps     = {'%.cpp'};
        rules(idx).commands = {'mex -c ${MEXFLAGS} "${CFLAGSKEY}${CFLAGS}" "${CXXFLAGSKEY}${CXXFLAGS}" "${LDFLAGSKEY}${LDFLAGS}" $<', ...
                               '[pathstr,name,ext] = fileparts (''$<'');', ...
                               'movefile ([name,''.'',''${OBJ_EXT}''], pathstr);'};
        idx = idx+1;
        rules(idx).target   = {'%.dlm'};
        rules(idx).deps     = {'%.mdl'};
        rules(idx).commands = {'rtwbuild(''$*'')'};
    else
        
        if options.DoCrossBuildWin64
            idx = 1;
            rules(idx).target   = {['%.' 'mexw64']};
            rules(idx).deps     = {'%.c'};
            rules(idx).commands = {'mex ${MEXFLAGS} -f "', options.CrossMexOptsFile, '" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CFLAGSKEY}="${CFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $< -output $@'};
            idx = idx+1;
            rules(idx).target   = {['%.' 'mexw64']};
            rules(idx).deps     = {'%.cpp'};
            rules(idx).commands = {'mex ${MEXFLAGS} -f "', options.CrossMexOptsFile, '" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $< -output $@'};
            idx = idx+1;
            rules(idx).target   = {['%.' vars.OBJ_EXT]}; % Note: in a normal function-style MMakefile.m, variable expansion is performed on targets and deps
            rules(idx).deps     = {'%.c'};
            rules(idx).commands = {'mex -c ${MEXFLAGS} -f "', options.CrossMexOptsFile, '" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CFLAGSKEY}="${CFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $< -outdir $&'};
            idx = idx+1;
            rules(idx).target   = {['%.' vars.OBJ_EXT]};
            rules(idx).deps     = {'%.cpp'};
            rules(idx).commands = {'mex -c ${MEXFLAGS} -f "', options.CrossMexOptsFile, '" ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $< -outdir $&'};
            idx = idx+1;
            rules(idx).target   = {'%.dlm'};
            rules(idx).deps     = {'%.mdl'};
            rules(idx).commands = {'rtwbuild(''$*'')'};
        else
            
            idx = 1;
            rules(idx).target   = {['%.' mexext]};
            rules(idx).deps     = {'%.c'};
            rules(idx).commands = {'mex ${MEXFLAGS} ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CFLAGSKEY}="${CFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $< -output $@'};
            idx = idx+1;
            rules(idx).target   = {['%.' mexext]};
            rules(idx).deps     = {'%.cpp'};
            rules(idx).commands = {'mex ${MEXFLAGS} ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $< -output $@'};
            idx = idx+1;
            rules(idx).target   = {['%.' vars.OBJ_EXT]}; % Note: in a normal function-style MMakefile.m, variable expansion is performed on targets and deps
            rules(idx).deps     = {'%.c'};
            rules(idx).commands = {'mex -c ${MEXFLAGS} ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CFLAGSKEY}="${CFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $< -outdir $&'};
            idx = idx+1;
            rules(idx).target   = {['%.' vars.OBJ_EXT]};
            rules(idx).deps     = {'%.cpp'};
            rules(idx).commands = {'mex -c ${MEXFLAGS} ${OPTIMFLAGSKEY}="${OPTIMFLAGS}" ${CXXFLAGSKEY}="${CXXFLAGS}" ${LDFLAGSKEY}="${LDFLAGS}" $< -outdir $&'};
            idx = idx+1;
            rules(idx).target   = {'%.dlm'};
            rules(idx).deps     = {'%.mdl'};
            rules(idx).commands = {'rtwbuild(''$*'')'};
        
        end
    end
end

% Recursively make the target, using the dependency information available
% in the state varaible.  Only runs the available commands if a dependant
% is newer than the requested target.  Returns:
%   (-1) if the target does not exist and there is no rule to build it
%   (0)  if the target exists and nothing needed to be done
%   (1)  if the target needed to be rebuilt.
function result = make(target, state)
    % see if we have a rule to make the target
    target_rules = find_matching_rules(target, state.rules);

    cmds = {};
    deps = {};
    
    for i=1:length(target_rules)
        if ~isempty(target_rules(i).commands)
            if ~isempty(cmds)
                warning('MJB:mmake:multiple_commands',['mmake: Overriding commands for target ',target]);
            end
            cmds = target_rules(i).commands;
        end
        % Concatenate the dependencies on the back
        deps = {deps{:}, target_rules(i).deps{:}};
    end
    
    if isempty(cmds)
        % We didn't find any explicit commands to make this target; try
        % the implicit rules
        matching_implicit_rules = find_matching_rules(target, state.implicitRules);
        for i=1:length(matching_implicit_rules)
            deps_exist = false;
            for j = 1:length(matching_implicit_rules(i).deps)
                if ~isempty(matching_implicit_rules(i).deps{j})
                    result = make(matching_implicit_rules(i).deps{j},state);
                    if result == -1
                        % The dependency didn't exist and we don't know how
                        % to make it
                        deps_exist = false;
                        break;
                    else
                        deps_exist = true;
                    end
                end
            end
            if deps_exist
                deps = {deps{:}, matching_implicit_rules(i).deps{:}};
                cmds = matching_implicit_rules(i).commands;
                break;
            end
        end
    end

    if isempty(cmds) && isempty(deps)
        % We don't know how to make it; ensure it exists:
        if file_exist(target)
            result = 0;
        else
            result = -1;
        end
        return;
    end
    
    
    if isempty(deps)
        newest_dependent_timestamp = inf;
    else
        newest_dependent_timestamp = 0;
        for i=1:length(deps)
            % Recursively make all the dependents
            status = make(deps{i}, state);
            if status == -1
                error('MJB:mmake:no_rule_found','mmake: No rule to build %s as required by %s', deps{i}, target);
            end

            % Ensure the dependent exists and check its timestamp
            deptime = ftime(deps{i});
            if isempty(deptime) % TODO: || isphony(file)
                % error('mmake: File %s not found as required by %s', deps{i}, target);
                newest_dependent_timestamp = inf;
            else
                newest_dependent_timestamp = max(newest_dependent_timestamp, deptime);
            end
        end
    end
    
    target_timestamp = -1;
    targettime = ftime(target);
    if ~isempty(targettime)
        target_timestamp = targettime;
    end
    
    
    if target_timestamp < newest_dependent_timestamp
        for i = 1:length(cmds)
            cmd = expand_vars(cmds{i}, state.vars);
            disp(cmd);
            eval(cmd);
        end
        result = 1;
    else
        result = 0;
    end
end

function state = read_mmakefile(state,path,fcnfileargs)
    if regexp(path,'\.m$','once')
        state = read_functional_mmakefile(state,path,fcnfileargs);
    else
        state = read_gnu_mmakefile(state,path);
    end
end

% Parse a MATLAB-function style MMakefile.
function state = read_functional_mmakefile(state,path,fcnfileargs)
    % We have an m-file function
    [~,fcn] = fileparts(path);
    assert(strcmp(path,which(fcn)),'Function that is called (%s) and filename (%s) do not match',which(fcn),path);
    
    if ~isfield(state,'vars'), state.vars = struct; end;
    try
        [state.rules, vars] = feval(fcn, fcnfileargs{:});
    catch EX
        error(['MJB:mmake:' EX.identifier],'Error reading MMakefile (%s):%s',path,EX.message);
    end
    
    % Iterate through the defined variables, add them to state and expand them
    fn = fieldnames(vars)';
    for fcell = fn
        f = char(fcell);
        state.vars.(f) = expand_vars(vars.(f),state.vars);
    end
    
    % Ensure the targets, deps, and commands exist and are in cells.
    for i = 1:length(state.rules)
        for field = {'target' 'deps' 'commands'}
            f = field{1};
            if ~isfield(state.rules(i),f) || isempty(state.rules(i).(f))
                state.rules(i).(f) = {};
            elseif ischar(state.rules(i).(f))
                state.rules(i).(f) = {state.rules(i).(f)}; 
            end;
        end
    end
    
    % Expand variables in the target and deps (postpone expanding the
    % commands as I support embedding the auto variables within a normal
    % variable; we won't know those until the rule is executed)
    for i = 1:length(state.rules)
        for field = {'target' 'deps'}
            f = field{1};
            if isfield(state.rules(i),f) && ~isempty(state.rules(i).(f))
                state.rules(i).(f) = expand_vars(state.rules(i).(f), state.vars);
            end;
        end
    end

end

% Parse a GNU-style MMakefile. Do this in two steps to mirror Make's behavior.
% TODO: This should really be implemented with a state-based parser instead
% of these regexp grabs.
function state = read_gnu_mmakefile(state,path)
    fid = fopen(path);
    
    if fid == -1
        state = [];
        return;
    end
    
    % Parse all the variables
    var_regex = '^\s*([A-Za-z]\w*)\s*:=(.*)$';
    line = fgetl(fid);
    while ischar(line)
        line = strip_comments(line);
        
        % Check for an immediate := assignment
        variable = regexp(line, var_regex, 'tokens', 'once');
        if length(variable) == 2
            state.vars.(variable{1}) = expand_vars(variable{2}, state.vars);
        end
        line = fgetl(fid);
    end
    frewind(fid);
    
    % Parse all rules
    state.rules = [];
    line = fgetl(fid);
    while ischar(line)
        line = strip_comments(line);
        
        % Check for a : that's missing the =
        rule = regexp(line, '^\s*(\S.*):(?!=)(.*)$', 'tokens', 'once');
        if length(rule) >= 1
            loc = length(state.rules)+1;
            state.rules(loc).target   = parse_shell_string(expand_vars(rule{1},state.vars));
            state.rules(loc).deps     = parse_shell_string(expand_vars(rule{2},state.vars));
            state.rules(loc).commands = {};
            
            % And check the next line for a rule
            line = fgetl(fid);
            while ischar(line) && ~isempty(regexp(line, '^(\t|\s\s\s\s)', 'once'))
                state.rules(loc).commands{end+1} = strtrim(strip_comments(line));
                line = fgetl(fid);
            end
        else
            if ~isempty(strtrim(line)) && isempty(regexp(line, var_regex, 'once'))
                % This was a non-trivial line that wasn't parsed. Report it!
                warning('MJB:mmake:ignored_line','Ignored the MMakefile line ''%s''',line);
            end
            line = fgetl(fid);
        end
    end
    
    % cleanup
    fclose(fid);
end

function out = strip_comments(str)
    loc = strfind(str, '#');
    if loc
        out = str(1:loc(1)-1);
    else
        out = str;
    end
end

% Given an arbitrary string, find all locations of variables, and call
% parse_var to expand their result.
function out = expand_vars(value, vars)
    if isempty(value)
        out = value;
        return;
    end

    if iscell(value)
        out = cell(size(value));
        for i=1:length(value)
            out{i} = expand_vars(value{i},vars);
        end
        return;
    end
    
    loc = 1;
    result = {};
    while loc < length(value)
        next_loc = find(value(loc:end)=='$');
        if isempty(next_loc)
            result{end+1} = value(loc:end); %#ok<*AGROW>
            break;
        end;
        
        next_loc = next_loc(1)+loc-1;
        
        if value(next_loc+1) == '(' || value(next_loc+1) == '{'
            result{end+1} = value(loc:next_loc-1);
            [result{end+1}, len] = parse_var(value(next_loc:end),vars);
            loc = next_loc + len;
        else
            result{end+1} = value(loc:next_loc);
            loc = loc + next_loc+1;
        end
    end
    
    out = [result{:}];
end

% Given a variable src in the form ${foo${bar}}, recursively expand all
% variables by ensuring the parentheses and braces pair properly.
function [result, len] = parse_var(src,vars)
    % First, find the endpoint
    p = 0; b = 0; % Parens/Brace nesting levels
    i = 3;
    j = 3;
    result = {};
    while i<=length(src)
        if src(i) == '$' && ( src(i+1) == '(' || src(i+1) == '{' )
            [val,len] = parse_var(src(i:end),vars);
            result{end+1} = [src(j:i-1) val];
            i = i + len;
            j = i; % The start of the next unexpanded text
            if i > length(src); break; end;
        end
        
        if src(i) == '('
            p = p+1;
        elseif src(i) == '{'
            b = b+1;
        elseif src(i) == ')'
            p = p-1;
            if p < 0 && src(2) == '('
                result{end+1} = src(j:i-1);
                j = i;
                break;
            end
        elseif src(i) == '}'
            b = b-1;
            if b < 0 && src(2) == '{'
                result{end+1} = src(j:i-1);
                j = i;
                break;
            end
        end
        
        i = i+1;
    end
    if src(2) == '{' && b >= 0
        error('MJB:mmake:unmatched_paren',['mmake: unmatched ''{'' in MMakefile variable ', src]);
    elseif src(2) == '(' && p >= 0
        error('MJB:mmake:unmatched_paren',['mmake: unmatched ''('' in MMakefile variable ', src]);
    end
    result{end+1} = src(j:i-1);
    result = [result{:}];
    len = i;
    
    if isfield(vars,result)
        result = vars.(result);
    elseif strncmp(result,'eval ',5)
        result = concat(eval(result(6:end)));
    else
        result = '';
    end
end


function out = expand_auto_vars(cmds, ruleset)
    all_deps = concat(ruleset.deps);
    if isempty(ruleset.deps)
        first_dep = ruleset.deps;
    else
        first_dep = ruleset.deps{1};
    end
    
    [target_dir,~,~] = fileparts(ruleset.target);
    if isempty(target_dir)
        target_dir = '.';
    end
    
    unique_deps = concat(str_unique(ruleset.deps));
    cmds = regexprep(cmds, '(\$\@|\$\{\@\}|\$\(\@\))', regexptranslate('escape',ruleset.target));
    cmds = regexprep(cmds, '(\$\*|\$\{\*\}|\$\(\*\))', regexptranslate('escape',ruleset.pattern));
    cmds = regexprep(cmds, '(\$<|\$\{<\}|\$\(<\))',    regexptranslate('escape',first_dep));
    cmds = regexprep(cmds, '(\$\^|\$\{\^\}|\$\(\^\))', regexptranslate('escape',unique_deps));
    cmds = regexprep(cmds, '(\$\+|\$\{\+\}|\$\(\+\))', regexptranslate('escape',all_deps));
    cmds = regexprep(cmds, '(\$\&|\$\{\&\}|\$\(\&\))', regexptranslate('escape',target_dir));
    
    if isoctave
        % in octave regexprep returns the escaped string, matlab returns
        % the unescaped string
        escapedchars = { '\$', '\.', '\?', '\[' };
        replacechars = { '$', '.', '?', '[' };
        for ind = 1:numel (escapedchars)
            cmds = strrep (cmds, escapedchars{ind}, replacechars{ind});
        end
    end
    
    out = cmds;
end

function out = str_unique(cell_arry)
    out = char(cell_arry) + 0;
    out = cellstr(char(unique(out, 'rows')));
end

function out = concat(obj)
    if isempty(obj)
        out = '';
    elseif ischar(obj) && size(obj,1) == 1
        out = obj;
    elseif ischar(obj) && size(obj,1) > 1
        obj(:,size(obj,2)+1) = ' ';
        out = obj(:)';
    elseif iscell(obj) && ~isempty(obj) && ischar(obj{1})
        out = strcat(obj, {' '});
        out = [out{:}];
    else
        % warning(['matmake: unable to convert object of type ', class(obj), ' to string']);
        out = '';
    end
end

function out = find_matching_rules(target, ruleset)
    out = [];
    target = strtrim(target);
    for i=1:length(ruleset)
        regex = cell(size(ruleset(i).target));
        for j = 1:length(regex)
            regex{j} = regexptranslate('wildcard', ruleset(i).target{j});
        end
        regex = strcat('^', regex, '$');
        match_idx = 0;
        pattern = '';
        if strfind(regex{1}, '%')
            % Percent matching only supported on single targets.
            regex = strrep(regex{1}, '%', '(\S+)');
            result = regexp(target, regex, 'tokens', 'once');
            if ~isempty(result)
                match_idx = 1;
                pattern = result{1};
            end
        else
            result = regexp(target, regex, 'once');
            match_idx = find(~cellfun(@isempty, result),1,'first');
        end
        if match_idx > 0
            loc = length(out) + 1;
            out(loc).target = target;
            out(loc).deps = strrep(ruleset(i).deps, '%', pattern);
            out(loc).pattern = pattern;
            out(loc).commands = expand_auto_vars(ruleset(i).commands, out(loc));
        end
    end
end

% Check if the path is absolute
function out = is_absolute_path(path)
    path = strtrim(path);
    if isempty(path)
        out = false;
        return
    end

    if ispc
        % path begins with disk identifier X:\
        key = ['^\w',regexptranslate('escape',[':',filesep])];
        out = regexp(path,key);
        if isempty(out), out = false; end;
    else
       % path begins with / or ~ (home directory)
       out = (path(1) == filesep | path(1) == '~');
    end
end

% A proper file existence check. MATLAB tools are insufficient:
% exist(*,'file') SEARCHES the path,
% dir(file) returns bad information for directories.
function b = file_exist(filename)
    tmp = javachk ('jvm');
    if isempty (tmp)
        a = javaObject ('java.io.File', filename);
        b=(a.exists() && ~a.isDirectory);
    else
        try
            b = existfile (filename);
            if b == true
                [~, msg] = fileattrib (filename);
                if msg.directory == 1
                    b = false;
                end
            end
        catch
            error ('MJB:mmake:file_exist', ...
                   'mmake requires java support or the existfile function.');
        end
    end
end

function t = ftime(filename)
    tmp = javachk ('jvm');
    if isempty (tmp)
        a = javaObject ('java.io.File', filename);
        if a.exists()
            t=a.lastModified();
        else
            t=[];
        end
    else
        if isoctave
            if existfile (filename)
                info = stat (filename);
                t = info.mtime;
            else
                t=[];
            end
        else
            error ('MJB:mmake:file_exist:java', ...
                   'mmake requires java support.');
        end
    end
end

function strs = parse_shell_string(str)
    % PARSESHELLSTRING - parse a list of space-delimited arguments as a shell 
    % might, accounting for quoting and escapes
    % 
    % Note that PARSESHELLSTRING does *not* support file globbing with '*' or
    % variable expansion or other such advanced features.
    % 
    % returns STRS, a cell array of arguments in the order they appear in STR

    IFS = {sprintf(' ') sprintf('\t') sprintf('\n')};

    if nargin ~= 1
        error('MJB:mmake:parse_shell_string:arguments','parse_shell_string requires one argument');
    end

    strs = {};

    tok = zeros(size(str));
    tokidx = 1;

    inDoubleQuote = false;
    inSingleQuote = false;

    i = 1;
    while i <= length(str)
        % Use a state-based parsing algorithm.
        switch str(i)
            case '\'
                if inDoubleQuote && str(i+1) ~= '"'
                    % Print literal \ when inside a "quote" and not escaping \"
                    tok(tokidx) = str(i);
                    tokidx = tokidx+1;
                elseif inSingleQuote && str(i+1) ~= ''''
                    % Print literal \ when inside a 'quote' and not escaping \'
                    tok(tokidx) = str(i);
                    tokidx = tokidx+1;
                else
                    % Ignore the \ and print whatever comes afterwards
                    i = i+1;
                    if i > length(str)
                        error('MJB:mmake:parse_shell_string:incomplete_escape',...
                            'Incomplete escape sequence at end of string');
                    end
                    tok(tokidx) = str(i);
                    tokidx = tokidx+1;
                end
            case '"'
                if inSingleQuote
                    tok(tokidx) = str(i);
                    tokidx = tokidx+1;
                elseif inDoubleQuote
                    inDoubleQuote = false;
                else
                    inDoubleQuote = true;
                end
            case ''''
                if inDoubleQuote
                    tok(tokidx) = str(i);
                    tokidx = tokidx+1;
                elseif inSingleQuote
                    inSingleQuote = false;
                else
                    inSingleQuote = true;
                end
            case IFS
                if inDoubleQuote || inSingleQuote
                    tok(tokidx) = str(i);
                    tokidx = tokidx+1;
                elseif tokidx > 1
                    % Only save a token if it actually has characters
                    strs{end+1} = char(tok(1:tokidx-1));
                    tok = zeros(1,length(str)-i);
                    tokidx = 1;
                end
            otherwise
                tok(tokidx) = str(i);
                tokidx = tokidx+1;
        end
        i = i+1;
    end

    if inDoubleQuote || inSingleQuote
        % We're done parsing, but still in a quote. This is an error.
        if inDoubleQuote, quoteChar = '"'; end
        if inSingleQuote, quoteChar = ''''; end
        error('MJB:mmake:parse_shell_string:incomplete_quote',['Unmatched ' quoteChar ' in input']);
    end

    % Save the final token
    lastTok = char(tok(1:tokidx-1));
    if ~isempty(lastTok)
        strs{end+1} = char(lastTok);
    end
end

function protectedStr = shell_protect(str)
    % shell_protect - protect a string STR with single quotes '' to allow for 
    % passing to a shell

    % Escape any existing single quotes in the string
    protectedStr = strrep(str,'''','\''');

    if iscell(str)
        for i=1:length(str)
            protectedStr{i} = ['''' protectedStr{i} ''''];
        end
    else
        protectedStr = ['''' protectedStr ''''];
    end
end

function t = isoctave()
% ISOCTAVE.M
% ISOCTAVE  True if the operating environment is octave.
%    Usage: t=isoctave();
% 
%    Returns 1 if the operating environment is octave, otherwise
%    0 (Matlab)
% 
% ---------------------------------------------------------------
%
% COPYRIGHT : (c) NUHAG, Dept.Math., University of Vienna, AUSTRIA
%             http://nuhag.eu/
%             Permission is granted to modify and re-distribute this
%             code in any manner as long as this notice is preserved.
%             All standard disclaimers apply.

    if exist('OCTAVE_VERSION')
        % Only Octave has this variable.
        t=1;
    else
        t=0;
    end

end
