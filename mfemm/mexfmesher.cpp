#include <iostream>

// If the windows flag is defined we use diferent file separators
// normally you would set this using the -DWINDOWS mex compiler option
// when compiling on windows
// #define WINDOWS

#include <string>
#include "fmesher.h"

#include "mex.h"

/*
 * mexfmesher.cpp
 *
 * gateway to fmesher program
 *
 * This is a MEX-file for MATLAB.
 *
 */

// extern void _main();

/* the gateway function */
void mexFunction( int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[])
{
    FMesher MeshObj;
    std::string FilePath;
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
    mexPrintf("Meshing file:  %s\n", buf);

    /* NOTE: You could add your own code here to manipulate
       the string */
    FilePath = buf;

    /* When finished using the string, deallocate it. */
    mxFree(buf);

    /*  call the FMesher subroutines */
    if (MeshObj.LoadFEMFile(FilePath) == false)
    {
        plhs[0] = mxCreateDoubleScalar(-1.0);
        return;
    }

    if (MeshObj.HasPeriodicBC() == true)
    {
        if (MeshObj.DoPeriodicBCTriangulation(FilePath) == false)
        {
            plhs[0] = mxCreateDoubleScalar(-2.0);
            return;
        }
    }
    else
    {
        if (MeshObj.DoNonPeriodicBCTriangulation(FilePath) == false)
        {
            plhs[0] = mxCreateDoubleScalar(-3.0);
            return;
        }
    }

}
