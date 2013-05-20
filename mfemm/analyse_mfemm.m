function ansfilename = analyse_mfemm(femfilename, usefemm, quiet)
% analyses a .fem file using the mfemm mex interface if present, or the
% original femm interface if not.
%
% The problem is both meshed and solved.
%
% Syntax
%
% ansfilename = analyse_mfemm(femfilename)
%
% Input
%
%   femfilename - string containing the full name of the .fem file to be
%     analysed
%
%   quiet - (optional) if true the output from fmesher and fsolver is not
%     displayed, if false, it is printed to the command line. Defaults to
%     true if not supplied.
%
%   usefemm - (optional) if true, forces the use of the original femm
%     interface to analyse the problem. If not supplied defaults to false.
%
% Output
%
%   ansfilename - string containing the name of the solution file (the .fem
%   file name with .fem replaced with .ans)
%
% See also: fmesher.m, fsolver.m, fpproc.m
% 

% Copyright 2013 Richard Crozier
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
    
    if strcmpi(femfilename(end-3:end), '.fem')
        ansfilename = [femfilename(1:end-4), '.ans'];
    else
        error('Supplied file name must have .fem extension');
    end

    if (exist('mexfmesher', 'file')==3) && (exist('mexfsolver', 'file')==3) ...
            && ~usefemm
        
        if quiet && ~isoctave
            % using xfemm interface
            % mesh the problem using fmesher
            fprintf(1, 'Meshing mfemm problem ...\n');
            evalc('fmesher(femfilename)');
            fprintf(1, 'mfemm problem meshed ...\n');
            % solve the fea problem using fsolver
            fprintf(1, 'Solving mfemm problem ...\n');
            evalc('fsolver(femfilename(1:end-4))');
            fprintf(1, 'mfemm problem solved ...\n');
        else
            % using xfemm interface
            % mesh the problem using fmesher
            fprintf(1, 'Meshing mfemm problem ...\n');
            fmesher(femfilename);
            fprintf(1, 'mfemm problem meshed ...\n');
            % solve the fea problem using fsolver
            fprintf(1, 'Solving mfemm problem ...\n');
            fsolver(femfilename(1:end-4));
            fprintf(1, 'mfemm problem solved ...\n');
        end
        
    else
        % using original femm interface
        opendocument(femfilename);
        mi_analyse(1);
        mi_close();
    end

end
