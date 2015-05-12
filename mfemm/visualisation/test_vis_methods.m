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

% the files should be in the same directory as this script
filepath = fileparts(which('test_vis_methods'));

openfemm;

% open the solution file from which the values in the file 
% prototype_sim.sol were taken
opendocument(fullfile(filepath, 'prototype_sim.ans'));

% get the 
% Afemm = mo_getpointvalues();

% now load the same solution values into a matlab matrix
prototype_sim = load( fullfile(filepath, 'prototype_sim.sol') );

n = 100; % number of subdivisions for calculation purposes

% range of data from which to sample in x direction
minx = min(prototype_sim(:,1));
% maxx = max(prototype_sim(:,1)); % whole sim region
% maxx = 0.19; % restrict to just outside the outer steel region
maxx = 0.019; % a bit inside inner magnet radius

% get the sample grid size in x direction
xspace = (maxx - minx) / n;

% range of data from which to sample in y direction
miny = min(prototype_sim(:,2));
maxy = max(prototype_sim(:,2));

% get the sample grid size in y direction
yspace = (maxy - miny) / n;

% make actual spacing smalles of the two spacings
gridspace = min(xspace, yspace);

% [xi,yi] = meshgrid(linspace(minx,maxx,n), linspace(miny,maxy,n));

% get suitible sample positions on an n x n grid  
[xi,yi] = meshgrid(minx:gridspace:maxx, miny:gridspace:maxy);

% create an interpolated data set on the grid from the solution values
A = griddata(prototype_sim(:,1),prototype_sim(:,2),prototype_sim(:,3), xi, yi);

% take samples from the solution file via femm for comparison
FEMMdata = mo_getpointvalues(xi(:), yi(:));

plotskip = 3;

% % make a scatter plot of the solution data
% scatter3(xi(1:plotskip:end), yi(1:plotskip:end), A(1:plotskip:end), '+r')
% 
% hold on
% % in axisymmetric sim FEMM returns 2 pi R A, so divide by this at each
% % point for comparison
% % scatter3(xi(1:plotskip:end), yi(1:plotskip:end), FEMMdata(1:plotskip:end,1)' ./ (2.*pi.*xi(1:plotskip:end)), '+b')
% scatter3(xi(1:plotskip:end), yi(1:plotskip:end), FEMMdata(1:plotskip:end,1)', '+b')
% hold off

% fprintf('Max Difference: %f \n', max(abs(FEMMdata(:,1) - A(:))))

%% calculate Flux Density B

% A is constant in z so curl formula reduced to:
%
% curl A = [-dA/dr] i + [] j + 0 k
% Get numerical gradients of A in x and y directions
[delAdelx, delAdely] = gradient(A, gridspace, gridspace);

% Use more sophisticated (but much slower) derivest based function
% DEdelAdelx = zeros(size(A));
% for i = 1:size(A,1)
%     DEdelAdelx(i,:) = derivestdata(xi(i,:), A(i,:), xi(i,:));
% end
% BzDE = DEdelAdelx; doDEplot = true;

% 
Br = -delAdely;
Bz = delAdelx;
magB = sqrt( Br.^2 + Bz.^2 );

% now compare

figure;
% make a scatter plot of the solution data
scatter3(xi(1:plotskip:end), yi(1:plotskip:end), Br(1:plotskip:end), '+r')

hold on
% in axisymmetric sim FEMM returns 2 pi R A, so divide by this at each
% point for comparison
% scatter3(xi(1:plotskip:end), yi(1:plotskip:end), FEMMdata(1:plotskip:end,1)' ./ (2.*pi.*xi(1:plotskip:end)), '+b')
scatter3(xi(1:plotskip:end), yi(1:plotskip:end), FEMMdata(1:plotskip:end,2)', '+b')
hold off
legend('Matlab', 'FEMM');

figure;
% make a scatter plot of the solution data
scatter3(xi(1:plotskip:end), yi(1:plotskip:end), Bz(1:plotskip:end), '+r')

hold on
% in axisymmetric sim FEMM returns 2 pi R A, so divide by this at each
% point for comparison
% scatter3(xi(1:plotskip:end), yi(1:plotskip:end), FEMMdata(1:plotskip:end,1)' ./ (2.*pi.*xi(1:plotskip:end)), '+b')
scatter3(xi(1:plotskip:end), yi(1:plotskip:end), FEMMdata(1:plotskip:end,3)', '+b')

if exist(doDEplot, 'var') && doDEplot
    scatter3(xi(1:plotskip:end), yi(1:plotskip:end), BzDE(1:plotskip:end), '+m')
end

hold off
legend('Matlab', 'FEMM');

%% 
 
% % Field Density Plot
% cmap = flipud(hot);
% 
% contourf(xi, yi, magB)
% colorbar;
% axis equal
% axpos = get(gca, 'Position');
% 
% % 2D vector plot
% hold on
% cmap = winter;
% skip = 2;
% mfemmdeps.cquiver(-delAdely(1:skip:end,1:skip:end), delAdelx(1:skip:end,1:skip:end), ...
%     gridspace, 'cmap', cmap, ...
%     'AxesProps', {'Color', 'none', 'Position', axpos});
% % colorbar;
% hold off

%%
% 
% r = 0.004
% z = 0.012
% 
% % get the values from femm 
% FEMMpointvals = mo_getpointvalues(r, z);
% 
% % get the calculated values
% BmagInterp = interp2(xi,yi,magB,r,z)
% BrInterp = interp2(xi,yi,Br,r,z)
% BzInterp = interp2(xi,yi,Bz,r,z)
% AInterp = interp2(xi,yi,A,r,z)
