#ifndef FEMM_CCPOINTPROP_H
#define FEMM_CCPOINTPROP_H

#include <string>
#include <iostream>
#include "femmcomplex.h"

namespace femm {

/**
 * @brief The CPointProp class
 * Contains data defined in section 2.1 of the .FEM file format ("[PointProps]").
 */
class CPointProp
{
public:

    CPointProp();

    // <PointName>
    std::string PointName;
    CComplex J;   ///< \brief J: applied point current \code<I_re>, <I_im>\endcode
    CComplex A;   ///< \brief A, magnetic vector potential \code<A_re>, <A_im>\endcode

    /**
     * @brief fromStream constructs a CPointProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CPointProp
     */
    static CPointProp fromStream( std::istream &input, std::ostream &err = std::cerr );
private:
};

}
#endif
