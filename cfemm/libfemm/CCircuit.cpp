#include "CCircuit.h"

#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"

#include <algorithm>
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

    if( parseToken(input, "<begincircuit>", err) )
    {
        string token;
        while (input.good() && token != "<endcircuit>")
        {
            nextToken(input, &token);

            if( token == "<voltgradient_re>" )
            {
                expectChar(input, '=', err);
                input >> prop.dVolts.re;
                continue;
            }

            if( token == "<voltgradient_im>" )
            {
                expectChar(input, '=', err);
                input >> prop.dVolts.im;
                continue;
            }

            if( token == "<totalamps_re>" )
            {
                expectChar(input, '=', err);
                input >> prop.Amps.re;
                continue;
            }

            if( token == "<totalamps_im>" )
            {
                expectChar(input, '=', err);
                input >> prop.Amps.im;
                continue;
            }

            if( token == "<circuittype>" )
            {
                expectChar(input, '=', err);
                input >> prop.CircType;
                continue;
            }
            err << "\nUnexpected token: "<<token;
        }
    }

    return prop;
}
