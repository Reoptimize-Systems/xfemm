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
    , MagDir(0.0)
    , InGroup(0)
    , Turns(1)
    , IsExternal(false)
    , IsSelected(false)
    , BlockType(-1)
    , InCircuit(-1)
    , BlockTypeName("<No Mesh>")
    , InCircuitName("<None>")
{
}

CBlockLabel::CBlockLabel(double X, double Y)
    :CBlockLabel()
{
    x = X;
    y = Y;
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
    return (BlockType!=-1);
}

bool CBlockLabel::isInCircuit() const
{
    return (InCircuit!=-1);
}


CBlockLabel CBlockLabel::fromStream(istream &input, ostream &)
{
    CBlockLabel prop;

    // scan in data
    input >> prop.x;
    input >> prop.y;
    input >> prop.BlockType;
    prop.BlockType--;
    input >> prop.MaxArea;
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

void CBlockLabel::toStream(ostream &out) const
{
    int extDefault = 0;
    if (IsExternal)
        extDefault |= 0x01;
    if (IsDefault)
        extDefault |= 0x02;

    out << x << " " << y << " " << (BlockType+1) << " " << MaxArea << " " << InGroup << " " << extDefault <<"\n";
}

CMBlockLabel::CMBlockLabel()
    : CBlockLabel()
    , ProximityMu(0)
    , bIsWound(false)
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

    int extDefault;
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
        << "\t" << sqrt(4.*MaxArea/PI)
        << "\t" << (InCircuit+1)
        << "\t" << MagDir
        << "\t" << InGroup
        << "\t" << Turns
        << "\t" << extDefault;
    if (!MagDirFctn.empty())
        out << "\t\"" << MagDirFctn << "\"";
    out << "\n";
}

ostream &operator<<(ostream &os, const CBlockLabel &lbl)
{
    lbl.toStream(os);
    return os;
}
