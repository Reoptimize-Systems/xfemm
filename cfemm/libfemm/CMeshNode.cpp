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

#include "femmcomplex.h"
#include "femmconstants.h"
#include "fullmatrix.h"
#include "stringTools.h"

#include <cstdlib>
#include <cmath>
#include <istream>
#include <sstream>

#define ElementsPerSkinDepth 10

using namespace std;
using namespace femmsolver;
using femm::trim;

// CMeshNode construction
CMeshNode::CMeshNode()
    : x(0.)
    , y(0.)
    , msk(0)
    , Q(0)
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
    Q = -2; // hack for PostProcessor::makeMask; Q is not used in magnetics problems
}

CMMeshNode CMMeshNode::fromStream(istream &input, ostream &)
{
    std::string line;
    // read whole line to prevent reading from the next line if a line is malformed/too short
    std::getline(input, line);
    trim(line);
    std::istringstream inputStream(line);

    CMMeshNode n;
    // scan in data
    inputStream >> n.x;
    inputStream >> n.y;
    inputStream >> n.A.re;
    inputStream >> n.A.im; // 4th field only applies when problem->Frequency is 0

    return n;
}

CHMeshNode::CHMeshNode()
    : CMeshNode()
    , T(0)
{
}

CHMeshNode CHMeshNode::fromStream(istream &input, ostream &)
{
    std::string line;
    // read whole line to prevent reading from the next line if a line is malformed/too short
    std::getline(input, line);
    trim(line);
    std::istringstream inputStream(line);

    CHMeshNode n;
    // scan in data
    inputStream >> n.x;
    inputStream >> n.y;
    inputStream >> n.T;
    inputStream >> n.Q;

    return n;
}

CSMeshNode::CSMeshNode()
    : CMeshNode()
    , IsSelected(false)
{
}

CSMeshNode CSMeshNode::fromStream(istream &input, ostream &)
{
    std::string line;
    // read whole line to prevent reading from the next line if a line is malformed/too short
    std::getline(input, line);
    trim(line);
    std::istringstream inputStream(line);

    CSMeshNode n;
    // scan in data
    inputStream >> n.x;
    inputStream >> n.y;
    inputStream >> n.V;
    inputStream >> n.Q;

    return n;
}
