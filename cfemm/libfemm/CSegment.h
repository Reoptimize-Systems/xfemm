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
#ifndef FEMM_CSEGMENT_H
#define FEMM_CSEGMENT_H

#include <string>

namespace femm {
// CSegment -- structure that holds information about lines joining control pts

/**
 * @brief The CSegment class
 * Holds data defined in section 4 of the .FEM file format description ("[NumSegments]").
 *
 * The official .FEM file format description states:
 * \verbatim
 * int   - number of the start point
 * int   - number of the end point
 * float - mesh size factor (-1 = auto)
 * int   - number of the boundary property, 1-indexed
 * int   - hide in postprocessor
 * int   - number of group
 * \endverbatim
 */
class CSegment
{
public:
    CSegment();

    // start and end points:
    int n0,n1;
    double MaxSideLength; ///< mesh size factor
    // BoundaryMarker: see child classes
    bool Hidden;          ///< hide in postprocessor
    int InGroup;          ///< group number

    bool IsSelected;
    void ToggleSelect();

    int BoundaryMarker;   ///< boundary property number, 0-indexed
    int InConductor;      ///< additional property for hpproc
    std::string BoundaryMarkerName;   ///< boundary property name
    std::string InConductorName;      ///< additional property for hpproc
    int cnt; ///< used by mesher for internal book keeping

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
