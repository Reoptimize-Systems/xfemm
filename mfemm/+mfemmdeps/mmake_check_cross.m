function vars = mmake_check_cross (winmexlibdir, vars)
% process cross-compiling mmake variables and check the cross libs directory
%
% Syntax
%
% vars = mmake_check_cross (winmexlibdir, vars)
%
% Decription
%
% mmake_check_cross is a function used by the mfemm mex function build
% system when cross-compiling the mex files on a Linux host for a win64
% target. It checks the specified location of the windows mex libraries and
% adds apporpriate entries to the mmake vars structure.
%
% Inputs
%
%  vars - structure containing at least the field 'MEXFLAGS'. This will
%    have a character vector appended adding the windows matlab libraries
%    directory to the compiler search path using the -L switch.
%
% Outputs
%
%  vars - structure containing at least the fields 'MEXFLAGS' and COMPILER.
%    The MEXFLAGS will have a character vector appended adding the windows
%    matlab libraries directory to the compiler search path using the -L
%    switch. The COMPILER field will be a character vector containing the
%    full path to the cross compiler executable.
%
%

    assert ( ~isempty (winmexlibdir), ...
             sprintf ( ['W64CrossBuild is true, but W64CrossBuildMexLibsDir is empty.\n', ...
                        'You must supply the location of the windows mingw64 windows libraries'] ) ...
           );

    assert ( all ( [ exist(fullfile (winmexlibdir, 'libmex.a'), 'file'), ...
                     exist(fullfile (winmexlibdir, 'libmx.a'), 'file'), ...
                     exist(fullfile (winmexlibdir, 'libmat.a'), 'file') ] ), ...
             'One of libmex.a, libmx.a or libmat.a was not found in %s', ...
             winmexlibdir );

    cross_full_path = mmake.cross.cross_prefix_full_path ('x86_64-w64-mingw32.static');

    vars.COMPILER = ['"', cross_full_path, '-gcc"'];

    vars.MEXFLAGS = [vars.MEXFLAGS, ' -L"', winmexlibdir, '"'];
        
        
end