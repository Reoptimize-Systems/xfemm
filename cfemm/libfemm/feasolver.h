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

// feasolver.h : interface of the generic feasolver class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef FEASOLVER_H
#define FEASOLVER_H

#include <string>
#include "mesh.h"
#include "spars.h"

enum LoadMeshErr
{
  BADFEMFILE,
  BADNODEFILE,
  BADPBCFILE,
  BADELEMENTFILE,
  BADEDGEFILE,
  MISSINGMATPROPS
};

class FEASolver
{

// Attributes
public:

    FEASolver();
    ~FEASolver();

    // General problem attributes
    double  Precision;
    int		LengthUnits;
    int		ACSolver;
    int     ProblemType;
    int	    Coords;
    bool    DoForceMaxMeshArea;
    bool    bMultiplyDefinedLabels;

    // axisymmetric external region parameters
    double  extRo,extRi,extZo;

    // CArrays containing the mesh information
    int	BandWidth;
    femm::CElement *meshele;

    int NumNodes;
    int NumEls;

    // lists of properties
    int NumBlockProps;
    int NumPBCs;
    int NumLineProps;
    int NumPointProps;
    int NumCircProps;
    int NumBlockLabels;

    femm::CBoundaryProp	*lineproplist;
    femm::CCommonPoint	*pbclist;

    // string to hold the location of the files
    std::string PathName;

// Operations
public:

    virtual int LoadMesh(bool deleteFiles=true) = 0;
    virtual int LoadProblemFile () = 0;
    int Cuthill(bool deleteFiles=true);
    int SortElements();

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);

private:

    virtual void SortNodes (int* newnum) = 0;

};

/////////////////////////////////////////////////////////////////////////////

double Power(double x, int n);

#endif
