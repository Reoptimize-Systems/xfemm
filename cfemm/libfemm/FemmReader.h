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

namespace femm {

/**
 * \brief The FemmReader class implements a parser for the Femm file format.
 * To allow for subtle differences between different flavors of the file format,
 * it is possible to define a handler function for tokens not handled by default.
 *
 * \sa FEASolver
 * \sa fmesher::FMesher
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
    FemmReader(std::ostream &errorpipe);
    virtual ~FemmReader();

    /**
     * @brief Parse the given file and store the data in the associated data object.
     * @param file
     * @return
     */
    bool parse(const std::string &file, FemmProblem &problem);

    /**
     * @brief setHandleTokenFunction sets a handler function for otherwise unhandled tokens.
     * \see handleToken
     */
    void setHandleTokenFunction(bool (*handlerFun)(const std::string &, std::istream &, std::ostream &));
    /**
     * @brief If ignoreUnhandledToken is set, the parser tries to continue if an unknown token is read.
     * This is mostly for debugging. Normally, every token should be handled (either in FemmReader, or in the handleToken function).
     * The default value is \c false.
     * @return the current property value
     */
    bool ignoreUnhandledToken() const;
    void setIgnoreUnhandledToken(bool value);

private:
    bool m_ignoreUnhandledToken;
    /// \brief Reference to the error message stream
    std::ostream &err;
    /// \see handleTokenDummy
    bool (*handleToken)(const std::string &, std::istream &, std::ostream &);

    /**
     * @brief handleToken is called by LoadProblemFile() when a token is encountered that it can not handle.
     *
     * This function is only a dummy that rejects any token, and is used if no other function is set using setHandleTokenFunction().
     * If a token is understood, this method should read its remaining text from the input stream and then return \c true.
     * If a token is not understood, the method must not change the position in the input stream, and must return \c false.
     *
     * @param token the token in question
     * @param input input stream
     * @param err output stream for error messages
     * @return \c false, if the token is not handled. \c true, if it is handled
     */
    static bool handleTokenDummy(const std::string &, std::istream &, std::ostream &) { return false; }
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
    MagneticsReader(std::ostream &errorpipe);
};

} //namespace

#endif
