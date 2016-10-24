#ifndef FEMM_CBLOCKLABEL_H
#define FEMM_CBLOCKLABEL_H

#include "femmcomplex.h"
#include <iostream>
#include <string>

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

    /**
     * @brief toStream serializes the data and inserts it into \p out.
     * This virtual method is called by the \c operator<<() and
     * needs to be overridden by any subclass.
     *
     * Unless \c NDEBUG is defined, this dummy implementation in the base class will call \c assert(false).
     *
     * @param out
     */
    virtual void toStream( std::ostream &out ) const;
private:

};

/**
 * @brief operator << serializes the data in \lbl and inserts it into \p os
 * @param os
 * @param lbl
 * @return \p os
 */
std::ostream& operator<< (std::ostream& os, const CBlockLabel& lbl);


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

    /**
     * @brief fromStream constructs a CSolverBlockLabel from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CSolverBlockLabel
     */
    static CSolverBlockLabel fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const override;
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

class CMSolverBlockLabel : public CSolverBlockLabel
{
public:
    CMSolverBlockLabel();

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

    /**
     * @brief fromStream constructs a CMSolverBlockLabel from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CMSolverBlockLabel
     */
    static CMSolverBlockLabel fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const override;
private:

};


}
#endif

