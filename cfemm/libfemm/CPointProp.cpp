#include "CPointProp.h"

#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"

#include <iostream>

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femm;

CPointProp::CPointProp()
    : PointName ( "New Point Property")
    , J()
    , A()
{
}

CPointProp CPointProp::fromStream(istream &input, ostream &err)
{
    CPointProp prop;

    if( expectToken(input, "<beginpoint>", err) )
    {
        string token;
        while (input.good() && token != "<endpoint>")
        {
            nextToken(input, &token);

            if( token == "<a_re>" )
            {
                expectChar(input, '=', err);
                input >> prop.A.re;
                continue;
            }

            if( token == "<a_im>" )
            {
                expectChar(input, '=', err);
                input >> prop.A.im;
                continue;
            }

            if( token == "<i_re>" )
            {
                expectChar(input, '=', err);
                input >> prop.J.re;
                continue;
            }

            if( token == "<i_im>" )
            {
                expectChar(input, '=', err);
                input >> prop.J.im;
                continue;
            }
            if ( token != "<endpoint>" )
                err << "\nUnexpected token: "<<token;
        }
    }

    return prop;
}

void CPointProp::toStream(ostream &out) const
{
    out << "<BeginPoint>\n";
    out << "<A_re> = " << A.re << "\n";
    out << "<A_im> = " << A.im << "\n";
    out << "<I_re> = " << J.re << "\n";
    out << "<I_im> = " << J.im << "\n";
    out << "<EndPoint>\n";
}


ostream &operator<<(ostream &os, const CPointProp &prop)
{
    prop.toStream(os);
    return os;
}
