function [FemmProblem, blockind] = addblocklabel_mfemm(FemmProblem, x, y, varargin)
% adds a new block label to an mfemm FemmProblem structure
%
% Syntax
%
% [FemmProblem, blockind] = addblocklabel_mfemm(FemmProblem, x, y, 'Parameter', 'Value')
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

    BlockLabel = newblocklabel_mfemm(x, y, varargin{:});
    
    if ~isfield(FemmProblem, 'BlockLabels') || isempty(FemmProblem.BlockLabels)
        
        FemmProblem.BlockLabels = BlockLabel;
        
        blockind = 1;
        
    else
        
        blockind = numel(FemmProblem.BlockLabels) + 1;
        
        FemmProblem.BlockLabels(blockind) = BlockLabel;
        
    end
    
end