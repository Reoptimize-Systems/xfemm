#include "CArcSegment.h"

#include <cstdlib>
#include <cmath>
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femm;

// CArcSegment construction
CArcSegment::CArcSegment()
    : n0(0)
    , n1(0)
    , ArcLength(90.)
    , MaxSideLength(-1)
    , Hidden(false)
    , InGroup(0)
    , IsSelected(false)
{
}

void CArcSegment::ToggleSelect()
{
    IsSelected = !IsSelected;
}


CMesherArcSegment::CMesherArcSegment()
    : CArcSegment()
    , BoundaryMarker("<None>")
    , InConductor("<None>")
    , selectFlag(0)
    , NormalDirection(true)
{
}

CSolverArcSegment::CSolverArcSegment()
    : CArcSegment()
    , BoundaryMarker(-1)
    , InConductor(-1)
{
}
