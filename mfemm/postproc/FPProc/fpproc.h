// fpproc.h : interface of the FPProc class
//
/////////////////////////////////////////////////////////////////////////////
#include "lua.h"
#include "luadebug.h"
//#include "luaconsoledlg.h"

// extern CFemmApp theApp; //<DP>

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

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
    int     LengthUnits;
    double *LengthConv;
    BOOL    ProblemType;
    BOOL    Coords;
    string  ProblemNote;
    BOOL    FirstDraw;
    BOOL    Smooth;
    BOOL    bMultiplyDefinedLabels;
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
    BOOL d_ShiftH;
    BOOL bHasMask;

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
    BOOL InTriangleTest(double x, double y, int i);
    BOOL GetPointValues(double x, double y, CPointVals &u);
    BOOL GetPointValues(double x, double y, int k, CPointVals &u);
    // void GetLineValues(CXYPlot &p, int PlotType, int npoints);
    void GetElementB(CElement &elm);
    void OnReload();
    void FindBoundaryEdges();
    int ClosestNode(double x, double y);
    CComplex Ctr(int i);
    double ElmArea(int i);
    double ElmArea(CElement *elm);
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
    BOOL ScanPreferences();
    void BendContour(double angle, double anglestep);
    BOOL MakeMask();
    CComplex HenrotteVector(int k);
    BOOL IsKosher(int k);
    double AECF(int k);
    void GetFillFactor(int lbl);

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

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(FPProc)
public:
    virtual BOOL ClearDocument();
    virtual BOOL NewDocument();
//     virtual void Serialize(CArchive& ar);
    virtual BOOL OpenDocument(std::string lpszPathName);
    //virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    //}}AFX_VIRTUAL

// Implementation
public:
    // lua extensions
    bool luafired;
    void initalise_lua();
    //PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
    //void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,HBITMAP hBMP, HDC hDC) ;

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

};
#endif

/////////////////////////////////////////////////////////////////////////////
char* StripKey(char *c);
