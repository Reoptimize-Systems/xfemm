function FemmProblem = newproblem_mfemm(probtype, varargin)
% newproblem_mfemm: creates a new mfemm FemmProblem Structure describing a
% finite element problem
%
% Syntax
% 
% FemmProblem = newproblem_mfemm(probtype, 'Parameter', 'Value', ...)
% 
% Description
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

    % set up the default problem info
    Inputs.Frequency = 0;
    Inputs.Precision = 1e-8;
    Inputs.MinAngle = 30;
    Inputs.Depth = 0;
    Inputs.LengthUnits = 'metres';
    Inputs.ProblemType = 'axi';
    Inputs.ACSolver = 0;
    Inputs.Coords = 'cart';

    Inputs = parse_pv_pairs(Inputs, varargin);

    if ischar(probtype)

        if strncmpi(probtype, 'planar', length(probtype))

            Inputs.ProblemType = 'planar';

            if Inputs.Depth == 0
                Inputs.Depth = 1;
            end

        elseif strncmpi(probtype, 'axisymmetric', length(probtype))

            Inputs.ProblemType = 'axisymmetric';

            if Inputs.Depth ~= 0
                Inputs.Depth = 0;
            end

        else
            error('Unrecognised problem type')
        end

    else

        if probtype == 0

            Inputs.ProblemType = 'planar';

            if Inputs.Depth == 0
                Inputs.Depth = 1;
            end

        else

            Inputs.ProblemType = 'axisymmetric';

            if Inputs.Depth ~= 0
                Inputs.Depth = 0;
            end

        end

    end
    
%     FemmProblem = struct('PointProps', [], ...
%                          'BoundaryProps', [], ... 
%                          'Materials', [], ... 
%                          'Circuits', [], ... 
%                          'Nodes', [], ... 
%                          'Segments', [], ... 
%                          'ArcSegments', [], ... 
%                          'BlockLabels', []);

    FemmProblem.ProbInfo = Inputs;
    
    FemmProblem.Materials = struct('Name', 'Air', ...
                                   'Mu_x', 1.000000, ...
                                   'Mu_y', 1.000000, ...
                                   'H_c', 0.000000, ...
                                   'H_cAngle', 0.000000, ...
                                   'J_re', 0.000000, ...
                                   'J_im', 0.000000, ...
                                   'Sigma', 0.000000, ...
                                   'd_lam', 0.000000, ...
                                   'Phi_h', 0.000000, ...
                                   'Phi_hx', 0.000000, ...
                                   'Phi_hy', 0.000000, ...
                                   'LamType', 0.000000, ...
                                   'LamFill', 1.000000, ...
                                   'NStrands', 0.000000, ...
                                   'WireD', 0.000000, ...
                                   'BHPoints', [], ...
                                   'Density', 1.225);

end