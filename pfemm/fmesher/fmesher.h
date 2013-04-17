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

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

#include <vector>

#include <string>
#include "nosebl.h"
#include "complex.h"
#include "intpoint.h"
//#include "lua.h"
//#include "luaconsoledlg.h"
//#include "luadebug.h"

#ifndef LineFraction
#define LineFraction 500.0
#endif

// preprocessor tolerance
#ifndef CLOSE_ENOUGH
#define CLOSE_ENOUGH 1.e-06
#endif

// Default mesh size is the diagonal of the geometry's
// bounding box divided by BoundingBoxFraction
#ifndef BoundingBoxFraction
#define BoundingBoxFraction 100.0
#endif

#ifndef DEFAULT_MINIMUM_ANGLE
#define DEFAULT_MINIMUM_ANGLE 30.0
#endif

#ifndef _MSC_VER
#define _stricmp(s1, s2) strcasecmp(s1, s2)
#define _strnicmp(s1, s2, n) strncasecmp(s1, s2, (n))
#endif

using namespace std;

// FMesher Class

class FMesher
{

protected:


// Attributes
public:

    FMesher();
    FMesher(string);

	// General problem attributes
	double  Frequency;
	double  Precision;
	double	MinAngle;
	double  Depth;
	int		LengthUnits;
	int		ACSolver;
	bool    ProblemType;
	bool	Coords;
	string  ProblemNote;
	bool	FirstDraw;
	bool	NoDraw;
	bool    DoForceMaxMeshArea;

	// default behaviors
	double	d_prec;
	double	d_minangle;
	double	d_freq;
	double	d_depth;
	int		d_coord;
	int		d_length;
	int		d_type;
	int		d_solver;

	string BinDir;

	// lists of nodes, segments, and block labels
	std::vector< CNode >       nodelist;
	std::vector< CSegment >    linelist;
	std::vector< CArcSegment > arclist;
	std::vector< CBlockLabel > blocklist;

	// lists of nodes, segments, and block labels for undo purposes...
	std::vector< CNode >       undonodelist;
	std::vector< CSegment >    undolinelist;
	std::vector< CArcSegment > undoarclist;
	std::vector< CBlockLabel > undoblocklist;

	// vectors containing the mesh information
	std::vector< myPoint >		meshline;
	std::vector< myPoint >		greymeshline;
	std::vector< CNode >		meshnode;

	// lists of properties
	std::vector< CMaterialProp > blockproplist;
	std::vector< CBoundaryProp > lineproplist;
	std::vector< CPointProp >    nodeproplist;
	std::vector< CCircuit >      circproplist;
	double extRo,extRi,extZo;

// Operations
public:

    bool LoadFEMFile(string PathName);
    bool SaveFEMFile(string PathName);
    bool WriteTriangulationFiles(const struct triangulateio &out, string Pathname);

    //void downstr(char *s);

	//virtual ~CFemmeDoc();

	//bool AddNode(double x, double y, double d);
	//bool AddSegment(int n0, int n1, double tol=0);
	//bool AddSegment(int n0, int n1, CSegment *parsegm, double tol=0);
	//bool AddArcSegment(CArcSegment &asegm, double tol=0);
	//bool AddBlockLabel(double x, double y, double d);
	//bool AddNode(CNode &node, double d);
	//bool AddSegment(CComplex p0, CComplex p1, CSegment &segm, double tol=0);
	//bool AddArcSegment(CComplex p0, CComplex p1, CArcSegment &asegm, double tol=0);
	//bool AddBlockLabel(CBlockLabel &blabel, double d);
    //bool OpBlkDlg();
	//void OpNodeDlg();
	//void OpSegDlg();
	//void OpArcSegDlg();
	//void OpGrpDlg();
	//bool DeleteSelectedNodes();
	//bool DeleteSelectedSegments();
	//bool DeleteSelectedArcSegments();
	//bool DeleteSelectedBlockLabels();
    //void RotateMove(CComplex c, double t, int EditAction);
	//void TranslateMove(double dx, double dy, int EditAction);
	//void ScaleMove(double bx, double by, double sf, int EditAction);
	//void MirrorSelected(double x0, double y0, double x1, double y1, int ea);
	//void RotateCopy(CComplex c, double t, int ncopies, int EditAction);
	//void TranslateCopy(double dx, double dy, int ncopies, int EditAction);
    //bool ReadDXF(string fname, double DefTol=-1.);
	//bool WriteDXF(string fname);
    //bool CanCreateRadius(int n);
	//bool CreateRadius(int n, double r);
    //bool SelectOrphans();
	//bool dxf_line_hook();

	int ClosestNode(double x, double y);
	int ClosestBlockLabel(double x, double y);
	int ClosestSegment(double x, double y);
	bool GetIntersection(int n0, int n1, int segm, double *xi, double *yi);
	int ClosestArcSegment(double x, double y);
	void GetCircle(CArcSegment &asegm,CComplex &c, double &R);
	int GetLineArcIntersection(CSegment &seg, CArcSegment &arc, CComplex *p);
	int GetArcArcIntersection(CArcSegment &arc1, CArcSegment &arc2, CComplex *p);
	double ShortestDistanceFromArc(CComplex p, CArcSegment &arc);

	double LineLength(int i);
//	bool ScanPreferences();

    // Core functions
	bool LoadMesh(string PathName);
	int DoNonPeriodicBCTriangulation(string PathName);
	int DoPeriodicBCTriangulation(string PathName);
	void UpdateUndo();
	void Undo();
	//bool OldOnOpenDocument(LPCTSTR lpszPathName);
	bool HasPeriodicBC();
    void UnselectAll();
	double ShortestDistance(double p, double q, int segm);


	void EnforcePSLG();	// makes sure that everything is kosher...
	void EnforcePSLG(double tol);
	void FancyEnforcePSLG(double tol);

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);

private:

	virtual bool Initialize();

};


