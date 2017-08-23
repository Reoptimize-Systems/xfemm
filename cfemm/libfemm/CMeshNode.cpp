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
#include "CMeshNode.h"

#include <cstdlib>
#include <cmath>
#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femmsolver;

// CMeshNode construction
CMeshNode::CMeshNode()
    : x(0.)
    , y(0.)
    , msk(0)
{
}

CComplex CMeshNode::CC()
{
    return (x+I*y);
}

double CMeshNode::GetDistance(double xo, double yo)
{
    return sqrt((x-xo)*(x-xo) + (y-yo)*(y-yo));
}

CMMeshNode::CMMeshNode()
    : CMeshNode()
    , A(0)
{
}

CHMeshNode::CHMeshNode()
    : CMeshNode()
    , T(0)
    , Q(0)
{
}

CSMeshNode::CSMeshNode()
    : CMeshNode()
    , Q(false)
    , IsSelected(false)
{
}
