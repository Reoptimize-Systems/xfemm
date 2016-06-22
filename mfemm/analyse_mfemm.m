function [ansfilename, femfilename] = analyse_mfemm(femprob, usefemm, quiet, keepmesh)
% analyses a .fem file using the mfemm mex interface if present, or the
% original femm interface if not.
%
% The problem is both meshed and solved.
%
% Syntax
%
% [ansfilename, femfilename] = analyse_mfemm(femprob)
%
% Input
%
%   femprob - either a string containing the full name of the .fem file to
%     be analysed, or an mfemm FemmProblem structure. If a structure the
%     problem will be written to a temporary file before evaluation.
%
%   quiet - (optional) if true the output from fmesher and fsolver is not
%     displayed, if false, it is printed to the command line. Defaults to
%     true if not supplied.
%
%   usefemm - (optional) if true, forces the use of the original femm
%     interface to analyse the problem. If not supplied defaults to false.
%
%   keepmesh - (optional) if true, and not using FEMM, allows the mesh
%    files to be kept after loading by fsolver.
% 
% Output
%
%   ansfilename - string containing the name of the solution file (the .fem
%     file name with .fem replaced with .ans)
%
%   femfilename - string containing the location of the .fem file used. If
%     a .fem file was supplied as input, this is the same file name. If a
%     FemmProblem structure was supplied, it is the location of the
%     generated .fem file for the problem.
%
% See also: fmesher.m, fsolver.m, fpproc.m, hsolver.m, hpproc.m
% 

% Copyright 2013-2014 Richard Crozier
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

    if nargin < 2
        usefemm = false;
    end
    
    if nargin < 3
        quiet = true;
    end
    
    if nargin < 4
        keepmesh = false;
    end
    
    if isstruct(femprob)
    
        % assume it is a FemmProblem structure,
        if strncmpi (femprob.ProbInfo.Domain, 'm', 1) 
            % create the .fem file
            femfilename = [ tempname(), '.fem' ];
        elseif strncmpi (femprob.ProbInfo.Domain, 'h', 1) 
            % create the .feh file
            femfilename = [ tempname(), '.feh' ];
        end
        
        writefemmfile(femfilename, femprob);
        
    elseif ischar(femprob)
        % assume it is the file name of the problem file
        femfilename = femprob;
        
    else
        error('First input should be a string or an mfemm FemmProblem structure.');
    end
    
    % use a structure to create an enum of file types
    ftype.magnetics = 0;
    ftype.heatflow = 1;
    
    if strcmpi(femfilename(end-3:end), '.fem')
        domain = ftype.magnetics;
        ansfilename = [femfilename(1:end-4), '.ans'];
    elseif strcmpi(femfilename(end-3:end), '.feh')
        domain = ftype.heatflow;
        ansfilename = [femfilename(1:end-4), '.anh'];
    else
        error('Supplied file name must have .fem extension');
    end
    
    switch domain
    
        case ftype.magnetics

            if (exist('mexfmesher', 'file')==3) && (exist('mexfsolver', 'file')==3) ...
                    && ~usefemm
                % using xfemm interface
                if quiet
                    % mesh the problem using fmesher
                    fmesher(femfilename);
                    % solve the fea problem using fsolver
                    fsolver(femfilename(1:end-4), false, ~keepmesh);
                else
                    % mesh the problem using fmesher
                    fprintf(1, 'Meshing mfemm problem ...\n');
                    fmesher(femprob);
                    fprintf(1, 'mfemm problem meshed ...\n');
                    % solve the fea problem using fsolver
                    fprintf(1, 'Solving mfemm problem ...\n');
                    fsolver(femprob(1:end-4), true, ~keepmesh);
                    fprintf(1, 'mfemm problem solved ...\n');
                end
                
            else
                % using original femm interface
                opendocument(femfilename);
                % writes the analysis file to disk when done
                mi_analyse(1);
                mi_close();
            end
            
        case ftype.heatflow
        
            if (exist('mexfmesher', 'file')==3) && (exist('mexhsolver', 'file')==3) ...
                    && ~usefemm
                % using xfemm interface
                if quiet
                    % mesh the problem using fmesher
                    fmesher(femfilename);
                    % solve the fea problem using fsolver
                    hsolver(femfilename(1:end-4), false, ~keepmesh);
                else
                    % mesh the problem using fmesher
                    fprintf(1, 'Meshing mfemm problem ...\n');
                    fmesher(femprob);
                    fprintf(1, 'mfemm problem meshed ...\n');
                    % solve the fea problem using fsolver
                    fprintf(1, 'Solving mfemm problem ...\n');
                    hsolver(femprob(1:end-4), true, ~keepmesh);
                    fprintf(1, 'mfemm problem solved ...\n');
                end
                
            else
                % using original femm interface
                opendocument(femfilename);
                % writes the analysis file to disk when done
                hi_analyse(1);
                hi_close();
            end
            
    end

end
