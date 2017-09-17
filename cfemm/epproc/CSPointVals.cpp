/* Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by
 * Linz Center of Mechatronics GmbH (LCM)
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */
#include "CSPointVals.h"

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
