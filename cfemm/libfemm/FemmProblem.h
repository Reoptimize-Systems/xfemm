/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of 
 * Mechatronics GmbH (LCM)
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
#include "femmenums.h"
#include "fparse.h"

#include <map>
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
 * \note
 * Currently, the FemmProblem is only used by the mesher and in lua code.
 * I.e. the mesh data is currently unused here.
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
     *
     * Internally, this calls writeProblemDescription.
     * @param filename
     * @return \c true if saving was successful, \c false otherwise
     */
    bool saveFEMFile( std::string &filename ) const;

    /**
     * @brief writeProblemDescription writes the problem description into an output stream.
     * @param output
     * @return
     * \internal
     * When adding a new file type, you need to check that all peculiarities are handled here.
     * E.g. the name of the conductor property section varies with file type.
     * For now, the following file types are fully supported: electrostatics, heat flow.
     *
     * ### FEMM source:
     *  - \femm42{femm/beladrawDoc.cpp,OnSaveDocument()}
     *  - \femm42{femm/CDRAWDOC.cpp,OnSaveDocument()}
     *  - \femm42{femm/FemmeDoc.cpp,OnSaveDocument()}
     *  - \femm42{femm/HDRAWDOC.cpp,OnSaveDocument()}
     * \endinternal
     */
    void writeProblemDescription( std::ostream &output ) const;

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
     * @brief Add an arc segment to the problem description.
     * This method takes care of intersections with other nodes or lines, and splits the arc segment if necessary.
     * No degenerate arc segments (with start point == end point) can be added.
     * @param asegm the proposed arc segment.
     * @param tol tolerance, i.e. minimum distance between arc segment and nodes
     * @return \c true if the arc segment could be added, \c false otherwise.
     */
    bool addArcSegment(femm::CArcSegment &asegm, double tol=0.);

    /**
     * @brief Add a block label to the problem description.
     * The method ensures that a block label can not be added on top
     * of another label, node or line.
     * @param x x-coordinate
     * @param y y-coordinate
     * @param d minimum distance to next node or label.
     * @return \c true if the label could be added or a block label already exists at that position, \c false otherwise.
     */
    bool addBlockLabel(double x, double y, double d);
    /**
     * @brief Add a block label to the problem description.
     * The method ensures that a block label can not be added on top
     * of another label, node or line.
     *
     * In contrast to the AddBlockLabel(double,double,double), this signature needs an existing CBlockLabel rvalue reference that is invalidated by this call.
     * Normally, you want to call the other variant.
     *
     * @param label the label to add
     * @param d minimum distance to next node or label.
     * @return \c true if the label could be added or a block label already exists at that position, \c false otherwise.
     */
    bool addBlockLabel(std::unique_ptr<femm::CBlockLabel> &&label, double d);

    /**
     * @brief Add a CNode to the problem description.
     * If necessary, adjust existing CSegments or CArcSegments.
     * The method also ensures that a new node can't be put atop of an existing node or a block label.
     * @param x node position x
     * @param y node position y
     * @param d minimum distance to next node or label.
     * @return \c true, if the node could be added, \c false otherwise.
     */
    bool addNode(double x, double y, double d);
    /**
     * @brief Add a CNode to the problem description.
     * If necessary, adjust existing CSegments or CArcSegments.
     * The method also ensures that a new node can't be put atop of an existing node or a block label.
     *
     * In contrast to the AddNode(double,double,double), this signature needs an existing CNode rvalue reference that is invalidated by this call.
     * Normally, you want to call the other variant.
     *
     * @param node the node to add.
     * @param d minimum distance to next node or label.
     * @return \c true, if the node could be added, \c false otherwise.
     */
    bool addNode(std::unique_ptr<femm::CNode> &&node, double d);

    /**
     * @brief Add a line (CSegment) to the problem description
     * The method checks for intersections, splits existing lines/arcs accordingly,
     * and adds nodes at intersections as needed.
     * It is ensured that a new line can not be added twice, and that no degenerate lines are added (with start point == end point)
     * @param n0 index of start node
     * @param n1 index of end node
     * @param tol tolerance, i.e. minimum distance between nodes
     * @return \c true, if the line could be added, \c false otherwise.
     */
    bool addSegment(int n0, int n1, double tol=0.);
    /**
     * @brief Add a line (CSegment) to the problem description
     * The method checks for intersections, splits existing lines/arcs accordingly,
     * and adds nodes at intersections as needed.
     * It is ensured that a new line can not be added twice, and that no degenerate lines are added (with start point == end point)
     * @param n0 index of start node
     * @param n1 index of end node
     * @param parsegm
     * @param tol tolerance, i.e. minimum distance between nodes
     * @return \c true, if the line could be added, \c false otherwise.
     */
    bool addSegment(int n0, int n1, const femm::CSegment *parsegm, double tol=0.);

    /**
     * @brief Find the closest arc segment for the given coordinates
     * @param x
     * @param y
     * @return an arc segment index, or -1 if the arclist is empty
     */
    int closestArcSegment(double x, double y) const;

    /**
     * @brief Find the closest block label for the given coordinates
     * @param x
     * @param y
     * @return a blocklabel index, or -1 if the labellist is empty
     */
    int closestBlockLabel(double x, double y) const;

    /**
     * @brief Find the closest node for the given coordinates
     * @param x
     * @param y
     * @return a node index, or -1 if the nodelist is empty
     */
    int closestNode(double x, double y) const;

    /**
     * @brief Find the closest line segment for the given coordinates
     * @param x
     * @param y
     * @return a segment index, or -1 if the linelist is empty
     */
    int closestSegment(double x, double y) const;

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

    /**
     * @brief Check whether the given node is suitable for creating a radius.
     * @param n node index
     * @return \c true if the node can be replaced with a radius, \c false otherwise
     */
    bool canCreateRadius(int n) const;

    /**
     * @brief Create an arc in place of the given node.
     * @param n the node index
     * @param r the arc radius
     * @return \c true if the arc could be created, \c false otherwise
     */
    bool createRadius(int n, double r);

    /**
     * @brief Delete all selected arc segments
     * @return \c true, if any segments were deleted, \c false otherwise.
     */
    bool deleteSelectedArcSegments();
    /**
     * @brief Delete all selected BlockLabels
     * @return \c true, if any blocks were deleted, \c false otherwise.
     */
    bool deleteSelectedBlockLabels();
    /**
     * @brief Delete all selected nodes.
     * If a node is part of a line or arc, that line or arc is also deleted.
     * @return \c true, if any node was deleted, \c false otherwise.
     */
    bool deleteSelectedNodes();
    /**
     * @brief Delete all selected segments.
     * @return \c true, if any segments were deleted, \c false otherwise.
     */
    bool deleteSelectedSegments();

    /**
     * @brief EnforcePSLG
     * Makes sure that there are no:
     * 1. no duplicate points
     * 2. no intersections between line segments, lines and arcs, or arcs
     * 3. no duplicate block labels
     * 4. no overlapping lines or arcs.
     *
     * @param tol tolerance
     *
     * \internal
     * We do this by cleaning out the various lists, and rebuilding them
     * using the ``add'' functions that ensure that things come out right.
     */
    void enforcePSLG(double tol=0);

    /**
     * @brief Intersect two arcs.
     * @param arc0
     * @param arc1
     * @param p intersection points, packed as CComplex (output variable)
     * @return the number of valid intersections (between 0 and 2 inclusive)
     */
    int getArcArcIntersection(const femm::CArcSegment &arc0, const femm::CArcSegment &arc1, CComplex *p) const;
    /**
     * @brief look at points, block labels, and arcs to get bounding box.
     * @param x
     * @param y
     * @return \c true on success, \c false on failure
     */
    bool getBoundingBox(double (&x)[2], double (&y)[2]) const;

    /**
     * @brief getCircle computes the matching circle to an arc segment.
     * @param asegm the arc segment
     * @param c the center of the circle, packed as CComplex (output variable)
     * @param R the radius of the circle (output variable)
     */
    void getCircle(const CArcSegment &arc,CComplex &c, double &R) const;

    /**
     * @brief GetIntersection between a line and a segment.
     * Only intersections that are not at or close to an endpoint are considered.
     *
     * @param n0 Node index for line point 0
     * @param n1 Node index for line point 1
     * @param segm Segment index
     * @param xi x coordinate of intersection (output variable)
     * @param yi y coordinate of intersection (output variable)
     * @return true, if a "proper" intersection is found, false otherwise.
     */
    bool getIntersection(int n0, int n1, int segm, double *xi, double *yi) const;

    /**
     * @brief intersection between line and arc segment
     * @param seg the line segment
     * @param arc the arc segment
     * @param p the intersection point(s), packed as CComplex (output variable)
     * @return the number of intersections
     */
    int getLineArcIntersection(const femm::CSegment &seg, const femm::CArcSegment &arc, CComplex *p) const;

    /**
     * @brief The document title.
     * Currently, this is the same as PathName
     * @return
     */
    std::string getTitle() const;

    /**
     * @brief LineLength
     * @param i index of line
     * @return the length of the line
     *
     * \internal
     * Original name: LineLength
     * \endinternal
     */
    double lengthOfLine(int i) const;

    /**
     * @brief Mirror copies of the selected objects about the given line.
     * The line is defined by two points along the line.
     * @param x0 x coordinate of point 0
     * @param y0 y coordinate of point 0
     * @param x1 x coordinate of point 1
     * @param y1 y coordinate of point 1
     * @param selector
     */
    void mirrorCopy(double x0, double y0, double x1, double y1, femm::EditMode selector);

    /**
     * @brief Rotate copies of the selected objects of the selected type.
     * Each copy is rotated by the given amount compared to the previous copy.
     * @param c center
     * @param dt (incremental) angle in degrees
     * @param ncopies number of copies
     * @param selector
     */
    void rotateCopy(CComplex c, double dt, int ncopies, femm::EditMode selector);

    /**
     * @brief Rotate the selected objects of the selected type.
     * @param c center
     * @param t angle
     * @param selector
     */
    void rotateMove(CComplex c, double t, femm::EditMode selector);

    /**
     * @brief Scale the selected objects relative to a base point
     * @param bx base-point x
     * @param by base-point y
     * @param sf scale factor
     * @param selector
     */
    void scaleMove(double bx, double by, double sf, femm::EditMode selector);

    /**
     * @brief shortestDistanceFromSegment computes the distance between a point and the nearest point of a line segment.
     *
     * Note: this can not be a member function of CSegment,
     * because the linw segments only store node indices of their points.
     * @param p the point
     * @param segm the line segment index
     * @return the shortest distance
     */
    double shortestDistanceFromSegment(double p, double q, int segm) const;
    /**
     * @brief shortestDistanceFromArc computes the distance between a point and the nearest point on an arc segment.
     *
     * Note: this can not be a member function of CArcSegment,
     * because the arc segments only store node indices of their points.
     * @param p the point
     * @param arc the arc segment index
     * @return the shortest distance
     */
    double shortestDistanceFromArc(CComplex p, const femm::CArcSegment &arc) const;

    /**
     * @brief Deselect all nodes, block labels, line segments, and arc segments.
     */
    void unselectAll();

    /**
     * @brief Revert data to the undo point.
     */
    void undo();
    /**
     * @brief Revert only linelist to the undo point.
     */
    void undoLines();
    /**
     * @brief Create an undo point.
     */
    void updateUndo();
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
    // currently, the mesh part of FemmProblem is not used at all
    std::vector< std::unique_ptr<femmsolver::CMeshNode>>   meshnodes;
    std::vector< std::unique_ptr<femmsolver::CElement>> meshelems;

    std::string pathName; ///< \brief pathname of the associated (.fem) file, if any.
    //std::string solutionFile; ///< \brief pathname of the associated solution file (.ans), if any.

    FileType filetype; ///< \brief file type of the problem description.
    std::map<std::string, int> blockMap; ///< \brief a map from BlockName to block index. \sa updateBlockMap
    std::map<std::string, int> lineMap; ///< \brief a map from BrdyName to line index. \sa updateLineMap
    std::map<std::string, int> circuitMap; ///< \brief a map from CircuitName to circuit index. \sa updateCircuitMap
    std::map<std::string, int> nodeMap; ///< \brief a map from PointName to node index. \sa updateNodeMap

private:
    // lists of nodes, segments, and block labels for undo purposes...
    std::vector< std::unique_ptr<femm::CNode> >       undonodelist;
    std::vector< std::unique_ptr<femm::CSegment> >    undolinelist;
    std::vector< std::unique_ptr<femm::CArcSegment> > undoarclist;
    std::vector< std::unique_ptr<femm::CBlockLabel> > undolabellist;
};

} //namespace

#endif
