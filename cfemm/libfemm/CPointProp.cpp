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
    , Jr(0.)
    , Ji(0.)
    , Ar(0.)
    , Ai(0.)
{
}

