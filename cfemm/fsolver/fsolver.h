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

#ifndef FSOLVER_H
#define FSOLVER_H

#include <string>
#include <vector>
#include "feasolver.h"
#include "mmesh.h"
#include "cspars.h"
#include "CBlockLabel.h"
#include "CCircuit.h"
#include "CNode.h"
#include "CPointProp.h"

class FSolver : public FEASolver<
        femm::CPointProp
        , femm::CMBoundaryProp
        , fsolver::CMMaterialProp
        , femm::CMCircuit
        , femm::CMSolverBlockLabel
        , femm::CSolverNode
        >
{

// Attributes
public:

    FSolver();
    ~FSolver();

    // General problem attributes
    double  Relax;

    // mesh information
    femm::CSolverNode *meshnode;
    int NumCircPropsOrig;


// Operations
public:

    int LoadMesh(bool deleteFiles=true);
    bool LoadProblemFile();
    int Static2D(CBigLinProb &L);
    int WriteStatic2D(CBigLinProb &L);
    int Harmonic2D(CBigComplexLinProb &L);
    int WriteHarmonic2D(CBigComplexLinProb &L);
    int StaticAxisymmetric(CBigLinProb &L);
    int HarmonicAxisymmetric(CBigComplexLinProb &L);
    void GetFillFactor(int lbl);
    double ElmArea(int i);

private:

    void MsgBox(const char* message);
    virtual void CleanUp() override;

    // override parent class virtual method
    void SortNodes (int* newnum);

};

/////////////////////////////////////////////////////////////////////////////

double GetNewMu(double mu,int BHpoints, CComplex *BHdata,double muc,double B);

#endif
