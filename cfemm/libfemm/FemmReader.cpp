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

// this should normally be defined, but it can be useful for debugging to continue reading after parse errors
#define STOP_ON_UNKNOWN_TOKEN

using namespace std;
using namespace femm;

template<class ProblemDataT>
FemmReader<ProblemDataT>
::FemmReader(ProblemDataT &dataObject, std::ostream &err)
    : m_data(dataObject)
    , err(err)
    , handleToken(&handleTokenDummy)
{
}

template<class ProblemDataT>
FemmReader<ProblemDataT>
::~FemmReader()
{
    // nothing to do
}

template<class ProblemDataT>
bool FemmReader<ProblemDataT>
::parse(const std::string &file)
{
    // convenience:
    using PointPropT = typename ProblemData_type::PointProp_type;
    using BoundaryPropT = typename ProblemData_type::BoundaryProp_type;
    using BlockPropT = typename ProblemData_type::BlockProp_type;
    using CircuitPropT = typename ProblemData_type::CircuitProp_type;
    using BlockLabelT = typename ProblemData_type::BlockLabel_type;
    //using NodeT = typename ProblemData_type::Node_type;

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
            success &= parseValue(input, m_data.FileFormat, err);
            continue;
        }

        // Precision
        if( token == "[precision]")
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, m_data.Precision, err);
            continue;
        }

        if( token == "[minangle]")
        {
            success &= expectChar(input, '=',err);
            success &= parseValue(input, m_data.MinAngle, err);
            continue;
        }

        // Depth for 2D planar problems;
        if( token == "[depth]")
        {
            success &= expectChar(input, '=',err);
            success &= parseValue(input, m_data.Depth, err);
            continue;
        }

        // Units of length used by the problem
        if( token == "[lengthunits]")
        {
            success &= expectChar(input, '=', err);
            nextToken(input, &token);

            if( token == "inches" ) m_data.LengthUnits=LengthInches;
            else if( token == "millimeters" ) m_data.LengthUnits=LengthMillimeters;
            else if( token == "centimeters" ) m_data.LengthUnits=LengthCentimeters;
            else if( token == "mils" ) m_data.LengthUnits=LengthMils;
            else if( token == "microns" ) m_data.LengthUnits=LengthMicrometers;
            else if( token == "meters" ) m_data.LengthUnits=LengthMeters;
            else err << "Unknown length unit: " << token << "\n";
            continue;
        }

        // Coordinates (cartesian or polar)
        if( token == "[coordinates]" )
        {
            success &= expectChar(input, '=', err);
            nextToken(input, &token);

            if ( token == "cartesian" ) m_data.Coords=CART;
            else if ( token == "polar" ) m_data.Coords=POLAR;
            else err << "Unknown coordinate type: " << token << "\n";
            continue;
        }

        // Problem Type (planar or axisymmetric)
        if( token == "[problemtype]" )
        {
            success &= expectChar(input, '=', err);
            nextToken(input, &token);

            if( token == "planar" ) m_data.ProblemType=PLANAR;
            else if( token == "axisymmetric" ) m_data.ProblemType=AXISYMMETRIC;
            else err << "Unknown problem type:" << token << "\n";
            continue;
        }

        // properties for axisymmetric external region
        if( token == "[extzo]" )
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, m_data.extZo, err);
            continue;
        }

        if( token == "[extro]" )
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, m_data.extRo, err);
            continue;
        }

        if( token == "[extri]" )
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, m_data.extRi, err);
            continue;
        }


        if( token == "[comment]" )
        {
            success &= expectChar(input, '=', err);
            parseString(input, &(m_data.comment), err);
            continue;
        }

        // AC Solver Type
        if( token == "[acsolver]")
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, m_data.ACSolver, err);
            continue;
        }

        // Option to force use of default max mesh, overriding
        // user choice
        if( token == "[forcemaxmesh]")
        {
            success &= expectChar(input, '=', err);
            success &= parseValue(input, m_data.DoForceMaxMeshArea, err);
            continue;
        }

        // Point Properties
        if( token == "[pointprops]" )
        {
            int k;
            success &= expectChar(input, '=', err);
            success &= parseValue(input, k, err);
            if (k>0) m_data.nodeproplist.reserve(k);

            while (input.good() && (int)m_data.nodeproplist.size() < k)
            {
                PointPropT next = PointPropT::fromStream(input, err);
                m_data.nodeproplist.push_back(next);
            }
            m_data.NumPointProps = m_data.nodeproplist.size();
            // message will be printed after parsing is done
            if (m_data.NumPointProps != k)
            {
                err << "Expected "<<k<<" PointProps, but got " << m_data.NumPointProps << "\n";
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
            if (k>0) m_data.lineproplist.reserve(k);

            while (input.good() && m_data.NumLineProps < k)
            {
                BoundaryPropT next = BoundaryPropT::fromStream(input, err);
                m_data.lineproplist.push_back(next);
                m_data.NumLineProps++;
            }
            // message will be printed after parsing is done
            if (m_data.NumLineProps != k)
            {
                err << "Expected "<<k<<" BoundaryProps, but got " << m_data.NumLineProps << "\n";
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
            if (k>0) m_data.blockproplist.reserve(k);

            while (input.good() && m_data.NumBlockProps < k)
            {
                BlockPropT next = BlockPropT::fromStream(input, err);
                m_data.blockproplist.push_back(next);
                m_data.NumBlockProps++;
            }
            // message will be printed after parsing is done
            if (m_data.NumBlockProps != k)
            {
                err << "Expected "<<k<<" BlockProps, but got " << m_data.NumBlockProps << "\n";
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
            if(k>0) m_data.circproplist.reserve(k);

            while (input.good() && m_data.NumCircProps < k)
            {
                CircuitPropT next = CircuitPropT::fromStream(input, err);
                m_data.circproplist.push_back(next);
                m_data.NumCircProps++;
            }
            // message will be printed after parsing is done
            if (m_data.NumCircProps != k)
            {
                err << "Expected "<<k<<" CircuitProps, but got " << m_data.NumCircProps << "\n";
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
            if (m_data.NumBlockLabels>0) m_data.labellist.reserve(m_data.NumBlockLabels);
            while (input.good() && m_data.NumBlockLabels < k)
            {
                BlockLabelT blk = BlockLabelT::fromStream(input,err);
                m_data.labellist.push_back(blk);
                m_data.NumBlockLabels++;
            }
            // message will be printed after parsing is done
            if (m_data.NumBlockLabels != k)
            {
                err << "Expected "<<k<<" BlockLabels, but got " << m_data.NumBlockLabels << "\n";
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
#ifdef STOP_ON_UNKNOWN_TOKEN
            // stop parsing:
            break;
#endif
        }
    }
    return success;
}

template<class ProblemDataT>
void FemmReader<ProblemDataT>
::setHandleTokenFunction(bool (*handlerFun)(const std::__cxx11::string &, std::istream &, std::ostream &))
{
    handleToken = handlerFun;
}

