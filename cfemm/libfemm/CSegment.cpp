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
    , BoundaryMarker(-1)
    , InConductor(-1)
    , BoundaryMarkerName("<None>")
    , InConductorName("<None>")
    , cnt(0)
{
}

void CSegment::ToggleSelect()
{
    IsSelected = !IsSelected;
}

