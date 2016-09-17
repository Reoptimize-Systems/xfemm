
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

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "femmcomplex.h"
//#include "spars.h"
//#include "mmesh.h"
#include "feasolver.h"
#include "fsolver.h"

//using namespace std;

//#include "lua.h"

int main(int argc, char** argv)
{
    FSolver theFSolver;
    char PathName[512];
    char outstr[1024];
//    int i;

    if (argc < 2)
    {
        // request the file name from the user
        printf("Enter fem file name without extension:\n");

        //char tempFilePath[512];

        //scanf("%s", tempFilePath);

        fgets(PathName, 512, stdin);
        char *pos;
        if ((pos=strchr(PathName, '\n')) != NULL)
            *pos = '\0';

        //PathName = tempFilePath;

    }
    else if(argc > 2)
    {
        printf("Too many arguments");
    }
    else
    {
        strcpy(PathName, argv[1]);
    }

    theFSolver.PathName = PathName;

    if (theFSolver.LoadProblemFile () != true)
    {
        theFSolver.WarnMessage("problem loading .fem file");
        return 1;
    }

    // load mesh
    int err = theFSolver.LoadMesh();
    if (err != 0)
    {
        theFSolver.WarnMessage("problem loading mesh:\n");

        switch (err)
	    {
	        case ( BADEDGEFILE ):
                theFSolver.WarnMessage("Could not open .edge file.\n");
                break;

	        case ( BADELEMENTFILE ):
                theFSolver.WarnMessage("Could not open .ele file.\n");
                break;

	        case( BADFEMFILE ):
                theFSolver.WarnMessage("Could not open .fem file.\n");
                break;

	        case( BADNODEFILE ):
                theFSolver.WarnMessage("Could not open .node file.\n");
                break;

	        case( BADPBCFILE ):
                theFSolver.WarnMessage("Could not open .pbc file.\n");
                break;

	        case( MISSINGMATPROPS ):
                theFSolver.WarnMessage("Material properties have not been defined for all regions.\n");
                break;

	        default:
                theFSolver.WarnMessage("AN unknown error occured.\n");
                break;
	    }

        return 2;
        //return -1;
    }

    // renumber using Cuthill-McKee
    printf("renumbering nodes");
    if (theFSolver.Cuthill() != true)
    {
        theFSolver.WarnMessage("problem renumbering node points");
        return 3;
    }

    printf("solving...");

    sprintf(outstr,"Problem Statistics:\n%i nodes\n%i elements\nPrecision: %3.2e\n",
            theFSolver.NumNodes,theFSolver.NumEls,theFSolver.Precision);

    printf(outstr);

//    double mr = (8.*((double) Doc.NumNodes)*((double) Doc.BandWidth)) / 1.e06;

    if(theFSolver.Frequency == 0)
    {

        CBigLinProb L;

        L.Precision = theFSolver.Precision;

        // initialize the problem, allocating the space required to solve it.
        if (L.Create(theFSolver.NumNodes, theFSolver.BandWidth) == false)
        {
            theFSolver.WarnMessage("couldn't allocate enough space for matrices");
            return 4;
        }

        // Create element matrices and solve the problem;
        if (theFSolver.ProblemType == false)
        {
            if (theFSolver.Static2D(L) == false)
            {
                theFSolver.WarnMessage("Couldn't solve the problem");
                return 5;
            }
            printf("Static 2-D problem solved");
        }
        else
        {

            if (theFSolver.StaticAxisymmetric(L) == false)
            {
                theFSolver.WarnMessage("Couldn't solve the problem");
                return 5;
            }
            printf("Static axisymmetric problem solved\n");
        }

        if (theFSolver.WriteStatic2D(L) == false)
        {
            theFSolver.WarnMessage("couldn't write results to disk");
            return 6;
        }
        printf("results written to disk\n");
    }
    else
    {
        CBigComplexLinProb L;

        L.Precision = theFSolver.Precision;

        // initialize the problem, allocating the space required to solve it.

        if (L.Create(theFSolver.NumNodes+theFSolver.NumCircProps, theFSolver.BandWidth, theFSolver.NumNodes) == false)
        {
            theFSolver.WarnMessage("couldn't allocate enough space for matrices");
            return 4;
        }

        // Create element matrices and solve the problem;
        if (theFSolver.ProblemType == false)
        {
            if (theFSolver.Harmonic2D(L) == false)
            {
                theFSolver.WarnMessage("Couldn't solve the problem");
                return 5;
            }
            printf("Harmonic 2-D problem solved");
        }
        else
        {
            if (theFSolver.HarmonicAxisymmetric(L) == false)
            {
                theFSolver.WarnMessage("Couldn't solve the problem");
                return 5;
            }
            printf("Harmonic axisymmetric problem solved");
        }


        if (theFSolver.WriteHarmonic2D(L)==false)
        {
            theFSolver.WarnMessage("couldn't write results to disk");
            return 6;
        }
        printf("results written to disk.");
    }

    //theFSolver.CleanUp();

    return 0;
}
