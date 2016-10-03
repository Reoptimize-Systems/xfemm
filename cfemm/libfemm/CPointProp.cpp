#include "CPointProp.h"

#include <cstdlib>
#include <cmath>
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femm;

CPointProp::CPointProp()
    : PointName ( "New Point Property")
    , J()
    , A()
{
}

