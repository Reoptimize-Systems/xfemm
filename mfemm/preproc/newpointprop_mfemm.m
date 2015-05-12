function PointProp = newpointprop_mfemm(PPropName, varargin)
% creates a new point property for an mfemm FemmProblem structure

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

    PointProp.I_re = 0;
    PointProp.I_im = 0;
    PointProp.A_re = 0;
    PointProp.A_im = 0;

    PointProp = mfemmdeps.parseoptions(PointProp, varargin);
    
    PointProp.Name = PPropName;
    
end