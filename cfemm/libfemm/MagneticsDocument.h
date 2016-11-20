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

#include "CArcSegment.h"
#include "CBlockLabel.h"
#include "CBoundaryProp.h"
#include "CCircuit.h"
#include "CMaterialProp.h"
#include "CNode.h"
#include "CPointProp.h"
#include "CSegment.h"
#include "IntPoint.h"

#ifndef MAGNETICSDOCUMENT_H
#define MAGNETICSDOCUMENT_H

namespace femm
{

class LuaInstance;

/**
 * @brief The MagneticsDocument class represents a document for a magnetics problem.
 * This class mostly corresponds to class CFemmeDoc in \femm42{femm/FemmeDoc.h}.
 *
 * \sa FMesher
 * \sa FSolver
 */
class MagneticsDocument
{
public:
    MagneticsDocument();
public:
    std::string PathName;

    // General problem attributes
    double  Frequency;
    double  Precision;
    double	MinAngle;
    double  Depth;
    int		LengthUnits;
    int		ACSolver;
    bool    ProblemType;
    bool	Coords;
    std::string ProblemNote;
    bool	FirstDraw;
    bool	NoDraw;
    std::string PrevSoln;

    // default behaviors
    double	d_prec;
    double	d_minangle;
    double	d_freq;
    double	d_depth;
    int		d_coord;
    int		d_length;
    int		d_type;
    int		d_solver;

    std::string BinDir;

    // lists of nodes, segments, and block labels
    std::vector< CMesherNode>             nodelist;
    std::vector< CMesherSegment>       linelist;
    std::vector< CMesherArcSegment> arclist;
    std::vector< CMesherBlockLabel> blocklist;

    // lists of nodes, segments, and block labels for undo purposes...
    std::vector< CMesherNode>             undonodelist;
    std::vector< CMesherSegment>       undolinelist;
    std::vector< CMesherArcSegment> undoarclist;
    std::vector< CMesherBlockLabel> undoblocklist;

    // std::vectors containing the mesh information
    std::vector< IntPoint>		   meshline;
    std::vector< IntPoint>		   greymeshline;
    std::vector< CMesherNode>			   meshnode;

    // lists of properties
    std::vector< CMMaterialProp > blockproplist;
    std::vector< CMBoundaryProp > lineproplist;
    std::vector< CPointProp    > nodeproplist;
    std::vector< CCircuit      > circproplist;
    double extRo,extRi,extZo;

    virtual bool OnNewDocument();
    virtual bool OnOpenDocument(std::string lpszPathName);
    virtual bool OnSaveDocument(std::string lpszPathName);

    // Implementation
public:
    virtual ~MagneticsDocument();

    void UnselectAll();
    double ShortestDistance(double p, double q, int segm);
    bool AddNode(double x, double y, double d);
    bool AddSegment(int n0, int n1, double tol=0);
    bool AddSegment(int n0, int n1, CMesherSegment *parsegm, double tol=0);
    bool AddArcSegment(CMesherArcSegment &asegm, double tol=0);
    bool AddBlockLabel(double x, double y, double d);
    bool AddNode(CMesherNode &node, double d);
    bool AddSegment(CComplex p0, CComplex p1, CMesherSegment &segm, double tol=0);
    bool AddArcSegment(CComplex p0, CComplex p1, CMesherArcSegment &asegm, double tol=0);
    bool AddBlockLabel(CMesherBlockLabel &blabel, double d);
    int ClosestNode(double x, double y);
    int ClosestBlockLabel(double x, double y);
    int ClosestSegment(double x, double y);
    bool GetIntersection(int n0, int n1, int segm, double *xi, double *yi);
    int ClosestArcSegment(double x, double y);
    void GetCircle(CMesherArcSegment &asegm,CComplex &c, double &R);
    int GetLineArcIntersection(CMesherSegment &seg, CMesherArcSegment &arc, CComplex *p);
    int GetArcArcIntersection(CMesherArcSegment &arc1, CMesherArcSegment &arc2, CComplex *p);
    double ShortestDistanceFromArc(CComplex p, CMesherArcSegment &arc);
    void RotateMove(CComplex c, double t, int EditAction);
    void TranslateMove(double dx, double dy, int EditAction);
    void ScaleMove(double bx, double by, double sf, int EditAction);
    void MirrorSelected(double x0, double y0, double x1, double y1, int ea);
    void RotateCopy(CComplex c, double t, int ncopies, int EditAction);
    void TranslateCopy(double dx, double dy, int ncopies, int EditAction);
    bool DeleteSelectedNodes();
    bool DeleteSelectedSegments();
    bool DeleteSelectedArcSegments();
    bool DeleteSelectedBlockLabels();
    bool LoadMesh();
    bool OnWritePoly();
    bool FunnyOnWritePoly();
    bool ReadDXF(std::string fname, double DefTol=-1.);
    bool WriteDXF(std::string fname);
    //bool OldOnOpenDocument(std::string lpszPathName);
    bool HasPeriodicBC();
    bool CanCreateRadius(int n);
    bool CreateRadius(int n, double r);
    double LineLength(int i);

    void UpdateUndo();
    void Undo();
    void EnforcePSLG();	// makes sure that everything is kosher...
    void EnforcePSLG(double tol);
    void FancyEnforcePSLG(double tol);
    bool SelectOrphans();
    bool dxf_line_hook();


    // replacement for MsgBox
    void (*WarnMessage)(const char*);
    static void PrintMsg(const char *msg);
};

} /* namespace */

#endif /* MAGNETICSDOCUMENT_H */
// vi:expandtab:tabstop=4 shiftwidth=4:
