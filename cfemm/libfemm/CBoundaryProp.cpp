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
        Johannes Zarl-Zierl
   Contact:
        szelitzkye@gmail.com
        sztibi82@yahoo.com
        richard.crozier@yahoo.co.uk
        johannes@zarl-zierl.at

 Contributions by Johannes Zarl-Zierl were funded by Linz Center of
 Mechatronics GmbH (LCM)
*/

#include "CBoundaryProp.h"

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
    // rotor angles (for building air gap element)
    InnerAngle = 0.;
    OuterAngle = 0.;
}

bool CMBoundaryProp::isPeriodic(PeriodicityType pt) const
{
    if (pt==PeriodicityType::Any || pt==PeriodicityType::Periodic)
    {
        if (BdryFormat==4 || BdryFormat==6)
        {
            return true;
        }
    }

    if (pt==PeriodicityType::Any || pt==PeriodicityType::AntiPeriodic)
    {
        if (BdryFormat==5 || BdryFormat==7)
        {
            return true;
        }
    }
    return false;
}

CMBoundaryProp CMBoundaryProp::fromStream(std::istream &input, std::ostream &err)
{
    CMBoundaryProp prop;

    if( expectToken(input, "<beginbdry>", err) )
    {
        string token;
        while (input.good() && token != "<endbdry>")
        {
            nextToken(input,&token);
            if( token == "<bdrytype>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.BdryFormat, err);
                continue;
            }

            if( token == "<mu_ssd>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.Mu, err);
                continue;
            }

            if( token == "<sigma_ssd>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.Sig, err);
                continue;
            }

            if( token == "<a_0>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.A0, err);
                continue;
            }

            if( token == "<a_1>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.A1, err);
                continue;
            }

            if( token == "<a_2>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.A2, err);
                continue;
            }

            if( token == "<phi>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.phi, err);
                continue;
            }

            if( token == "<c0>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.c0.re, err);
                continue;
            }

            if( token == "<c1>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.c1.re, err);
                continue;
            }

            if( token == "<c0i>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.c0.im, err);
                continue;
            }

            if( token == "<c1i>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.c1.im, err);
                continue;
            }

            if( token == "<innerangle>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.InnerAngle, err);
                continue;
            }

            if( token == "<outerangle>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.OuterAngle, err);
                continue;
            }

            if( token == "<bdryname>" )
            {
                expectChar(input, '=', err);
                parseString(input, &prop.BdryName);
                continue;
            }

            if ( token != "<endbdry>")
                err << "CMBoundaryProp: unexpected token: "<<token << "\n";
        }
    }

    return prop;
}

void CMBoundaryProp::toStream(ostream &out) const
{
    out << "  <BeginBdry>\n";
    out << "    <BdryName> = \"" << BdryName << "\"\n";
    out << "    <BdryType> = " << BdryFormat << "\n";
    out << "    <A_0> = " << A0 << "\n";
    out << "    <A_1> = " << A1 << "\n";
    out << "    <A_2> = " << A2 << "\n";
    out << "    <Phi> = " << phi << "\n";
    out << "    <c0> = " << c0.re << "\n";
    out << "    <c0i> = " << c0.im << "\n";
    out << "    <c1> = " << c1.re << "\n";
    out << "    <c1i> = " << c1.im << "\n";
    out << "    <Mu_ssd> = " << Mu << "\n";
    out << "    <Sigma_ssd> = " << Sig << "\n";
    out << "    <innerangle> = " << InnerAngle << "\n";
    out << "    <outerangle> = " << OuterAngle << "\n";
    out << "  <EndBdry>\n";
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

bool CHBoundaryProp::isPeriodic(PeriodicityType pt) const
{
    if (pt==PeriodicityType::Any || pt==PeriodicityType::Periodic)
        if (BdryFormat==4)
            return true;
    if (pt==PeriodicityType::Any || pt==PeriodicityType::AntiPeriodic)
        if (BdryFormat==5)
            return true;
    return false;
}

CHBoundaryProp CHBoundaryProp::fromStream(istream &input, ostream &err)
{
    CHBoundaryProp prop;

    if( expectToken(input, "<beginbdry>", err) )
    {
        string token;
        while (input.good() && token != "<endbdry>")
        {
            nextToken(input,&token);
            if( token == "<bdrytype>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.BdryFormat, err);
                continue;
            }

            if( token == "<tset>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.Tset, err);
                continue;
            }

            if( token == "<qs>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.qs, err);
                continue;
            }

            if( token == "<beta>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.beta, err);
                continue;
            }

            if( token == "<h>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.h, err);
                continue;
            }

            if( token == "<tinf>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.Tinf, err);
                continue;
            }

            if( token == "<bdryname>" )
            {
                expectChar(input, '=', err);
                parseString(input, &prop.BdryName);
                continue;
            }

            if ( token != "<endbdry>")
                err << "CHBoundaryProp: unexpected token: "<<token << "\n";
        }
    }

    return prop;
}

void CHBoundaryProp::toStream(ostream &out) const
{
    out << "  <BeginBdry>\n";
    out << "    <BdryType> = " << BdryFormat << "\n";
    out << "    <Tset> = " << Tset << "\n";
    out << "    <qs> = " << qs << "\n";
    out << "    <beta> = " << beta << "\n";
    out << "    <h> = " << h << "\n";
    out << "    <Tinf> = " << Tinf << "\n";
    out << "    <BdryName> = \"" << BdryName << "\"\n";
    out << "  <EndBdry>\n";
}



ostream &operator<<(ostream &os, const CBoundaryProp &prop)
{
    prop.toStream(os);
    return os;
}

CSBoundaryProp::CSBoundaryProp()
    : CBoundaryProp()
    , V(0)
    , c0(0)
    , c1(0)
    , qs(0)
{
}

bool CSBoundaryProp::isPeriodic(PeriodicityType pt) const
{
    if (pt==PeriodicityType::Any || pt==PeriodicityType::Periodic)
        if (BdryFormat==3)
            return true;
    if (pt==PeriodicityType::Any || pt==PeriodicityType::AntiPeriodic)
        if (BdryFormat==4)
            return true;
    return false;
}

CSBoundaryProp CSBoundaryProp::fromStream(istream &input, ostream &err)
{
    CSBoundaryProp prop;

    if( expectToken(input, "<beginbdry>", err) )
    {
        string token;
        while (input.good() && token != "<endbdry>")
        {
            nextToken(input,&token);

            if( token == "<bdryname>" )
            {
                expectChar(input, '=', err);
                parseString(input, &prop.BdryName);
                continue;
            }

            if( token == "<bdrytype>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.BdryFormat, err);
                continue;
            }

            if( token == "<vs>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.V, err);
                continue;
            }

            if( token == "<qs>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.qs, err);
                continue;
            }

            if( token == "<c0>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.c0, err);
                continue;
            }

            if( token == "<c1>" )
            {
                expectChar(input, '=', err);
                parseValue(input, prop.c1, err);
                continue;
            }

            if ( token != "<endbdry>")
                err << "CSBoundaryProp: unexpected token: "<<token << "\n";
        }
    }

    return prop;
}

void CSBoundaryProp::toStream(ostream &out) const
{
    out << "  <BeginBdry>\n";
    out << "    <BdryName> = \"" << BdryName << "\"\n";
    out << "    <BdryType> = " << BdryFormat << "\n";
    out << "    <Vs> = " << V << "\n";
    out << "    <qs> = " << qs << "\n";
    out << "    <c0> = " << c0 << "\n";
    out << "    <c1> = " << c1 << "\n";
    out << "  <EndBdry>\n";
}
