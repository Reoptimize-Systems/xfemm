function BoundaryProps = emptyboundaryprops_mfemm()
% creates a boundary property for an mfemm FemmProblem structure
% 
% Syntax
%
% BoundaryProps = emptyboundaryprops_mfemm()
%
% Description
%
% emptyboundaryprops_mfemm() creates a structure with the following fields
% and contents:
%
% BoundaryProps.Name = '';
% BoundaryProps.BdryType = [];
% BoundaryProps.A_0 = 0;
% BoundaryProps.A_1 = 0;
% BoundaryProps.A_2 = 0;
% BoundaryProps.Phi = 90;
% BoundaryProps.c0 = 0;
% BoundaryProps.c0i = 0;
% BoundaryProps.c1 = 0;
% BoundaryProps.c1i = 0;
% BoundaryProps.Mu_ssd = 0;
% BoundaryProps.Sigma_ssd = 0;
%
% These values correspond to boundary conditions in an mfemm finite element
% problem description. emptyboundaryprops_mfemm is not usually called
% directly, but is a utility function for the functions
% newboundaryprop_mfemm and addboundaryprop_mfemm
%
%
% See also: newboundaryprop_mfemm.m, addboundaryprop_mfemm.m
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

    BoundaryProps.Name = '';
    BoundaryProps.BdryType = [];
    BoundaryProps.A_0 = 0;
    BoundaryProps.A_1 = 0;
    BoundaryProps.A_2 = 0;
    BoundaryProps.Phi = 0;
    BoundaryProps.c0 = 0;
    BoundaryProps.c0i = 0;
    BoundaryProps.c1 = 0;
    BoundaryProps.c1i = 0;
    BoundaryProps.Mu_ssd = 0;
    BoundaryProps.Sigma_ssd = 0;

end