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

    double mu_x,mu_y;		// permeabilities, relative
    int BHpoints;
    double   *Bdata;
    CComplex *Hdata;
    CComplex *slope;
    double H_c;				// magnetization, A/m
    double Theta_m;			// orientation of magnetization, degrees
    double Jr,Ji;			// applied current density, MA/m^2
    double Cduct;		    // conductivity of the material, MS/m
    double Lam_d;			// lamination thickness, mm
    double Theta_hn;		// hysteresis angle, degrees
    double Theta_hx;		// hysteresis angle, degrees
    double Theta_hy;		// hysteresis angle, degrees
    int LamType;			// lamination type/orientation
    double LamFill;			// lamination fill factor;
    double WireD;			// strand diameter, mm
    int NStrands;			// number of strands per wire

    // Methods
public:

    CMaterialProp();
    ~CMaterialProp();
    CMaterialProp( const CMaterialProp & );
    void GetSlopes();
    void GetSlopes(double omega);
    CComplex GetH(double B);
    CComplex GetdHdB(double B);
    CComplex Get_dvB2(double B);
    CComplex Get_v(double B);
    void GetBHProps(double B, CComplex &v, CComplex &dv);
    void GetBHProps(double B, double &v, double &dv);
    CComplex LaminatedBH(double omega, int i);

private:
};

class CBoundaryProp
{
public:

    int BdryFormat;			// type of boundary condition we are applying
    // 0 = constant value of A
    // 1 = Small skin depth eddy current BC
    // 2 = Mixed BC

    double A0,A1,A2,phi;	// set value of A for BdryFormat=0;

    double Mu,Sig;			// material properties necessary to apply
    // eddy current BC

    CComplex c0,c1;			// coefficients for mixed BC

    // TO DO:  ``flux pipe?'' and ''line currents''
    // Line currents might be redundant, since we already have magnetization.

private:
};

class CPointProp
{
public:

    double Jr,Ji; // applied point current, A
    double Ar,Ai; // prescribed nodal value;

private:
};

class CCircuit
{
public:

    double dVolts_re, dVolts_im;
    double Amps_re, Amps_im;
    int CircType;
    int OrigCirc;

    // some member variables used to store some results
    CComplex J;
    CComplex dV;
    int Case;

private:
};

#endif
