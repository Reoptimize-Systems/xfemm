#include "CElement.h"

femmsolver::CElement::CElement()
    : p{0,0,0}
    , e{0,0,0}
    , blk(0)
    , lbl(0)
    , ctr(0)
    , rsqr(0)
    , n{0,0,0}
{
}

femmsolver::CElement::~CElement()
{
}

femmsolver::CMElement::CMElement()
    : CElement()
    , mu1(0)
    , mu2(0)
    , B1(0)
    , B2(0)
    , b1{0,0,0}
    , b2{0,0,0}
    , magdir(0)
{
}

femmsolver::CMElement::~CMElement()
{
}

femmsolver::CHElement::CHElement()
    : CElement()
    , D(0)
    , d{0,0,0}
{
}
