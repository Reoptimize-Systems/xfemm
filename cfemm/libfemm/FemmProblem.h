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

#include "CBlockLabel.h"
#include "CBoundaryProp.h"
#include "CCircuit.h"
#include "CMaterialProp.h"
#include "CNode.h"
#include "CPointProp.h"
#include "fparse.h"

#include <string>
#include <vector>

namespace femm {

class FemmProblemBase
{
public:
    virtual ~FemmProblemBase();
};

/**
 * \brief The FemmProblem class holds all data concerning a problem.
 *
 * The general problem parameters are a superset of all possible fields.
 * This means a little overhead, but memory-wise it shouldn't really hurt...
 *
 * \internal
 * Implementation is in the header file to make things easier with templating.
 */
template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
class FemmProblem : public FemmProblemBase
{
public:
    using FemmProblem_type = FemmProblem<PointPropT,BoundaryPropT,BlockPropT,CircuitPropT,BlockLabelT,NodeT>;
    using PointProp_type = PointPropT;
    using BoundaryProp_type = BoundaryPropT;
    using BlockProp_type = BlockPropT;
    using CircuitProp_type = CircuitPropT;
    using BlockLabel_type = BlockLabelT;
    using Node_type = NodeT;

    FemmProblem()
        : FileFormat(-1)
    , Frequency(0.0)
    , Precision(1.e-08)
    , MinAngle(0)
    , Depth(-1)
    , LengthUnits(LengthInches)
    , Coords(CART)
    , ProblemType(PLANAR)
    , extZo(0)
    , extRo(0)
    , extRi(0)
    , comment()
    , ACSolver(0)
    , dT(0)
    , PrevSoln()
    , DoForceMaxMeshArea(false)
    , nodeproplist()
    , lineproplist()
    , blockproplist()
    , circproplist()
    , labellist()
    , nodelist()
    {}

    virtual ~FemmProblem() {}

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

    std::vector< PointPropT > nodeproplist;
    std::vector< BoundaryPropT > lineproplist;
    std::vector< BlockPropT > blockproplist;
    std::vector< CircuitPropT > circproplist;
    std::vector< BlockLabelT > labellist;
    std::vector< NodeT > nodelist;
};

using MagneticsProblem = FemmProblem<femm::CPointProp
        , femm::CMBoundaryProp
        , femm::CMMaterialProp
        , femm::CMCircuit
        , femm::CMBlockLabel
        , femm::CNode>;

} //namespace

#endif
