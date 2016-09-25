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

}

CMBoundaryProp::CMBoundaryProp()
    : CBoundaryProp()
{
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

CHBoundaryProp::CHBoundaryProp()
    : CBoundaryProp(),
      Tset(0.0),
      Tinf(0.0),
      qs(0.0),
      beta(0.0),
      h(0.0)
{

}
