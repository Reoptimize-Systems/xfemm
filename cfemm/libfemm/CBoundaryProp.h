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
     * 0 = constant value of A
     * 1 = Small skin depth eddy current BC
     * 2 = Mixed BC
     * 3 = stratigic dual image
     * 4 = periodic
     * 5 = anti-periodic
     * \endverbatim
     */
    int BdryFormat;

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
};

/**
 * @brief operator << serializes the data in \prop and inserts it into \p os
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

}
#endif
