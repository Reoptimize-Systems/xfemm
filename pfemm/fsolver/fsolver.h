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

// fsolver.h : interface of the FSolver class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef muo
#define muo 1.2566370614359173e-6
#endif

#ifndef Golden
#define Golden 0.3819660112501051517954131656
#endif

// replace original windows BOOL type, which is actually
// just an int
//#ifndef BOOL
//#define BOOL int
//#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#include "mesh.h"
#include "spars.h"

#ifndef FSOLVER_H
#define FSOLVER_H
class FSolver
{



// Attributes
public:

    FSolver();
    ~FSolver();

    // General problem attributes
    double  Frequency;
    double  Precision;
    double  Relax;
    int		LengthUnits;
    int		ACSolver;
    int     ProblemType;
    int	    Coords;

    // axisymmetric external region parameters
    double  extRo,extRi,extZo;

    //CFknDlg *TheView;

    // CArrays containing the mesh information
    int	BandWidth;
    CNode *meshnode;
    CElement *meshele;

    int NumNodes;
    int NumEls;

    // lists of properties
    int NumBlockProps;
    int NumPBCs;
    int NumLineProps;
    int NumPointProps;
    int NumCircProps;
    int NumBlockLabels;
    int NumCircPropsOrig;

    CMaterialProp	*blockproplist;
    CBoundaryProp	*lineproplist;
    CPointProp		*nodeproplist;
    CCircuit		*circproplist;
    CBlockLabel		*labellist;
    CCommonPoint	*pbclist;
    // stuff usually kept track of by CDocument
    char *PathName;


// Operations
public:

    int LoadMesh();
    int LoadFEMFile();
    int Cuthill();
    int SortElements();
    int Static2D(CBigLinProb &L);
    int WriteStatic2D(CBigLinProb &L);
    int Harmonic2D(CBigComplexLinProb &L);
    int WriteHarmonic2D(CBigComplexLinProb &L);
    int StaticAxisymmetric(CBigLinProb &L);
    int HarmonicAxisymmetric(CBigComplexLinProb &L);
    void GetFillFactor(int lbl);
    double ElmArea(int i);
    void CleanUp();
    void AfxMessageBox(const char* message);
    void MsgBox(const char* message);

};
#endif

/////////////////////////////////////////////////////////////////////////////

double GetNewMu(double mu,int BHpoints, CComplex *BHdata,double muc,double B);
double Power(double x, int n);
