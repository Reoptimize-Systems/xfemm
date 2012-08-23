% mexfmesher(filename)
%
% meshes a problem domains defined in fem files using the fmesher program
%
% Syntax
%
% status = mexfmesher(filename)
%
% This is a mexfunction routine which calls fmesher, a C++ program based on
% the meshing routines originally used in the program FEMM. The program
% meshes a problem defined in a plain text .fem file, the location of which
% is specified in filename.
%
% fmesher creates a triangular mesh based on triangle (
% http://www.cs.cmu.edu/~quake/triangle.html ) with additional support for
% creating periodic and anti-periodic boundary types at the edges of a
% domain.
%
% Several output files are produced by mexfmesher, which are the same as
% those produced by the program triangle.c
% 
% 
% See Also, WRITEFEMMFILE


