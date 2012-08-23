function [FemmProblem, ind] = addboundaryprop_mfemm(FemmProblem, Name, BdryType, varargin)
% creates and adds a new boundary to an mfemm FemmProblem structure
% 
% Syntax
% 
% FemmProblem = addboundaryprop_mfemm(FemmProblem, Name, BdryType)
% FemmProblem = addboundaryprop_mfemm(..., 'Parameter', 'Value')
%
% Description
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

    % get the number of existing boundaries etc. if any
    elcount = elementcount_mfemm(FemmProblem);

    NBoundaryProps = elcount.NBoundaryProps;
    
    % Create a new boundary with a unique id if possible
    BoundaryProp = newboundaryprop_mfemm(Name, BdryType, NBoundaryProps, varargin{:});
    
    if ~isfield(FemmProblem, 'BoundaryProps') || NBoundaryProps == 0
        
        FemmProblem.BoundaryProps = BoundaryProp;
        
        ind = 1;
        
    else
        
        ind = numel(FemmProblem.BoundaryProps) + 1;
        
        FemmProblem.BoundaryProps(ind) = BoundaryProp;
        
    end
    
end