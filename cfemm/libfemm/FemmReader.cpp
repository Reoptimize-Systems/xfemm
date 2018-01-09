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

#include "FemmReader.h"

#include "fparse.h"
#include "stringTools.h"

#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>


using namespace std;
using namespace femm;

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          >
FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT>
::FemmReader(std::shared_ptr<FemmProblem> problem, std::ostream &err)
    : problem(problem)
    , solutionReader(nullptr)
    , ignoreUnhandled(false)
    , err(err)
{
    assert(problem);
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          >
FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT>
::FemmReader(std::shared_ptr<FemmProblem> problem, SolutionReader *r, std::ostream &errorpipe)
    : problem(problem)
    , solutionReader(r)
    , ignoreUnhandled(false)
    , err(errorpipe)
{
    assert(problem);
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          >
FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT>
::~FemmReader()
{
    // nothing to do
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          >
ParserResult FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT>
::parse(const std::string &file)
{
    std::ifstream input;
    std::streampos linestart;
    string line;

    input.open(file.c_str(), std::ifstream::in);
    if (!input.is_open())
    {
        err << "Couldn't read from file " << file<< "\n";
        return F_FILE_NOT_OPENED;
    }
    problem->pathName = file;

    // parse the file

#ifdef DEBUG_PARSER
    std::cout << "FemmReader starting parsing" << std::endl;
#endif // DEBUG_PARSER

    bool success = true;
    bool readSolutionData = false;
    while (input.good() && success)
    {
        if (!input.eof())
        {
            // store the current position so we can examine the line in the event of failure
            linestart = input.tellg();
            // store it
            std::getline(input, line);
            // go back to start of line
            input.seekg(linestart);
        }

        if (line.empty())
        {
#ifdef DEBUG_PARSER
            std::cout << "current line is empty, moving on" << std::endl;
#endif // DEBUG_PARSER
            continue;
        }

#ifdef DEBUG_PARSER
        std::cout << "current line reads:" << std::endl
                  << line << std::endl;
#endif // DEBUG_PARSER

        string token;

        if (input.eof())
        {
            success = true;
            break;
        }

        nextToken(input, &token);

        if( token == "[format]")
        {
            success &= expectChar(input, '=',err);
            success &= parseValue(input, problem->FileFormat, err);
            continue;
        }

        // Precision
        if( token == "[precision]")
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, problem->Precision, err);
            continue;
        }

        if( token == "[minangle]")
        {
            success &= expectChar(input, '=',err);
            success &= parseValue(input, problem->MinAngle, err);
            continue;
        }

        // Depth for 2D planar problems;
        if( token == "[depth]")
        {
            success &= expectChar(input, '=',err);
            success &= parseValue(input, problem->Depth, err);
            continue;
        }

        // Units of length used by the problem
        if( token == "[lengthunits]")
        {
            success &= expectChar(input, '=', err);
            nextToken(input, &token);

            if( token == "inches" ) problem->LengthUnits=LengthInches;
            else if( token == "millimeters" ) problem->LengthUnits=LengthMillimeters;
            else if( token == "centimeters" ) problem->LengthUnits=LengthCentimeters;
            else if( token == "mils" ) problem->LengthUnits=LengthMils;
            else if( token == "microns" ) problem->LengthUnits=LengthMicrometers;
            else if( token == "meters" ) problem->LengthUnits=LengthMeters;
            else err << "Unknown length unit: " << token << "\n";
            continue;
        }

        // Coordinates (cartesian or polar)
        if( token == "[coordinates]" )
        {
            success &= expectChar(input, '=', err);
            nextToken(input, &token);

            if ( token == "cartesian" ) problem->Coords=CART;
            else if ( token == "polar" ) problem->Coords=POLAR;
            else err << "Unknown coordinate type: " << token << "\n";
            continue;
        }

        // Problem Type (planar or axisymmetric)
        if( token == "[problemtype]" )
        {
            success &= expectChar(input, '=', err);
            nextToken(input, &token);

            if( token == "planar" ) problem->problemType=PLANAR;
            else if( token == "axisymmetric" ) problem->problemType=AXISYMMETRIC;
            else err << "Unknown problem type:" << token << "\n";
            continue;
        }

        // properties for axisymmetric external region
        if( token == "[extzo]" )
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, problem->extZo, err);
            continue;
        }

        if( token == "[extro]" )
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, problem->extRo, err);
            continue;
        }

        if( token == "[extri]" )
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, problem->extRi, err);
            continue;
        }


        if( token == "[comment]" )
        {
            success &= expectChar(input, '=', err);
            parseString(input, &(problem->comment), err);
            continue;
        }

        // AC Solver Type
        if( token == "[acsolver]")
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, problem->ACSolver, err);
            continue;
        }

        if( token == "[prevsoln]" )
        {
            expectChar(input, '=', err);
            parseString(input,&(problem->PrevSoln));
            continue;
        }

        // Option to force use of default max mesh, overriding
        // user choice
        if( token == "[forcemaxmesh]")
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, problem->DoForceMaxMeshArea, err);
            continue;
        }

        // Option to use smart meshing
        if( token == "[dosmartmesh]" )
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, problem->DoSmartMesh, err);
            continue;
        }

        // Point Properties
        if( token == "[pointprops]" )
        {
            int k;
            success &= expectChar(input, '=', err);
            success &= parseValue(input, k, err);
            if (k>0) problem->nodeproplist.reserve(k);

            while (input && (int)problem->nodeproplist.size() < k)
            {
                std::unique_ptr<PointPropT> next;
                next = std::make_unique<PointPropT>(PointPropT::fromStream(input, err));
                problem->nodeproplist.push_back(std::move(next));
            }
            // message will be printed after parsing is done
            if ((int)problem->nodeproplist.size() != k)
            {
                err << "Expected "<<k<<" PointProps, but got " << problem->nodeproplist.size() << "\n";
                break; // stop parsing
            }
            continue;
        }


        // Boundary Properties;
        if( token == "[bdryprops]" )
        {
            success &= expectChar(input, '=', err);
            int k;
            success &= parseValue(input, k, err);
            if (k>0) problem->lineproplist.reserve(k);

            while (input && (int)problem->lineproplist.size() < k)
            {
                std::unique_ptr<BoundaryPropT> next;
                next = std::make_unique<BoundaryPropT>(BoundaryPropT::fromStream(input, err));
                problem->lineproplist.push_back(std::move(next));
            }
            // message will be printed after parsing is done
            if ((int)problem->lineproplist.size() != k)
            {
                err << "Expected "<<k<<" BoundaryProps, but got " << problem->lineproplist.size() << "\n";
                break; // stop parsing
            }
            continue;
        }


        // Block Properties;
        if( token == "[blockprops]" )
        {
            success &= expectChar(input, '=', err);
            int k;
            success &= parseValue(input, k, err);
            if (k>0) problem->blockproplist.reserve(k);

            while (input && (int)problem->blockproplist.size() < k)
            {
                std::unique_ptr<BlockPropT> next;
                next = std::make_unique<BlockPropT>(BlockPropT::fromStream(input, err));
                problem->blockproplist.push_back(std::move(next));
            }
            // message will be printed after parsing is done
            if ((int)problem->blockproplist.size() != k)
            {
                err << "Expected "<<k<<" BlockProps, but got " << problem->blockproplist.size() << "\n";
                break; // stop parsing
            }
            continue;
        }

        // Circuit Properties
        if( token == "[circuitprops]" || token == "[conductorprops]")
        {
            success &= expectChar(input, '=', err);
            int k;
            success &= parseValue(input, k, err);
            if(k>0) problem->circproplist.reserve(k);

            while (input && (int)problem->circproplist.size() < k)
            {
                std::unique_ptr<CircuitPropT> next;
                next = std::make_unique<CircuitPropT>(CircuitPropT::fromStream(input, err));
                problem->circproplist.push_back(std::move(next));
            }
            // message will be printed after parsing is done
            if ((int)problem->circproplist.size() != k)
            {
                err << "Expected "<<k<<" CircuitProps, but got " << problem->circproplist.size() << "\n";
                break; // stop parsing
            }
            continue;
        }


        // read in regional attributes
        if(token == "[numblocklabels]" )
        {
            success &= expectChar(input, '=', err);
            int k;
            success &= parseValue(input, k, err);
            if (k>0) problem->labellist.reserve(problem->labellist.size()+k);

            // labellist contains both BlockLabels and holes. Therefore we can't use labellist.size:
            int num=0;
            while (input && num < k)
            {
                std::unique_ptr<BlockLabelT> next;
                next = std::make_unique<BlockLabelT>(BlockLabelT::fromStream(input, err));
                problem->labellist.push_back(std::move(next));
                num++;
            }
            // message will be printed after parsing is done
            if (num != k)
            {
                err << "Expected "<<k<<" BlockLabels, but got " << num << "\n";
                break; // stop parsing
            }
            continue;
        }
        if(token == "[numholes]" )
        {
            success &= expectChar(input, '=', err);
            int k;
            success &= parseValue(input, k, err);
            if (k>0 && !solutionReader) problem->labellist.reserve(problem->labellist.size()+k);

            // labellist contains both BlockLabels and holes. Therefore we can't use labellist.size:
            int num=0;
            // operate on a line-by-line level;
            std::string line;
            while (num < k && getline(input, line))
            {
                // holes are not relevant for the post processor -> skip them when reading the solution
                if (!solutionReader)
                {
                    size_t pos;
                    std::unique_ptr<BlockLabelT> label;
                    label = std::make_unique<BlockLabelT>();

                    trim(line);
                    label->x = std::stod(line, &pos);
                    line = line.substr(pos);
                    label->y = std::stod(line, &pos);
                    line = line.substr(pos);
                    label->InGroup = std::stoi(line, &pos);

                    problem->labellist.push_back(std::move(label));
                }
                num++;
            }
            // message will be printed after parsing is done
            if (num != k)
            {
                err << "Expected "<<k<<" holes, but got " << num << "\n";
                break; // stop parsing
            }
            continue;
        }

        if (token == "[numpoints]")
        {
            success &= expectChar(input, '=', err);
            int k;
            success &= parseValue(input, k, err);
            if (k>0) problem->nodelist.reserve(k);

            // operate on a line-by-line level;
            std::string line;
            while ((int)problem->nodelist.size() < k && getline(input, line))
            {
                size_t pos;
                std::unique_ptr<CNode> node;
                node = std::make_unique<CNode>();

                trim(line);
                node->x = std::stod(line, &pos);
                line = line.substr(pos);
                node->y = std::stod(line, &pos);
                line = line.substr(pos);
                node->BoundaryMarker = std::stoi(line, &pos);
                // correct for 1-based indexing:
                node->BoundaryMarker--;
                line = line.substr(pos);
                node->InGroup = std::stoi(line, &pos);
                line = line.substr(pos);

                if (problem->filetype == femm::FileType::HeatFlowFile ||
                        problem->filetype == femm::FileType::ElectrostaticsFile )
                {
                    node->InConductor = std::stoi(line, &pos);
                    // correct for 1-based indexing:
                    node->InConductor--;
                    line = line.substr(pos);
                }
                problem->nodelist.push_back(std::move(node));
            }
            // message will be printed after parsing is done
            if ((int)problem->nodelist.size() != k)
            {
                err << "Expected "<<k<<" points, but got " << problem->nodelist.size() << "\n";
                break; // stop parsing
            }
            continue;
        }
        if (token == "[numsegments]")
        {
            success &= expectChar(input, '=', err);
            int k;
            success &= parseValue(input, k, err);
            if (k>0) problem->linelist.reserve(k);

            // operate on a line-by-line level;
            std::string line;
            while ((int)problem->linelist.size() < k && getline(input, line))
            {
                size_t pos;
                std::unique_ptr<CSegment> segm;
                segm = std::make_unique<CSegment>();

                trim(line);
                segm->n0 = std::stoi(line, &pos);
                line = line.substr(pos);
                segm->n1 = std::stoi(line, &pos);
                line = line.substr(pos);
                segm->MaxSideLength = std::stod(line, &pos);
                line = line.substr(pos);
                segm->BoundaryMarker = std::stoi(line, &pos);
                // correct for 1-based indexing:
                segm->BoundaryMarker--;
                line = line.substr(pos);
                segm->Hidden = (0 != std::stoi(line, &pos));
                line = line.substr(pos);
                segm->InGroup = std::stoi(line, &pos);
                line = line.substr(pos);
                if (problem->filetype == femm::FileType::HeatFlowFile ||
                        problem->filetype == femm::FileType::ElectrostaticsFile )
                {
                    segm->InConductor = std::stoi(line, &pos);
                    // correct for 1-based indexing:
                    segm->InConductor--;
                    line = line.substr(pos);
                }
                problem->linelist.push_back(std::move(segm));
            }
            // message will be printed after parsing is done
            if ((int)problem->linelist.size() != k)
            {
                err << "Expected "<<k<<" segments, but got " << problem->linelist.size() << "\n";
                break; // stop parsing
            }
            continue;
        }
        if (token == "[numarcsegments]")
        {
            success &= expectChar(input, '=', err);
            int k;
            success &= parseValue(input, k, err);
            if (k>0) problem->arclist.reserve(k);

            // operate on a line-by-line level;
            std::string line;
            while ((int)problem->arclist.size() < k && getline(input, line))
            {
                size_t pos;
                std::unique_ptr<CArcSegment> asegm;
                asegm = std::make_unique<CArcSegment>();

                trim(line);
                asegm->n0 = std::stoi(line, &pos);
                line = line.substr(pos);
                asegm->n1 = std::stoi(line, &pos);
                line = line.substr(pos);
                asegm->ArcLength = std::stod(line, &pos);
                line = line.substr(pos);
                asegm->MaxSideLength = std::stod(line, &pos);
                line = line.substr(pos);
                asegm->BoundaryMarker = std::stoi(line, &pos);
                // correct for 1-based indexing:
                asegm->BoundaryMarker--;
                line = line.substr(pos);
                asegm->Hidden = (0 != std::stoi(line, &pos));
                line = line.substr(pos);
                asegm->InGroup = std::stoi(line, &pos);
                line = line.substr(pos);
                if (problem->filetype == femm::FileType::HeatFlowFile ||
                        problem->filetype == femm::FileType::ElectrostaticsFile )
                {
                    trim(line);
                    if (!line.empty())
                    {
                        asegm->InConductor = std::stoi(line, &pos);
                        // correct for 1-based indexing:
                        asegm->InConductor--;
                    }
                }
                problem->arclist.push_back(std::move(asegm));
            }
            // message will be printed after parsing is done
            if ((int)problem->arclist.size() != k)
            {
                err << "Expected "<<k<<" arc segments, but got " << problem->arclist.size() << "\n";
                break; // stop parsing
            }
            continue;
        }

        if (token == "[solution]")
        {
            // discard rest of current input line
            getline(input,token);
            readSolutionData = true;
            break;
        }

        // fall-through; token was not used
        if (!handleToken(token, input, err))
        {
            err << "Unknown token: " << token << "\n";
            success = false;
            if (!ignoreUnhandled) {
                err << "remaining input:\n";
                std::string s;
                while (getline(input,s))
                    err << s << "\n";
                // stop parsing:
                break;
            }
        }
    }
    // fill in text indices:
    problem->updateLabelsFromIndex();
    // build initial lookup maps (needed if problem is changed using lua)
    problem->updateBlockMap();
    problem->updateCircuitMap();
    problem->updateLineMap();
    problem->updateNodeMap();

    if (readSolutionData && success)
    {
        if (solutionReader)
            return solutionReader->parseSolution(input,err);
        else
            err << "Ignoring solution data...\n";
    } else {
        if (solutionReader)
        {
            err << "File contains no solution!\n";
            return F_FILE_MALFORMED;
        }
    }

    return success ? F_FILE_OK : F_FILE_MALFORMED;
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          >
bool FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT>
::ignoreUnhandledTokens() const
{
    return ignoreUnhandled;
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          >
void FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT>
::setIgnoreUnhandledTokens(bool value)
{
    ignoreUnhandled = value;
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          >
bool FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT>
::handleToken(const string &, istream &, ostream &)
{
    return false;
}


// template instantiation
template class femm::FemmReader<
        femm::CMPointProp
        , femm::CMBoundaryProp
        , femm::CMSolverMaterialProp
        , femm::CMCircuit
        , femm::CMBlockLabel
        >;

MagneticsReader::MagneticsReader(std::shared_ptr<FemmProblem> problem, ostream &errorpipe)
    : FemmReader_type(problem, errorpipe)
{
}

MagneticsReader::MagneticsReader(std::shared_ptr<FemmProblem> problem, SolutionReader *r, ostream &errorpipe)
    : FemmReader_type(problem,r,errorpipe)
{
}

bool MagneticsReader::handleToken(const string &token, istream &input, ostream &err)
{
    // Frequency of the problem
    if( token == "[frequency]")
    {
        expectChar(input, '=',err);
        parseValue(input, problem->Frequency, err);
        return true;
    }

    return false;
}

// template instantiation
template class femm::FemmReader<
        femm::CHPointProp
        , femm::CHBoundaryProp
        , femm::CHMaterialProp
        , femm::CHConductor
        , femm::CHBlockLabel
        >;

HeatFlowReader::HeatFlowReader(std::shared_ptr<FemmProblem> problem, ostream &errorpipe)
    : FemmReader_type(problem, errorpipe)
{
}

HeatFlowReader::HeatFlowReader(std::shared_ptr<FemmProblem> problem, SolutionReader *r, ostream &errorpipe)
    : FemmReader_type(problem,r,errorpipe)
{
}

bool HeatFlowReader::handleToken(const string &token, istream &input, ostream &err)
{
    if( token == "[dt]" )
    {
        expectChar(input, '=', err);
        parseValue(input, problem->dT, err);
        return true;
    }
    if( token == "[frequency]")
    {
        err << "Warning: [frequency] is not an allowed parameter for heat flow problems!\n";
        std::string line;
        std::getline(input, line); // ignore line
        return true;
    }
    return false;
}

// template instantiation
template class femm::FemmReader<
        femm::CSPointProp
        , femm::CSBoundaryProp
        , femm::CSMaterialProp
        , femm::CSCircuit
        , femm::CSBlockLabel
        >;

ElectrostaticsReader::ElectrostaticsReader(std::shared_ptr<FemmProblem> problem, ostream &errorpipe)
    : FemmReader_type(problem, errorpipe)
{
}

ElectrostaticsReader::ElectrostaticsReader(std::shared_ptr<FemmProblem> problem, SolutionReader *r, ostream &errorpipe)
    : FemmReader_type(problem,r,errorpipe)
{
}
