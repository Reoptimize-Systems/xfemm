function Matlib = parsematlib_mfemm(filename)
% converts a femm materials library to an array of sturctures containing
% the relevant information
%
% Syntax
%
% Matlib = parsematlib_mfemm(filename)
%
% Input
%
% filename is a string containing the location of the materials library to
% be imported
%
%

% Copyright 2012 Richard Crozier
% 
%    Licensed under the Apache License, Version 2.0 (the "License");
%    you may not use this file except in compliance with the License.
%    You may obtain a copy of the License at
% 
%        http://www.apache.org/licenses/LICENSE-2.0
% 
%    Unless required by applicable law or agreed to in writing, software
%    distributed under the License is distributed on an "AS IS" BASIS,
%    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%    See the License for the specific language governing permissions and
%    limitations under the License.

    if nargin == 0
        filename = which('matlib.dat');
    end
    
    % open the library file
    fid = fopen(filename, 'r');
    
    % ensure file is closed on exit or error
    C = onCleanup(@()fclose(fid));
    
%     inblock = false;
%     gotfoldername = false;
    getbhpoints = false;
    gettkpoints = false;
    i = 1;
    j = 0;
    
    % preallocate an empty structure array to hold the materials data
    Matlib = emptymaterial_mfemm ();

    while 1

        tline = fgetl(fid);

        if ~ischar(tline),   
            break,   
        end

        if getbhpoints
            
            % we are extracting the b-h points data for a material

            % get the point number
            j = j + 1;

            bhdata = textscan(tline, '%f');

            if ~(numel(bhdata) == 1 && all(size(bhdata{1}) == [2, 1]))
                error('Error parsing B-H data for material %s in material library', Matlib(i).Name);
            end

            Matlib(i).BHPoints(j,1:2) = bhdata{1}';

            % check if we're done getting the points
            if j == nbhpoints
                getbhpoints = false;
            end

        elseif gettkpoints  
            
            % we are extracting the t-k points for a material
            % get the point number
            j = j + 1;

            tkdata = textscan(tline, '%f');

            if ~(numel(tkdata) == 1 && all(size(tkdata{1}) == [2, 1]))
                error('Error parsing B-H data for material %s in material library', Matlib(i).Name);
            end

            Matlib(i).TKPoints(j,1:2) = tkdata{1}';

            % check if we're done getting the points
            if j == ntkpoints
                gettkpoints = false;
            end
            

        else
            
            % search the line for a sequence of alphanumeric characters  preceded
            % by < and followed by >, to get the variable name on this line, we
            % also trim any whitespace from the result

            [start_idx, end_idx, extents, match] = regexp(tline, '(?<=<\s*)[A-Za-z][A-Za-z0-9_]*(?=\s*>)', 'once');
            
            if ~isempty(match)

                % now take action based on the variable
                switch match

                    case 'BeginBlock'

%                         Matlib(i) = emptymatstruct_mfemm();

                    case 'EndBlock'

                        % increment the cell array
                        i = i + 1;

                    case 'BeginFolder'


                    case 'FolderName'


                    case 'EndFolder'


                    case 'BlockName'

                        % get the name of the material
                        name = strtrim(regexp(tline, '(?<=").*(?=")', 'match'));

                        if iscellstr(name) && ~isempty(name)
                            Matlib(i).Name = name{1};
                        else
                            Matlib(i).Name = '';
                        end

                    case 'BHPoints'

                        % get the number of B-H points for the material
                        nbhpoints = str2double(strtrim(regexp(tline, '(?<=[=\s*])[0-9\.]*', 'match')));
                        
                        if nbhpoints > 0
                            getbhpoints = true;

                            j = 0;
                        end

                    case 'TKPoints'
                        
                        ntkpoints = str2double(strtrim(regexp(tline, '(?<=[=\s*])[0-9\.]*', 'match')));
                        
                        if ntkpoints > 0
                            
                            gettkpoints = true;
                            
                            j = 0;
                            
                        end

                    otherwise

                        % get the value of the variable
                        varval =  str2double(strtrim(regexp(tline, '(?<=[=\s*])[0-9\.]*', 'match')));

                        Matlib(i).(match) = varval;

                end
            end

        end

    end


end