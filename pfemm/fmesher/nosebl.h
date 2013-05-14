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

#include <vector>
#include <string>
//#include "stdstring.h"
#include "complex.h"

using namespace std;

namespace femmedata
{

/////////////////////////////////////////////////////////////////////////////
// CNode -- structure that holds information about each control point.

class CNode
{
	public:
		CNode();

		double x,y;
		int xs,ys;
		int IsSelected;
		string BoundaryMarker;
		int InGroup;

		double GetDistance(double xo, double yo);
		CComplex CC();
		void ToggleSelect();

	private:

};

/////////////////////////////////////////////////////////////////////////////
// CSegment -- structure that holds information about lines joining control pts

class CSegment
{
	public:
		CSegment();

		int n0,n1;
		int IsSelected;
		bool Hidden;
		double MaxSideLength;
		string BoundaryMarker;
		int InGroup;

		void ToggleSelect();

	private:

};

class CArcSegment
{
	public:
		CArcSegment();

		int n0,n1;
		bool NormalDirection;
		int IsSelected;
		bool Hidden;
		double MaxSideLength,ArcLength;
		string BoundaryMarker;
		int InGroup;

		void ToggleSelect();

	private:

};
/////////////////////////////////////////////////////////////////////////////
// CBlockLabel -- structure that holds block label information

class CBlockLabel
{
	public:
		CBlockLabel();

		double x,y;
		double MaxArea;
		double MagDir;
		int    Turns;
		int IsSelected;
		string BlockType;
		string InCircuit;
		string MagDirFctn;
		int InGroup;
		bool IsExternal;

		void ToggleSelect();
		double GetDistance(double xo, double yo);

	private:

};

class CMaterialProp
{
	public:

		CMaterialProp();
		~CMaterialProp();
		void StripBHData(string &b, string &h);
		void BHDataToCString(string &b, string &h);

		string BlockName;
		double mu_x,mu_y;		// permeabilities, relative

		int    BHpoints;		// number of B-H datapoints;
		std::vector<CComplex> BHdata; // array of B-H pairs;

		int    LamType;			// flag that tells how block is laminated;
								//	0 = not laminated or laminated in plane;
								//  1 = laminated in the x-direction;
								//  2 = laminated in the y-direction;
		double LamFill;			// lamination fill factor, dimensionless;
		double Theta_m;			// direction of the magnetization, degrees
		double H_c;				// magnetization, A/m
		CComplex Jsrc;			// applied current density, MA/m^2
		double Cduct;		    // conductivity of the material, MS/m
		double Lam_d;			// lamination thickness, mm
		double Theta_hn;		// max hysteresis angle, degrees, for nonlinear problems
		double Theta_hx;		// hysteresis angle, degrees, x-direction
		double Theta_hy;		// and y-direction, for anisotropic linear problems.
		int    NStrands;		// number of strands per wire
		double WireD;			// strand diameter, mm

	private:
};

class CBoundaryProp
{
	public:

		CBoundaryProp();

		string BdryName;
		int BdryFormat;			// type of boundary condition we are applying
								// 0 = constant value of A
								// 1 = Small skin depth eddy current BC
								// 2 = Mixed BC
								// 3 = SDI
								// 4 = Periodic
								// 5 = Antiperiodic

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

		CPointProp();

		string PointName;
		CComplex Jp;			// applied point current, Amps
		CComplex Ap;			// prescribed nodal value of vector potential;

	private:
};

class CCircuit
{
	public:

		CCircuit();

		string CircName;
		CComplex Amps;
		int		CircType;

	private:

};

class CPeriodicBoundary
{
	public:

		CPeriodicBoundary();

		string BdryName;
		int BdryFormat;			// 0 = Periodic
								// 1 = Antiperiodic
		int nseg;				// number of segs with this bc
		int narc;				// number of arcs with this bc
		int seg[2];				// (arc)segments to which is applied

	private:
};

class CCommonPoint
{
	public:

		CCommonPoint();
		void Order();

		int x,y,t;

	private:
};

}

using namespace femmedata;
