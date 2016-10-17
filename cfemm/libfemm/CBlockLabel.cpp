#include "CBlockLabel.h"

#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"

#include <cmath>
#include <ctype.h>
#include <istream>

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



CSolverBlockLabel::CSolverBlockLabel()
    : CBlockLabel()
    , BlockType(-1)
    , InCircuit(0)
{
}

CSolverBlockLabel *CSolverBlockLabel::fromStream(istream &input, ostream &err)
{
    CSolverBlockLabel *prop = new CSolverBlockLabel;
    prop->initFromStream(input,err);

    return prop;
}

void CSolverBlockLabel::initFromStream(istream &input, ostream &)
{
    // scan in data
    input >> x;
    input >> y;
    input >> BlockType;
    BlockType--;
    input >> MaxArea;
    input >> InCircuit;
    InCircuit--;
    input >> MagDir;
    input >> InGroup;
    input >> Turns;

    int extDefault;
    input >> extDefault;
    // second last bit in extDefault flag, we mask the other bits
    // and take the resulting value, if not zero it will evaluate to true
    IsDefault  = extDefault & 2;
    // last bit in extDefault flag, we mask the other bits
    // and take the resulting value, if not zero it will evaluate to true
    IsExternal = extDefault & 1;

    // MagDirFctn is an extra field not formally described in the .fem file format spec
    ParseString(input, &MagDirFctn);
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

CMSolverBlockLabel *CMSolverBlockLabel::fromStream(istream &input, ostream &err)
{
    CMSolverBlockLabel *prop = new CMSolverBlockLabel;

    prop->initFromStream(input,err);

    return prop;
}
