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
