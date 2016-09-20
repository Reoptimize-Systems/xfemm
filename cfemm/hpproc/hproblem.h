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
        sztibi82@gmail.com
        richard.crozier@yahoo.co.uk
*/

#include <string>
#include "femmcomplex.h"


#ifndef PROBLEM_H
#define PROBLEM_H
namespace HPProcdata{
/////////////////////////////////////////////////////////////////////////////
// CNode -- structure that holds information about each control point.

using std::string;

class CNode
{
	public:
		CNode();

		double x,y;
		int xs,ys;
		bool IsSelected;
		int BoundaryMarker;
		int InGroup,InConductor;

		double GetDistance(double xo, double yo);
		CComplex CC();
		void ToggleSelect();

	private:

};

class CMeshNode
{
	public:
		CMeshNode();

		double x,y,T;
		double msk;
		int xs,ys;
		bool Q,IsSelected;

		double GetDistance(double xo, double yo);
		CComplex CC();

	private:

};
/////////////////////////////////////////////////////////////////////////////
// CSegment -- structure that holds information about lines joining control pts

class CSegment
{
	public:
		CSegment();

		int n0,n1;
		double MaxSideLength;
		bool IsSelected;
		bool Hidden;
		int BoundaryMarker;
		int InGroup,InConductor;

		void ToggleSelect();

	private:

};

class CArcSegment
{
	public:
		CArcSegment();

		int n0,n1;
		bool IsSelected;
		bool Hidden;
		double MaxSideLength,ArcLength;
		int BoundaryMarker;
		int InGroup, InConductor;

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
		bool IsSelected;
		int BlockType;
		int InGroup;
		bool IsExternal;
		bool IsDefault;

		void ToggleSelect();
		double GetDistance(double xo, double yo);

	private:

};

class CMaterialProp
{
	public:

		string BlockName;
		double Kx,Ky;		// thermal conductivity for linear (possibly anisotropic) regions
		double Kt;			// volumetric heat capacity
		double qv;			// volume heat generation

		// properties for nonlinear conductivity
		int npts;			// number of points in the nonlinear conductivity curve
		CComplex Kn[128];   // here, I'm being _very_ lazy by defining a fixed-length buffer for the
		                    // thermal conductivity data points.
		CComplex GetK(double t);

		CMaterialProp();

	private:
};

class CBoundaryProp
{
	public:

		CBoundaryProp();

		string BdryName;
		string InConductor;
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

		CPointProp();

		string PointName;
		string InConductor;
		double V,qp;

	private:
};

class CCircuit
{
	public:

		CCircuit();

		string CircName;
		double V,q;
		int		CircType;

	private:

};

class CElement
{
	public:

		int p[3];
		int blk,lbl;
		CComplex D;		// elemental flux density
		CComplex d[3];  // smoothed flux density at corners
		CComplex ctr;
		double rsqr;
		int n[3];  // Add 3 ints to store elem's neigh.

		CComplex E();

	private:
};

class CPointVals
{
	public:

		double T;		// temperature
		CComplex F;		// heat flux density
		CComplex K;		// thermal conductivity
		CComplex G;		// temperature gradient

		CPointVals();

	private:
};


}
#endif


using namespace HPProcdata;
