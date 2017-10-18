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

#include "femmcomplex.h"

#include <iostream>
#include <string>

namespace femmsolver {

class CMeshNode
{
public:
    CMeshNode();
    virtual ~CMeshNode() = default;

    double x,y;
    double msk;

    double GetDistance(double xo, double yo);
    CComplex CC();

    /**
     * @brief Q boundary flag
     * Indicates whether the node lies on a boundary, or in a conductor, or none.
     */
    int Q;

    bool IsSelected; ///< Used to compute the “weighted stress tensor” force and torque integrals; not used in magnetics problems.
    void ToggleSelect();
private:

};

class CMMeshNode : public CMeshNode
{
public:
    CMMeshNode();
    /**
     * @brief fromStream constructs a CMMeshNode from an input stream (usually an input file stream)
     * \note If Frequency is 0, make sure to clear A.im!
     * @param input
     * @param err output stream for error messages
     * @return a CMMeshNode
     */
    static CMMeshNode fromStream( std::istream &input, std::ostream &err = std::cerr );
    CComplex A;
};

class CHMeshNode : public CMeshNode
{
public:
    CHMeshNode();

    /**
     * @brief fromStream constructs a CHMeshNode from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CHMeshNode
     */
    static CHMeshNode fromStream( std::istream &input, std::ostream &err = std::cerr );

    double T;  ///< temperature at the node
};

/**
 * @brief The CSMeshNode class
 *
 * \internal
 * ### FEMM reference source:
 * - \femm42{femm/bv_problem.h}
 */
class CSMeshNode : public CMeshNode
{
public:
    CSMeshNode();

    /**
     * @brief fromStream constructs a CSMeshNode from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CSMeshNode
     * \internal
     * ### FEMM reference source:
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::OnOpenDocument()}
     * \endinternal
     */
    static CSMeshNode fromStream( std::istream &input, std::ostream &err = std::cerr );

    double V;
};

} //namespace
#endif
