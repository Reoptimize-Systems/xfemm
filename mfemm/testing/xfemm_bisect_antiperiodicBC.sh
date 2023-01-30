#!/usr/bin/env octave

strrepfile('/home/rcrozier/src/xfemm/mfemm/MMakefile_fsolver.m', '5.1.0', '8.0.91')

mfemm_setup ('ForceMexRecompile', true);

filename = '/home/rcrozier/Documents/MATLAB/femmcli_antiperiodicBC_flux.fem';

failed = test_xfemm_antiperiodic_BC (filename);

if failed>0
    quit (1);
else
    quit (0);
end