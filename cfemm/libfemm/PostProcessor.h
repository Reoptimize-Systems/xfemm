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
   By: Richard Crozier
       Johannes Zarl-Zierl
   Contact:
	    richard.crozier@yahoo.co.uk
       johannes@zarl-zierl.at

   Contributions by Johannes Zarl-Zierl were funded by
	Linz Center of Mechatronics GmbH (LCM)
*/
#ifndef FEMM_POSTPROCESSOR_H
#define FEMM_POSTPROCESSOR_H

#include "femmcomplex.h"
#include "fparse.h"
#include "FemmProblem.h"

#include <vector>


class PostProcessor
{
public:

    PostProcessor();
    virtual ~PostProcessor();

    std::shared_ptr<femm::FemmProblem> problem;
    // General problem attributes
    double *LengthConv;
    bool    Smooth;
    bool    bMultiplyDefinedLabels;

    double  PlotBounds[9][2];
    double  d_PlotBounds[9][2];

    // Some default behaviors
    int  d_LineIntegralPoints;
    bool bHasMask;

    // List of elements connected to each node;
    int *NumList;
    int **ConList;

    // list of points in a user-defined contour;
    std::vector< CComplex > contour;

    // member functions
    int InTriangle(double x, double y);
    bool InTriangleTest(double x, double y, int i);
    CComplex Ctr(int i);
    double ElmArea(int i);
    double ElmArea(femmsolver::CElement *elm);

    void BendContour(double angle, double anglestep);
    CComplex HenrotteVector(int k);
    void FindBoundaryEdges();

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);
    //	void MsgBox(const char* message);
};
#endif
