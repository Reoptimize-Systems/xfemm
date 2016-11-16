// fpproc.h : interface of the FPProc class
//
/////////////////////////////////////////////////////////////////////////////
#include <vector>
#include "lua.h"
#include "luadebug.h"
#include "femmcomplex.h"
#include "problem.h"
#include "fparse.h"
//#include "luaconsoledlg.h"

// extern CFemmApp theApp; //<DP>

//#ifndef PLANAR
//#define PLANAR 0
//#endif
//
//#ifndef AXISYMMETRIC
//#define AXISYMMETRIC 1
//#endif

using namespace FPProcdata;

#ifndef FPPROC_H
#define FPPROC_H

class FPProc
{

// Attributes
public:

    FPProc();
    virtual ~FPProc();

    // General problem attributes
    double  Frequency;
    double  Depth;
    double  Precision;
    int     LengthUnits;
    double *LengthConv;
    femm::ProblemType problemType;
    int    Coords;
    string  ProblemNote;
    bool    FirstDraw;
    bool    Smooth;
    bool    bMultiplyDefinedLabels;
    double  extRo,extRi,extZo;

    double  A_High, A_Low;
    double  A_lb, A_ub;

    double  PlotBounds[9][2];
    double  d_PlotBounds[9][2];
//    double  Br_High,Br_Low;
//    double  Bi_High,Bi_Low;
    double  B_High, B_Low;
    double  H_High;
//    double  B_lb, B_ub;
//    double  Jr_High,Jr_Low;
//    double  Ji_High,Ji_Low;
//    double  J_High, J_Low;

    // Some default behaviors
    int  d_LineIntegralPoints;
    bool d_ShiftH;
    bool bHasMask;

    // lists of nodes, segments, and block labels
    std::vector< CNode >       nodelist;
    std::vector< CSegment >    linelist;
    std::vector< CArcSegment > arclist;
    std::vector< CBlockLabel > blocklist;

    // vectors containing the mesh information
    std::vector< CMeshNode >  meshnode;
    std::vector< CElement >   meshelem;

    // List of elements connected to each node;
    int *NumList;
    int **ConList;

    // lists of properties
    std::vector< CMaterialProp > blockproplist;
    std::vector< CBoundaryProp > lineproplist;
    std::vector< CPointProp >    nodeproplist;
    std::vector< CCircuit >      circproplist;

    // list of points in a user-defined contour;
    std::vector< CComplex > contour;

    // stuff that PTLOC needs
    std::vector< CMeshNode >  *pmeshnode;
    std::vector< CElement >   *pmeshelem;

//    TriEdge recenttri;
//    int samples;
//    unsigned long randomseed;
//    TriEdge bdrylinkhead[512];
//    int numberofbdrylink;

    // member functions
    int InTriangle(double x, double y);
    bool InTriangleTest(double x, double y, int i);
    bool GetPointValues(double x, double y, CPointVals &u);
    bool GetPointValues(double x, double y, int k, CPointVals &u);
    // void GetLineValues(CXYPlot &p, int PlotType, int npoints);
    void GetElementB(CElement &elm);
    void OnReload();
    void FindBoundaryEdges();
    int ClosestNode(double x, double y);
    CComplex Ctr(int i);
    double ElmArea(int i);
    double ElmArea(CElement *elm);
    double ElmVolume(int i);
    //double ElmVolume(CElement *elm);
    void GetPointB(double x, double y, CComplex &B1, CComplex &B2, CElement &elm);
    void GetNodalB(CComplex *b1, CComplex *b2,CElement &elm);
    CComplex BlockIntegral(int inttype);
    void LineIntegral(int inttype, CComplex *z);
    int ClosestArcSegment(double x, double y);
    void GetCircle(CArcSegment &asegm,CComplex &c, double &R);
    double ShortestDistanceFromArc(CComplex p, CArcSegment &arc);
    double ShortestDistanceFromSegment(double p, double q, int segm);
    CComplex GetJA(int k,CComplex *J,CComplex *A);
    CComplex PlnInt(double a, CComplex *u, CComplex *v);
    CComplex AxiInt(double a, CComplex *u, CComplex *v,double *r);
    bool ScanPreferences();
    void BendContour(double angle, double anglestep);
    bool MakeMask();
    CComplex HenrotteVector(int k);
    bool IsKosher(int k);
    double AECF(int k);
    void GetFillFactor(int lbl);

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);
//	void MsgBox(const char* message);

    CComplex GetStrandedVoltageDrop(int lbl);
    CComplex GetVoltageDrop(int circnum);
    CComplex GetFluxLinkage(int circnum);
    CComplex GetStrandedLinkage(int lbl);
    CComplex GetSolidAxisymmetricLinkage(int lbl);
    CComplex GetParallelLinkage(int numcirc);
    CComplex GetParallelLinkageAlt(int numcirc);
    void GetMu(CComplex b1, CComplex b2,CComplex &mu1, CComplex &mu2, int i);
    void GetMu(double b1, double b2, double &mu1, double &mu2, int i);
    void GetMagnetization(int n, CComplex &M1, CComplex &M2);
    void GetH(double b1, double b2, double &h1, double &h2, int k);
    void GetH(CComplex b1, CComplex b2, CComplex &h1, CComplex &h2, int k);
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
    //virtual void AssertValid() const;
    //virtual void Dump(CDumpContext& dc) const;
//#endif

};
#endif
