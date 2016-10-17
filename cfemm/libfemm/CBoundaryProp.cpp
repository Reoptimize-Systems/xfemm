#include "CBoundaryProp.h"

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

using namespace std;
using namespace femm;

CBoundaryProp::CBoundaryProp()
    : BdryName("New Boundary")
    , BdryFormat(0)
{
}

CMBoundaryProp::CMBoundaryProp()
    : CBoundaryProp()
{
    // set value of A for BdryFormat = 0;
    A0 = 0.;
    A1 = 0.;
    A2 = 0.;
    phi = 0.;
    // material properties necessary to apply eddy current BC
    Sig = 0.;
    Mu = 0.;
    // coefficients for mixed BC
    c0 = 0.;
    c1 = 0.;
}

CMBoundaryProp CMBoundaryProp::fromStream(std::istream &input, std::ostream &err)
{
    CMBoundaryProp prop;

    if( parseToken(input, "<beginbdry>", err) )
    {
        string token;
        while (input.good() && token != "<endbdry>")
        {
            nextToken(input,&token);
            if( token == "<bdrytype>" )
            {
                expectChar(input, '=', err);
                input >> prop.BdryFormat;
                continue;
            }

            if( token == "<mu_ssd>" )
            {
                expectChar(input, '=', err);
                input >> prop.Mu;
                continue;
            }

            if( token == "<sigma_ssd>" )
            {
                expectChar(input, '=', err);
                input >> prop.Sig;
                continue;
            }

            if( token == "<a_0>" )
            {
                expectChar(input, '=', err);
                input >> prop.A0;
                continue;
            }

            if( token == "<a_1>" )
            {
                expectChar(input, '=', err);
                input >> prop.A1;
                continue;
            }

            if( token == "<a_2>" )
            {
                expectChar(input, '=', err);
                input >> prop.A2;
                continue;
            }

            if( token == "<phi>" )
            {
                expectChar(input, '=', err);
                input >> prop.phi;
                continue;
            }

            if( token == "<c0>" )
            {
                expectChar(input, '=', err);
                input >> prop.c0.re;
                continue;
            }

            if( token == "<c1>" )
            {
                expectChar(input, '=', err);
                input >> prop.c1.re;
                continue;
            }

            if( token == "<c0i>" )
            {
                expectChar(input, '=', err);
                input >> prop.c0.im;
                continue;
            }

            if( token == "<c1i>" )
            {
                expectChar(input, '=', err);
                input >> prop.c1.im;
                continue;
            }
            err << "\nUnexpected token: "<<token;
        }
    }

    return prop;
}

CHBoundaryProp::CHBoundaryProp()
    : CBoundaryProp(),
      Tset(0.0),
      Tinf(0.0),
      qs(0.0),
      beta(0.0),
      h(0.0)
{

}

CHBoundaryProp CHBoundaryProp::fromStream(istream &input, ostream &err)
{
    CHBoundaryProp prop;

    if( parseToken(input, "<beginbdry>", err) )
    {
        string token;
        while (input.good() && token != "<endbdry>")
        {
            nextToken(input,&token);
            if( token == "<bdrytype>" )
            {
                expectChar(input, '=', err);
                input >> prop.BdryFormat;
                continue;
            }

            if( token == "<tset>" )
            {
                expectChar(input, '=', err);
                input >> prop.Tset;
                continue;
            }

            if( token == "<qs>" )
            {
                expectChar(input, '=', err);
                input >> prop.qs;
                continue;
            }

            if( token == "<beta>" )
            {
                expectChar(input, '=', err);
                input >> prop.beta;
                continue;
            }

            if( token == "<h>" )
            {
                expectChar(input, '=', err);
                input >> prop.h;
                continue;
            }

            if( token == "<tinf>" )
            {
                expectChar(input, '=', err);
                input >> prop.Tinf;
                continue;
            }

            err << "\nUnexpected token: "<<token;
        }
    }

    return prop;
}


