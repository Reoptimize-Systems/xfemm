#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "fsolver.h"
#include "spars.h"
#include "mesh.h"

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
void voidmexPrintF(const char *message);
void dummymexPrintF(const char *message);

/* the gateway function */
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    FSolver SolveObj;
    std::string FilePath;
    char *inputbuf;
    mwSize buflen;
    int status;
    bool verbose = false;

    //(void) plhs;    /* unused parameters */

    /* Check for proper number of input and output arguments */
    if (nrhs != 3)
    {
        mexErrMsgIdAndTxt("MFEMM:fsolver:nargin", 
          "Three input arguments required.");
    }

    if (nlhs > 1)
    {
        mexErrMsgIdAndTxt("MFEMM:fsolver:nargout", 
          "Too many output arguments.");
    }

    /* Check for proper input type */
    if (!mxIsChar(prhs[0]) || (mxGetM(prhs[0]) != 1 ) )
    {
        mexErrMsgIdAndTxt("MFEMM:fsolver:badinput", 
          "First input argument must be a string.");
    }

    /* Find out how long the input string is.  Allocate enough memory
       to hold the converted string.  NOTE: MATLAB stores characters
       as 2 byte unicode ( 16 bit ASCII) on machines with multi-byte
       character sets.  You should use mxChar to ensure enough space
       is allocated to hold the string */

    buflen = mxGetN (prhs[0]) * sizeof (mxChar)+1;
    inputbuf = (char *) mxMalloc (buflen);

    /* Copy the string data into buf. */
    status = mxGetString (prhs[0], inputbuf, buflen);
    
    if ((!mxIsNumeric(prhs[1])) || (mxGetM(prhs[1]) != 1) || (mxGetN(prhs[1]) != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fsolver:inputnotscalar",
                           "Second input must be a scalar.");
    }
    
    /* get the verbosity flag */
    verbose = (bool)mxGetScalar (prhs[1]);
    
    if ((!mxIsNumeric(prhs[2])) || (mxGetM(prhs[2]) != 1) || (mxGetN(prhs[2]) != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fsolver:inputnotscalar",
                           "Third input must be a scalar.");
    }
    
    /* get the flag deermiing if mesh files are deleted after loading */
    bool deleteMeshFiles = (bool)mxGetScalar (prhs[2]);

    if (verbose != 0.0)
    {
        mexPrintf("Solving file: %s\n",inputbuf);
        // if verbose make the warning message function point to mexPrintf
        SolveObj.WarnMessage = &voidmexPrintF;
    }
    else
    {
        // if not verbose make the warning message function point to a dummy
        // function
        SolveObj.WarnMessage = &dummymexPrintF;
    }
    
    // Tell FSolver the location of the mesh and fem files this should be
    // an extension free base name
    SolveObj.PathName = inputbuf;
    
    // free the input buffer
    mxFree(inputbuf);
    
    status = SolveObj.LoadProblemFile ();
    
    if (status != true)
    {
        mexPrintf("problem loading .fem file\n");
        plhs[0] = mxCreateDoubleScalar (1.0);
        return;
    }

    // load mesh
    status = SolveObj.LoadMesh(deleteMeshFiles);
    if (status != 0)
    {

        mexPrintf("problem loading mesh\n");
        plhs[0] = mxCreateDoubleScalar(status);

        switch (status)
        {
        case ( BADEDGEFILE ):

            mexErrMsgIdAndTxt("MFEMM:fsolver:badedgefile", 
                "Could not open .edge file.\n");
            break;

        case ( BADELEMENTFILE ):
            mexErrMsgIdAndTxt("MFEMM:fsolver:badelefile", 
                "Could not open .ele file.\n");
            break;

        case( BADFEMFILE ):
            mexErrMsgIdAndTxt("MFEMM:fsolver:badfemfile", 
                "Could not open .fem file.\n");
            break;

        case( BADNODEFILE ):
            mexErrMsgIdAndTxt("MFEMM:fsolver:badnodefile", 
                "Could not open .node file.\n");
            break;

        case( BADPBCFILE ):
            mexErrMsgIdAndTxt("MFEMM:fsolver:badpbcfile", 
                "Could not open .pbc file.\n");
            break;

        case( MISSINGMATPROPS ):
            mexErrMsgIdAndTxt("MFEMM:fsolver:missingmatprops", 
                "Material properties have not been defined for all regions.\n");
            break;

        default:
            mexErrMsgIdAndTxt("MFEMM:fsolver:unknown", 
                "An unknown error occured.\n");
            break;
        }

        return;
        //return -1;
    }

    // renumber using Cuthill-McKee
    if (verbose != 0.0) mexPrintf("renumbering nodes");
      
    if (SolveObj.Cuthill(deleteMeshFiles) != true)
    {
        if (verbose != 0.0) mexPrintf("problem renumbering node points\n");
        plhs[0] = mxCreateDoubleScalar(3.0);
        return;
    }

    if (verbose != 0.0) mexPrintf("solving...");

    if (verbose != 0.0) mexPrintf("Problem Statistics:\n%i nodes\n%i elements\nPrecision: %3.2e\n",
              SolveObj.NumNodes,SolveObj.NumEls,SolveObj.Precision);
