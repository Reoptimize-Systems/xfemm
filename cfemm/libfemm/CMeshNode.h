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
