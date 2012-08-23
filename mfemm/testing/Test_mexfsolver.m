% Test_mexfsolver.m
%
% A script to test the mexfsolver function  

% Copyright (C) 2012 Richard C Crozier
%
% Author: Richard C Crozier <s0237326@oban.see.ed.ac.uk>
% Created: 2012-08-20

cd(fileparts(which('fsolversetup.m')));
filename = fullfile(fileparts(which('fsolversetup.m')), 'fsolver', 'test', 'Temp.fem')
mexfsolver(filename);