//
//     mexPrintf(outstr);

    //double mr = (8.*((double) SolveObj.NumNodes)*((double) SolveObj.BandWidth)) / 1.e06;

    if(SolveObj.Frequency == 0)
    {

        CBigLinProb L;

        L.Precision = SolveObj.Precision;

        // initialize the problem, allocating the space required to solve it.
        if (L.Create(SolveObj.NumNodes, SolveObj.BandWidth) == false)
        {
            mexPrintf("couldn't allocate enough space for matrices\n");
            plhs[0] = mxCreateDoubleScalar(4.0);
            return;
        }

        // Create element matrices and solve the problem;
        if (SolveObj.ProblemType == false)
        {
            if (SolveObj.Static2D(L) == false)
            {
                mexPrintf("Couldn't solve the problem\n");
                plhs[0] = mxCreateDoubleScalar(5.0);
                return;
            }
            if (verbose != 0.0) mexPrintf("Static 2-D problem solved\n");
        }
        else
        {

            if (SolveObj.StaticAxisymmetric(L) == false)
            {
                mexPrintf("Couldn't solve the problem\n");
                plhs[0] = mxCreateDoubleScalar(5.0);
                return;
            }
            if (verbose != 0.0) mexPrintf("Static axisymmetric problem solved\n");
        }

        if (SolveObj.WriteStatic2D(L) == false)
        {
            mexPrintf("couldn't write results to disk\n");
            plhs[0] = mxCreateDoubleScalar(6.0);
            return;
        }
        if (verbose != 0.0) mexPrintf("results written to disk\n");
    }
    else
    {
        CBigComplexLinProb L;

        L.Precision = SolveObj.Precision;

        // initialize the problem, allocating the space required to solve it.

        if (L.Create(SolveObj.NumNodes+SolveObj.NumCircProps, SolveObj.BandWidth, SolveObj.NumNodes) == false)
        {
            mexPrintf("couldn't allocate enough space for matrices\n");
            plhs[0] = mxCreateDoubleScalar(4.0);
            return;
        }

        // Create element matrices and solve the problem;
        if (SolveObj.ProblemType == false)
        {
            if (SolveObj.Harmonic2D(L) == false)
            {
                mexPrintf("Couldn't solve the problem\n");
                plhs[0] = mxCreateDoubleScalar(5.0);
                return;
            }
            if (verbose != 0.0) mexPrintf("Harmonic 2-D problem solved\n");
        }
        else
        {
            if (SolveObj.HarmonicAxisymmetric(L) == false)
            {
                mexPrintf("Couldn't solve the problem\n");
                plhs[0] = mxCreateDoubleScalar(5.0);
                return;
            }
            if (verbose != 0.0) mexPrintf("Harmonic axisymmetric problem solved\n");
        }


        if (SolveObj.WriteHarmonic2D(L)==false)
        {
            mexPrintf("couldn't write results to disk\n");
            plhs[0] = mxCreateDoubleScalar(6.0);
            return;
        }
        if (verbose != 0.0) mexPrintf("results written to disk.\n");
    }

    plhs[0] = mxCreateDoubleScalar(0.0);

}


void voidmexPrintF(const char *message)
{
    mexPrintf(message);
}

void dummymexPrintF(const char *message)
{

}
