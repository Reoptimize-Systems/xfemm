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
#ifndef FEMM_CCIRCUIT_H
#define FEMM_CCIRCUIT_H

#include "femmcomplex.h"
#include <iostream>
#include <string>

namespace femm {
/**
 * @brief The CCircuit class
 * Holds data defined in section 2.4 of the .FEM file format description ("[CircuitProps]").
 */
class CCircuit
{
public:

    CCircuit();

    /// \verbatim <circuitName> \endverbatim
    std::string CircName;

    /**
         * @brief CircType
         * \verbatim
         * <type>
         * 0 = parallel
         * 1 = series
         * \endverbatim
         *
         * Heat flow problems:
         * \verbatim
         * <type>
         * 0 = total heat flow
         * 1 = fixed temperature
         * \endverbatim
         *
         * Electrostatics problems:
         * \verbatim
         * <type>
         * 0 = prescribed voltage
         * 1 = total charge
         * \endverbatim
         */
    int CircType;

    /**
     * @brief toStream serializes the data and inserts it into \p out.
     * This virtual method is called by the \c operator<<() and
     * needs to be overridden by any subclass.
     *
     * @param out
     */
    virtual void toStream( std::ostream &out ) const = 0;
private:

};

/**
 * @brief operator << serializes the data in \p prop and inserts it into \p os
 * @param os
 * @param prop
 * @return \p os
 */
std::ostream& operator<< (std::ostream& os, const CCircuit& prop);


/**
 * @brief The CMCircuit class specializes a CCircuit for the fsolver component.
 */
class CMCircuit : public CCircuit
{
public:
    CMCircuit();

    /**
         * @brief Amps
         * Circuit current.
         *
         * Unit: \c A
         * \verbatim
         * <TotalAmpsre> <TotalAmpsim>
         * \endverbatim
         */
    CComplex  Amps;

    CComplex dVolts;
    int OrigCirc;

    // some member variables used to store some results
    CComplex J;
    CComplex dV;
    int Case;

    /**
     * @brief fromStream constructs a CMCircuit from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CMCircuit
     */
    static CMCircuit fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const override;

private:
};

class CHConductor : public CCircuit
{
public:
    CHConductor();

    /**
     * @brief V thermal conductivity(?)
     *
     * \note CCircuit in \femm42{femm/hd_nosebl.h} names this field (maybe more appropriately) "T".
     *
     * Unit: ?
     * \verbatim
     * <tc>
     * \endverbatim
     */
    double V;
    /**
     * @brief q
     *
     * Unit: ?
     * \verbatim
     * <qc>
     * \endverbatim
     */
    double q;

    /**
     * @brief fromStream constructs a CHConductor from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CHConductor
     */
    static CHConductor fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const override;
private:
};

/**
 * @brief The CSCircuit class specializes a CCircuit for the electrostatics problems.
 */
class CSCircuit : public CCircuit
{
public:
    CSCircuit();

    double V;   ///< Prescribed voltage
    double q;   ///< Total charge

    /**
     * @brief fromStream constructs a CSCircuit from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CSCircuit
     */
    static CSCircuit fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const override;

private:
};

} //namespace
#endif
