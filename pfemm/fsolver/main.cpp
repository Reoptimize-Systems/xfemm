
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
	FSolver Doc;
	char PathName[512];
	char outstr[1024];
	int i;

    if (argc < 2)
    {
        // request the file name from the user
        printf("Enter fem file name without extension:\n");

        char tempFilePath[512];

        //scanf("%s", tempFilePath);

        gets(PathName);

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

	Doc.PathName = PathName;

	if (Doc.LoadFEMFile() != TRUE){
		printf("problem loading .fem file");
		exit(1);
	}

	// load mesh
	if (Doc.LoadMesh() != TRUE){
		printf("problem loading mesh");
		exit(2);
		//return -1;
	}

	// renumber using Cuthill-McKee
    printf("renumbering nodes");
	if (Doc.Cuthill() != TRUE){
		printf("problem renumbering node points");
		exit(3);
	}

    printf("solving...");

	sprintf(outstr,"Problem Statistics:\n%i nodes\n%i elements\nPrecision: %3.2e\n",
			Doc.NumNodes,Doc.NumEls,Doc.Precision);

    printf(outstr);

	double mr = (8.*((double) Doc.NumNodes)*((double) Doc.BandWidth)) / 1.e06;

	if(Doc.Frequency == 0){

		CBigLinProb L;

		L.Precision = Doc.Precision;

		// initialize the problem, allocating the space required to solve it.
		if (L.Create(Doc.NumNodes, Doc.BandWidth) == FALSE)
		{
            printf("couldn't allocate enough space for matrices");
			exit(4);
		}

		// Create element matrices and solve the problem;
		if (Doc.ProblemType == FALSE)
		{
			if (Doc.Static2D(L) == FALSE)
			{
				Doc.AfxMessageBox("Couldn't solve the problem");
				exit(5);
			}
			printf("Static 2-D problem solved");
		}
		else{

			if (Doc.StaticAxisymmetric(L) == FALSE)
			{
				Doc.AfxMessageBox("Couldn't solve the problem");
				exit(5);
			}
            printf("Static axisymmetric problem solved");
		}

		if (Doc.WriteStatic2D(L) == FALSE)
		{
			Doc.AfxMessageBox("couldn't write results to disk");
			exit(6);
		}
        printf("results written to disk");
	}
	else
	{
		CBigComplexLinProb L;

		L.Precision = Doc.Precision;

		// initialize the problem, allocating the space required to solve it.

		if (L.Create(Doc.NumNodes+Doc.NumCircProps, Doc.BandWidth, Doc.NumNodes) == FALSE)
		{
            Doc.AfxMessageBox("couldn't allocate enough space for matrices");
			exit(4);
		}

		// Create element matrices and solve the problem;
		if (Doc.ProblemType == FALSE)
		{
			if (Doc.Harmonic2D(L) == FALSE)
			{
				Doc.AfxMessageBox("Couldn't solve the problem");
				exit(5);
			}
			printf("Harmonic 2-D problem solved");
		}
		else
		{
            if (Doc.HarmonicAxisymmetric(L) == FALSE)
            {
                Doc.AfxMessageBox("Couldn't solve the problem");
                exit(5);
            }
            printf("Harmonic axisymmetric problem solved");
		}


		if (Doc.WriteHarmonic2D(L)==FALSE)
		{
			Doc.AfxMessageBox("couldn't write results to disk");
			exit(6);
		}
        printf("results written to disk.");
	}

	Doc.CleanUp();

	exit(0);
}
