function [FemmProblem, conductorind] = addconductor_mfemm(FemmProblem, Name, varargin)
% adds a new conductor to an mfemm FemmProblem structure
%
% Syntax
%
% [FemmProblem, conductorind] = addconductor_mfemm(FemmProblem, Name, 'Parameter', 'Value')
%
% Description
%
% addcircuit_mfemm(FemmProblem, Name) adds a circuit structure with the
% name in 'Name' to the mfemm problem in FemmProblem. The following
% optional parameter value pairs can also be provided:
%
%   'ConductorType' - 1 or 0, if 0 it is a fixed temperature. If 1 a heat flux.
%     Defaults to 1, a heat flux, if not specified.
%
%   'Tc' - Temperature over the conductor for fixed temperature conductors
%
%   'qc' - Heat flux in W in the conductor for heat flux conductors
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

    Conductor = newconductor_mfemm(Name, varargin{:});
    
    if ~isfield(FemmProblem, 'Circuits') || isempty(FemmProblem.Conductors)
        
        FemmProblem.Conductors = Conductor;
        
        conductorind = 1;
        
    else
        
        conductorind = numel(FemmProblem.Conductors) + 1;
        
        FemmProblem.Conductors(conductorind) = Conductor;
        
    end
    
end