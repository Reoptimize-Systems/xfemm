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

#ifndef FEMM_CBLOCKLABEL_H
#define FEMM_CBLOCKLABEL_H

#include "femmcomplex.h"
#include <iostream>
#include <memory>
#include <string>

namespace femm {

/**
 * @brief The CBlockLabel class holds block label information
 * Holds data defined in section 6 and 7 of the .FEM file format description ("[NumHoles]", "[NumBlockLabels]").
 *
 * The official .FEM file format description states for holes:
 * \verbatim
 * float - x-position of the BlockLabel or r (axisymmetric)
 * float - y-position of the BlockLabel or z (axisymmetric)
 * int   - number of the group associated with the hole
 * \endverbatim
 *
 * The official .FEM file format description states for BlockLabels:
 * \verbatim
 * float - x-position of the BlockLabel or r (axisymmetric)
 * float - y-position of the BlockLabel or z (axisymmetric)
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
 * float - x-position of the BlockLabel or r (axisymmetric)
 * float - y-position of the BlockLabel or z (axisymmetric)
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
    int InGroup;     ///< number of the group
    bool IsExternal; ///< is located in external region

    bool IsDefault;  ///< additional property for hpproc

    bool IsSelected;
    void ToggleSelect();
    double GetDistance(double xo, double yo);

    int BlockType;   ///< number of block type (region)
    int InCircuit;   ///< number of associated circuit (0-indexed)
    std::string BlockTypeName; ///< name of block type
    std::string InCircuitName; ///< name of associated circuit

    /**
     * @brief hasBlockType
     * A block label without block type is a hole.
     * @return \c true, if BlockTypeName is set, \c false otherwise.
     */
    bool hasBlockType() const;
    /**
     * @brief isInCircuit
     * @return \c true, if InCircuitName is set, \c false otherwise.
     */
    bool isInCircuit() const;

    /**
     * @brief toStream serializes the data and inserts it into \p out.
     * This virtual method is called by the \c operator<<() and
     * needs to be overridden by any subclass.
     *
     * @param out
     */
    virtual void toStream( std::ostream &out ) const = 0;

    /**
     * @brief clone creates a copy of the block label.
     * The copy is memory-managed through a unique_ptr.
     * @return a unique_ptr holding a copy of this object.
     */
    virtual std::unique_ptr<CBlockLabel> clone() const = 0;
private:

};

/**
 * @brief operator << serializes the data in \p lbl and inserts it into \p os
 * @param os
 * @param lbl
 * @return \p os
 */
std::ostream& operator<< (std::ostream& os, const CBlockLabel& lbl);


/**
 * @brief The CMBlockLabel class specializes CBlockLabel for electromagnetics problems.
 */
class CMBlockLabel : public CBlockLabel
{
public:
    CMBlockLabel();

    //---- fsolver attributes:
    // used for proximity effect regions only.
    CComplex ProximityMu;
    int    Turns;    ///< number of turns
    bool bIsWound; ///< true, if Turns>1, but also in some other conditions; set by \c FSolver::GetFillFactor()

    //---- fpproc attributes:
    double MagDir;   ///< magnetization direction (\c deg), if constant. \sa MagDirFctn
    std::string MagDirFctn; ///< \brief Lua expression describing magnetization direction
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
    static CMBlockLabel fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const;
    virtual std::unique_ptr<CBlockLabel> clone() const;
private:

};

/**
 * @brief The CHBlockLabel class specializes CBlockLabel for heat flow problems.
 */
class CHBlockLabel : public CBlockLabel
{
public:
    CHBlockLabel();

    /**
     * @brief fromStream constructs a CHBlockLabel from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CBlockLabel
     */
    static CHBlockLabel fromStream(std::istream &input, std::ostream &err = std::cerr);
    virtual void toStream( std::ostream &out ) const;
    virtual std::unique_ptr<CBlockLabel> clone() const;
};

/**
 * @brief The CSBlockLabel class specializes CBlockLabel for electrostatics problems.
 * \internal
 * ### FEMM reference source:
 * - \femm42{femm/bd_nosebl.h}
 */
class CSBlockLabel : public CBlockLabel
{
public:
    CSBlockLabel();

    /**
     * @brief fromStream constructs a CSBlockLabel from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CBlockLabel
     *
     * \internal
     * ### FEMM reference source:
     * - \femm42{femm/beladrawDoc.cpp,CbeladrawDoc::OnOpenDocument()}
     */
    static CSBlockLabel fromStream(std::istream &input, std::ostream &err = std::cerr);
    virtual void toStream( std::ostream &out ) const;
    virtual std::unique_ptr<CBlockLabel> clone() const;
};

}
#endif

