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
    virtual ~CPointProp();

    // <PointName>
    std::string PointName;

    /**
     * @brief toStream serializes the data and inserts it into \p out.
     * This virtual method is called by the \c operator<<() and
     * needs to be overridden by any subclass.
     *
     * @param out
     */
    virtual void toStream( std::ostream &out ) const = 0;

protected:
    CPointProp();
};

class CMPointProp : public CPointProp
{
public:

    CMPointProp();

    CComplex J;   ///< \brief J: applied point current \code<I_re>, <I_im>\endcode
    CComplex A;   ///< \brief A, magnetic vector potential \code<A_re>, <A_im>\endcode

    /**
     * @brief fromStream constructs a CPointProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CPointProp
     */
    static CMPointProp fromStream( std::istream &input, std::ostream &err = std::cerr );
    /**
     * @brief toStream serializes the data and inserts it into \p out.
     * This virtual method is called by the \c operator<<() and
     * needs to be overridden by any subclass.
     *
     * @param out
     */
    virtual void toStream( std::ostream &out ) const override;
private:
};

class CHPointProp : public CPointProp
{
public:
    CHPointProp();
    /**
     * @brief fromStream constructs a CHPointProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CHPointProp
     */
    static CHPointProp fromStream( std::istream &input, std::ostream &err = std::cerr );

    double V; ///< \brief Fixed temperature in Kelvin. \code<Tp>\endcode
    double qp; ///< \brief Point heat generation in Watt per meter. \code<qp>\endcode

    virtual void toStream( std::ostream &out ) const override;
private:
};

class CSPointProp : public CPointProp
{
public:
    CSPointProp();

    double V; ///< \brief Specified potential. \code<Vp>\endcode
    double qp; ///< \brief Point charge density (C/m). \code<qp>\endcode

    /**
     * @brief fromStream constructs a CSPointProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CSPointProp
     */
    static CSPointProp fromStream( std::istream &input, std::ostream &err = std::cerr );

    virtual void toStream( std::ostream &out ) const override;
};

/**
 * @brief operator << serializes the data in \p prop and inserts it into \p os
 * @param os
 * @param prop
 * @return \p os
 */
std::ostream& operator<< (std::ostream& os, const CMPointProp& prop);
}
#endif
