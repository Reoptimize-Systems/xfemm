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

    /**
     * @brief Unselect all block labels.
     * This also invalidates the mask, if one has been made.
     */
    void clearBlockSelection();

    /**
     * @brief Remove all contour points.
     */
    void clearContour();

    /**
     * @brief Unselect all items.
     * \internal
     * Virtual, because ElectrostaticsPostProcessor needs to override.
     * \endinternal
     */
    virtual void clearSelection();

    double ElmArea(int i) const;

    /**
     * @brief getMeshElement gets an element from meshelems.
     *
     * @param idx element index
     * @return the element, or a \c nullptr if idx is invalid
     */
    virtual const femmsolver::CElement *getMeshElement(int idx) const = 0;

    virtual const femmsolver::CMeshNode *getMeshNode(int idx) const = 0;

    const std::vector< std::unique_ptr<femmsolver::CMeshNode>> &getMeshNodes() const;

    /**
     * @brief getProblem
     * @return a const pointer to the problem description
     */
    const femm::FemmProblem *getProblem() const;

    /**
     * @brief isSelectionOnAxis
     * Only meaningful for axisymmetric problems.
     *
     * @return \c true, if the selection has items that lie on r=0.
     *
     * \internal
     * Virtual, because ElectrostaticsPostProcessor needs to override.
     *
     * \note Originally part of MakeMask.
     * \endinternal
     */
    virtual bool isSelectionOnAxis() const;

    /**
     * @brief makeMask
     * @return \c true, if a mask was established, \c false otherwise.
     * \internal
     * Electrostatics and current flow problems share the same implementation,
     * magnetics problems use a slightly different implementation.
     * ### FEMM sources:
     * - \femm42{femm/bv_makemask.cpp}
     * - \femm42{femm/cv_makemask.cpp}
     * \endinternal
     */
    virtual bool makeMask();
    int numElements() const override;

    int numNodes() const override;

    /**
     * @brief  Toggle the selection of the block that contains point (x,y).
     *
     * @param px x coordinate of the point
     * @param py y coordinate of the point
     * @return \c true, if a matching block label was found, \c false otherwise.
     */
    bool selectBlocklabel(double px, double py);

    /**
     * @brief setSmoothing
     * @param value
     */
    void setSmoothing(bool value);

    /**
     * @brief toggles the selection for all BlockLabels in a group.
     * This also invalidates the mask, if one has been made.
     * @param group the group index, or 0 for all.
     */
    void toggleSelectionForGroup(int group);


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
    /**
     * @brief AECF
     * Computes the permeability correction factor for axisymmetric
     * external regions.  This is sort of a kludge, but it's the best
     * way I could fit it in.  The structure of the code wasn't really
     * designed to have a permeability that varies with position in a
     * continuous way.
     * @param elem
     * @return
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::AECF(int)}
     * \endinternal
     */
    double AECF(const femmsolver::CElement *elem) const;
    /**
     * @brief AECF
     * Correction factor for a point within the element, rather than
     * for the center of the element.
     * @param elem
     * @param p
     * @return
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::AECF(int,CComplex)}
     * \endinternal
     */
    double AECF(const femmsolver::CElement *elem, CComplex p) const;

    /**
     * @brief getNodalD computes D for node N
     * \note This command only is implemented for heatflow and magnetics problems.
     * @param d the output parameter
     * @param N
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::GetNodalD(CComplex*,int)}
     * - \femm42{femm/hviewDoc.cpp,ChviewDoc::GetNodalD(CComplex*,int)}
     * \endinternal
     */
    void getNodalD(CComplex *d, int N) const;

    /**
     * @brief getPointD
     * \note This command only is implemented for heatflow and magnetics problems.
     * @param x
     * @param y
     * @param D
     * @param elm a reference to the element that contains the point of interest.
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::GetPointD()}
     * - \femm42{femm/hviewDoc.cpp,ChviewDoc::GetPointD()}
     * \endinternal
     */
    void getPointD(double x, double y, CComplex &D, const femmsolver::CElement &element) const;

    int InTriangle(double x, double y) const;
    // currently virtual until we merge hpproc version of it:
    virtual bool InTriangleTest(double x, double y, int i) const;

    /**
     * @brief isKosher
     * If:
     *    1) this is an axisymmetric problem;
     *    2) the selected geometry lies along the r=0 axis, and
     *    3) we have a node on the r=0 axis that we are trying to determine
     *     if we should set to zero.
     * This routine determines whether the node is at the extents of
     * the r=0 domain (or lies at a break in some sub-interval).
     *
     * @param k a mesh node index
     * @return \c true, if it is Ok to define the node as zero, \c false otherwise.
     *
     * \internal
     * ### FEMM sources (identical implementation):
     * - \femm42{femm/bv_makemask.cpp}
     * - \femm42{femm/cv_makemask.cpp}
     * - \femm42{femm/makemask.cpp}
     * \endinternal
     */
    bool isKosher(int k) const;

    /**
     * @brief Check if two mesh elements have the same material.
     * Materials are compared based on their properties.
     *
     * @param e1 element 1
     * @param e2 element 2
     * @return \c true, if the materials of e1 and e2 are the same, \c false otherwise
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::IsSameMaterial(int,int)}
     *
     * \note Instead of having different IsSameMaterial implementations, the real logic has been shifted to CMaterialProp.
     * \endinternal
     */
    bool isSameMaterial(const femmsolver::CElement &e1, const femmsolver::CElement &e2) const;

    CComplex Ctr(int i);
    double ElmArea(femmsolver::CElement *elm);

    CComplex HenrotteVector(int k) const;
    void FindBoundaryEdges();

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);
    //	void MsgBox(const char* message);

protected:
    PostProcessor();
    std::shared_ptr<femm::FemmProblem> problem;
};

} //namespace
#endif
