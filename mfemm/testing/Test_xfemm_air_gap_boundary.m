% Test_xfemm_air_gap_boundary.m
%
%
%


%% Test only xfemm

thisdir = mfemmdeps.getmfilepath ('Test_xfemm_air_gap_boundary');

problemdir = fullfile (thisdir, '..', '..', 'test');

problemfile = fullfile (problemdir, 'TorqueBenchmark.fem');

fp = loadfemmfile (problemfile);

angles = linspace (0, 90, 10);

fp.BoundaryProps(3).InnerAngle = angles(1);

[firstansfilename, femfilename] = analyse_mfemm(fp, 'Quiet', true);

sol = fpproc (firstansfilename);

tq = sol.gapintegral ('AGE', 0);
    
for ind = 2:numel (angles)
   
    fp.PrevSolutionFile = firstansfilename;
    fp.PrevType = 0;
    
    fp.BoundaryProps(3).InnerAngle = angles(ind);
    
    [ansfilename, femfilename] = analyse_mfemm(fp, 'Quiet', true, 'KeepMesh', false);
    
    sol = fpproc (ansfilename);
    
    tq(ind) = sol.gapintegral ('AGE', 0);
    
    delete (femfilename);
    delete (ansfilename);
    
end

figure;
plot (angles, tq);

legend ('xfemm solver, xfemm pp');

%% Test and compare all combinations of FEMM and xfemm

thisdir = mfemmdeps.getmfilepath ('Test_xfemm_air_gap_boundary');

problemdir = fullfile (thisdir, '..', '..', 'test');

problemfile = fullfile (problemdir, 'TorqueBenchmark.fem');

fp = loadfemmfile (problemfile);

% turn off smart mesh
femprob.ProbInfo.SmartMesh = false;

[firstansfilename, femfilename] = analyse_mfemm(fp, 'Quiet', false);

sol = fpproc (firstansfilename);

tq = sol.gapintegral ('AGE', 0);

opendocument (femfilename);
opendocument (firstansfilename);

tq_femm_pp = mo_gapintegral ('AGE', 0);

mi_close ();
mo_close ();

newfemfilename = [tempname(), '.fem'];
newansfilename = strrep (newfemfilename, '.fem', '.ans');
copyfile (femfilename, newfemfilename);
opendocument (newfemfilename);

mi_analyse (1);

mi_loadsolution ();

tq_femm_all = mo_gapintegral ('AGE', 0);

sol = fpproc (newansfilename);
    
try
    tq_femm_sol_fpproc_pp = sol.gapintegral ('AGE', 0);
catch
    tq_femm_sol_fpproc_pp = nan;
end

delete (newfemfilename);
delete (newansfilename);
    
angles = linspace (0, 90, 10);

for ind = 2:numel (angles)
   
    fp.PrevSolutionFile = firstansfilename;
    fp.PrevType = 0;
    
    fp.BoundaryProps(3).InnerAngle = angles(ind);
    
    [ansfilename, femfilename] = analyse_mfemm(fp, 'Quiet', false, 'KeepMesh', false);
    
    sol = fpproc (ansfilename);
    
    tq(ind) = sol.gapintegral ('AGE', 0);
    
    opendocument (femfilename);
    opendocument (ansfilename);
    
    tq_femm_pp(ind) = mo_gapintegral ('AGE', 0);
    
    mi_close ();
    mo_close ();
    
    newfemfilename = [tempname(), '.fem'];
    newansfilename = strrep (newfemfilename, '.fem', '.ans');
    copyfile (femfilename, newfemfilename);
    opendocument (newfemfilename);
    
    mi_analyse (1);
    
    mi_loadsolution ();
    
    tq_femm_all(ind) = mo_gapintegral ('AGE', 0);
    
    sol = fpproc (newansfilename);
    
    try
        tq_femm_sol_fpproc_pp(ind) = sol.gapintegral ('AGE', 0);
    catch
        tq_femm_sol_fpproc_pp(ind) = nan;
    end
    
    delete (femfilename);
    delete (ansfilename);
    delete (newfemfilename);
    delete (newansfilename);
    
end

figure;
plot (angles, [tq(:), tq_femm_pp(:), tq_femm_all(:), tq_femm_sol_fpproc_pp(:)]);
title ('xfemm Air Gap Boundayr Test (SmartMesh OFF)');
legend ('xfemm solver, xfemm post-proc', 'xfemm solver, FEMM post-proc', 'FEMM Solver, FEMM post-proc', 'FEMM Solver, xfemm post-proc', 'location', 'best');


%% Repeat with smart mesh on

thisdir = mfemmdeps.getmfilepath ('Test_xfemm_air_gap_boundary');

problemdir = fullfile (thisdir, '..', '..', 'test');

problemfile = fullfile (problemdir, 'TorqueBenchmark.fem');

fp = loadfemmfile (problemfile);

% turn on smart mesh
femprob.ProbInfo.SmartMesh = true;

[firstansfilename, femfilename] = analyse_mfemm(fp, 'Quiet', false);

sol = fpproc (firstansfilename);

tq = sol.gapintegral ('AGE', 0);

opendocument (femfilename);
opendocument (firstansfilename);

tq_femm_pp = mo_gapintegral ('AGE', 0);

mi_close ();
mo_close ();

newfemfilename = [tempname(), '.fem'];
newansfilename = strrep (newfemfilename, '.fem', '.ans');
copyfile (femfilename, newfemfilename);
opendocument (newfemfilename);

mi_analyse (1);

mi_loadsolution ();

tq_femm_all = mo_gapintegral ('AGE', 0);

sol = fpproc (newansfilename);
    
try
    tq_femm_sol_fpproc_pp = sol.gapintegral ('AGE', 0);
catch
    tq_femm_sol_fpproc_pp = nan;
end

delete (newfemfilename);
delete (newansfilename);
    
angles = linspace (0, 90, 10);

for ind = 2:numel (angles)
   
    fp.PrevSolutionFile = firstansfilename;
    fp.PrevType = 0;
    
    fp.BoundaryProps(3).InnerAngle = angles(ind);
    
    [ansfilename, femfilename] = analyse_mfemm(fp, 'Quiet', false, 'KeepMesh', false);
    
    sol = fpproc (ansfilename);
    
    tq(ind) = sol.gapintegral ('AGE', 0);
    
    opendocument (femfilename);
    opendocument (ansfilename);
    
    tq_femm_pp(ind) = mo_gapintegral ('AGE', 0);
    
    mi_close ();
    mo_close ();
    
    newfemfilename = [tempname(), '.fem'];
    newansfilename = strrep (newfemfilename, '.fem', '.ans');
    copyfile (femfilename, newfemfilename);
    opendocument (newfemfilename);
    
    mi_analyse (1);
    
    mi_loadsolution ();
    
    tq_femm_all(ind) = mo_gapintegral ('AGE', 0);
    
    sol = fpproc (newansfilename);
    
    try
        tq_femm_sol_fpproc_pp(ind) = sol.gapintegral ('AGE', 0);
    catch
        tq_femm_sol_fpproc_pp(ind) = nan;
    end
    
    delete (femfilename);
    delete (ansfilename);
    delete (newfemfilename);
    delete (newansfilename);
    
end

figure;
plot (angles, [tq(:), tq_femm_pp(:), tq_femm_all(:), tq_femm_sol_fpproc_pp(:)]);
title ('xfemm Air Gap Boundayr Test (SmartMesh ON)');
legend ('xfemm solver, xfemm post-proc', 'xfemm solver, FEMM post-proc', 'FEMM Solver, FEMM post-proc', 'FEMM Solver, xfemm post-proc', 'location', 'best');

