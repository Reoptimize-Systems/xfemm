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

   Date Modified: 2017
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
        Johannes Zarl-Zierl
   Contact:
        szelitzkye@gmail.com
        sztibi82@gmail.com
        richard.crozier@yahoo.co.uk
        johannes.zarl-zierl@jku.at

   Contributions by Johannes Zarl-Zierl were funded by
   Linz Center of Mechatronics GmbH (LCM)
*/

// hpproc.h : interface of the HPProc class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef HPPROC_H_INCLUDED
#define HPPROC_H_INCLUDED

#include "CHPointVals.h"

#include "CHPointVals.h"
#include "FemmReader.h"
#include "PostProcessor.h"

#include <vector>

class HPProc
        : public femm::PostProcessor
        , public femm::SolutionReader
{

// Attributes
public:

	HPProc();
    virtual ~HPProc();

    CComplex blockIntegral(int inttype);

    virtual const femmsolver::CHSElement *getMeshElement(int idx) const;
    virtual const femmsolver::CHMeshNode *getMeshNode(int idx) const;

    bool getPointValues(double x, double y, CHPointVals &u);
    bool getPointValues(double x, double y, int k, CHPointVals &u);

    void lineIntegral(int inttype, double *z);

    bool OpenDocument(std::string solutionFile);
    femm::ParserResult parseSolution( std::istream &input, std::ostream &err = std::cerr ) override;
protected:
	// General problem attributes
	double  A_High, A_Low;
	double	A_lb, A_ub;

	// member functions
    bool InTriangleTest(double x, double y, int i) const override;
	//void GetLineValues(CXYPlot &p, int PlotType, int npoints);
    void getElementD(int k);

    CComplex E(const femmsolver::CHSElement *elem) const;
    CComplex e(const femmsolver::CHSElement *elem, int i) const;
};
#endif
