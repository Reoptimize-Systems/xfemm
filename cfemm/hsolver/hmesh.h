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

// guard hmesh class definitions
#ifndef HMESH_H
#define HMESH_H

#include "mesh.h"
#include "femmcomplex.h"


/////////////////////////////////////////////////////////////////////////////
// Classes that hold property data:  CMaterialProp, CHBoundaryProp, CPointProp
class CHMaterialProp
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

    CHMaterialProp();
    ~CHMaterialProp();
    CHMaterialProp( const CHMaterialProp & );
    CComplex GetK(double t);

private:
};

class CHBoundaryProp : public femm::CBoundaryProp
{
public:

    double Tset;			// Fixed value of temperature for BdryFormat=0;
    double Tinf;			// External temperature for convection or radiation
    double qs;				// Heat flux;
    double beta;			// radiosity coefficient
    double h;				// Heat transfer coefficient

private:
};

class CHPointProp
{
public:

    double V,qp;

private:
};

class CHConductor : public femm::CCircuit
{
public:

    double V,q;

private:
};

#endif
