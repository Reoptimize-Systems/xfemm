% Test_xfemm_air_gap_boundary.m

thisdir = mfemmdeps.getmfilepath ('Test_xfemm_air_gap_boundary');

problemdir = fullfile (thisdir, '..', '..', 'test');

problemfile = fullfile (problemdir, 'TorqueBenchmark.fem');

fp = loadfemmfile (problemfile);

[ansfilename, femfilename] = analyse_mfemm(fp);

sol = fpproc (ansfilename);

angles = linspace (0, 90, 10);

for ind = 1:numel (angles)
   
    fp.PrevSolutionFile = ansfilename;
    fp.PrevType = 0;
    
    fp.BoundaryProps(3).InnerAngle = angles(ind);
    
    writefemmfile (fullfile (problemdir, 'tmp_TorqueBenchmark.fem'), fp);
    
    [ansfilename, femfilename] = analyse_mfemm(fp);
    
    sol = fpproc (ansfilename);
    
    tq(ind) = sol.gapintegral ('AGE', 0);    
    
end