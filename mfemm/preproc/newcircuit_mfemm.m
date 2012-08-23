function Circuit = newcircuit_mfemm(Name, varargin)
% creates a new circuit for an mfemm FemmProblem structure

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

    Circuit.CircType = 1; % series
	Circuit.TotalAmps_re = 0;
    Circuit.TotalAmps_im = 0;
    
    Circuit = parse_pv_pairs(Circuit, varargin);
    
    Circuit.Name = Name;

end