Welcome to xfemm
================

Welcome to the xfemm project. xfemm is a software project intended to
create a direct interface to a high quality magnetics finite element code
based on FEMM. The objective of xfemm is to create a cross-platform
command line magnetics finite element solver written in standard C++, a
set of magnetics problem definition and post-processing functions in
native !Matlab/Octave code, and a mex interface to the solvers

--------    NOTE TO USERS  --------

If you use xfemm, particularly for industrial work, but also academic, 
it will be greatly appreciated if you could write an email stating this 
and how it has supported your work. This is a low-cost way to ensure 
further development and maintenance will continue! Contact the authors 
on the discussion forum, or you will find an email address in the source 
files.

If you wish to cite xfemm in your work, please use the following:

Crozier, R, Mueller, M., "A New MATLAB and Octave Interface to a 
Popular Magnetics Finite Element Code", Proceedings of the 22nd 
International Conference on Electric Machines (ICEM 2016), September 
2016.

We would also suggest you cite the original FEMM program.

----------------------------------


Installation and Setup
-----------------------

There are two ways to make use of the xfemm project. One is to use it as
a collections of standalone programs run from the command line. The other
is to use it through the Matlab/Octave programming language.

The standalone programs are fmesher and fsolver. There is also a library
of post-processing functions called fpproc, but no standalone program
interface is provided to this at this time, you will have to create your
own (note that the Matlab/Octave interface does, however, provide full
access to fpproc). 

Compiling Standalone Binary Programs
------------------------------------

Released versions of xfemm come with pre-built binaries. But if you want
to compile xfemm on your platform, you can do so quite easily with cmake
and your compiler of choice. Run cmake on the CMakeLists.txt in the cfemm
directory to create the build system, and then build the project. OnLinus
this would be done as

cd <install dir>/xfemm/cfemm
cmake .
make

the binary files are found in the xfemm/cfemm/bin directory

Compiling Matlab Interface
--------------------------

Detailed instructions for compiling the Matlab inteface can be found in
the README file provided in the mfemm directory. The process is fairly well 
automated.
