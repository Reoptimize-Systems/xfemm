#ifndef FEMM_CARCSEGMENT_H
#define FEMM_CARCSEGMENT_H

#include <string>
#include "femmcomplex.h"

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
class CArcSegment
{
    public:
        CArcSegment();

        // start and end point
        int n0,n1;
        double ArcLength; ///< arc angle
        double MaxSideLength; ///< max segment
        int BoundaryMarker; ///< boundary property number, 0-indexed
        bool Hidden; ///< hide in postproc
        int InGroup; ///< number of group
        int InConductor; ///< additional property for hpproc

        bool IsSelected;
        void ToggleSelect();

    private:

};

}
#endif
