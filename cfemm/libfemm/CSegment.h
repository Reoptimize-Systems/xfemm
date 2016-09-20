#ifndef FEMM_CSEGMENT_H
#define FEMM_CSEGMENT_H

#include <string>
#include "femmcomplex.h"

namespace femm {
// CSegment -- structure that holds information about lines joining control pts

/**
 * @brief The CSegment class
 * Holds data defined in section 4 of the .FEM file format description ("[NumSegments]").
 *
 * The official .FEM file format description states:
 * \verbatim
 * int   - number of the start poin
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
        int BoundaryMarker;   ///< boundary property number, 0-indexed
        bool Hidden;          ///< hide in postprocessor
        int InGroup;          ///< group number
        int InConductor;      ///< additional property for hpproc

        bool IsSelected;
        void ToggleSelect();

    private:

};

}
#endif
