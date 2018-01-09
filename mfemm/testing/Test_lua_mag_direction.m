
xfemm_test_dir = fullfile ( fileparts ( which ('mfemm_setup')), '..', 'test');

lua_mag_dir_test_fem_file = fullfile (xfemm_test_dir, 'test_lua_mag_direction.fem');

theta = linspace (0, 45, 100);
rho = repmat (7.25, size (theta));
[x,y] = pol2cart (deg2rad (theta - 90), rho);

quiet = false;

%% using femm

usefemm = true;
[ansfilename, femfilename] = analyse_mfemm (lua_mag_dir_test_fem_file, usefemm, quiet);

pause (30);

opendocument (ansfilename);

Bfemm = [];
for ind = 1:numel (theta)
    pause (0.1);
    Bfemm(ind,:) = mo_getb (x(ind), y(ind));
    
end
pause (0.1);
mo_close ();


%% using mfemm

usefemm = false;
[ansfilename, femfilename] = analyse_mfemm (lua_mag_dir_test_fem_file, usefemm, quiet);

solution = fpproc (ansfilename);

Bmfemm = [];
for ind = 1:numel (theta)
    
    Bmfemm(ind,:) = solution.getb (x(ind), y(ind));
    
end

%% plot

plot (theta, Bfemm, theta, Bmfemm);

legend ('FEMM Bx using theta', 'FEMM Bx using theta', 'xfemm Bx using theta', 'xfemm By using theta');
xlabel ('theta')
ylabel ('theta')