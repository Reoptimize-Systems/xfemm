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
{
    n0=0;
    n1=0;
    IsSelected=false;
    MaxSideLength=-1;
    ArcLength=90.;
    BoundaryMarker=-1;
}

void CArcSegment::ToggleSelect()
{
    IsSelected = !IsSelected;
}

