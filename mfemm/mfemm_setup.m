% mfemm_setup
%
% Script to compile the mfemm C++ programs and matlab mex interfaces.
%

% add the required directories to the path
addpath(genpath(fileparts(which('mfemm_setup.m'))));

if exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'libfemm', 'libfemm.a'), 'file') ...
    && exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'liblua', 'liblua.a'), 'file') ...
    && exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'fpproc', 'libfpproc.a'), 'file')
    
    fmeshersetup;
    fsolversetup;
    fpprocsetup;
    
else
    
    if ~exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'libfemm', 'libfemm.a'), 'file') 
        fprintf(1, 'mfemm_setup can''t find the libfemm library (libfemm.a), have you built it?')
    end
    
    if ~exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'liblua', 'liblua.a'), 'file') 
        fprintf(1, 'mfemm_setup can''t find the liblua library (liblua.a), have you built it?')
    end
    
    if ~exist(fullfile(fileparts(which('mfemm_setup.m')), 'pfemm', 'fpproc', 'libfpproc.a'), 'file')
        fprintf(1, 'mfemm_setup can''t find the libfpproc library (libfpproc.a), have you built the fpproc project as a library?')
    end
    
    error('mfemm setup failed.')
    
end

