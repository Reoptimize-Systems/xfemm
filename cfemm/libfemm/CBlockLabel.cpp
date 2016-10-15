#include "CBlockLabel.h"

#include <cstdlib>
#include <cmath>
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

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
