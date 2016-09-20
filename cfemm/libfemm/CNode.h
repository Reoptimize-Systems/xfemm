#ifndef FEMM_CNODE_H
#define FEMM_CNODE_H

#include <string>
#include "femmcomplex.h"

namespace femm {

using std::string;

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

        double x; ///< \brief x x-position of the point or r (axisymmetric)
        double y; ///< \brief x x-position of the point or r (axisymmetric)
        bool IsSelected;
        int BoundaryMarker;
        int InGroup,InConductor;

        double GetDistance(double xo, double yo);
        CComplex CC();
        void ToggleSelect();

    private:

};

}
#endif
