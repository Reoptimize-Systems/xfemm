#include "CBoundaryProp.h"

#include <cstdlib>
#include <cmath>
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

#define ElementsPerSkinDepth 10

using namespace femm;

CBoundaryProp::CBoundaryProp()
{
    BdryName = "New Boundary";

    BdryFormat = 0;    // type of boundary condition we are applying
    // 0 = constant value of A
    // 1 = Small skin depth eddy current BC
    // 2 = Mixed BC

    // set value of A for BdryFormat = 0;
    A0 = 0.;
    A1 = 0.;
    A2 = 0.;
    phi = 0.;
    // material properties necessary to apply eddy current BC
    Sig = 0.;
    Mu = 0.;
    // coefficients for mixed BC
    c0 = 0.;
    c1 = 0.;
}
