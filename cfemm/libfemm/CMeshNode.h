#ifndef FEMM_CMESHNODE_H
#define FEMM_CMESHNODE_H

#include <string>
#include "femmcomplex.h"

namespace femmsolver {

class CMeshNode
{
public:
    CMeshNode();

    double x,y;
    double msk;

    double GetDistance(double xo, double yo);
    CComplex CC();

private:

};

class CMMeshNode : public CMeshNode
{
public:
    CMMeshNode();
    CComplex A;
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

/**
 * @brief The CSMeshNode class
 *
 * \internal
 * ### FEMM reference source:
 * - \femm42{bv_problem.h}
 */
class CSMeshNode : public CMeshNode
{
public:
    CSMeshNode();

    double V;
    bool Q;
    bool IsSelected; //Note(ZaJ) why is this only in the electrostatics?
};

} //namespace
#endif
