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
#ifndef FEMM_CNODE_H
#define FEMM_CNODE_H

#include "femmcomplex.h"
#include <iostream>
#include <memory>
#include <string>

namespace femm {

/**
 * @brief The CNode class holds information about each control point.
 * Holds data defined in section 3 of the .FEM file format description ("[NumPoints]").
 *
 * The official .FEM file format description states:
 * \verbatim
 * float - x-position of the point or r (axisymmetric)
 * float - y-position of the point or z (axisymmetric)
 * int - number of associated node property (0 = no associated property)
 * int - number of group
 * \endverbatim
 *
 * For fpproc, the code shows:
 * \verbatim
 * float - x-position of the point or r (axisymmetric)
 * float - y-position of the point or z (axisymmetric)
 * int - BoundaryMarker
 * \endverbatim
 *
 * For hpproc, the code shows:
 * \verbatim
 * float - x-position of the point or r (axisymmetric)
 * float - y-position of the point or z (axisymmetric)
 * int - BoundaryMarker
 * int - number of group
 * int - InConductor
 * \endverbatim
 */
class CNode
{
public:
    CNode();
    CNode(double x, double y);
    virtual ~CNode();

    double x; ///< \brief x x-position of the point or r (axisymmetric)
    double y; ///< \brief x x-position of the point or r (axisymmetric)
    int InGroup;
    bool IsSelected;

    int BoundaryMarker; ///< \c bc in FEMM42
    int InConductor;  ///< The index of the conductor, or -1. \sa FemmProblem::circproplist
    std::string BoundaryMarkerName;
    std::string InConductorName; ///< The name of a conductor, or the special value "<None>".

    double GetDistance(const double xo, const double yo) const;
    /**
     * @brief CC
     * @return the coordinates as a complex number
     */
    CComplex CC() const;
    void ToggleSelect();

    /**
     * @brief clone returns a copy of the CNode that is memory managed using a unique_ptr.
     * This is a convenience function to make code more readable.
     * @return a unique_ptr holding a copy of this object.
     */
    std::unique_ptr<CNode> clone() const;
    /**
     * @brief hasBoundaryMarker
     * @return \c true, if the BoundaryMarker is set, \c false otherwise
     */
    bool hasBoundaryMarker() const;
    /**
     * @brief isInConductor
     * @return \c true, if InConductor is set, \c false otherwise
     */
    bool isInConductor() const;
private:
};

}
#endif
