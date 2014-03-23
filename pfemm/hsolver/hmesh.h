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

   Date Modified: 2014 - 03 - 21
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
   Contact:
        szelitzkye@gmail.com
        sztibi82@yahoo.com
        richard.crozier@yahoo.co.uk
*/

#include "complex.h"

// replace original windows BOOL type, which is actually
// just an int
//#ifndef BOOL
//#define BOOL int
//#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


// guard mesh class definitions
#ifndef MESH_H
#define MESH_H

/////////////////////////////////////////////////////////////////////////////
// CNode -- structure that holds information about each mesh point.
class CNode
{
public:

    double x,y;
    int xs,ys;
    int BoundaryMarker;
    int InGroup,InConductor;
    int bc;

private:

};

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

class CBlockLabel
{
public:

    double x,y;
    double MaxArea;
    double MagDir;
    int BlockType;
    int InCircuit;
    int InMask;
    int InGroup;
    int Turns;
    int IsExternal;
    int IsDefault;
    char *MagDirFctn;

    // used for proximity effect regions only.
    CComplex ProximityMu;
    int bIsWound;

private:

};

class CCommonPoint
{
public:
    int x,y,t;

private:
};
/////////////////////////////////////////////////////////////////////////////
// Classes that hold property data:  CMaterialProp, CBoundaryProp, CPointProp
class CMaterialProp
{
    // Properties
public:

   		double Kx,Ky;		// thermal conductivity for linear (possibly anisotropic) regions
		double Kt;			// volumetric heat capacity
		double qv;			// volume heat generation

		// properties for nonlinear conductivity
		int npts;			// number of points in the nonlinear conductivity curve
		CComplex Kn[128];   // here, I'm being _very_ lazy by defining a fixed-length buffer for the
		                    // thermal conductivity data points.

    // Methods
public:

    CMaterialProp();
    ~CMaterialProp();
    CMaterialProp( const CMaterialProp & );
    CComplex GetK(double t);

private:
};

class CBoundaryProp
{
public:

    int BdryFormat;

		double Tset;			// Fixed value of temperature for BdryFormat=0;
		double Tinf;			// External temperature for convection or radiation
		double qs;				// Heat flux;
		double beta;			// radiosity coefficient
		double h;				// Heat transfer coefficient


private:
};

class CPointProp
{
public:

    double V,qp;

private:
};

class CCircuit
{
public:

    double V,q;
    int	CircType;

private:
};

#endif
