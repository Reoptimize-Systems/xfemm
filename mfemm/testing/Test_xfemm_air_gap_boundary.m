% Test_xfemm_air_gap_boundary.m

thisdir = mfemmdeps.getmfilepath ('Test_xfemm_air_gap_boundary');

problemdir = fullfile (thisdir, '..', '..', 'test');

problemfile = fullfile (problemdir, 'TorqueBenchmark.fem');

fp = loadfemmfile (problemfile);

[firstansfilename, femfilename] = analyse_mfemm(fp, 'Quiet', false);

sol = fpproc (firstansfilename);

tq(1) = sol.gapintegral ('AGE', 0);

angles = linspace (0, 90, 50);

for ind = 2:numel (angles)
   
    fp.PrevSolutionFile = firstansfilename;
    fp.PrevType = 0;
    
    fp.BoundaryProps(3).InnerAngle = angles(ind);
    
    [ansfilename, femfilename] = analyse_mfemm(fp, 'Quiet', false, 'KeepMesh', true);
    
    sol = fpproc (ansfilename);
    
%     opendocument (ansfilename); pause (1);
    
    tq(ind) = sol.gapintegral ('AGE', 0);
    
    delete (ansfilename);
    delete (femfilename);
    
end

plot (angles, tq);