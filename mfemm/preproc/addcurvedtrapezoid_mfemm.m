function [FemmProblem, seginds, aseginds, nodeinds, nodeids, centre] = addcurvedtrapezoid_mfemm (FemmProblem, Ri, Ro, angi, ango, varargin)
% adds a curved trapezoidal region 
%
% Syntax
%
% [FemmProblem, seginds, nodeinds, nodeids, centre] = ...
%       addcurvedtrapezoid_mfemm (FemmProblem, Ri, Ro, angi, ango, 'Parameter', Value)
%
% 

% Copyright 2015 Richard Crozier
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

    % add two nodes for the circle
    [x,y] = pol2cart ([angi/2; -angi/2; -ango/2; ango/2], [Ri; Ri; Ro; Ro]);
    
    [FemmProblem, nodeinds, nodeids] = addnodes_mfemm (FemmProblem, x, y);
    
    % add arc segments
    [FemmProblem, aseginds] = addarcsegments_mfemm (FemmProblem, ...
                                    [nodeids(2);      nodeids(3) ], ...
                                    [nodeids(1);      nodeids(4) ], ...
                                    [ rad2deg(angi); rad2deg(ango)], ...
                                    varargin{:});
    
    [FemmProblem, seginds] = addsegments_mfemm (FemmProblem, ...
                                    [nodeids(1);  nodeids(2) ], ...
                                    [nodeids(4);  nodeids(3) ], ...
                                    varargin{:});

	centre = [ (Ri + Ro) / 2, 0];
    
end