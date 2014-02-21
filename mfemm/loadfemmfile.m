function FemmProblem = loadfemmfile(filename)
% loads a femm problem from a file and stores it in a FemmProblem structure
%
% Syntax
%
% [FemmProblem] = loadfemmfile(filename)
%
% Input
%
%   filename - string containing the location of the femm solution file to
%     be loaded
%
%   problemonly - flag to determine whether the solution is actually
%     loaded, or only the FemmProblem description. Defaults to false,
%     meaning everything is loaded. If true, Solution will be empty.
%
% Output
%
%   FemmProblem - a structure describing the finite element problem, see
%     writefemmfile.m for details.
%

% Copyright 2012 Richard Crozier
% 
%    Licensed under the Apache License, Version 2.0 (the "License");
%    you may not use this file except in compliance with the License.
%    You may obtain a copy of the License at
% 
%        http:%www.apache.org/licenses/LICENSE-2.0
% 
%    Unless required by applicable law or agreed to in writing, software
%    distributed under the License is distributed on an "AS IS" BASIS,
%    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%    See the License for the specific language governing permissions and
%    limitations under the License.

    % this function is just a thin wrapper for loadfemmsolution which also
    % works on .fem input files
    FemmProblem = loadfemmsolution(filename, true);
    
end