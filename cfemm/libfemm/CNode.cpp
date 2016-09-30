#include "CNode.h"

#include <cstdlib>
#include <cmath>
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femm;

// CNode construction
femm::CNode::CNode()
    : x(0.)
    , y(0.)
    , InGroup(0)
    , IsSelected(false)
{
}

CComplex femm::CNode::CC()
{
    return CComplex(x,y);
}

double femm::CNode::GetDistance(double xo, double yo)
{
    return sqrt((x-xo)*(x-xo) + (y-yo)*(y-yo));
}

void femm::CNode::ToggleSelect()
{
    IsSelected = ! IsSelected;
}

CSolverNode::CSolverNode()
    : CNode()
    , BoundaryMarker(-1)
    , InConductor(-1)
{
}


CMesherNode::CMesherNode()
    : CNode()
    , BoundaryMarker("<None>")
    , InConductor("<None>")
    , selectFlag(0)
{
}
