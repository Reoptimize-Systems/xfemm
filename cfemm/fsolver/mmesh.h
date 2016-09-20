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
#ifndef MMESH_H
#define MMESH_H

#include "mesh.h"
#include "femmcomplex.h"
#include "CBlockLabel.h"

class CMBlockLabel : public femm::CBlockLabel
{
public:
    // used for proximity effect regions only.
    CComplex ProximityMu;
    int bIsWound;// TODO: bIsWound == Turns???

private:

};

/////////////////////////////////////////////////////////////////////////////
// Classes that hold property data:  CMaterialProp, CBoundaryProp, CPointProp
class CMMaterialProp
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

    CMMaterialProp();
    ~CMMaterialProp();
    CMMaterialProp( const CMMaterialProp & );
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

class CMBoundaryProp : public femm::CBoundaryProp
{
public:

    double A0,A1,A2,phi;	// set value of A for BdryFormat=0;

    double Mu,Sig;			// material properties necessary to apply
    // eddy current BC

    CComplex c0,c1;			// coefficients for mixed BC

    // TO DO:  ``flux pipe?'' and ''line currents''
    // Line currents might be redundant, since we already have magnetization.

private:
};

class CMPointProp
{
public:

    double Jr,Ji; // applied point current, A
    double Ar,Ai; // prescribed nodal value;

private:
};

class CMCircuit : public femm::CCircuit
{
public:

    double dVolts_re, dVolts_im;
    double Amps_re, Amps_im;
    int OrigCirc;

    // some member variables used to store some results
    CComplex J;
    CComplex dV;
    int Case;

private:
};

#endif
