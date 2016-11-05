#include "CBlockLabel.h"

#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"

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

void CBlockLabel::toStream(ostream &out) const
{
    out << "CBlockLabel without toStream implementation!\n";
    assert(false && "CBlockLabel without toStream");
}



CSolverBlockLabel::CSolverBlockLabel()
    : CBlockLabel()
    , BlockType(-1)
    , InCircuit(0)
{
}

CSolverBlockLabel CSolverBlockLabel::fromStream(istream &input, ostream &)
{
    CSolverBlockLabel prop;

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

void CSolverBlockLabel::toStream(ostream &out) const
{
    int extDefault = 0;
    if (IsExternal)
        extDefault |= 0x01;
    if (IsDefault)
        extDefault |= 0x02;

    out << x << y << (BlockType+1) << MaxArea << InGroup << extDefault <<"\n";
}

CMesherBlockLabel::CMesherBlockLabel()
    : CBlockLabel()
    , BlockType("<None>")
    , InCircuit("<None>")
{
}

CMSolverBlockLabel::CMSolverBlockLabel()
    : CSolverBlockLabel()
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

CMSolverBlockLabel CMSolverBlockLabel::fromStream(istream &input, ostream &)
{
    std::string line;
    // read whole line to prevent reading from the next line if a line is malformed/too short
    std::getline(input, line);
    trim(line);
    std::istringstream inputStream(line);

#ifdef DEBUG_PARSING
    std::cerr << "Reading line: " << line <<"\n";
#endif

    CMSolverBlockLabel prop;
    // scan in data
    inputStream >> prop.x;
    inputStream >> prop.y;
    inputStream >> prop.BlockType;
    prop.BlockType--;
    inputStream >> prop.MaxArea;
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

void CMSolverBlockLabel::toStream(ostream &out) const
{
    int extDefault = 0;
    if (IsExternal)
        extDefault |= 0x01;
    if (IsDefault)
        extDefault |= 0x02;

    out << x << y << (BlockType+1) << MaxArea << (InCircuit+1) << MagDir << InGroup << Turns << extDefault;
    if (!MagDirFctn.empty())
        out << "\"" << MagDirFctn << "\"";
    out << "\n";
}

ostream &operator<<(ostream &os, const CBlockLabel &lbl)
{
    lbl.toStream(os);
    return os;
}
