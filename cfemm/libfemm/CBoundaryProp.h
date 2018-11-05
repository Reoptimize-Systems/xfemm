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

#ifndef FEMM_CBOUNDARYPROP_H
#define FEMM_CBOUNDARYPROP_H

#include <string>
#include <iostream>
#include "femmcomplex.h"

namespace femm {

/**
 * @brief The CBoundaryProp class
 * Holds data defined in section 2.2 of the .FEM file format description ("[BdryProps]").
 */
class CBoundaryProp
{
public:

    CBoundaryProp();

    std::string BdryName; /// \verbatim <BdryName> \endverbatim
    /**
     * @brief BdryFormat
     * Type of boundary condition we are applying.
     *
     * For magnetic problems, this is defined as:
     * \verbatim
     * <type>
     * 0 = Constant value of A
     * 1 = Small skin depth eddy current BC
     * 2 = Mixed BC
     * 3 = SDI boundary (deprecated)
     * 4 = Periodic
     * 5 = Antiperiodic
     * 6 = Periodic AGE
     * 7 = Antiperiodic AGE
     * \endverbatim
     *
     * For electrostatics problems, this is defined as:
     * \verbatim
     * 0 = Fixed Voltage
     * 1 = Mixed BC
     * 2 = Surface Charge Density
     * 3 = Periodic
     * 4 = Antiperiodic
     * \endverbatim
     */
    int BdryFormat;

    /**
     * @brief InnerAngle
     * Inner rotor angle (for building air gap element)
     * \verbatim
     * <InnerAngle>
     * \endverbatim
     */
    double InnerAngle;

    /**
     * @brief OuterAngle
     * Outer rotor angle (for building air gap element)
     * \verbatim
     * <OuterAngle>
     * \endverbatim
     */
    double OuterAngle;

    enum class PeriodicityType { Any, Periodic, AntiPeriodic };
    /**
     * @brief Check periodicity of the boundary condition.
     *
     * This method provides an abstraction for the problem-type-specific values of BdryFormat.
     * @param pt Optional parameter to allow checking specific type of periodicity.
     * @return \c true, if the BdryFormat is (anti-)periodic, \c false otherwise.
     */
    virtual bool isPeriodic( PeriodicityType pt = PeriodicityType::Any) const = 0;

    /**
     * @brief toStream serializes the data and inserts it into \p out.
     * This virtual method is called by the \c operator<<() and
     * needs to be overridden by any subclass.
     *
     * @param out
     */
    virtual void toStream( std::ostream &out ) const = 0;
};

/**
 * @brief operator << serializes the data in \p prop and inserts it into \p os
 * @param os
 * @param prop
 * @return \p os
 */
std::ostream& operator<< (std::ostream& os, const CBoundaryProp& prop);

/**
 * @brief The CMBoundaryProp class holds BoundaryProp data for magnetics problems.
 */
class CMBoundaryProp : public CBoundaryProp
{
public:
    CMBoundaryProp();
    /**
     * @brief A0
     * Magnetic vector potential on the segment (for BdryFormat=0).
     *
     * Unit: \c Wb/m
     * \verbatim
     * <A_0>
     * \endverbatim
     */
    double A0;
    /**
     * @brief A1
     * Magnetic vector potential on the segment in x direction (for BdryFormat=0).
     *
     * Unit:
     *
     * Unit: \c Wb/m
     * \verbatim
     * <A_1>
     * \endverbatim
     */
    double A1;
    /**
     * @brief A2
     * Magnetic vector potential on the segment in y direction (for BdryFormat=0).
     *
     * Unit: \c Wb/m
     * \verbatim
     * <A_2>
     * \endverbatim
     */
    double A2;
    /**
     * @brief phi
     * Angle (for BdryFormat=0).
     *
     * Unit: \c deg
     * \verbatim
     * <Phi>
     * \endverbatim
     */
    double phi;

    /**
     * @brief Mu
     * Relative permeability (for BdryFormat=1).
     * Necessary to apply eddy current BC.
     * \verbatim
     * <Mussd>
     * \endverbatim
     */
    double Mu;
    /**
     * @brief Sig
     * Conductivity (for BdryFormat=1).
     * Necessary to apply eddy current BC.
     *
     * Unit: \c MS/m
     * \verbatim
     * <Sigmassd>
     * \endverbatim
     */
    double Sig;

    /**
     * @brief c0
     * First coefficient for mixed BC (for BdryFormat=2).
     * \verbatim
     * <c0> <c0i>
     * \endverbatim
     */
    CComplex c0;
    /**
     * @brief c1
     * Second coefficient for mixed BC (for BdryFormat=2).
     * \verbatim
     * <c1> <c1i>
     * \endverbatim
     */
    CComplex c1;

    virtual bool isPeriodic(PeriodicityType pt) const override;

    /**
     * @brief fromStream constructs a CMBoundaryProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CMBoundaryProp
     */
    static CMBoundaryProp fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const override;
private:
};

/**
 * @brief The CHBoundaryProp class holds BoundaryProp data for heat flow problems.
 */
class CHBoundaryProp : public CBoundaryProp
{
public:

    CHBoundaryProp();

    double Tset;			// Fixed value of temperature for BdryFormat=0;
    double Tinf;			// External temperature for convection or radiation
    double qs;				// Heat flux;
    double beta;			// radiosity coefficient
    double h;				// Heat transfer coefficient

    virtual bool isPeriodic(PeriodicityType pt) const override;

    /**
     * @brief fromStream constructs a CHBoundaryProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CHBoundaryProp
     */
    static CHBoundaryProp fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const override;
private:
};

/**
 * @brief The CSBoundaryProp class holds BoundaryProp data for electrostatics problems.
 */
class CSBoundaryProp : public CBoundaryProp
{
public:

    CSBoundaryProp();

    double V;            ///< Fixed value of V for BdryFormat=0;
    double c0,c1;        ///< Coefficients for BdryFormat=1;
    double qs;           ///< Surface charge density for Bdryformat=2;


    virtual bool isPeriodic(PeriodicityType pt) const override;
    /**
     * @brief fromStream constructs a CSBoundaryProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CSBoundaryProp
     */
    static CSBoundaryProp fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const override;
private:
};
}
#endif
