function conv = metres2length_mfemm(metres, units)
% converts a length specified in metres to the desired input units
% units used in the problem definition
%
% Syntax
%
% conv = length2metres_mfemm(length, units)
%
% Inputs
%
%   metres - the original length, or matrix of lengths to be converted from
%     metres.
%
%   units - integer defining the original length units, as typically stored
%     in the FemmProblem.Probinfo.LengthUnits field. units can have values
%     0 through 5 where the numbers denote the units below:
%
%     0: inches
%     1: millimeters
%     2: centimeters
%     3: meters
%     4: mils
%     5: micrometers
%
% Output
%
%   conv - the supplied lengths converted to the desired units
%
% See also: loadfemmfile.m, length2metres_mfemm.m
%

% Copyright 2012 Richard Crozier
% 
%    Licensed under the Apache License, Version 2.0 (the "License");
%    you may not use this file except in compliance with the License.
%    You may obtain a copy of the License at
% 
%        http:%www.apache.org/licenses/LICENSE-2.0
% 
%    Unless required by applicable law or agreed to in writing, software
%    distributed under the License is distributed on an "AS IS" BASIS,
%    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%    See the License for the specific language governing permissions and
%    limitations under the License.

    if ischar (units)
        
        if strncmpi (units, 'i', 1)
            units = 0;
        elseif strncmpi (units, 'mill', 4)
            units = 1;
        elseif strncmpi (units, 'c', 1)
            units = 2;
        elseif strncmpi (units, 'me', 2)
            units = 3;
        elseif strncmpi (units, 'mils', 4)
            units = 4;
        elseif strncmpi (units, 'mic', 3)
            units = 5;
        else
            units = nan;
        end
        
    end
    
    switch units
        
        case 0
            scaleval = 0.0254;   % inches
        case 1
            scaleval = 0.001;    % millimeters
        case 2
            scaleval = 0.01;     % centimeters
        case 3
            scaleval = 1;        % meters
        case 4
            scaleval = 2.54e-05; % mils
        case 5
            scaleval = 1.e-06;   % micrometers
        otherwise
            error('Unrecognised mfemmdeps.unit.')
    end
    
    conv = metres ./ scaleval;

end