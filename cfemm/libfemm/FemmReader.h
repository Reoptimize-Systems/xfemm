/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of 
 * Mechatronics GmbH (LCM)
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

enum ParserResult { F_FILE_OK, F_FILE_UNKNOWN_TYPE, F_FILE_NOT_OPENED, F_FILE_MALFORMED};

/**
 * @brief The SolutionReader interface is used by FemmReader to parse solution data if the solution tag is encountered.
 */
class SolutionReader {
public:
    virtual ParserResult parseSolution( std::istream &input, std::ostream &err = std::cerr ) = 0;
protected:
    virtual ~SolutionReader(){}
};

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
          >
class FemmReader
{
// Attributes
public:
    using FemmReader_type = FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT>;
    using PointProp_type = PointPropT;
    using BoundaryProp_type = BoundaryPropT;
    using BlockProp_type = BlockPropT;
    using CircuitProp_type = CircuitPropT;
    using BlockLabel_type = BlockLabelT;

    /**
     * @brief FemmReader constructor
     * @param problem The FemmProblem that shall contain the data.
     * @param errorpipe
     */
    FemmReader(std::shared_ptr<FemmProblem> problem, std::ostream &errorpipe);
    /**
     * @brief FemmReader constructor for also reading solution data
     * @param problem The FemmProblem that shall contain the data.
     * @param r The SolutionReader to be called
     * @param errorpipe
     */
    FemmReader(std::shared_ptr<FemmProblem> problem, SolutionReader *r, std::ostream &errorpipe);
    virtual ~FemmReader();

    /**
     * @brief Parse the given file and store the data in the associated data object.
     * This also sets the PathName of the FemmProblem.
     *
     * If a SolutionReader was set, but no solution is encountered, \c F_FILE_MALFORMED is returned
     * If no SolutionReader was set, but a solution is encountered, a diagnostic is issued and parsing stops.
     * @param file
     * @return
     */
    ParserResult parse(const std::string &file);

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
    virtual bool handleToken(const std::string &token, std::istream &input, std::ostream &err);

    std::shared_ptr<FemmProblem> problem;
    SolutionReader *solutionReader;
private:
    bool ignoreUnhandled;
    /// \brief Reference to the error message stream
    std::ostream &err;
};

class MagneticsReader : public FemmReader<
        femm::CMPointProp
        , femm::CMBoundaryProp
        , femm::CMSolverMaterialProp
        , femm::CMCircuit
        , femm::CMBlockLabel
        >
{
public:
    MagneticsReader(std::shared_ptr<FemmProblem> problem, std::ostream &errorpipe);
    MagneticsReader(std::shared_ptr<FemmProblem> problem, SolutionReader *r, std::ostream &errorpipe);
protected:
    bool handleToken(const std::string &token, std::istream &input, std::ostream &err) override;
};

class HeatFlowReader : public FemmReader<
        femm::CHPointProp
        , femm::CHBoundaryProp
        , femm::CHMaterialProp
        , femm::CHConductor
        , femm::CHBlockLabel
        >
{
public:
    HeatFlowReader(std::shared_ptr<FemmProblem> problem, std::ostream &errorpipe);
    HeatFlowReader(std::shared_ptr<FemmProblem> problem, SolutionReader *r, std::ostream &errorpipe);
protected:
    bool handleToken(const std::string &token, std::istream &input, std::ostream &err) override;
};

class ElectrostaticsReader : public FemmReader<
        femm::CSPointProp
        , femm::CSBoundaryProp
        , femm::CSMaterialProp
        , femm::CSCircuit
        , femm::CSBlockLabel
        >
{
public:
    ElectrostaticsReader(std::shared_ptr<FemmProblem> problem, std::ostream &errorpipe);
    ElectrostaticsReader(std::shared_ptr<FemmProblem> problem, SolutionReader *r, std::ostream &errorpipe);
};
} //namespace

#endif
