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
#include "CNode.h"

#include "fullmatrix.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"

#include <cstdlib>
#include <cmath>

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femm;

// CNode construction
femm::CNode::CNode()
    : x(0.)
    , y(0.)
    , InGroup(0)
    , IsSelected(false)
    , BoundaryMarker(-1)
    , InConductor(-1)
    , BoundaryMarkerName("<None>")
    , InConductorName("<None>")
{
}

CNode::CNode(double x, double y)
    : x(x)
    , y(y)
    , InGroup(0)
    , IsSelected(false)
    , BoundaryMarker(-1)
    , InConductor(-1)
    , BoundaryMarkerName("<None>")
    , InConductorName("<None>")
{
}

CNode::~CNode()
{
}

CComplex femm::CNode::CC() const
{
    return CComplex(x,y);
}

double femm::CNode::GetDistance(double xo, double yo)
{
    return sqrt((x-xo)*(x-xo) + (y-yo)*(y-yo));
}

void femm::CNode::ToggleSelect()
{
    IsSelected = ! IsSelected;
}

std::unique_ptr<CNode> CNode::clone() const
{
    return std::unique_ptr<CNode>(new CNode(*this));
}

bool CNode::hasBoundaryMarker() const
{
    return (BoundaryMarker!=-1);
}

bool CNode::isInConductor() const
{
    return (InConductor!=-1);
}

