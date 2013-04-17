#include <iostream>

// If the windows flag is defined we use diferent file separators
// normally you would set this using the -DWINDOWS mex compiler option
// when compiling on windows
// #define WINDOWS

#include <string>
#include "fmesher.h"
// include triangle.h for error code definitions
#include "triangle.h"

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
    char *buf;
    mwSize buflen;
    int status,tristatus;
    bool hasperiodic;

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
    std::string FilePath(buf);

    /* When finished using the string, deallocate it. */
    mxFree(buf);

    /*  call the FMesher subroutines */
    if (MeshObj.LoadFEMFile(FilePath) == false)
    {
        plhs[0] = mxCreateDoubleScalar(-1.0);
        return;
    }

    hasperiodic = MeshObj.HasPeriodicBC();
    if (hasperiodic == true)
    {
        tristatus = MeshObj.DoPeriodicBCTriangulation(FilePath);

        if (tristatus == -1)
        {
            tristatus = -2;
        }
//        if (MeshObj.DoPeriodicBCTriangulation(FilePath) != 0)
//        {
//            plhs[0] = mxCreateDoubleScalar(-2.0);
//            return;
//        }
    }
    else
    {
        tristatus = MeshObj.DoNonPeriodicBCTriangulation(FilePath) ;

        if (tristatus == -1)
        {
            tristatus = -3;
        }
//        if (MeshObj.DoNonPeriodicBCTriangulation(FilePath) != 0)
//        {
//            plhs[0] = mxCreateDoubleScalar(-3.0);
//            return;
//        }
    }

    switch (tristatus)
    {
        case 0
            plhs[0] = mxCreateDoubleScalar(0.0);
            return tristatus;
        case -1
            plhs[0] = mxCreateDoubleScalar((double)tristatus);
            return -2;
        case TRIERR_OUT_OF_MEM
            plhs[0] = mxCreateDoubleScalar((double)tristatus);
            mexErrMsgTxt("OUT OF MEM");
            return tristatus;
        case TRIERR_INTERNAL_ERR
            mexErrMsgTxt("INTERNAL ERR");
            return tristatus;
        case TRIERR_ZERO_MAX_AREA
            mexErrMsgTxt("ZERO MAX AREA");
            return tristatus;
        case TRIERR_I_SWITCH_WITH_REFINE
            mexErrMsgTxt("I SWITCH WITH REFINE");
            return tristatus;
        case TRIERR_INPUT_VERTICES_IDENTICAL
            mexErrMsgTxt("INPUT VERTICES IDENTICAL");
            return tristatus;
        case TRIERR_NOT_ENOUGH_VERTICES
            mexErrMsgTxt("NOT ENOUGH VERTICES");
            return tristatus;
        case TRIERR_CANNOT_ACCESS_ELEFILE
            mexErrMsgTxt("CANNOT ACCESS ELEFILE");
            return tristatus;
        case TRIERR_CANNOT_ACCESS_AREAFILE
            mexErrMsgTxt("CANNOT ACCESS AREAFILE");
            return tristatus;
        case TRIERR_ELEFILE_AND_AREAFILE_DIFF_N_TRIANGLES
            mexErrMsgTxt("ELEFILE AND AREAFILE DIFF N TRIANGLES");
            return tristatus;
        case TRIERR_TRI_INVALID_VERTEX_INDEX
            mexErrMsgTxt("TRI INVALID VERTEX INDEX");
            return tristatus;
        case TRIERR_MISSING_VERTEX_INDEX
            mexErrMsgTxt("MISSING VERTEX_INDEX");
            return tristatus;
        case TRIERR_SEGMENT_NO_END_POINTS
            mexErrMsgTxt("SEGMENT NO END POINTS");
            return tristatus;
        case TRIERR_SEGMENT_NO_SECOND_END_POINT
            mexErrMsgTxt("SEGMENT NO SECOND END POINT");
            return tristatus;
        case TRIERR_SEG_INVALID_VERTEX_INDEX
            mexErrMsgTxt("SEG INVALID VERTEX INDEX");
            return tristatus;
        case TRIERR_SPLIT_SEG_TOO_SMALL
            mexErrMsgTxt("SPLIT SEG TOO SMALL");
            return tristatus;
        case TRIERR_UNEXPECTED_EOF
            mexErrMsgTxt("UNEXPECTED EOF 16");
            return tristatus;
        case TRIERR_CANNOT_ACCESS_POLYFILE
            mexErrMsgTxt("CANNOT ACCESS POLYFILE");
            return tristatus;
        case TRIERR_CANNOT_ACCESS_NODEFILE
            mexErrMsgTxt("CANNOT ACCESS NODEFILE");
            return tristatus;
        case TRIERR_MUST_HAVE_THREE_VERTICES
            mexErrMsgTxt("MUST HAVE THREE VERTICES");
            return tristatus;
        case TRIERR_TOO_MANY_MESH_DIMENSIONS
            mexErrMsgTxt("TOO MANY MESH DIMENSIONS");
            return tristatus;
        case TRIERR_VERTEX_NO_X
            mexErrMsgTxt("VERTEX NO X");
            return tristatus;
        case TRIERR_VERTEX_NO_Y
            mexErrMsgTxt("VERTEX NO Y");
            return tristatus;
        case TRIERR_HOLE_NO_X
            mexErrMsgTxt("HOLE NO X");
            return tristatus;
        case TRIERR_HOLE_NO_Y
            mexErrMsgTxt("HOLE NO Y");
            return tristatus;
        case TRIERR_REGION_NO_X
            mexErrMsgTxt("REGION NO Y");
            return tristatus;
        case TRIERR_REGION_NO_Y
            mexErrMsgTxt("REGION NO Y");
            return tristatus;
        case TRIERR_REGION_NO_ATTRIBUTE_OR_AREA_CON
            mexErrMsgTxt("REGION NO ATTRIBUTE OR AREA CON");
            return tristatus;
        case TRIERR_CANNOT_CREATE_NODEFILE
            mexErrMsgTxt("CANNOT CREATE NODEFILE");
            return tristatus;
        case TRIERR_CANNOT_CREATE_ELEFILE
            mexErrMsgTxt("CANNOT CREATE ELEFILE");
            return tristatus;
        case TRIERR_CANNOT_CREATE_POLYFILE
            mexErrMsgTxt("CANNOT CREATE POLYFILE");
            return tristatus;
        case TRIERR_CANNOT_CREATE_EDGEFILE
            mexErrMsgTxt("CANNOT CREATE EDGEFILE");
            return tristatus;
        case TRIERR_CANNOT_CREATE_VNODEFILE
            mexErrMsgTxt("CANNOT CREATE VNODEFILE");
            return tristatus;
        case TRIERR_CANNOT_CREATE_VEDGEFILE
            mexErrMsgTxt("CANNOT CREATE VEDGEFILE");
            return tristatus;
        case TRIERR_CANNOT_CREATE_NEIGHBOURFILE
            mexErrMsgTxt("CANNOT CREATE NEIGHBOURFILE");
            return tristatus;
        case TRIERR_CANNOT_CREATE_OFFFILE
            mexErrMsgTxt("CANNOT CREATE OFFFILE");
            return tristatus;
        default
            mexErrMsgTxt("Unexpected error code");
            return -1;
    }

}
