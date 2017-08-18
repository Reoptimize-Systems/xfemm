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

class CHPointVals
{
	public:

		double T;		// temperature
		CComplex F;		// heat flux density
		CComplex K;		// thermal conductivity
		CComplex G;		// temperature gradient

        CHPointVals();

	private:
};


}
#endif


using namespace HPProcdata;
