/*******************      mfemm     ***********************/

Copyright Richard Crozier 2012-2015

CONTENTS
1. Introduction
2. Licencing
3. History
4. Using the Package


1. Introduction

mfemm is a collection of m-files (with .m extension) for use
with Octave or Matlab, for the generation of finite problem
definitions suitable for the program FEMM, and the cfemm
system (based on FEMM). Also included are some useful C++ code
files which provide a direct interface between Octave/Matlab and
cfemm. Further information on FEMM and cfemm is provided in
Section 3.

The provided m-files can be used for the creation of
magnetic finite element problem definitions, and the
generation of associated .fem files suitible for reading by
the cfemm system. The m-files also have routines for
problem visualisation and reading of solution files produced
by the cfemm system.


2. Licencing

All m-files provided in this package are licenced under the
Apache Version 2.0 licence. A copy of the text of this licence
is supplied in the file LICENCE.txt. This licence also applies
to the C++ files in the top level directory which provide a
mex interface to Octave/Matlab for the cfemm routines fmesher
and fsolver. fmesher and fsolver have their own licencing terms
and are not covered by the Apache Version 2.0 licence. You
should take care to adhere to the licencing of these separate
codes.

The mfiles in this directory may be modified and redistributed
according to the Apache Version 2.0 Licence, but the author(s)
request that you would also send any bug fixes or improvements to
them so that others can benefit as you have.

3. History

FEMM (Finite Element Method Magnetics, www.femm.info) is a high
quality open source program developed by Dr. David Meeker for the
solution of 2D planar and axisymmetric finite element problems
for magnetics, current flow and heat flow. While FEMM had a well
developed interface for Octave/Matlab, there were issues with
the speed and reliability of the connection. This was
particularly the case for Linux systems where FEMM could not
be installed run natively, requiring to be run under WINE, and
the efficient Windows based ActiveX interface was replaced with
a file based method.

To speed the creation of Finite Element problem files from
Octave/Matlab the system of m-files provided with this package
was created. In parallel, a separate project (cfemm) was created
to allow the native compilation and running of the core magnetics
solution and meshing algorithms on non-windows systems. As FEMM
was licenced under the Alladin Free Public Licence, the cfemm
project was licenced under the same terms.


4. Using the Package

The documentation for mfemm is a work-in-progress, but there is a
detailed tutorial in the examples directory in the form of a heavily
commented m-file. This gives a good overview of the system.

Installation instructions can be found in INSTALLATION.txt in the same
directory as this readme file.

If you improve the m-files in any way, please do send your
improvements to the author for inclusion in the package.



