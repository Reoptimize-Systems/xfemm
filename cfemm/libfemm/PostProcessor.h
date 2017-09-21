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

namespace femm {

/**
 * @brief The PProcIface class is the minimum shared interface required to provide a uniform API to postprocessor classes.
 * This class allows a minimum effort solution to have a shared interface as is required by the xfemm lua interface,
 * without having to deduplicate all code just now.
 */
class PProcIface
{
public:
    virtual ~PProcIface();

    virtual bool OpenDocument( std::string solutionFile ) = 0;
    /**
     * @brief numElements is the same as meshelem.size()
     * @return the number of elements in the output mesh
     */
    virtual int numElements() const = 0;
    /**
     * @brief numNodesis the same as meshnode.size()
     * @return the numer of nodes in the output mesh
     */
    virtual int numNodes() const = 0;

protected:
    PProcIface();
};

/**
 * @brief The PostProcessor class provides all functionality that is shared between the different postprocessors.
 */
class PostProcessor : public PProcIface
{
public:
    virtual ~PostProcessor();
    int numElements() const override;
    int numNodes() const override;

    void addContourPoint(CComplex p);
    /**
     * @brief Adds a contour point at the closest input point to (x,y).
     *
     * If the selected point and a previous selected points lie at the ends of an arcsegment,
     * a contour is added that traces along the arcsegment.
     * @param mx x coordinate
     * @param my y coordinate
     *
     * \internal
     * ### FEMM sources:
     * - \femm42{femm/belaviewLua.cpp,lua_selectline()}
     * - \femm42{femm/femmviewLua.cpp,lua_selectline()}
     * \endinternal
     */
    void addContourPointFromNode(double mx, double my);
    void bendContour(double angle, double anglestep);
    void clearContour();

protected:
    // General problem attributes
    double *LengthConv;
    bool    Smooth;
    bool    bMultiplyDefinedLabels;

    double  PlotBounds[9][2];
    double  d_PlotBounds[9][2];

    // Some default behaviors
    int  d_LineIntegralPoints;
    bool bHasMask;

    // mesh data
    std::vector< std::unique_ptr<femmsolver::CMeshNode>>   meshnodes;
    std::vector< std::unique_ptr<femmsolver::CElement>> meshelems;

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

    CComplex HenrotteVector(int k);
    void FindBoundaryEdges();

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);
    //	void MsgBox(const char* message);

protected:
    PostProcessor();
    std::unique_ptr<femm::FemmProblem> problem;
};

} //namespace
#endif
