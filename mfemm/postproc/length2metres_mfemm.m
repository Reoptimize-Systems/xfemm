function conv = length2metres_mfemm(length, units)
% converts a length specified in a FemmProblem to metres from the input
% units used in the problem definition
%
% Syntax
%
% conv = length2metres_mfemm(length, units)
%
% Inputs
%
%   length - the original length, or matrix of lengths to be converted to
%     metres.
%
%   units - either an integer or string defining the original length units,
%     as typically stored in the FemmProblem.Probinfo.LengthUnits field. If
%     an integer, can have values 0 through 5 where the numbers denote the
%     units below:
%
%     0: inches
%     1: millimeters
%     2: centimeters
%     3: meters
%     4: mils
%     5: micrometers
%
%     If a string, one of the following: inches, millimeters, centimeters,
%     meters, mils, micrometers
%
% Output
%
%   conv - the supplied lengths converted to units of metres
%
% See also: loadfemmfile.m, metres2length_mfemm.m
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
        
        if strcmp (units, 'inches')
            units = 0;
        elseif strcmp (units, 'millimeters')
            units = 1;
        elseif strcmp (units, 'centimeters')
            units = 2;
        elseif strcmp (units, 'metres')
            units = 3;
        elseif strcmp (units, 'mils')
            units = 4;
        elseif strcmp (units, 'micrometers')
            units = 5;
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
    
    conv = length .* scaleval;

end