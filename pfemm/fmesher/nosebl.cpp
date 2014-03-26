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

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include "nosebl.h"

namespace femm
{


//*************************************************************************//
//                                                                         //
//                           CNode definitions                             //
//                                                                         //
//*************************************************************************//


CNode::CNode()
{
    x = 0.;
    y = 0.;
    IsSelected = 0;
    InGroup = 0;
    BoundaryMarker = "<None>";
    InConductor="<None>";
}

double CNode::GetDistance(double xo, double yo)
{
    return std::sqrt((x-xo)*(x-xo) + (y-yo)*(y-yo));
}

CComplex CNode::CC()
{
    return CComplex(x,y);
}

void CNode::ToggleSelect()
{
    if (IsSelected!=0)
    {
        IsSelected=0;
    }
    else
    {
        IsSelected = 1;
    }
}

//*************************************************************************//
//                                                                         //
//                         CSegment definitions                            //
//                                                                         //
//*************************************************************************//


CSegment::CSegment()
{
    n0 = 0;
    n1 = 0;
    IsSelected = 0;
    Hidden = false;
    MaxSideLength = -1;
    BoundaryMarker = "<None>";
    InConductor="<None>";
    InGroup = 0;
}

void CSegment::ToggleSelect()
{
    if (IsSelected!=0)
    {
        IsSelected=0;
    }
    else
    {
        IsSelected = 1;
    }
}

//*************************************************************************//
//                                                                         //
//                       CArcSegment definitions                           //
//                                                                         //
//*************************************************************************//


CArcSegment::CArcSegment()
{
    n0 = 0;
    n1 = 0;
    IsSelected = 0;
    Hidden = false;
    ArcLength = 90.;
    MaxSideLength = 10.;
    BoundaryMarker = "<None>";
    InConductor="<None>";
    InGroup = 0;
    NormalDirection = true;
}

void CArcSegment::ToggleSelect()
{
    if (IsSelected!=0)
    {
        IsSelected=0;
    }
    else
    {
        IsSelected = 1;
    }
}

//*************************************************************************//
//                                                                         //
//                       CBlockLabel definitions                           //
//                                                                         //
//*************************************************************************//

CBlockLabel::CBlockLabel()
{
    // common properties
    x = 0.;
    y = 0.;
    MaxArea = 0.;
    IsSelected = 0;
    BlockType = "<None>";
    InGroup = 0;

    // problem specific properties
    InCircuit = "<None>";

}

void CBlockLabel::ToggleSelect()
{
    if (IsSelected!=0)
    {
        IsSelected=0;
    }
    else
    {
        IsSelected = 1;
    }
}

double CBlockLabel::GetDistance(double xo, double yo)
{
    return std::sqrt((x-xo)*(x-xo) + (y-yo)*(y-yo));
}


//*************************************************************************//
//                                                                         //
//                      CBoundaryProp definitions                          //
//                                                                         //
//*************************************************************************//


CBoundaryProp::CBoundaryProp()
{
    BdryName = "New Boundary";
    BdryFormat = 0;                // type of boundary condition we are applying
    // 0 = constant value of A
    // 1 = Small skin depth eddy current BC
    // 2 = Mixed BC

}

//*************************************************************************//
//                                                                         //
//                        CPointProp definitions                           //
//                                                                         //
//*************************************************************************//

CPointProp::CPointProp()
{
    PointName = "New Point Property";
}

//*************************************************************************//
//                                                                         //
//                         CCircuit definitions                            //
//                                                                         //
//*************************************************************************//


CCircuit::CCircuit()
{
    CircName = "New Circuit";
    CircType=1;
};


//*************************************************************************//
//                                                                         //
//                    CPeriodicBoundary definitions                        //
//                                                                         //
//*************************************************************************//

CPeriodicBoundary::CPeriodicBoundary()
{
    BdryName="";
    BdryFormat=0;
    nseg=0;
    narc=0;
    seg[0]=0;
    seg[1]=0;
}


//*************************************************************************//
//                                                                         //
//                       CCommonPoint definitions                          //
//                                                                         //
//*************************************************************************//


CCommonPoint::CCommonPoint()
{
    x=y=t=0;
}

void CCommonPoint::Order()
{
    int z;

    if(x>y)
    {
        z=y;
        y=x;
        x=z;
    }
}

} // namespace femme
