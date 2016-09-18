#include "CCircuit.h"

#include <cstdlib>
#include <cmath>
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

#define ElementsPerSkinDepth 10

using namespace femm;
CCircuit::CCircuit()
{
    CircName = "New Circuit";
    CircType = 0;
    Amps = 0.;
}
