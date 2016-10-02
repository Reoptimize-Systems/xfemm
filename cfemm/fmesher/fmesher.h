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

#ifndef FMESHER_H
#define FMESHER_H

#include <vector>

#include <string>
#include "nosebl.h"
#include "femmcomplex.h"
#include "intpoint.h"
#include "triangle.h"

#include "CArcSegment.h"
#include "CBlockLabel.h"
#include "CNode.h"
#include "CSegment.h"

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

namespace femm
{

// FMesher Class

class FMesher
{

protected:


// Attributes
public:

    enum filetypes { F_TYPE_UNKNOWN, F_TYPE_MAGNETICS, F_TYPE_HEATFLOW };

    enum loaderrors { F_FILE_OK, F_FILE_UNKNOWN_TYPE, F_FILE_NOT_OPENED, F_FILE_MALFORMED};

    FMesher();
    FMesher(std::string);

	// General problem attributes
	int     filetype;
	double	MinAngle;
	bool    DoForceMaxMeshArea;

	// default behaviors
	double	d_minangle;
	bool    Verbose;

	std::string BinDir;

	// lists of nodes, segments, and block labels
    std::vector< femm::CMesherNode >       nodelist;
    std::vector< femm::CMesherSegment >    linelist;
    std::vector< femm::CMesherArcSegment > arclist;
    std::vector< femm::CMesherBlockLabel > blocklist;

	// lists of nodes, segments, and block labels for undo purposes...
    std::vector< femm::CMesherNode >       undonodelist;
    std::vector< femm::CMesherSegment >    undolinelist;
    std::vector< femm::CMesherArcSegment > undoarclist;
    std::vector< femm::CMesherBlockLabel > undoblocklist;

	// vectors containing the mesh information
	std::vector< myPoint >      meshline;
	std::vector< myPoint >      greymeshline;
    std::vector< femm::CMesherNode >	meshnode;

	// lists of properties
	std::vector< femm::CBoundaryProp > lineproplist;
	std::vector< femm::CPointProp >    nodeproplist;
	std::vector< femm::CCircuit >      circproplist;

    // used to echo start of input file to output
    std::vector< std::string > probdescstrings;

// Operations
public:

    static int GetFileType (std::string PathName);
    int LoadFEMFile(std::string PathName);
    int LoadFEMFile(std::string PathName, int ftype);
    bool SaveFEMFile(std::string PathName);
    bool WriteTriangulationFiles(const struct triangulateio &out, std::string Pathname);

    //void downstr(char *s);

	int ClosestNode(double x, double y);
	int ClosestBlockLabel(double x, double y);
	int ClosestSegment(double x, double y);
	bool GetIntersection(int n0, int n1, int segm, double *xi, double *yi);
	int ClosestArcSegment(double x, double y);
    void GetCircle(femm::CMesherArcSegment &asegm,CComplex &c, double &R);
    int GetLineArcIntersection(femm::CMesherSegment &seg, femm::CMesherArcSegment &arc, CComplex *p);
    int GetArcArcIntersection(femm::CMesherArcSegment &arc1, femm::CMesherArcSegment &arc2, CComplex *p);
    double ShortestDistanceFromArc(CComplex p, femm::CMesherArcSegment &arc);

	double LineLength(int i);

    // Core functions
	bool LoadMesh(std::string PathName);
	int DoNonPeriodicBCTriangulation(std::string PathName);
	int DoPeriodicBCTriangulation(std::string PathName);
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

    // pointer to function to use for triangle to issue warning messages
    int (*TriMessage)(const char * format, ...);

private:

	virtual bool Initialize();
	void addFileStr (char * q);

};

} // namespace femm

#endif
