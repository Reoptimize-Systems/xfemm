function Conductor = newconductor_mfemm(Name, varargin)
% creates a new conductor for an mfemm FemmProblem structure
% 
% Syntax
%
% Conductor = newconductor_mfemm(Name)
% Conductor = newconductor_mfemm(..., 'Paramter', 'value')
% 
% Description
%
% newconductor_mfemm(Name) creates a conductor structure with the name in
% 'Name'. The following optional parameter value pairs can also be
% provided:
%
%   'ConductorType' - 1 or 0, if 0 it is a fixed temperature. If 1 a heat flux.
%     Defaults to 1, a heat flux, if not specified.
%
%   'Tc' - Real component of the current in the circuit in amps,
%     defaults to zero if not supplied.
%
%   'qc' - Imaginary component of the current in the circuit in
%     amps, defaults to zero if not supplied.
%
% 


% Copyright 2014 Richard Crozier
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

    Conductor.ConductorType = 1; % heat flux
	Conductor.Tc = 0;
    Conductor.qc = 0;
    
    Conductor = mfemmdeps.parse_pv_pairs(Conductor, varargin);
    
    Conductor.Name = Name;

end