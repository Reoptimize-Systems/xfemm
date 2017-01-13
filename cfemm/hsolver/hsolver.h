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

#ifndef HSOLVER_H
#define HSOLVER_H

#include <string>
#include "feasolver.h"
#include "hmesh.h"
#include "hspars.h"
#include "CNode.h"
#include "CBlockLabel.h"
#include "CBoundaryProp.h"

class HSolver : public FEASolver<
        CHPointProp
        , femm::CHBoundaryProp
        , CHMaterialProp
        , CHConductor
        , femm::CBlockLabel
        , femm::CNode
        >
{

// Attributes
public:

	HSolver();
	~HSolver();


    // General problem attributes
    double	dT; ///< \brief delta T used by hsolver \verbatim[dT]\endverbatim

    // mesh information
    femm::CNode *meshnode;

	// Vector containing previous solution for time-transient analysis
	double *Tprev;


// Operations
public:

    int LoadMesh(bool deleteFiles=true);
    int LoadPrev();
    bool LoadProblemFile();
    double ChargeOnConductor(int OnConductor, CHBigLinProb &L);
	int WriteResults(CHBigLinProb &L);
    int AnalyzeProblem(CHBigLinProb &L);
    void (*WarnMessage)(const char*);

private:

    void MsgBox(const char* message);
    void CleanUp();

    // override parent class virtual method
    void SortNodes (int* newnum);

    virtual bool handleToken(const std::string &token, std::istream &input, std::ostream &err) override;

};

#endif
