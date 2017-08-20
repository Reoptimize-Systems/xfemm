/*
   This code is a modified version of an algorithm
   forming part of the software program Finite
   Element Method Magnetics (FEMM), authored by
   David Meeker. The original software code is
   subject to the Aladdin Free Public Licence
   version 8, November 18, 1999. For more information
   on FEMM see www.femm.info. This modified version
   is not endorsed in any way by the original
   authors of FEMM.

   This software has been modified to use the C++
   standard template libraries and remove all Microsoft (TM)
   MFC dependent code to allow easier reuse across
   multiple operating system platforms.

   Date Modified: 2014 - 03 - 21
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
   Contact:
        szelitzkye@gmail.com
        sztibi82@gmail.com
        richard.crozier@yahoo.co.uk
*/

#include "CMPointVals.h"

CMPointVals::CMPointVals()
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
