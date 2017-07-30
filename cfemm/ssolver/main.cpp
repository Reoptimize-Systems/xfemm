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

    Date Modified: 2017-07-28
    By:  Emoke Szelitzky
         Tibor Szelitzky
         Richard Crozier
         Johannes Zarl-Zierl
    Contact:
         szelitzkye@gmail.com
         sztibi82@gmail.com
         richard.crozier@yahoo.co.uk
         johannes@zarl-zierl.at

    Contributions by Johannes Zarl-Zierl were funded by
    Linz Center of Mechatronics GmbH (LCM)
*/

#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "femmcomplex.h"
#include "spars.h"
#include "ssolver.h"


int main(int argc, char** argv)
{
    SSolver solverInstance;
    char PathName[512];
    char outstr[1024];

    if (argc < 2)
    {
        // request the file name from the user
        printf("Enter fee file name without extension:\n");

        fgets(PathName, 512, stdin);
        char *pos;
        if ((pos=strchr(PathName, '\n')) != NULL)
            *pos = '\0';

    }
    else if(argc > 2)
    {
        printf("Too many arguments");
    }
    else
    {
        strcpy(PathName, argv[1]);
    }

    solverInstance.PathName = PathName;

    if (solverInstance.LoadProblemFile () != true)
    {
        solverInstance.WarnMessage("problem loading .fee file");
        return 1;
    }

    // load mesh
    int err = solverInstance.LoadMesh();
    if (err != 0)
    {
        solverInstance.WarnMessage("problem loading mesh:\n");
        switch (err)
        {
        case ( BADEDGEFILE ):
            solverInstance.WarnMessage("Could not open .edge file.\n");
            break;

        case ( BADELEMENTFILE ):
            solverInstance.WarnMessage("Could not open .ele file.\n");
            break;

        case( BADFEMFILE ):
            solverInstance.WarnMessage("Could not open .fem file.\n");
            break;

        case( BADNODEFILE ):
            solverInstance.WarnMessage("Could not open .node file.\n");
            break;

        case( BADPBCFILE ):
            solverInstance.WarnMessage("Could not open .pbc file.\n");
            break;

        case( MISSINGMATPROPS ):
            solverInstance.WarnMessage("Material properties have not been defined for all regions.\n");
            break;

        default:
            solverInstance.WarnMessage("AN unknown error occured.\n");
            break;
        }

        return 2;
    }

    // renumber using Cuthill-McKee
    printf("renumbering nodes\n");
    if (solverInstance.Cuthill() != true)
    {
        solverInstance.WarnMessage("problem renumbering node points");
        return 3;
    }

    printf("solving...");

    sprintf(outstr,"Problem Statistics:\n%i nodes\n%i elements\nPrecision: %3.2e\n",
            solverInstance.NumNodes,solverInstance.NumEls,solverInstance.Precision);

    printf(outstr);

    CBigLinProb L;

    L.Precision = solverInstance.Precision;
    if (L.Create(solverInstance.NumNodes+solverInstance.NumCircProps,solverInstance.BandWidth)==false)
    {
        solverInstance.WarnMessage("couldn't allocate enough space for matrices");
        return 4;
    }

    if (solverInstance.AnalyzeProblem(L)==false)
    {
        solverInstance.WarnMessage("Couldn't solve the problem");
        return 5;
    }

    printf("Problem solved\n");

    if (solverInstance.WriteResults(L)==false)
    {
        solverInstance.WarnMessage("couldn't write results to disk");
        return 6;
    }
    printf("results written to disk\n");

    return 0;
}
