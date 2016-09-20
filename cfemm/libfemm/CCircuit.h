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
         * @brief CircType
         * \verbatim
         * <type>
         * 0 = parallel
         * 1 = series
         * \endverbatim
         */
        int        CircType;

    private:

};
}
#endif
