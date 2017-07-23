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
		  Johannes Zarl-Zierl
   Contact:
        szelitzkye@gmail.com
        sztibi82@yahoo.com
        richard.crozier@yahoo.co.uk
		  johannes.zarl-zierl@jku.at

	Contributions by Johannes Zarl-Zierl were funded by
	Linz Center of Mechatronics GmbH (LCM)
*/

// esolver.h : interface of the ESolver class

#ifndef ESOLVER_H
#define ESOLVER_H

#include "feasolver.h"
#include "spars.h"
#include "CBlockLabel.h"
#include "CBoundaryProp.h"
#include "CCircuit.h"
#include "CNode.h"
#include "CMaterialProp.h"
#include "CPointProp.h"

#include <string>

class ESolver : public FEASolver<
        femm::CSPointProp
        , femm::CSBoundaryProp
        , femm::CSMaterialProp
        , femm::CSCircuit
        , femm::CSBlockLabel
        , femm::CNode
        >
{

// Attributes
public:

	ESolver();
	~ESolver();


    // mesh information
    femm::CNode *meshnode;

// Operations
public:

    int LoadMesh(bool deleteFiles=true);
    bool LoadProblemFile();
    double ChargeOnConductor(int OnConductor, CBigLinProb &L);
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
