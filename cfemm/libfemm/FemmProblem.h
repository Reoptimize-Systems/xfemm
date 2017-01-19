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

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace femm {
/**
 * @brief The FileType enum determines how the problem description is written to disc.
 */
enum FileType { UnknownFile, MagneticsFile, HeatFlowFile, CurrentFlowFile, ElectrostaticsFile };

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
    explicit FemmProblem( FileType fileType);

    virtual ~FemmProblem();

    /**
     * @brief saveFEMFile saves the problem description into a .fem file.
     * If the file type is UnknownFile, the method fails.
     * @param filename
     * @return \c true if saving was successful, \c false otherwise
     */
    bool saveFEMFile( std::string &filename ) const;

    /**
     * @brief Update BoundaryMarkerName and InConductorName textual references from their index.
     * The .fem file formats store this information as integer index.
     * The mesher uses text-based lookup, though.
     * Therefore, the text-based references need to be set if the index based references changed.
     *
     * Note: FemmReader::parse() already calls this method.
     */
    void updateLabelsFromIndex();

    /**
     * @brief Update the blockMap.
     * Call this function whenever the block properties change (i.e. whenever a new element is added or a BlockName changes).
     */
    void updateBlockMap();
    /**
     * @brief Update the circuitMap.
     * Call this function whenever the circuit properties change (i.e. whenever a new element is added or a CircuitName changes).
     */
    void updateCircuitMap();
    /**
     * @brief Update the lineMap.
     * Call this function whenever the line properties change (i.e. whenever a new element is added or a BoundaryMarkerName changes).
     */
    void updateLineMap();
    /**
     * @brief Update the nodeMap.
     * Call this function whenever the node properties change (i.e. whenever a new element is added or a PointName changes).
     */
    void updateNodeMap();

    /**
     * @brief Invalidate the mesh data if the problem was already meshed.
     * Call this method whenever you change the problem structure.
     */
    void invalidateMesh();

    /**
     * @brief meshed
     * @return \c true, if the problem contains mesh data, \c false otherwise.
     */
    bool isMeshed() const;

    /**
     * @brief Run a basic consistency check.
     * In particular, this checks:
     *  * For block labels
     *    * whether integer-indices of InCircuit and BlockType are in the correct range
     *    * whether the textual references (InCircuitName, BlockTypeName) match the integer indices
     *  * For points, lines, and arcs:
     *    * whether integer-indices of InConductor and BoundaryMarker are in the correct range
     *    * whether the textual references (InConductorName, BoundaryMarkerName) match the integer indices
     *
     * @return \c true, if the data seems consistent, \c false otherwise.
     */
    bool consistencyCheckOK() const;

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

    FileType filetype; ///< \brief file type of the problem description.
    std::map<std::string, int> blockMap; ///< \brief a map from BlockName to block index. \sa updateBlockMap
    std::map<std::string, int> lineMap; ///< \brief a map from BrdyName to line index. \sa updateLineMap
    std::map<std::string, int> circuitMap; ///< \brief a map from CircuitName to circuit index. \sa updateCircuitMap
    std::map<std::string, int> nodeMap; ///< \brief a map from PointName to node index. \sa updateNodeMap
};

} //namespace

#endif
