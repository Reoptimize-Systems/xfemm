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

#include "CBlockLabel.h"

#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"
#include "stringTools.h"

#include <cassert>
#include <cmath>
#include <ctype.h>
#include <istream>
#include <sstream>

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femm;

// CBlockLabel construction
CBlockLabel::CBlockLabel()
    : x(0.)
    , y(0.)
    , MaxArea(0.)
    , InGroup(0)
    , IsExternal(false)
    , IsDefault(false)
    , IsSelected(false)
    , BlockType(-1)
    , InCircuit(-1)
    , BlockTypeName("<No Mesh>")
    , InCircuitName("<None>")
{
}

void CBlockLabel::ToggleSelect()
{
    IsSelected = !IsSelected;
}

double CBlockLabel::GetDistance(double xo, double yo)
{
    return sqrt((x-xo)*(x-xo) + (y-yo)*(y-yo));
}

bool CBlockLabel::hasBlockType() const
{
    return ( (BlockTypeName != "<No Mesh>") && (BlockTypeName != "<Inf>") );
}

bool CBlockLabel::isHole() const
{
    //return (BlockTypeName == "<No Mesh>");
    return (BlockType == -1);
}

bool CBlockLabel::isInCircuit() const
{
    return (InCircuit!=-1);
}


CMBlockLabel::CMBlockLabel()
    : CBlockLabel()
    , ProximityMu(0)
    , Turns(1)
    , bIsWound(false)
    , MagDir(0.0)
    , MagDirFctn()
    , Case(0)
    , J(0.)
    , dVolts(0.)
    , FillFactor(1)
    , o(0.)
    , mu(0.)
{
}

CMBlockLabel CMBlockLabel::fromStream(istream &input, ostream &)
{
    std::string line;
    // read whole line to prevent reading from the next line if a line is malformed/too short
    std::getline(input, line);
    trim(line);
    std::istringstream inputStream(line);

#ifdef DEBUG_PARSER
    std::cerr << "Reading line: " << line <<"\n";
#endif

    CMBlockLabel prop;
    // scan in data
    inputStream >> prop.x;
    inputStream >> prop.y;
    inputStream >> prop.BlockType;
    prop.BlockType--;
    inputStream >> prop.MaxArea;
    if (prop.MaxArea<=0)
        prop.MaxArea = 0;
    else
        prop.MaxArea *= PI * prop.MaxArea / 4.;
    inputStream >> prop.InCircuit;
    prop.InCircuit--;
    inputStream >> prop.MagDir;
    inputStream >> prop.InGroup;
    inputStream >> prop.Turns;

    int extDefault = 0;
    inputStream >> extDefault;
    // second last bit in extDefault flag, we mask the other bits
    // and take the resulting value, if not zero it will evaluate to true
    prop.IsDefault  = extDefault & 2;
    // last bit in extDefault flag, we mask the other bits
    // and take the resulting value, if not zero it will evaluate to true
    prop.IsExternal = extDefault & 1;

    // MagDirFctn is an extra field not formally described in the .fem file format spec
    parseString(inputStream, &prop.MagDirFctn);

    return prop;
}

void CMBlockLabel::toStream(ostream &out) const
{
    int extDefault = 0;
    if (IsExternal)
        extDefault |= 0x01;
    if (IsDefault)
        extDefault |= 0x02;

    out << x << "\t" << y
        << "\t" << (BlockType+1)
        << "\t" << ((MaxArea>0) ? sqrt(4.*MaxArea/PI) : -1)
        << "\t" << (InCircuit+1)
        << "\t" << MagDir
        << "\t" << InGroup
        << "\t" << Turns
        << "\t" << extDefault;
    if (!MagDirFctn.empty())
        out << "\t\"" << MagDirFctn << "\"";
    out << "\n";
}

std::unique_ptr<CBlockLabel> CMBlockLabel::clone() const
{
    return std::unique_ptr<CMBlockLabel>(new CMBlockLabel(*this));
}


ostream &operator<<(ostream &os, const CBlockLabel &lbl)
{
    lbl.toStream(os);
    return os;
}

CHBlockLabel::CHBlockLabel()
    : CBlockLabel()
{
}

CHBlockLabel CHBlockLabel::fromStream(istream &input, ostream &)
{
    CHBlockLabel prop;

    // scan in data
    input >> prop.x;
    input >> prop.y;
    input >> prop.BlockType;
    prop.BlockType--;
    input >> prop.MaxArea;
    if (prop.MaxArea<=0)
        prop.MaxArea = 0;
    else
        prop.MaxArea *= PI * prop.MaxArea / 4.;
    input >> prop.InGroup;

    int extDefault;
    input >> extDefault;
    // second last bit in extDefault flag, we mask the other bits
    // and take the resulting value, if not zero it will evaluate to true
    prop.IsDefault  = extDefault & 2;
    // last bit in extDefault flag, we mask the other bits
    // and take the resulting value, if not zero it will evaluate to true
    prop.IsExternal = extDefault & 1;

    return prop;
}

void CHBlockLabel::toStream(ostream &out) const
{
    int extDefault = 0;
    if (IsExternal)
        extDefault |= 0x01;
    if (IsDefault)
        extDefault |= 0x02;

    out << x << "\t" << y
        << "\t" << (BlockType+1)
        << "\t" << ((MaxArea>0) ? sqrt(4.*MaxArea/PI) : -1)
        << "\t" << InGroup
        << "\t" << extDefault
        <<"\n";
}

std::unique_ptr<CBlockLabel> CHBlockLabel::clone() const
{
    return std::unique_ptr<CHBlockLabel>(new CHBlockLabel(*this));
}

CSBlockLabel::CSBlockLabel()
    : CBlockLabel()
{

}

CSBlockLabel CSBlockLabel::fromStream(istream &input, ostream &)
{
    std::string line;
    // read whole line to prevent reading from the next line if a line is malformed/too short
    std::getline(input, line);
    trim(line);
    std::istringstream inputStream(line);

#ifdef DEBUG_PARSER
    std::cerr << "Reading line: " << line <<"\n";
#endif

    CSBlockLabel prop;
    // scan in data
    inputStream >> prop.x;
    inputStream >> prop.y;
    inputStream >> prop.BlockType;
    prop.BlockType--;
    // BlockTypeName is set by FemmProblem::updateLabelsFromIndex()
    inputStream >> prop.MaxArea;
    if (prop.MaxArea<=0)
        prop.MaxArea = 0;
    else
        prop.MaxArea *= PI * prop.MaxArea / 4.;
    inputStream >> prop.InGroup;

    int extDefault = 0;
    inputStream >> extDefault;
    // second last bit in extDefault flag, we mask the other bits
    // and take the resulting value, if not zero it will evaluate to true
    prop.IsDefault  = extDefault & 2;
    // last bit in extDefault flag, we mask the other bits
    // and take the resulting value, if not zero it will evaluate to true
    prop.IsExternal = extDefault & 1;

    return prop;
}

void CSBlockLabel::toStream(ostream &out) const
{
    int extDefault = 0;
    if (IsExternal)
        extDefault |= 0x01;
    if (IsDefault)
        extDefault |= 0x02;

    out << x << "\t" << y
        << "\t" << (BlockType+1)
        << "\t" << ((MaxArea>0) ? sqrt(4.*MaxArea/PI) : -1)
        << "\t" << InGroup
        << "\t" << extDefault;
    out << "\n";
}

std::unique_ptr<CBlockLabel> CSBlockLabel::clone() const
{
    return std::unique_ptr<CSBlockLabel>(new CSBlockLabel(*this));
}
