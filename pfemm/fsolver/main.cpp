
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
#include "complex.h"
#include "spars.h"
#include "mesh.h"
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

    if (theFSolver.LoadFEMFile() != TRUE)
    {
        theFSolver.WarnMessage("problem loading .fem file");
        exit(1);
    }

    // load mesh
    if (theFSolver.LoadMesh() != TRUE)
    {
        theFSolver.WarnMessage("problem loading mesh");
        exit(2);
        //return -1;
    }

    // renumber using Cuthill-McKee
    printf("renumbering nodes");
    if (theFSolver.Cuthill() != TRUE)
    {
        theFSolver.WarnMessage("problem renumbering node points");
        exit(3);
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
        if (L.Create(theFSolver.NumNodes, theFSolver.BandWidth) == FALSE)
        {
            theFSolver.WarnMessage("couldn't allocate enough space for matrices");
            exit(4);
        }

        // Create element matrices and solve the problem;
        if (theFSolver.ProblemType == FALSE)
        {
            if (theFSolver.Static2D(L) == FALSE)
            {
                theFSolver.WarnMessage("Couldn't solve the problem");
                exit(5);
            }
            printf("Static 2-D problem solved");
        }
        else
        {

            if (theFSolver.StaticAxisymmetric(L) == FALSE)
            {
                theFSolver.WarnMessage("Couldn't solve the problem");
                exit(5);
            }
            printf("Static axisymmetric problem solved\n");
        }

        if (theFSolver.WriteStatic2D(L) == FALSE)
        {
            theFSolver.WarnMessage("couldn't write results to disk");
            exit(6);
        }
        printf("results written to disk\n");
    }
    else
    {
        CBigComplexLinProb L;

        L.Precision = theFSolver.Precision;

        // initialize the problem, allocating the space required to solve it.

        if (L.Create(theFSolver.NumNodes+theFSolver.NumCircProps, theFSolver.BandWidth, theFSolver.NumNodes) == FALSE)
        {
            theFSolver.WarnMessage("couldn't allocate enough space for matrices");
            exit(4);
        }

        // Create element matrices and solve the problem;
        if (theFSolver.ProblemType == FALSE)
        {
            if (theFSolver.Harmonic2D(L) == FALSE)
            {
                theFSolver.WarnMessage("Couldn't solve the problem");
                exit(5);
            }
            printf("Harmonic 2-D problem solved");
        }
        else
        {
            if (theFSolver.HarmonicAxisymmetric(L) == FALSE)
            {
                theFSolver.WarnMessage("Couldn't solve the problem");
                exit(5);
            }
            printf("Harmonic axisymmetric problem solved");
        }


        if (theFSolver.WriteHarmonic2D(L)==FALSE)
        {
            theFSolver.WarnMessage("couldn't write results to disk");
            exit(6);
        }
        printf("results written to disk.");
    }

    theFSolver.CleanUp();

    exit(0);
}
