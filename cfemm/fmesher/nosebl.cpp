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

   Date Modified: 2017
   By: Richard Crozier
       Johannes Zarl-Zierl
   Contact:
        richard.crozier@yahoo.co.uk
        johannes.zarl-zierl@jku.at

   Contributions by Johannes Zarl-Zierl were funded by
   Linz Center of Mechatronics GmbH (LCM)
*/

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include <utility>
#include <algorithm>
#include "nosebl.h"

namespace fmesher
{

//*************************************************************************//
//                                                                         //
//                    CPeriodicBoundary definitions                        //
//                                                                         //
//*************************************************************************//

CPeriodicBoundary::CPeriodicBoundary()
{
    BdryName="";
    BdryFormat=0;
    antiPeriodic=false;
    nseg=0;
    narc=0;
    seg[0]=0;
    seg[1]=0;
}

std::unique_ptr<CPeriodicBoundary> CPeriodicBoundary::clone() const
{
    return std::unique_ptr<CPeriodicBoundary>(new CPeriodicBoundary(*this));
}

} // namespace femme
