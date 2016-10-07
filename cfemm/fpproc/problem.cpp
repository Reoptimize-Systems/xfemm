#include <cstdlib>
#include <cmath>
#include "problem.h"
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

#define ElementsPerSkinDepth 10

using namespace std;

//////////////////////         CCircuit       /////////////////////////////
CCircuit::CCircuit()
{
    CircName = "New Circuit";
    CircType = 0;
    Amps = 0.;
}
