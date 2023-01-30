function failed = test_xfemm_antiperiodic_BC (filename)
% femmcli_antiperiodicBC_flux.lua
% This checks the correct flux density calculation for a motor with antiperiodic BC.
% (The calculation contained errors in xfemm 2.0)
% Output:
% SUCCE

    MAX_DIFF = 0;
    MAX_DIFF_REL = 0;
    
    if nargin < 1

        filename = '/tmp/femmcli_antiperiodicBC_flux.result.fem';
        
        getmfilepath ('mfemm_setup');
        
        copyfile(fullfile (getmfilepath ('mfemm_setup'), '..', 'cfemm', 'femmcli', 'test', 'femmcli_antiperiodicBC_flux.fem'), filename);

    end

    filename = fmesher(filename);
    ansfile = fsolver(filename);
    myfpproc = fpproc();
    myfpproc.opendocument(ansfile);
    
    
    tolerance = 0.02;
    tolerance_rel = 70;
    
    % reference values from FEMM42:
    B_abs_ref = [ 2.0172349211547e-005;
    9.980515629468256e-005;
    0.0001299999163508703;
    0.0001123790790807998;
    0.0001035638039144443;
    4.712390174349349e-005;
    1.52445645287574e-005;
    2.238963159664765e-005;
    5.255076860801661e-005;
    0.0001501846812706276;
    0.0001875769644342095;
    0.3434866174139751;
    0.3134173948888234;
    0.2044994410897019;
    0.007451582859305774;
    0.0008821347188708024;
    6.601243412719332e-005;
    7.025226578942951e-005;
    0.4604507368660987;
    0.7077796643063485;
    0.000977829137071488;
    0.001296664349720595;
    0.3067033815500693;
    0.001112765134440902;
    0.001093521065145376;
    0.1565090701118708;
    0.2693002040535217;
    0.000862706441337431;
    0.9772489557271694;
    0.00123522025689945;
    0.01587650688903416;
    0.6762699821164218;
    0.01412679651697672;
    0.002241449307702012;
    0.348186232581696;
    0.0007352310639869735;
    0.002163325834308883;
    0.1874576095909183;
    1.162091824546714;
    1.0038388444812;
    0.2763546286608281;
    1.001015575730773;
    2.847901526518666;
    0.7007337071437815;
    0.00312702704756613;
    ];
    
    failed=0;
    idx = 1;
    for x = -40:5:-20
	    for y = -20:5:20
    
            pvals = myfpproc.getpointvalues(x,y);
		    %A,Bx,By,Sig,E,H1,H2,Je,Js,Mu1,Mu2,Pe,Ph = mo_getpointvalues(x,y);
    
		    B_abs = abs(pvals(2)) + abs(pvals(3));
		    %write("B_abs_ref["..idx.."] = " .. B_abs)
            [fail, MAX_DIFF, MAX_DIFF_REL] = test_mfemm_check_B_abs_ref_val(sprintf('|B| @ %e, %e', x, y), B_abs, B_abs_ref(idx), tolerance, tolerance_rel, MAX_DIFF, MAX_DIFF_REL);
		    failed = failed + fail;
		    idx = idx + 1;
	    end
    end
    fprintf(1, 'Maximum difference (abs,rel) = %e, %e\n', MAX_DIFF, MAX_DIFF_REL);
    fprintf(1, 'Maximum tolerated difference (abs,rel) = %e, %e\n', tolerance, tolerance_rel);

end