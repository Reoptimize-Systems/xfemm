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

   Date Modified: 2014 - 03 - 21
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
   Contact:
        szelitzkye@gmail.com
        sztibi82@yahoo.com
        richard.crozier@yahoo.co.uk
*/

// hsolver.h : interface of the HSolver class
//
/////////////////////////////////////////////////////////////////////////////
#include <string>

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
#include "hspars.h"

#ifndef FSOLVER_H
#define FSOLVER_H

enum LoadMeshErr
{
  BADFEMFILE,
  BADNODEFILE,
  BADPBCFILE,
  BADELEMENTFILE,
  BADEDGEFILE,
  MISSINGMATPROPS
};

class HSolver
{

// Attributes
public:


	HSolver();
	~HSolver();


 // General problem attributes
	double  Precision;
	double  Depth;
	int		LengthUnits;
	int    ProblemType;
	int	Coords;
	double	dT;
	char *PrevSoln;

	// Axisymmetric external region parameters
	double extRo,extRi,extZo;

	//ChsolvDlg *TheView;

	// CArrays containing the mesh information
	int	BandWidth;
	CNode *meshnode;
	CElement	*meshele;

	int NumNodes;
	int NumEls;

	// Vector containing previous solution for time-transient analysis
	double *Tprev;

	// lists of properties
	int NumBlockProps;
	int NumPBCs;
	int NumLineProps;
	int NumPointProps;
	int NumCircProps;
	int NumBlockLabels;

	CMaterialProp	*blockproplist;
	CBoundaryProp	*lineproplist;
	CPointProp		*nodeproplist;
	CCircuit		*circproplist;
	CBlockLabel		*labellist;
	CCommonPoint	*pbclist;

	// string to hold the location of the files
    std::string PathName;


// Operations
public:

    int LoadMesh();
    int LoadPrev();
	int LoadFEHFile();
	void CleanUp();
    int Cuthill();
	int SortElements();
	void MsgBox(const char* message);
    double ChargeOnConductor(int OnConductor, CHBigLinProb &L);
	int WriteResults(CHBigLinProb &L);

    int AnalyzeProblem(CHBigLinProb &L);
    void (*WarnMessage)(const char*);

};

/////////////////////////////////////////////////////////////////////////////

double Power(double x, int n);
#endif
