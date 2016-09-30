#include "CSegment.h"


using namespace std;
using namespace femm;

// CSegment construction
CSegment::CSegment()
    : n0(0)
    , n1(0)
    , MaxSideLength(-1)
    , Hidden(false)
    , InGroup(0)
    , IsSelected(false)
{
}

void CSegment::ToggleSelect()
{
    IsSelected = !IsSelected;
}

CSolverSegment::CSolverSegment()
    : CSegment()
    , BoundaryMarker(-1)
    , InConductor(-1)
{
}

CMesherSegment::CMesherSegment()
    : CSegment()
    , BoundaryMarker("<None>")
    , InConductor("<None>")
    , selectFlag(0)
{
}
