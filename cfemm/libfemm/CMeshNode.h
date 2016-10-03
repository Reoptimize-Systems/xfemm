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

class CHMeshNode : public CMeshNode
{
public:
    CHMeshNode();

    double T;  ///< temperature at the node
    /**
     * @brief Q boundary flag
     * Indicates whether the node lies on a boundary, or in a conductor, or none.
     */
    int Q;
};
}
#endif
