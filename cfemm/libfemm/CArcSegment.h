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

   Date Modified: 2017 - 01 - 14
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
        Johannes Zarl-Zierl
   Contact:
        szelitzkye@gmail.com
        sztibi82@yahoo.com
        richard.crozier@yahoo.co.uk
        johannes@zarl-zierl.at

 Contributions by Johannes Zarl-Zierl were funded by Linz Center of 
 Mechatronics GmbH (LCM)
*/

#ifndef FEMM_CARCSEGMENT_H
#define FEMM_CARCSEGMENT_H

#include "CSegment.h"
#include "femmcomplex.h"
#include <string>

namespace femm {

/**
 * @brief The CArcSegment class
 * Holds data defined in section 5 of the .FEM file format description ("[NumArcSegments]").
 *
 * The official .FEM file format description states:
 * \verbatim
 * int   - number of the start poin
 * int   - number of the end point
 * float - arc angle (1 to 180 deg)
 * float - max segment (0.01 to 10)
 * int   - number of the boundary property, 1-indexed
 * int   - hide in postprocessor
 * int   - number of group
 * \endverbatim
 */
class CArcSegment : public CSegment
{
public:
    CArcSegment();

    double ArcLength; ///< arc angle [deg]
    bool NormalDirection; ///< mesher-specific property
private:

};

}
#endif
