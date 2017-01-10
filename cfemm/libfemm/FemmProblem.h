/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#ifndef FEMMPROBLEM_H
#define FEMMPROBLEM_H

#include "CArcSegment.h"
#include "CBlockLabel.h"
#include "CBoundaryProp.h"
#include "CCircuit.h"
#include "CElement.h"
#include "CMaterialProp.h"
#include "CMeshNode.h"
#include "CNode.h"
#include "CPointProp.h"
#include "CSegment.h"
#include "fparse.h"

#include <memory>
#include <string>
#include <vector>

namespace femm {

/**
 * \brief The FemmProblem class holds all data concerning a problem.
 *
 * The general problem parameters are a superset of all possible fields.
 * This means a little overhead, but memory-wise it shouldn't really hurt...
 *
 * \internal
 * In contrast to FEMM42 and older XFemm classes we use vectors of pointers, not vectors of objects.
 * This allows us to use inheritance to have a common data description which can be used for mesher, solver and pproc.
 * Using unique_ptrs makes the pointers a little more of a PITA, but they allow us to have the same
 * ownership semantics as with objects stored in vectors (i.e. the data is freed when the vector is freed).
 */
class FemmProblem
{
public:
    FemmProblem();

    virtual ~FemmProblem();

public: // data members
    double FileFormat; ///< \brief format version of the file
    double Frequency;  ///< \brief Frequency for harmonic problems [Hz]
    double Precision;  ///< \brief Computing precision within FEMM
    double MinAngle;   ///< \brief angle restriction for triangulation [deg]
    double Depth;      ///< \brief typical length in z-direction [lfac]
    femm::LengthUnit  LengthUnits;  ///< \brief Unit for lengths. Also referred to as \em lfac.
    femm::CoordsType  Coords;  ///< \brief definition of the coordinate system
    femm::ProblemType ProblemType; ///< \brief The 2D problem is either planar or axisymmetric
    // axisymmetric external region parameters
    double extZo;  ///< \brief center of exterior [lfac], only valid for axisymmetric problems
    double extRo;  ///< \brief radius of exterior [lfac], only valid for axisymmetric problems
    double extRi;  ///< \brief radius of interior [lfac], only valid for axisymmetric problems
    std::string comment; ///< \brief Problem description

    int ACSolver; ///< \brief .succ. approcimation or .Newton is possible
    double dT; ///< \brief delta T used by hsolver \verbatim[dT]\endverbatim
    std::string PrevSoln; ///y \brief   name of a previous solution file for hsolver \verbatim[prevsoln]\endverbatim

    bool    DoForceMaxMeshArea; ///< \brief Property introduced by xfemm.

    // lists of nodes, segments, and block labels
    std::vector< std::unique_ptr<CNode>> nodelist;
    std::vector< std::unique_ptr<CSegment>> linelist;
    std::vector< std::unique_ptr<CArcSegment>> arclist;
    std::vector< std::unique_ptr<CBlockLabel>> labellist;

    std::vector< std::unique_ptr<CPointProp>> nodeproplist;
    std::vector< std::unique_ptr<CBoundaryProp>> lineproplist;
    std::vector< std::unique_ptr<CMaterialProp>> blockproplist;
    std::vector< std::unique_ptr<CCircuit>> circproplist;

    bool solved;
    // vectors containing the mesh information
    std::vector< std::unique_ptr<CMeshNode>>   meshnodes;
    std::vector< std::unique_ptr<CElement>> meshelems;

    std::string pathName; ///< \brief pathname of the associated (.fem) file, if any.
    //std::string solutionFile; ///< \brief pathname of the associated solution file (.ans), if any.
};

} //namespace

#endif
