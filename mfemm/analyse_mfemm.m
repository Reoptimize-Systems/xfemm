function ansfilename = analyse_mfemm(femfilename, usefemm)
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
    
    if strcmp(femfilename(end-3:end), '.fem')
        ansfilename = [femfilename(1:end-4), '.ans'];
    else
        error('Supplied file name must have .fem extension');
    end

    if (exist('mexfmesher', 'file')==3) && (exist('mexfsolver', 'file')==3) ...
            && ~usefemm
        % using xfemm interface
        % mesh the problem using fmesher
        fmesher(femfilename);
        % solve the fea problem using fsolver
        fsolver(femfilename(1:end-4));
    else
        % using original femm interface
        opendocument(femfilename);
        mi_analyse(1);
        mi_close();
    end

end