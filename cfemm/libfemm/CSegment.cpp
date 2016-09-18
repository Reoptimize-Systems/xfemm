#include "CSegment.h"

#include <cstdlib>
#include <cmath>
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femm;

// CSegment construction
CSegment::CSegment()
{
    n0 = 0;
    n1 = 0;
    IsSelected = false;
    BoundaryMarker = -1;
}

void CSegment::ToggleSelect()
{
    if (IsSelected==true)
    {
        IsSelected = false;
    }
    else
    {
        IsSelected = true;
    }
}
