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
    /**
     * @brief toStream serializes the data and inserts it into \p out.
     * This virtual method is called by the \c operator<<() and
     * needs to be overridden by any subclass.
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
std::ostream& operator<< (std::ostream& os, const CPointProp& prop);
}
#endif
