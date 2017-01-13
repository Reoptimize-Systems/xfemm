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

#include "FemmReader.h"

#include "fparse.h"

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
          , class NodeT
          >
FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::FemmReader( std::shared_ptr<FemmProblem> problem, std::ostream &err)
    : problem(problem)
    , ignoreUnhandled(false)
    , err(err)
{
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::~FemmReader()
{
    // nothing to do
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
bool FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::parse(const std::string &file)
{
    std::ifstream input;

    input.open(file.c_str(), std::ifstream::in);
    if (!input.is_open())
    {
        err << "Couldn't read from specified .fem file\n";
        return false;
    }

    // parse the file

    string token;
    bool success = true;
    while (input.good() && success)
    {
        nextToken(input, &token);
        if (input.eof())
            break;

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

            if( token == "planar" ) problem->ProblemType=PLANAR;
            else if( token == "axisymmetric" ) problem->ProblemType=AXISYMMETRIC;
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

        // Option to force use of default max mesh, overriding
        // user choice
        if( token == "[forcemaxmesh]")
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, problem->DoForceMaxMeshArea, err);
            continue;
        }

        // Point Properties
        if( token == "[pointprops]" )
        {
            int k;
            success &= expectChar(input, '=', err);
            success &= parseValue(input, k, err);
            if (k>0) problem->nodeproplist.reserve(k);

            while (input.good() && (int)problem->nodeproplist.size() < k)
            {
                std::unique_ptr<PointPropT> next;
                next = std::make_unique<PointPropT>(std::move(PointPropT::fromStream(input, err)));
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

            while (input.good() && (int)problem->lineproplist.size() < k)
            {
                std::unique_ptr<BoundaryPropT> next;
                next = std::make_unique<BoundaryPropT>(std::move(BoundaryPropT::fromStream(input, err)));
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

            while (input.good() && (int)problem->blockproplist.size() < k)
            {
                std::unique_ptr<BlockPropT> next;
                next = std::make_unique<BlockPropT>(std::move(BlockPropT::fromStream(input, err)));
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

            while (input.good() && (int)problem->circproplist.size() < k)
            {
                std::unique_ptr<CircuitPropT> next;
                next = std::make_unique<CircuitPropT>(std::move(CircuitPropT::fromStream(input, err)));
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
            if (k>0) problem->labellist.reserve(k);

            while (input.good() && (int)problem->labellist.size() < k)
            {
                std::unique_ptr<BlockLabelT> next;
                next = std::make_unique<BlockLabelT>(std::move(BlockLabelT::fromStream(input, err)));
                problem->labellist.push_back(std::move(next));
            }
            // message will be printed after parsing is done
            if ((int)problem->labellist.size() != k)
            {
                err << "Expected "<<k<<" BlockLabels, but got " << problem->labellist.size() << "\n";
                break; // stop parsing
            }
            continue;
        }

        if(token == "[numpoints]"
                || token == "[numsegments]"
                || token == "[numarcsegments]"
                || token == "[numholes]"
                )
        {
            success &= expectChar(input, '=', err);
            // the nodes in the .fem file seem to be unused, and
            // the original femm code does not even read them here
            // -> just skip the lines here:
            int i;
            success &= parseValue(input, i, err);
            for (std::string line; i>0; i--)
                std::getline(input,line);
            // FIXME(ZaJ): this will probably need fixing
            continue;
        }

        // fall-through; token was not used
        if (!handleToken(token, input, err))
        {
            err << "Unknown token: " << token << "\n";
            success = false;
            if (!ignoreUnhandled) {
                // stop parsing:
                break;
            }
        }
    }
    return success;
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
bool FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::ignoreUnhandledTokens() const
{
    return ignoreUnhandled;
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
void FemmReader<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::setIgnoreUnhandledTokens(bool value)
{
    ignoreUnhandled = value;
}


// template instantiation
template class FemmReader<
        femm::CPointProp
        , femm::CMBoundaryProp
        , femm::CMMaterialProp
        , femm::CMCircuit
        , femm::CMBlockLabel
        , femm::CNode
        >;

MagneticsReader::MagneticsReader(std::shared_ptr<FemmProblem> problem, ostream &errorpipe)
    : FemmReader_type(problem, errorpipe)
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
