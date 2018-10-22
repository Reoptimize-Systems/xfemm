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

   Date Modified: 2017
   By: Richard Crozier
       Johannes Zarl-Zierl
   Contact:
	    richard.crozier@yahoo.co.uk
       johannes@zarl-zierl.at

   Contributions by Johannes Zarl-Zierl were funded by
	Linz Center of Mechatronics GmbH (LCM)
*/
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
{
}

CPointProp::~CPointProp()
{
}

CMPointProp::CMPointProp()
    : CPointProp()
    , J()
    , A()
{
}

CMPointProp CMPointProp::fromStream(istream &input, ostream &err)
{
    CMPointProp prop;

    if( expectToken(input, "<beginpoint>", err) )
    {
        string token;
        while (input.good() && token != "<endpoint>")
        {
            nextToken(input, &token);

            if( token == "<pointname>" )
            {
                expectChar(input, '=', err);
                parseString(input, &prop.PointName, err);
                continue;
            }

            if( token == "<a_re>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.A.re, err);
                continue;
            }

            if( token == "<a_im>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.A.im, err);
                continue;
            }

            if( token == "<i_re>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.J.re, err);
                continue;
            }

            if( token == "<i_im>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.J.im, err);
                continue;
            }
            if ( token != "<endpoint>" )
                err << "CPointProp: unexpected token: "<<token << "\n";
        }
    }

    return prop;
}

void CMPointProp::toStream(ostream &out) const
{
    out << "  <BeginPoint>\n";
    out << "    <PointName> = \"" << PointName << "\"\n";
    out << "    <I_re> = " << J.re << "\n";
    out << "    <I_im> = " << J.im << "\n";
    out << "    <A_re> = " << A.re << "\n";
    out << "    <A_im> = " << A.im << "\n";
    out << "  <EndPoint>\n";
}


CHPointProp::CHPointProp()
    : CPointProp()
    , V(0)
    , qp(0)
{
}

CHPointProp CHPointProp::fromStream(std::istream &input, std::ostream &err)
{
    CHPointProp prop;

    if( expectToken(input, "<beginpoint>", err) )
    {
        string token;
        while (input.good() && token != "<endpoint>")
        {
            nextToken(input, &token);

            if( token == "<tp>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.V, err);
                continue;
            }

            if( token == "<qp>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.qp, err);
                continue;
            }
            if (token != "<endpoint>")
                err << "CHPointProp: unexpected token: "<<token << "\n";
        }
    }

    return prop;
}

void CHPointProp::toStream(std::ostream &out) const
{
    out << "  <BeginPoint>\n";
    out << "    <Tp> = " << V << "\n";
    out << "    <qp> = " << qp << "\n";
    out << "  <EndPoint>\n";
}

ostream &operator<<(ostream &os, const CMPointProp &prop)
{
    prop.toStream(os);
    return os;
}

CSPointProp::CSPointProp()
    : CPointProp()
    , V(0)
    , qp(0)
{
}

CSPointProp CSPointProp::fromStream(istream &input, ostream &err)
{
    CSPointProp prop;

    if( expectToken(input, "<beginpoint>", err) )
    {
        string token;
        while (input.good() && token != "<endpoint>")
        {
            nextToken(input, &token);

            if( token == "<vp>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.V, err);
                continue;
            }

            if( token == "<qp>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.qp, err);
                continue;
            }

            if( token == "<pointname>" )
            {
                expectChar(input, '=', err);
                parseString(input, &prop.PointName, err);
                continue;
            }
            if (token != "<endpoint>")
                err << "CSPointProp: unexpected token: "<<token << "\n";
        }
    }

    return prop;
}

void CSPointProp::toStream(ostream &out) const
{
    out << "  <BeginPoint>\n";
    out << "    <PointName> = \"" << PointName << "\"\n";
    out << "    <Vp> = " << V << "\n";
    out << "    <qp> = " << qp << "\n";
    out << "  <EndPoint>\n";
}
