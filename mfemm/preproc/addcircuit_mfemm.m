function [FemmProblem, circuitind] = addcircuit_mfemm(FemmProblem, Name, varargin)
% adds a new circuit to an mfemm FemmProblem structure
%
% Syntax
%
% [FemmProblem, circuitind] = addcircuit_mfemm(FemmProblem, Name, 'Parameter', 'Value')
%
% Description
%
% addcircuit_mfemm(FemmProblem, Name) adds a circuit structure with the
% name in 'Name' to the mfemm problem in FemmProblem. The following
% optional parameter value pairs can also be provided:
%
%   'CircType' - 1 or 0, if 1 it is a series circuit. If zero a parallel
%     circuit. Defaults to 1, a series circuit, if not specified.
%
%   'TotalAmps_re' - Real component of the current in the circuit in amps,
%     defaults to zero if not supplied.
%
%   'TotalAmps_im' - Imaginary component of the current in the circuit in
%     amps, defaults to zero if not supplied.
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

    Circuit = newcircuit_mfemm(Name, varargin{:});
    
    if ~isfield(FemmProblem, 'Circuits') || isempty(FemmProblem.Circuits)
        
        FemmProblem.Circuits = Circuit;
        
        circuitind = 1;
        
    else
        
        circuitind = numel(FemmProblem.Circuits) + 1;
        
        FemmProblem.Circuits(circuitind) = Circuit;
        
    end
    
end