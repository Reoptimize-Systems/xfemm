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
% newproblem_mfemm creates a new FemmProblem structure of the type
% specified in 'probtype'. probtype is either a string or scalar value. If
% a string probtype can be either 'planar' or 'axisymmetric', or any
% unambiguous shorter starting substring, e.g. 'axi','pl' or even just 'a'
% or 'p'. If a scalar value, the value zero chooses a planar type
% simulation. Any other value selects an axisymmetric simulation.
%
% Additional problem settings can be selected through the use of
% parameter-value pairs, with defaults for those not explicily chosen.
% Possible p-v pairs are:
%
%   'Frequency'   - The frequency of the simulation, defualts to zero for
%                   a magnetostatic simulation
%
%   'Precision'   - Precision of the iterative solution on nonlinear
%                   problems, defaults to 1e-8.
%
%   'MinAngle'    - Minimum angle triangles in the mesh can have in 
%                   degrees, defaults to 30.
%
%   'Depth'       - For planar problems the depth of the solution, for
%                   axisymmetric problems this is set to zero.
%
%   'LengthUnits' - String specifying units of lenght the problem uses.
%                   Possible values are: 'millimeters', 'centimeters',
%                   'meters', 'mils', 'microns', or 'inches'. The same
%                   values can be set by using the scalar values 1, 2, 3,
%                   4, 5, or any other value to yield 'inches'. Default is
%                   'meters'. Note the american spelling of these
%                   quantities.
%
%   'ACSolver'    - Determines the type of solver used. Can be a scalar
%                   value of 0 or 1. If zero, a Successive Approximation
%                   method is used, if 1 a Newton method is used. Defalts
%                   to the Successive Approximation method.
%
%   'Coords'      - String determining whether cartesian of polar
%                   coordinates are used. 'cart' specifies cartesian
%                   coordinates, 'polar' specifies polar coordinates.
%                   Default is cartesian type coordinates.
%
%   'ForceMaxMesh'- Force the use of a default max mesh size, overriding
%                   the values in block labels. This is chosen
%                   algorithmically 
%
%
%   'SmartMesh'   - Boolean value determining whether smart meshing is to
%                   be used when evaluating the problem. Smart meshing adds
%                   extra mesh points at corners to force a smoother mesh
%                   gradient in these areas. Default is true.
%
% Output
%
% FemmProblem - A structure containing the field 'Probinfo' with the same
%   fields as described in the p-v pairs. In addition the sturcture will
%   contain the field 'Materials' containing a single material structure
%   with the material information for air.
%
%
% See also: writefemmfile.m
%

% Copyright 2012-2017 Richard Crozier
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
    Inputs.ACSolver = 0;
    Inputs.Coords = 'cart';
    Inputs.Domain = 'Magnetics';
    Inputs.PrevSolutionFile = '';
    Inputs.PrevSolutionType = 0;
    Inputs.dT = 0;
    Inputs.SmartMesh = true;

    Inputs = mfemmdeps.parse_pv_pairs(Inputs, varargin);
    
    if ~(islogical (Inputs.SmartMesh) && isscalar (Inputs.SmartMesh))
        error ('SmartMesh must be a scalar logical value (true or false)');
    end
    
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
            error('Unrecognised problem type');
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

    FemmProblem.ProbInfo = Inputs;
    
    AirTK = [ 200.000000,0.018100;
              250.000000,0.022300;
              300.000000,0.026100;
              350.000000,0.029700;
              400.000000,0.033100;
              450.000000,0.036300;
              500.000000,0.039500;
              550.000000,0.042600;
              600.000000,0.045600;
              700.000000,0.051300;
              800.000000,0.056900;
              900.000000,0.062500;
              1000.000000,0.067200;
              1200.000000,0.075900;
              1400.000000,0.083500;
              1600.000000,0.090400;
              1800.000000,0.097000;
              2000.000000,0.103200 ];

    
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
                                   'Density', 1.225, ...
                                   'Kx', 0.0181, ...
                                   'Ky', 0.0181, ...
                                   'Kt', 3, ...
                                   'qv', 0, ...
                                   'TKPoints', AirTK);
                               
    FemmProblem.Segments = [];
    FemmProblem.ArcSegments = [];
    FemmProblem.Nodes = [];
    FemmProblem.BoundaryProps = [];
    FemmProblem.Circuits = [];
    FemmProblem.BlockLabels = [];
    FemmProblem.Holes = [];
    FemmProblem.PointProps = [];
    FemmProblem.Groups = struct ();
    FemmProblem.PrevSolutionFile = Inputs.PrevSolutionFile;
    FemmProblem.PrevSolutionType = Inputs.PrevSolutionType;
    

end