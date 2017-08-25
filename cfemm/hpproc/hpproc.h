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
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
        Johannes Zarl-Zierl
   Contact:
        szelitzkye@gmail.com
        sztibi82@gmail.com
        richard.crozier@yahoo.co.uk
        johannes.zarl-zierl@jku.at

   Contributions by Johannes Zarl-Zierl were funded by
   Linz Center of Mechatronics GmbH (LCM)
*/

// hpproc.h : interface of the HPProc class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef HPPROC_H_INCLUDED
#define HPPROC_H_INCLUDED

#include "CHPointVals.h"

#include "femmcomplex.h"
#include "fparse.h"
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

#include <vector>

class HPProc
{

// Attributes
public:

	HPProc();
    virtual ~HPProc();

	// General problem attributes
	double	Depth;
	int		LengthUnits;
	double *LengthConv;
    femm::ProblemType problemType;
	bool	Coords;
    std::string ProblemNote;
	bool	Smooth;
	bool	bMultiplyDefinedLabels;
	double  extRo, extRi, extZo;

	double  A_High, A_Low;
	double	A_lb, A_ub;

	double  d_PlotBounds[4][2];
	double  PlotBounds[4][2];


	// Some default behaviors
    std::string BinDir;
	int  d_LineIntegralPoints;
	bool bHasMask;

	// lists of nodes, segments, and block labels
    std::vector< femm::CNode >             nodelist;
    std::vector< femm::CSegment >       	 linelist;
    std::vector< femm::CHBlockLabel > 		 blocklist;
    std::vector< femm::CArcSegment > 		 arclist;

	// CArrays containing the mesh information
    std::vector< femmsolver::CHMeshNode >			meshnode;
	std::vector< femmsolver::CHElement >				meshelem;

	// List of elements connected to each node;
	int *NumList;
	int **ConList;

	// lists of properties
    std::vector< femm::CHMaterialProp > 	blockproplist;
    std::vector< femm::CHBoundaryProp > 	lineproplist;
    std::vector< femm::CHPointProp > 		nodeproplist;
    std::vector< femm::CHConductor > 		circproplist;

	// list of points in a user-defined contour;
	std::vector< CComplex  > 		contour;

	// member functions
	int InTriangle(double x, double y);
	bool InTriangleTest(double x, double y, int i);
    bool GetPointValues(double x, double y, CHPointVals &u);
    bool GetPointValues(double x, double y, int k, CHPointVals &u);
	//void GetLineValues(CXYPlot &p, int PlotType, int npoints);
	void GetElementD(int k);
	void OnReload();
	int ClosestNode(double x, double y);
	CComplex Ctr(int i);
	double ElmArea(int i);
	double ElmArea(femmsolver::CHElement *elm);
	void GetPointD(double x, double y, CComplex &D, femmsolver::CHElement &elm);
	void GetNodalD(CComplex *d, int i);
	CComplex BlockIntegral(int inttype);
	void LineIntegral(int inttype, double *z);
	int ClosestArcSegment(double x, double y);
    void GetCircle(femm::CArcSegment &asegm,CComplex &c, double &R);
    double ShortestDistanceFromArc(CComplex p, femm::CArcSegment &arc);
	double ShortestDistanceFromSegment(double p, double q, int segm);
	int ClosestSegment(double x, double y);
	bool IsSameMaterial(int e1, int e2);
	double AECF(int k);
	double AECF(int k, CComplex p);

	bool ScanPreferences();
	void BendContour(double angle, double anglestep);
	CComplex HenrotteVector(int k);
	bool IsKosher(int k);
	void FindBoundaryEdges();
	CComplex E(int k);
	CComplex D(int k);
	CComplex e(int k, int i);
	CComplex d(int k, int i);

	// pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);

	void ClearDocument();
    bool NewDocument();
//     virtual void Serialize(CArchive& ar);
    bool OpenDocument(std::string lpszPathName);


// Implementation
public:
    // lua extensions
    bool luafired;
    //void initalise_lua();
    //PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
    //void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,HBITMAP hBMP, HDC hDC) ;

//#ifdef _DEBUG
//    virtual void AssertValid() const;
//    virtual void Dump(CDumpContext& dc) const;
//#endif

};
#endif
