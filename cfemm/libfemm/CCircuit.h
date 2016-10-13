#ifndef FEMM_CCIRCUIT_H
#define FEMM_CCIRCUIT_H

#include <string>
#include "femmcomplex.h"

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
         */
        int        CircType;

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
    private:

};

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

private:
};

}
#endif
