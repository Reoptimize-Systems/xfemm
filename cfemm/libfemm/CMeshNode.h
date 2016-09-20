#ifndef FEMM_CMESHNODE_H
#define FEMM_CMESHNODE_H

#include <string>
#include "femmcomplex.h"

namespace femm {

class CMeshNode
{
    public:
        CMeshNode();

        double x,y;
        CComplex A;
        double msk;
        int xs,ys;

        double GetDistance(double xo, double yo);
        CComplex CC();

    private:

};
}
#endif
