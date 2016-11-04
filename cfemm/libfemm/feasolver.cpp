/*
   This code is a modified version of an algorithm
   forming part of the software program Finite
   Element Method Magnetics (FEMM), authored by
   David Meeker. The original software code is
   subject to the Aladdin Free Public Licence
   version 8, November 18, 1999. For more information
   on FEMM see www.femm.info. This modified version
   is not endorsed in any way by the original
   authors of FEMM.

   This software has been modified to use the C++
   standard template libraries and remove all Microsoft (TM)
   MFC dependent code to allow easier reuse across
   multiple operating system platforms.

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

// fsolver.cpp : implementation of the FSolver class
//

#include "femmcomplex.h"
#include "mesh.h"
#include "spars.h"
#include "fparse.h"
#include "feasolver.h"

#include <ctype.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifndef _MSC_VER
#define _strnicmp strncasecmp
#endif

#define STOP_ON_UNKNOWN_TOKEN

using namespace std;
using namespace femm;

/////////////////////////////////////////////////////////////////////////////
// FEASolver construction/destruction

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
FEASolver<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::FEASolver()
    : FileFormat(-1)
    , Precision(1.e-08)
    , MinAngle(0.)
    , Depth(-1)
    , LengthUnits(LengthInches)
    , Coords(CART)
    , ProblemType(PLANAR)
    , extZo(0.0)
    , extRo(0.0)
    , extRi(0.0)
    , comment()
    , ACSolver(0)
    , DoForceMaxMeshArea(false)
    , bMultiplyDefinedLabels(false)
    , BandWidth(0)
    , meshele()
    , NumNodes(0)
    , NumEls(0)
    , NumBlockProps(0)
    , NumPBCs(0)
    , NumLineProps(0)
    , NumPointProps(0)
    , NumCircProps(0)
    , NumBlockLabels(0)
    , pbclist()
    , PathName()
    , nodeproplist()
    , lineproplist()
    , blockproplist()
    , circproplist()
    , labellist()
    , nodes()
{

    // initialise the warning message box function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
FEASolver<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::~FEASolver()
{
}
template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
void FEASolver<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::CleanUp()
{
    FileFormat = -1;
    Precision = 1.e-08;
    MinAngle = 0.;
    Depth = -1;
    LengthUnits = LengthInches;
    Coords = CART;
    ProblemType = PLANAR;
    extZo = 0.0;
    extRo = 0.0;
    extRi = 0.0;
    comment.clear();
    ACSolver = 0;
    DoForceMaxMeshArea = false;
    bMultiplyDefinedLabels = false;
    BandWidth = 0;
    meshele.clear();
    NumNodes = 0;
    NumEls = 0;
    NumBlockProps = 0;
    NumPBCs = 0;
    NumLineProps = 0;
    NumPointProps = 0;
    NumCircProps = 0;
    NumBlockLabels = 0;
    pbclist.clear();
    PathName.clear();

    nodeproplist.clear();
    lineproplist.clear();
    blockproplist.clear();
    circproplist.clear();
    labellist.clear();
    nodes.clear();
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
bool FEASolver<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::LoadProblemFile(std::string &file)
{
    std::ifstream input;
    std::stringstream err;
    err >> noskipws; // don't discard whitespace from message stream

    input.open(file.c_str(), std::ifstream::in);
    if (!input.is_open())
    {
        printf("Couldn't read from specified .fem file\n");
        return false;
    }

    // define some defaults
    CleanUp();

    // parse the file

    string token;
    while (input.good())
    {
        nextToken(input, &token);

        if( token == "[format]")
        {
            expectChar(input, '=',err);
            parseValue(input, FileFormat, err);
            continue;
        }

        // Precision
        if( token == "[precision]")
        {
            expectChar(input, '=', err);
            parseValue(input, Precision, err);
            continue;
        }

        if( token == "[minangle]")
        {
            expectChar(input, '=',err);
            parseValue(input, MinAngle, err);
            continue;
        }

        // Depth for 2D planar problems;
        if( token == "[depth]")
        {
            expectChar(input, '=',err);
            parseValue(input, Depth, err);
            continue;
        }

        // Units of length used by the problem
        if( token == "[lengthunits]")
        {
            expectChar(input, '=', err);
            nextToken(input, &token);

            if( token == "inches" ) LengthUnits=LengthInches;
            else if( token == "millimeters" ) LengthUnits=LengthMillimeters;
            else if( token == "centimeters" ) LengthUnits=LengthCentimeters;
            else if( token == "mils" ) LengthUnits=LengthMils;
            else if( token == "microns" ) LengthUnits=LengthMicrometers;
            else if( token == "meters" ) LengthUnits=LengthMeters;
            continue;
        }

        // Coordinates (cartesian or polar)
        if( token == "[coordinates]" )
        {
            expectChar(input, '=', err);
            nextToken(input, &token);

            if ( token == "cartesian" ) Coords=CART;
            if ( token == "polar" ) Coords=POLAR;
            continue;
        }

        // Problem Type (planar or axisymmetric)
        if( token == "[problemtype]" )
        {
            expectChar(input, '=', err);
            nextToken(input, &token);

            if( token == "planar" ) ProblemType=PLANAR;
            if( token == "axisymmetric" ) ProblemType=AXISYMMETRIC;
            continue;
        }

        // properties for axisymmetric external region
        if( token == "[extzo]" )
        {
            expectChar(input, '=', err);
            parseValue(input, extZo, err);
            continue;
        }

        if( token == "[extro]" )
        {
            expectChar(input, '=', err);
            parseValue(input, extRo, err);
            continue;
        }

        if( token == "[extri]" )
        {
            expectChar(input, '=', err);
            parseValue(input, extRi, err);
            continue;
        }


        if( token == "[comment]" )
        {
            expectChar(input, '=', err);
            parseString(input, &comment, err);
            continue;
        }

        // AC Solver Type
        if( token == "[acsolver]")
        {
            expectChar(input, '=', err);
            parseValue(input, ACSolver, err);
            continue;
        }

        // Option to force use of default max mesh, overriding
        // user choice
        if( token == "[forcemaxmesh]")
        {
            expectChar(input, '=', err);
            parseValue(input, DoForceMaxMeshArea, err);
            continue;
        }

        // Point Properties
        if( token == "[pointprops]" )
        {
            int k;
            expectChar(input, '=', err);
            parseValue(input, k, err);
            if (k>0) nodeproplist.reserve(k);
            while (input.good() && NumPointProps < k)
            {
                PointPropT next = PointPropT::fromStream(input, err);
                nodeproplist.push_back(next);
                NumPointProps++;
            }
            // message will be printed after parsing is done
            if (NumPointProps != k)
            {
                err << "Expected "<<k<<" PointProps, but got " << NumPointProps << "\n";
                break; // stop parsing
            }
            continue;
        }


        // Boundary Properties;
        if( token == "[bdryprops]" )
        {
            expectChar(input, '=', err);
            int k;
            parseValue(input, k, err);
            if (k>0) lineproplist.reserve(k);

            while (input.good() && NumLineProps < k)
            {
                BoundaryPropT next = BoundaryPropT::fromStream(input, err);
                lineproplist.push_back(next);
                NumLineProps++;
            }
            // message will be printed after parsing is done
            if (NumLineProps != k)
            {
                err << "Expected "<<k<<" BoundaryProps, but got " << NumLineProps << "\n";
                break; // stop parsing
            }
            continue;
        }


        // Block Properties;
        if( token == "[blockprops]" )
        {
            expectChar(input, '=', err);
            int k;
            parseValue(input, k, err);
            if (k>0) blockproplist.reserve(k);

            while (input.good() && NumBlockProps < k)
            {
                BlockPropT next = BlockPropT::fromStream(input, err);
                blockproplist.push_back(next);
                NumBlockProps++;
            }
            // message will be printed after parsing is done
            if (NumBlockProps != k)
            {
                err << "Expected "<<k<<" BlockProps, but got " << NumBlockProps << "\n";
                break; // stop parsing
            }
            continue;
        }

        // Circuit Properties
        if( token == "[circuitprops]" || token == "[conductorprops]")
        {
            expectChar(input, '=', err);
            int k;
            parseValue(input, k, err);
            if(k>0) circproplist.reserve(k);

            while (input.good() && NumCircProps < k)
            {
                CircuitPropT next = CircuitPropT::fromStream(input, err);
                circproplist.push_back(next);
                NumCircProps++;
            }
            // message will be printed after parsing is done
            if (NumCircProps != k)
            {
                err << "Expected "<<k<<" CircuitProps, but got " << NumCircProps << "\n";
                break; // stop parsing
            }
            continue;
        }


        // read in regional attributes
        if(token == "[numblocklabels]" )
        {
            expectChar(input, '=', err);
            int k;
            parseValue(input, k, err);
            if (k>0) labellist.reserve(k);
            NumBlockLabels=k;
            for(int i=0; i<k; i++)
            {
                BlockLabelT blk = BlockLabelT::fromStream(input,err);

                labellist.push_back(blk);
            }
            continue;
        }
        // fall-through; token was not used
        if (!handleToken(token, input, err))
        {
            err << "Unknown token: " << token << "\n";
#ifdef STOP_ON_UNKNOWN_TOKEN
            // stop parsing:
            break;
#endif
        }
    }
    if (input.fail() || ! input.eof())
    {
        string msg = "Parse error while reading input file " + file + "!\n";
        msg += "Last token was: " + token +"\n";
        msg += err.str();
        WarnMessage(msg.c_str());
        return false;
    }

    if (!err.str().empty())
    {
        WarnMessage(err.str().c_str());
    }

    return true;
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
bool FEASolver<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::handleToken(const string &, istream &, ostream &)
{
    // token not handled
    return false;
}

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
void FEASolver<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>
::toStream(ostream &os) const
{
    os << "[Format] = " << FileFormat << "\n";
    os << "[Precision] = " << Precision << "\n";
    os << "[MinAngle] = " << MinAngle << "\n";
    os << "[Depth] = " << Depth << "\n";
    os << "[LengthUnits] = " << (int)LengthUnits << "\n";
    os << "[ProblemType] = " << (int)ProblemType << "\n";
    os << "[Coordinates] = " << (int)Coords << "\n";
    os << "[Comment] = \"" << comment <<"\"\n";

    os << "[PointProps] = " << nodeproplist.size() <<"\n";
    for( const auto &prop: nodeproplist)
    {
        prop.toStream(os);
    }

    os << "[BdryProps] = " << lineproplist.size() <<"\n";
    for( const auto &prop: lineproplist)
    {
        prop.toStream(os);
    }

    os << "[BlockProps] = " << blockproplist.size() <<"\n";
    for( const auto &prop: blockproplist)
    {
        prop.toStream(os);
    }

    // FIXME: get section header from CircuitPropT
    os << "[ConductorProps] = " << circproplist.size() <<"\n";
    for( const auto &prop: circproplist)
    {
        prop.toStream(os);
    }

    os << "[NumBlockLabels] = " << labellist.size() <<"\n";
    for( const auto &prop: labellist)
    {
        prop.toStream(os);
    }
}



