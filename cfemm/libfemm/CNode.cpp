#include "CNode.h"

#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"

#include <cstdlib>
#include <cmath>

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femm;

// CNode construction
femm::CNode::CNode()
    : x(0.)
    , y(0.)
    , InGroup(0)
    , IsSelected(false)
    , BoundaryMarker(-1)
    , InConductor(-1)
    , BoundaryMarkerName("<None>")
    , InConductorName("<None>")
{
}

CNode::CNode(double x, double y)
    : x(x)
    , y(y)
    , InGroup(0)
    , IsSelected(false)
    , BoundaryMarker(-1)
    , InConductor(-1)
    , BoundaryMarkerName("<None>")
    , InConductorName("<None>")
{
}

CNode::~CNode()
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

