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
         */
    int CircType;

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
 * @brief operator << serializes the data in \p prop and inserts it into \p os
 * @param os
 * @param prop
 * @return \p os
 */
std::ostream& operator<< (std::ostream& os, const CCircuit& prop);


/**
 * @brief The CMCircuit class specializes a CCircuit for the fsolver component.
 */
class CMCircuit : public femm::CCircuit
{
public:
    CMCircuit();

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

}
#endif
