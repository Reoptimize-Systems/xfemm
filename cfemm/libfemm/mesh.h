/*
   This code is a modified version of an algorithm
   forming part of the software program Finite
   Element Method Magnetics (FEMM), authored by
   David Meeker. The original software code is
   subject to the Aladdin Free Public Licence
   version 8, November 18, 1999. For more information
   on FEMM see www.femm.info. This modified version
   is not endorsed in any way by the original
   authors of FEMM.

   This software has been modified to use the C++
   standard template libraries and remove all Microsoft (TM)
   MFC dependent code to allow easier reuse across
   multiple operating system platforms.

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

// guard mesh class definitions
#ifndef MESH_H
#define MESH_H

#include "femmcomplex.h"

namespace femm {

class CMeshline
{
public:

    int n0,n1;

private:
};

class CElement
{
public:

    int p[3];
    int e[3];
    CComplex mu1,mu2;
    int blk;
    int lbl;

private:
};


class CCommonPoint
{
public:
    int x,y,t;

private:
};


class CBoundaryProp
{
public:

    int BdryFormat;			// type of boundary condition we are applying
    // 0 = constant value of A
    // 1 = Small skin depth eddy current BC
    // 2 = Mixed BC

private:
};

class CCircuit
{
public:

    int CircType;

private:
};

} //namespace femm

#endif
