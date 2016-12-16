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

#include <iostream>
#include <string>

namespace femm {

/**
 * \brief The FemmReader class implements a parser for the Femm file format.
 * The reader object is tied to the object holding the data (i.e. the data object).
 *
 * Requirements on the ProblemData_type:
 *  * Data members:
 *    * \code std::vector<PointProp_type> nodeproplist\endcode
 *    * \code std::vector<BoundaryProp_type> lineproplist\endcode
 *    * \code std::vector<BlockProp_type> blockproplist\endcode
 *    * \code std::vector<CircuitProp_type> circproplist\endcode
 *    * \code std::vector<BlockLabel_type> labellist\endcode
 *    * \code std::vector<Node_type> nodelist\endcode
 *
 * The referenced data types must be defined (e.g. \code ProblemData_type::PointProp_type\endcode).
 * Obviously, this class must have access to the data members mentioned above (i.e. they either have to be public, or this class is declared as friend).
 *
 * \sa FEASolver
 * \sa fmesher::FMesher
 */
template<class ProblemDataT>
class FemmReader
{

// Attributes
public:
    using ProblemData_type = ProblemDataT;

    /**
     * @brief FemmReader constructor.
     * @param dataObject the object where data is stored.
     * @param err an output stream where error messages should be written to
     */
    FemmReader( ProblemData_type &dataObject, std::ostream &err = std::cerr);
    virtual ~FemmReader();

    /**
     * @brief Parse the given file and store the data in the associated data object.
     * @param file
     * @return
     */
    bool parse(const std::string &file);

    /**
     * @brief setHandleTokenFunction sets a handler function for otherwise unhandled tokens.
     * \see handleToken
     */
    void setHandleTokenFunction(bool (*handlerFun)(const std::string &, std::istream &, std::ostream &));
private:
    /// \brief Reference to the data object
    ProblemData_type &m_data;
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
    bool handleTokenDummy(const std::string &token, std::istream &input, std::ostream &err) { return false; }
};

} //namespace

#endif
