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

   Date Modified: 2017 - 01 - 14
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
        Johannes Zarl-Zeril
   Contact:
        szelitzkye@gmail.com
        sztibi82@yahoo.com
        richard.crozier@yahoo.co.uk
        johannes@zarl-zierl.at
*/
#include "CCircuit.h"

#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctype.h>
#include <istream>

#define ElementsPerSkinDepth 10

using namespace femm;
using namespace std;

CCircuit::CCircuit()
    : CircName("New Circuit")
    , CircType(0)
{
}

void CCircuit::toStream(ostream &out) const
{
    out << "CCircuit without toStream implementation!\n";
    assert(false && "Circuit without toStream");
}

CMCircuit::CMCircuit()
    : CCircuit()
    , Amps()
    , dVolts()
    , OrigCirc(0)
    , J()
    , dV()
    , Case(0)
{
}

CMCircuit CMCircuit::fromStream(std::istream &input, std::ostream &err)
{
    CMCircuit prop;

    if( expectToken(input, "<begincircuit>", err) )
    {
        string token;
        while (input.good() && token != "<endcircuit>")
        {
            nextToken(input, &token);

            if( token == "<circuitname>" )
            {
                expectChar(input, '=', err);
                parseString(input, &prop.CircName, err);
                continue;
            }

            if( token == "<voltgradient_re>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.dVolts.re, err);
                continue;
            }

            if( token == "<voltgradient_im>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.dVolts.im, err);
                continue;
            }

            if( token == "<totalamps_re>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.Amps.re, err);
                continue;
            }

            if( token == "<totalamps_im>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.Amps.im, err);
                continue;
            }

            if( token == "<circuittype>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.CircType, err);
                continue;
            }
            if (token != "<endcircuit>")
                err << "CMCircuit: unexpected token: "<<token << "\n";
        }
    }

    return prop;
}

void CMCircuit::toStream(ostream &out) const
{
    out << "  <BeginCircuit>\n";
    out << "    <CircuitName> =\"" << CircName << "\"\n";
    out << "    <TotalAmps_re> = " << Amps.re << "\n";
    out << "    <TotalAmps_im> = " << Amps.im << "\n";
    out << "    <CircuitType> = " << CircType << "\n";
    out << "    <Voltgradient_re> = " << dVolts.re << "\n";
    out << "    <Voltgradient_im> = " << dVolts.im << "\n";
    out << "  <EndCircuit>\n";
}

CHConductor::CHConductor()
    : CCircuit()
    , V(0)
    , q(0)
{
}

CHConductor CHConductor::fromStream(std::istream &input, std::ostream &err)
{
    CHConductor prop;

    if( expectToken(input, "<beginconductor>", err) )
    {
        string token;
        while (input.good() && token != "<endconductor>")
        {
            nextToken(input, &token);

            if( token == "<tc>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.V, err);
                continue;
            }

            if( token == "<qc>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.q, err);
                continue;
            }

            if( token == "<conductortype>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.CircType, err);
                continue;
            }

            if ( token != "<endconductor>")
                err << "CHConductor: unexpected token: "<<token << "\n";
        }
    }

    return prop;
}

void CHConductor::toStream(std::ostream &out) const
{
    out << "<BeginConductor>\n";
    out << "<Tc>" << V << "\n";
    out << "<qc>" << q << "\n";
    out << "<ConductorType>" << CircType << "\n";
    if (!CircName.empty())
        out << "<ConductorName> =\"" << CircName << "\"\n";
    out << "<EndConductor>\n";
}
ostream &operator<<(ostream &os, const CCircuit &prop)
{
    prop.toStream(os);
    return os;
}
