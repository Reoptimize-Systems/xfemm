#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>

// If the windows flag is defined we use diferent file separators
// normally you would set this using the -DWINDOWS mex compiler option
// when compiling on windows
// #define WINDOWS

#ifdef WINDOWS
#include "fsolver\stdstring.h"
#include "fsolver\fsolver.h"
#include "fsolver\complex.h"
#include "fsolver\spars.h"
#include "fsolver\mesh.h"
#else
#include "fsolver/stdstring.h"
#include "fsolver/fsolver.h"
#include "fsolver/complex.h"
#include "fsolver/spars.h"
#include "fsolver/mesh.h"
#endif

#include "mex.h"

/*
 * mexfsolver.cpp
 *
 * gateway to fsolver program
 *
 * This is a MEX-file for MATLAB.
 *
 */

// extern void _main();

/* the gateway function */
void mexFunction( int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[])
{
    FSolver SolveObj;
    CStdString FilePath;
    char *buf;
    mwSize buflen;
    int status;

    //(void) plhs;    /* unused parameters */

    /* Check for proper number of input and output arguments */
    if (nrhs != 1) {
        mexErrMsgTxt("One input argument required.");
    }

    if (nlhs > 1) {
        mexErrMsgTxt("Too many output arguments.");
    }

    /* Check for proper input type */
    if (!mxIsChar(prhs[0]) || (mxGetM(prhs[0]) != 1 ) )  {
        mexErrMsgTxt("Input argument must be a string.");
    }

    /* Find out how long the input string is.  Allocate enough memory
       to hold the converted string.  NOTE: MATLAB stores characters
       as 2 byte unicode ( 16 bit ASCII) on machines with multi-byte
       character sets.  You should use mxChar to ensure enough space
       is allocated to hold the string */

    buflen = mxGetN(prhs[0])*sizeof(mxChar)+1;
    buf = (char *) mxMalloc(buflen);

    /* Copy the string data into buf. */
    status = mxGetString(prhs[0], buf, buflen);
    mexPrintf("Solving file: %s\n", buf);

    /* NOTE: You could add your own code here to manipulate
       the string */
//     FilePath


    // Tell FSolver the location of the mesh and fem files this should be 
    // an extension free base name
    SolveObj.PathName = buf;
    
    /* When finished using the string, deallocate it. */
    //mxFree(buf);
    
	if (SolveObj.LoadFEMFile() != TRUE){
		mexPrintf("problem loading .fem file");
		plhs[0] = mxCreateDoubleScalar(1.0);
        return;
	}

	// load mesh
	if (SolveObj.LoadMesh() != TRUE){
		mexPrintf("problem loading mesh");
		plhs[0] = mxCreateDoubleScalar(2.0);
        return;
		//return -1;
	}

	// renumber using Cuthill-McKee
    mexPrintf("renumbering nodes");
	if (SolveObj.Cuthill() != TRUE){
		mexPrintf("problem renumbering node points");
		plhs[0] = mxCreateDoubleScalar(3.0);
        return;
	}

    mexPrintf("solving...");

	mexPrintf("Problem Statistics:\n%i nodes\n%i elements\nPrecision: %3.2e\n",
			SolveObj.NumNodes,SolveObj.NumEls,SolveObj.Precision);
// 
//     mexPrintf(outstr);

	double mr = (8.*((double) SolveObj.NumNodes)*((double) SolveObj.BandWidth)) / 1.e06;

	if(SolveObj.Frequency == 0){

		CBigLinProb L;

		L.Precision = SolveObj.Precision;

		// initialize the problem, allocating the space required to solve it.
		if (L.Create(SolveObj.NumNodes, SolveObj.BandWidth) == FALSE)
		{
            mexPrintf("couldn't allocate enough space for matrices");
			plhs[0] = mxCreateDoubleScalar(4.0);
            return;
		}

		// Create element matrices and solve the problem;
		if (SolveObj.ProblemType == FALSE)
		{
			if (SolveObj.Static2D(L) == FALSE)
			{
				mexPrintf("Couldn't solve the problem");
				plhs[0] = mxCreateDoubleScalar(5.0);
                return;
			}
			mexPrintf("Static 2-D problem solved");
		}
		else{

			if (SolveObj.StaticAxisymmetric(L) == FALSE)
			{
				mexPrintf("Couldn't solve the problem");
				plhs[0] = mxCreateDoubleScalar(5.0);
                return;
			}
            mexPrintf("Static axisymmetric problem solved");
		}

		if (SolveObj.WriteStatic2D(L) == FALSE)
		{
			mexPrintf("couldn't write results to disk");
			plhs[0] = mxCreateDoubleScalar(6.0);
            return;
		}
        mexPrintf("results written to disk");
	}
	else
	{
		CBigComplexLinProb L;

		L.Precision = SolveObj.Precision;

		// initialize the problem, allocating the space required to solve it.

		if (L.Create(SolveObj.NumNodes+SolveObj.NumCircProps, SolveObj.BandWidth, SolveObj.NumNodes) == FALSE)
		{
            mexPrintf("couldn't allocate enough space for matrices");
			plhs[0] = mxCreateDoubleScalar(4.0);
            return;
		}

		// Create element matrices and solve the problem;
		if (SolveObj.ProblemType == FALSE)
		{
			if (SolveObj.Harmonic2D(L) == FALSE)
			{
				mexPrintf("Couldn't solve the problem");
				plhs[0] = mxCreateDoubleScalar(5.0);
                return;
			}
			mexPrintf("Harmonic 2-D problem solved");
		}
		else
		{
            if (SolveObj.HarmonicAxisymmetric(L) == FALSE)
            {
                mexPrintf("Couldn't solve the problem");
                plhs[0] = mxCreateDoubleScalar(5.0);
                return;
            }
            mexPrintf("Harmonic axisymmetric problem solved");
		}


		if (SolveObj.WriteHarmonic2D(L)==FALSE)
		{
			mexPrintf("couldn't write results to disk");
			plhs[0] = mxCreateDoubleScalar(6.0);
            return;
		}
        mexPrintf("results written to disk.");
	}

	SolveObj.CleanUp();

	plhs[0] = mxCreateDoubleScalar(0.0);

}
