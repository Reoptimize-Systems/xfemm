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
#include <utility>
#include <algorithm>
#include "nosebl.h"

namespace femm
{


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
    selectFlag = 0;
    BlockType = "<None>";
    InGroup = 0;

    // problem specific properties
    InCircuit = "<None>";

}

void CBlockLabel::ToggleSelect()
{
    if (selectFlag!=0)
    {
        selectFlag=0;
    }
    else
    {
        selectFlag = 1;
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



} // namespace femme
