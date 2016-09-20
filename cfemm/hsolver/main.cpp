//#undef min
//#undef max
//#define NOMINMAX
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
        sztibi82@gmail.com
        richard.crozier@yahoo.co.uk
*/

#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "femmcomplex.h"
#include "hspars.h"
#include "hmesh.h"
#include "hsolver.h"

//using namespace std;

//#include "lua.h"

int main(int argc, char** argv)
{
    HSolver theHSolver;
    char PathName[512];
    char outstr[1024];

    if (argc < 2)
    {
        // request the file name from the user
        printf("Enter feh file name without extension:\n");

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

    theHSolver.PathName = PathName;

    if (theHSolver.LoadProblemFile () != true)
    {
        theHSolver.WarnMessage("problem loading .feh file");
        return 1;
    }

    // load mesh
    int err = theHSolver.LoadMesh();
    if (err != 0)
    {
        theHSolver.WarnMessage("problem loading mesh:\n");

        switch (err)
	    {
	        case ( BADEDGEFILE ):
                theHSolver.WarnMessage("Could not open .edge file.\n");
                break;

	        case ( BADELEMENTFILE ):
                theHSolver.WarnMessage("Could not open .ele file.\n");
                break;

	        case( BADFEMFILE ):
                theHSolver.WarnMessage("Could not open .fem file.\n");
                break;

	        case( BADNODEFILE ):
                theHSolver.WarnMessage("Could not open .node file.\n");
                break;

	        case( BADPBCFILE ):
                theHSolver.WarnMessage("Could not open .pbc file.\n");
                break;

	        case( MISSINGMATPROPS ):
                theHSolver.WarnMessage("Material properties have not been defined for all regions.\n");
                break;

	        default:
                theHSolver.WarnMessage("AN unknown error occured.\n");
                break;
	    }

        return 2;
    }



    if (theHSolver.LoadPrev()==false)
    {
        printf("Loading previous solution\n");
    }

    // renumber using Cuthill-McKee
    printf("renumbering nodes\n");
    if (theHSolver.Cuthill() != true)
    {
        theHSolver.WarnMessage("problem renumbering node points");
        return 3;
    }

    printf("solving...");

    sprintf(outstr,"Problem Statistics:\n%i nodes\n%i elements\nPrecision: %3.2e\n",
            theHSolver.NumNodes,theHSolver.NumEls,theHSolver.Precision);

    printf(outstr);

    CHBigLinProb L;

    L.Precision = theHSolver.Precision;
    if (L.Create(theHSolver.NumNodes+theHSolver.NumCircProps,theHSolver.BandWidth)==false)
    {
        theHSolver.WarnMessage("couldn't allocate enough space for matrices");
        return 4;
    }

    if (theHSolver.AnalyzeProblem(L)==false)
    {
        theHSolver.WarnMessage("Couldn't solve the problem");
        return 5;
    }

    printf("Problem solved\n");

    if (theHSolver.WriteResults(L)==false)
    {
       theHSolver.WarnMessage("couldn't write results to disk");
       return 6;
    }
    printf("results written to disk\n");

    return 0;
}
