#include <cstdlib>
#include <cmath>
#include "problem.h"
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

#define ElementsPerSkinDepth 10

using namespace std;

//////////////////////        CPointProp      /////////////////////////////
CPointProp::CPointProp()
{
    PointName = "New Point Property";
    Jr=0.; Ji=0.;                    // applied point current, A
    Ar=0.; Ai=0.;                    // prescribed nodal value;
}

//////////////////////         CCircuit       /////////////////////////////
CCircuit::CCircuit()
{
    CircName = "New Circuit";
    CircType = 0;
    Amps = 0.;
}
