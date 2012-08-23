function [FemmProblem, circuitind] = addcircuit_mfemm(FemmProblem, Name, varargin)
% adds a new circuit to an mfemm FemmProblem structure
%
% Syntax
%
% [FemmProblem, circuitind] = addcircuit_mfemm(FemmProblem, Name, 'Parameter', 'Value')
%

    Circuit = newcircuit_mfemm(Name, varargin{:});
    
    if ~isfield(FemmProblem, 'Circuits') || isempty(FemmProblem.Circuits)
        
        FemmProblem.Circuits = Circuit;
        
        circuitind = 1;
        
    else
        
        circuitind = numel(FemmProblem.Circuits) + 1;
        
        FemmProblem.Circuits(circuitind) = Circuit;
        
    end
    
end