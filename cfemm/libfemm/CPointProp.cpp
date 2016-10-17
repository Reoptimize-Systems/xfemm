#include "CPointProp.h"

#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"

#include <algorithm>
#include <ctype.h>
#include <istream>

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femm;

CPointProp::CPointProp()
    : PointName ( "New Point Property")
    , J()
    , A()
{
}

CPointProp *CPointProp::fromStream(istream &input, ostream &err)
{
    CPointProp *prop = NULL;

    if( parseToken(input, "<beginpoint>", err) )
    {
        string token;
        prop = new CPointProp;
        while (input.good() && token != "<endpoint>")
        {
            nextToken(input, &token);

            if( token == "<a_re>" )
            {
                expectChar(input, '=', err);
                input >> prop->A.re;
                continue;
            }

            if( token == "<a_im>" )
            {
                expectChar(input, '=', err);
                input >> prop->A.im;
                continue;
            }

            if( token == "<i_re>" )
            {
                expectChar(input, '=', err);
                input >> prop->J.re;
                continue;
            }

            if( token == "<i_im>" )
            {
                expectChar(input, '=', err);
                input >> prop->J.im;
                continue;
            }
            err << "\nUnexpected token: "<<token;
        }
    }

    return prop;
}

