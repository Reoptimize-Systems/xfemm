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

clear

testfile = 'prototype_sim';
% testfile = 'tutorial';
% testfile = 'cogging_torque';

% the files should be in the same directory as this script
filepath = fileparts(which('test_vis_methods'));

openfemm;

cd(filepath)

% open the solution file from which the values in the file 
% prototype_sim.sol were taken
opendocument(fullfile(filepath, [testfile, '.fem']));

mi_analyse();

mi_loadsolution;

%%

n = 100; % number of subdivisions for calculation purposes


switch testfile
    
    case 'prototype_sim'
        
        % range of data from which to sample in x direction
        minx = 0;
        % maxx = max(prototype_sim(:,1)); % whole sim region
        maxx = 0.19; % restrict to just outside the outer steel region
%         maxx = 0.019; % a bit inside inner magnet radius

        % get the sample grid size in x direction
        xspace = (maxx - minx) / n;

        % range of data from which to sample in y direction
        miny = -0.0629;
        maxy = -miny;
        
        isaxi = true;

    case 'tutorial'
        
        % range of data from which to sample in x direction
        minx = 0;

        maxx = 2.6; 

        % get the sample grid size in x direction
        xspace = (maxx - minx) / n;

        % range of data from which to sample in y direction
        miny = -2;
        maxy = -miny;
        
        isaxi = true;
        
    case 'cogging_torque'
        
        % range of data from which to sample in x direction
        minx = 0.03;

        maxx = 0.084; 

        % get the sample grid size in x direction
        xspace = (maxx - minx) / n;

        % range of data from which to sample in y direction
        miny = -0.033;
        maxy = -miny;
        
        isaxi = false;
        
    otherwise
        
        error('File not an option')
        
end

% get the sample grid size in y direction
yspace = (maxy - miny) / n;

% make actual spacing smalles of the two spacings
gridspace = min(xspace, yspace);

% get suitible sample positions on an n x n grid  
[xi,yi] = meshgrid(minx:gridspace:maxx, miny:gridspace:maxy);

% take samples from the solution file via femm for comparison
FEMMdata = mo_getpointvalues(xi(:), yi(:));

% reshape the A values to the meshgrid produced locations
A = reshape(FEMMdata(:,1), size(xi));

%% calculate Flux Density B

% A is constant in z so curl formula reduced to:
%
% curl A = [-dA/dr] i + [] j + 0 k
% Get numerical gradients of A in x and y directions

A = A ./ (2 * pi * xi) 

[junk, delAdely] = gradient(A ./ (2 * pi * xi), gridspace, gridspace);

[delAdelx, junk] = gradient(A ./ (2 * pi), gridspace, gridspace);

nonzerxiinds = xi ~= 0;

Br = -delAdely;
Bz = delAdelx;
    
%     Br(nonzerxiinds) = Br(nonzerxiinds) ./ (2 * pi * xi(nonzerxiinds));
%     Bz(nonzerxiinds) = Bz(nonzerxiinds) ./ (2 * pi * xi(nonzerxiinds));
Bz(nonzerxiinds) = Bz(nonzerxiinds) ./ (xi(nonzerxiinds));



magB = sqrt( Br.^2 + Bz.^2 );

% Use more sophisticated (but much slower) derivest based function
DEdelAdelx = zeros(size(A));
for i = 1:size(A,1)
    DEdelAdelx(i,:) = derivestdata(xi(i,:), A(i,:), xi(i,:));
end
doDEplot = true;

if isaxi
    BzDE = DEdelAdelx; 
    BzDE(nonzerxiinds) = BzDE(nonzerxiinds) ./ (2 * pi * xi(nonzerxiinds));
else
    BzDE = -DEdelAdelx; 
end

% now compare
% plotskip = 3;
levels = 100;

%% 
h = figure;
% make a scatter plot of the solution data
% scatter3(xi(1:plotskip:end), yi(1:plotskip:end), Br(1:plotskip:end), '+r')
subplot(2,1,1);
contourf(xi, yi, Br)
axis equal
hax1 = gca;
title('MATLAB Calculated Br')
colormap autumn
colorbar
subplot(2,1,2);
colormap autumn
contourf(xi, yi, reshape(FEMMdata(:,2)', size(xi)));
axis equal
hax2 = gca;
title('FEMM Br')
colorbar

mfemmdeps.sameclims([hax1; hax2]);

% Clim1 = get(hax1, 'CLim');
% Clim2 = get(hax2, 'CLim');
% CLim = [max(CLim1(1), CLim2(1)), min(CLim1(2), CLim2(2))];
% 
% set(hax1, 'CLim', CLim)
% set(hax2, 'CLim', CLim)


figure;
% make a scatter plot of the solution data
% scatter3(xi(1:plotskip:end), yi(1:plotskip:end), Br(1:plotskip:end), '+r')
subplot(2,1,1);
contourf(xi, yi, BzDE);
hax1 = gca;
axis equal
colormap autumn
colorbar
title('MATLAB Calculated Bz')
subplot(2,1,2);
contourf(xi, yi, reshape(FEMMdata(:,3)', size(xi)));
hax2 = gca;
axis equal
colormap autumn
colorbar
title('FEMM Bz')

mfemmdeps.sameclims([hax1; hax2]);

%% 
 
% 2D vector plot
% cmap = winter;
plotskip = 3;

BrFEMM = reshape(FEMMdata(:,2), size(xi));
BzFEMM = reshape(FEMMdata(:,3), size(xi));

figure; 
mfemmdeps.cquiver(BrFEMM(1:plotskip:end,1:plotskip:end), BzFEMM(1:plotskip:end,1:plotskip:end), plotskip * gridspace)
title('FEMM B Vector')
colorbar;


figure;
mfemmdeps.cquiver(Br(1:plotskip:end,1:plotskip:end), Bz(1:plotskip:end,1:plotskip:end), plotskip * gridspace)
title('MATLAB Calculated B Vector')
colorbar;



