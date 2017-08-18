#include "CPointVals.h"

femm::CMPointVals::CMPointVals()
    : A(0)
    , B1(0)
    , B2(0)
    , mu1(1)
    , mu2(1)
    , H1(0)
    , H2(0)
    , Je(0)
    , Js(0)
    , c(0)
    , E(0)
    , Ph(0)
    , Pe(0)
    , ff(1)
{
}


femm::CSPointVals::CSPointVals()
    : V(0)
    , D(0)
    , e(0)
    , E(0)
    , nrg(0)
{
}

femm::CHPointVals::CHPointVals()
    : T(0)
    , F(0)
    , K(0)
    , G(0)
{
}
