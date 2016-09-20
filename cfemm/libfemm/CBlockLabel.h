#ifndef FEMM_CBLOCKLABEL_H
#define FEMM_CBLOCKLABEL_H

#include <string>
#include "femmcomplex.h"

namespace femm {

/**
 * @brief The CBlockLabel class holds block label information
 * Holds data defined in section 6 and 7 of the .FEM file format description ("[NumHoles]", "[NumBlockLabels]").
 *
 * The official .FEM file format description states for holes:
 * \verbatim
 * float - x-position of the BlcokLabel or r (axisymmetric)
 * float - y-position of the BlcokLabel or z (axisymmetric)
 * int   - number of the group associated with the hole
 * \endverbatim
 *
 * The official .FEM file format description states for BlockLabels:
 * \verbatim
 * float - x-position of the BlcokLabel or r (axisymmetric)
 * float - y-position of the BlcokLabel or z (axisymmetric)
 * int   - number of block type (region)
 * float - desired mesh size
 * int   - number of associated circuit (1-indexed; 0 = no associated circuit)
 * float - magnetization direction (deg)
 * int   - number of the group
 * int   - number of turns (for stranded conductor coupling), 1 if not stranded
 * int   - flag: 1 = block label is located in external region (for axisymmetric Kelvin transformation boundary conditions); normally 0
 * \endverbatim
 *
 * hpproc uses a slightly different format:
 * \verbatim
 * float - x-position of the BlcokLabel or r (axisymmetric)
 * float - y-position of the BlcokLabel or z (axisymmetric)
 * int   - number of block type (region)
 * float - desired mesh size
 * int   - number of the group
 * int   - flag: 1 = block label is located in external region (for axisymmetric Kelvin transformation boundary conditions); normally 0
 * \endverbatim
 */
class CBlockLabel
{
    public:
        CBlockLabel();

        double x,y;
        int BlockType;   ///< number of block type (region)
        double MaxArea;  ///< desired mesh size
        int InCircuit;   ///< number of associated circuit (0-indexed)
        double MagDir;   ///< magnetization direction (\c deg)
        int InGroup;     ///< number of the group
        int    Turns;    ///< number of turns
        bool IsExternal; ///< is located in external region

        std::string MagDirFctn; ///< additional property for fpproc
        bool IsDefault;  ///< additional property for hpproc

        bool IsSelected;
        void ToggleSelect();
        double GetDistance(double xo, double yo);

    private:

};

}
#endif

