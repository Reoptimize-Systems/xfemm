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
    , Amps()
{
}

void CCircuit::toStream(ostream &out) const
{
    out << "CCircuit without toStream implementation!\n";
    assert(false && "Circuit without toStream");
}

CMCircuit::CMCircuit()
    : CCircuit(),
      dVolts(),
      OrigCirc(0),
      J(),
      dV(),
      Case(0)
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
    out << "    <Voltgradient_re> = " << dVolts.re << "\n";
    out << "    <Voltgradient_im> = " << dVolts.im << "\n";
    out << "    <TotalAmps_re> = " << Amps.re << "\n";
    out << "    <TotalAmps_im> = " << Amps.im << "\n";
    out << "    <CircuitType> = " << CircType << "\n";
    out << "    <CircuitName> =\"" << CircName << "\"\n";
    out << "  <EndCircuit>\n";
}

ostream &operator<<(ostream &os, const CCircuit &prop)
{
    prop.toStream(os);
    return os;
}
