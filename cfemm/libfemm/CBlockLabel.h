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
    double MaxArea;  ///< desired mesh size
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

/**
 * @brief The CSolverBlockLabel class
 * Specialization of the CBlockLabel class for the solvers.
 * In the fmesher, \c BlockType and \c InCircuit are indices referring to nodes/properties/circuits.
 */
class CSolverBlockLabel : public CBlockLabel
{
public:
    CSolverBlockLabel();

    int BlockType;   ///< number of block type (region)
    int InCircuit;   ///< number of associated circuit (0-indexed)
};

/**
 * @brief The CMesherBlockLabel class
 * Specialization of the CBlockLabel class for the mesher.
 * In the fmesher, \c BlockType and \c InCircuit are holding the name of the referred object.
 */
class CMesherBlockLabel : public CBlockLabel
{
public:
    CMesherBlockLabel();

    std::string BlockType;
    // problem specific properties
    std::string InCircuit;
    int selectFlag;
};

class CMBlockLabel : public CSolverBlockLabel
{
public:
    CMBlockLabel();

    //---- fsolver attributes:
    // used for proximity effect regions only.
    CComplex ProximityMu;
    bool bIsWound; ///< true, if Turns>1, but also in some other conditions; set by \c FSolver::GetFillFactor()

    //---- fpproc attributes:
    std::string MagDirFctn;
    int Case;
    CComplex  J,dVolts;
    // attributes used to keep track of wound coil properties...
    double FillFactor;
    CComplex o,mu;

private:

};
}
#endif

