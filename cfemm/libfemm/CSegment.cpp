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
       johannes@zarl-zierl.at

   Contributions by Johannes Zarl-Zierl were funded by
	Linz Center of Mechatronics GmbH (LCM)
*/
#include "CSegment.h"


using namespace std;
using namespace femm;

// CSegment construction
CSegment::CSegment()
    : n0(0)
    , n1(0)
    , MaxSideLength(-1)
    , Hidden(false)
    , InGroup(0)
    , IsSelected(false)
    , BoundaryMarker(-1)
    , InConductor(-1)
    , BoundaryMarkerName("<None>")
    , InConductorName("<None>")
    , cnt(0)
{
}

void CSegment::ToggleSelect()
{
    IsSelected = !IsSelected;
}

std::unique_ptr<CSegment> CSegment::clone() const
{
    return std::unique_ptr<CSegment>(new CSegment(*this));
}

bool CSegment::hasBoundaryMarker() const
{
    return (BoundaryMarker!=-1);
}

bool CSegment::isInConductor() const
{
    return (InConductor!=-1);
}

