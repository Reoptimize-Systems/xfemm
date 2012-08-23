function [FemmProblem, circuitind] = addcircuit_mfemm(FemmProblem, Name, varargin)
% adds a new circuit to an mfemm FemmProblem structure
%
% Syntax
%
% [FemmProblem, circuitind] = addcircuit_mfemm(FemmProblem, Name, 'Parameter', 'Value')
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