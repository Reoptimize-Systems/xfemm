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


#ifndef PROBLEM_H
#define PROBLEM_H
#include <string>
#include "femmcomplex.h"

namespace HPProcdata{

class CMeshNode
{
	public:
		CMeshNode();

        double x,y;
        double T;  ///< temperature at the node
		double msk;
		int xs,ys;
        /**
         * @brief Q boundary flag
         * Indicates whether the node lies on a boundary, or in a conductor, or none.
         */
        int Q;
        bool IsSelected;

		double GetDistance(double xo, double yo);
		CComplex CC();

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

        std::string BlockName;
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

class CPointProp
{
	public:

		CPointProp();

        std::string PointName;
        std::string InConductor;
		double V,qp;

	private:
};

class CCircuit
{
	public:

		CCircuit();

        std::string CircName;
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
