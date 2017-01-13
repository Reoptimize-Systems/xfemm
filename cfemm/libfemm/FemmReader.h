/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#ifndef FEMMREADER_H
#define FEMMREADER_H

#include "FemmProblem.h"

#include <iostream>
#include <string>
#include <memory>

namespace femm {

/**
 * \brief The FemmReader class implements a parser for the Femm file format.
 * To allow for subtle differences between different flavors of the file format,
 * it is possible to define a handler function for tokens not handled by default.
 *
 * \sa FEASolver
 * \sa fmesher::FMesher
 *
 * \internal
 * The functionality of this class is not in FemmProblem, because I didn't want to make
 * FemmProblem a template class.
 * However, using a template class for FemmReader allows us a type-checked way of
 * creating data objects of the correct type. This is also possible using factory methods, but
 * would require much more boiler-plate code that way.
 */
template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
class FemmReader
{
// Attributes
public:
    using FemmReader_type = FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>;
    using PointProp_type = PointPropT;
    using BoundaryProp_type = BoundaryPropT;
    using BlockProp_type = BlockPropT;
    using CircuitProp_type = CircuitPropT;
    using BlockLabel_type = BlockLabelT;
    using Node_type = NodeT;

    /**
     * @brief FemmReader constructor
     * @param errorpipe
     */
    FemmReader(std::shared_ptr<FemmProblem>problem, std::ostream &errorpipe);
    virtual ~FemmReader();

    /**
     * @brief Parse the given file and store the data in the associated data object.
     * @param file
     * @return
     */
    bool parse(const std::string &file);

    /**
     * @brief If ignoreUnhandledTokens is set, the parser tries to continue if an unknown token is read.
     * This is mostly for debugging. Normally, every token should be handled (either in FemmReader, or in the handleToken function).
     * The default value is \c false.
     * @return the current property value
     */
    bool ignoreUnhandledTokens() const;
    void setIgnoreUnhandledTokens(bool value);
protected:
    /**
     * @brief handleToken is called by parse() when a token is encountered that it can not handle.
     *
     * \internal
     * This function is only a dummy that rejects any token, and is overridden in derived classes.
     * If a token is understood, this method should read its remaining text from the input stream and then return \c true.
     * If a token is not understood, the method must not change the position in the input stream, and must return \c false.
     *
     * @param token the current token that was already read from input
     * @param input input stream
     * @param err output stream for error messages
     * @return \c false, if the token is not handled. \c true, if it is handled
     */
    virtual bool handleToken(const std::string &, std::istream &, std::ostream &) { return false; }

    std::shared_ptr<FemmProblem> problem;
private:
    bool ignoreUnhandled;
    /// \brief Reference to the error message stream
    std::ostream &err;
};

class MagneticsReader : public FemmReader<
        femm::CPointProp
        , femm::CMBoundaryProp
        , femm::CMMaterialProp
        , femm::CMCircuit
        , femm::CMBlockLabel
        , femm::CNode
        >
{
public:
    MagneticsReader(std::shared_ptr<FemmProblem> problem, std::ostream &errorpipe);
protected:
    bool handleToken(const std::string &token, std::istream &input, std::ostream &err) override;
};

} //namespace

#endif
