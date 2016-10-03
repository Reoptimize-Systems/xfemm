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

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

#include <vector>
#include <string>
#include "femmcomplex.h"

namespace femm
{


/////////////////////////////////////////////////////////////////////////////
// CPeriodicBoundary -- class that holds information on periodic boundaries

class CPeriodicBoundary
{
public:

    CPeriodicBoundary();

    std::string BdryName;
    int BdryFormat;            // 0 = Periodic
    // 1 = Antiperiodic
    int nseg;                // number of segs with this bc
    int narc;                // number of arcs with this bc
    int seg[2];                // (arc)segments to which is applied

private:
};


} // namespace femm

