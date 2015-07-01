function matlibdat2mat(datfile)
% converts a materials library .dat file to a .mat file of the same name 
% containing the material information saved as an array of structures
%
% Syntax
%
% matlibdat2mat(datfile)
%
% 

% Copyright 2012-2014 Richard Crozier
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

    if nargin == 0
    	matlib = parsematlib_mfemm();
    else
        matlib = parsematlib_mfemm(datfile);
    end
        
    rootpath = mfemmdeps.getmfilepath('matlibdat2mat');
    
    save(fullfile(rootpath, '..', 'matlib.mat'), 'matlib');
    
end
