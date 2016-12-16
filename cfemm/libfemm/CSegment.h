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
private:

};

}
#endif
