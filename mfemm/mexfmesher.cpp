#include <iostream>
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

void FmesherInterfaceWarning(const char* warningmsg)
{
    mexWarnMsgIdAndTxt("MFEMM:fmesher", warningmsg);
}

// extern void _main();

using namespace femm;

/* the gateway function */
void mexFunction( int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[])
{
    FMesher MeshObj;
    char *buf;
    mwSize buflen;
    int status,tristatus;
    bool hasperiodic;
    double verbose = 0.0;

    //(void) plhs;    /* unused parameters */

    /* Check for proper number of input and output arguments */
    if ((nrhs > 2) | (nrhs < 1)) {
        mexErrMsgIdAndTxt("MFEMM:fmesher:numargs", 
                          "One or two input arguments required.");
    }

    if (nlhs > 1) {
        mexErrMsgIdAndTxt("MFEMM:fmesher:numargs",
                          "Too many output arguments.");
    }

    /* Check for proper input type */
    if (!mxIsChar(prhs[0]) || (mxGetM(prhs[0]) != 1 ) )  
    {
        mexErrMsgIdAndTxt("MFEMM:fmesher:badfirstarg", 
                          "Input argument must be a string.");
    }
    
    if (nrhs == 2)
    {
        /*  get the dimensions of the matrix input x */
        size_t rows = mxGetM(prhs[1]);
        size_t cols = mxGetN(prhs[1]);
        
        if ((!mxIsNumeric(prhs[1])) || (rows != 1) || (cols != 1))
        {
            mexErrMsgIdAndTxt( "MFEMM:fmesher:inputnotscalar",
                               "Second input must be a scalar.");
        }
        
        verbose = mxGetScalar(prhs[1]);
    }
    else
    {
       verbose = 0.0;
    }
    
    if (verbose == 0.0)
    {
        MeshObj.Verbose = false;
    }
    else
    {
        MeshObj.Verbose = true;
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

    /* When finished using the char array, deallocate it. */
    mxFree(buf);
    
    MeshObj.WarnMessage = &FmesherInterfaceWarning;

    MeshObj.TriMessage = &mexPrintf;

    // attempt to discover the file type from the file name
    MeshObj.filetype = FMesher::GetFileType (FilePath);

    /*  call the FMesher subroutines */
    status = MeshObj.LoadFEMFile(FilePath);

    if (status != FMesher::F_FILE_OK)
    {
        switch (status)
        {
            case FMesher::F_FILE_NOT_OPENED:
                mexErrMsgIdAndTxt( "MFEMM:fmesher:badfile",
                                  "The input file %s could not be opened.", FilePath.c_str ());
            case FMesher::F_FILE_MALFORMED:
                mexErrMsgIdAndTxt( "MFEMM:fmesher:malformedfile",
                                  "The input file appears to be malformed and could not be parsed");
            case FMesher::F_FILE_UNKNOWN_TYPE:
                mexErrMsgIdAndTxt( "MFEMM:fmesher:unknownfiletype",
                                  "The input file problem type could not be determined from its extension.");
            default:

                break;

        }

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
    }
    else
    {
        tristatus = MeshObj.DoNonPeriodicBCTriangulation(FilePath) ;

        if (tristatus == -1)
        {
            tristatus = -3;
        }
    }

    switch (tristatus)
    {
        case 0:
            break;
        case -1:
            break;
        case TRIERR_OUT_OF_MEM:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:outofmemory", "OUT OF MEM");
            break;
        case TRIERR_INTERNAL_ERR:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trierror", "TRIANGLE INTERNAL ERR");
            break;
        case TRIERR_ZERO_MAX_AREA:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trizeromaxarea", "ZERO MAX AREA");
            break;
        case TRIERR_I_SWITCH_WITH_REFINE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:triiswitch", "I SWITCH WITH REFINE");
            break;
        case TRIERR_INPUT_VERTICES_IDENTICAL:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:triidenticalverts", "INPUT VERTICES IDENTICAL");
            break;
        case TRIERR_NOT_ENOUGH_VERTICES:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:tritoofewverts", "NOT ENOUGH VERTICES");
            break;
        case TRIERR_CANNOT_ACCESS_ELEFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinoelefileaccess", "CANNOT ACCESS ELEFILE");
            break;
        case TRIERR_CANNOT_ACCESS_AREAFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinoareafileaccess", "CANNOT ACCESS AREAFILE");
            break;
        case TRIERR_ELEFILE_AND_AREAFILE_DIFF_N_TRIANGLES:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trieleandareatrinums", "ELEFILE AND AREAFILE DIFF NUMBER OF TRIANGLES");
            break;
        case TRIERR_TRI_INVALID_VERTEX_INDEX:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:triinvalidvert", "TRI INVALID VERTEX INDEX");
            break;
        case TRIERR_MISSING_VERTEX_INDEX:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trimissingvert", "MISSING VERTEX_INDEX");
            break;
        case TRIERR_SEGMENT_NO_END_POINTS:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trimissingsegpoints", "SEGMENT NO END POINTS");
            break;
        case TRIERR_SEGMENT_NO_SECOND_END_POINT:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trimissingsegpoint", "SEGMENT NO SECOND END POINT");
            break;
        case TRIERR_SEG_INVALID_VERTEX_INDEX:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:triinvalidvert", "SEG INVALID VERTEX INDEX");
            break;
        case TRIERR_SPLIT_SEG_TOO_SMALL:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trismallseg", "SPLIT SEG TOO SMALL");
            break;
        case TRIERR_UNEXPECTED_EOF:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:triunexpectedeof", "UNEXPECTED EOF 16");
            break;
        case TRIERR_CANNOT_ACCESS_POLYFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinopolyfileaccess", "CANNOT ACCESS POLYFILE");
            break;
        case TRIERR_CANNOT_ACCESS_NODEFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinonodefileaccess", "CANNOT ACCESS NODEFILE");
            break;
        case TRIERR_MUST_HAVE_THREE_VERTICES:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trivertnum", "MUST HAVE THREE VERTICES");
            break;
        case TRIERR_TOO_MANY_MESH_DIMENSIONS:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:tritoomanydims", "TOO MANY MESH DIMENSIONS");
            break;
        case TRIERR_VERTEX_NO_X:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trivertnox", "VERTEX NO X");
            break;
        case TRIERR_VERTEX_NO_Y:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trivertnoy", "VERTEX NO Y");
            break;
        case TRIERR_HOLE_NO_X:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:triholenox", "HOLE NO X");
            break;
        case TRIERR_HOLE_NO_Y:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:triholenoy", "HOLE NO Y");
            break;
        case TRIERR_REGION_NO_X:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:triregnox", "REGION NO Y");
            break;
        case TRIERR_REGION_NO_Y:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:triregnoy", "REGION NO Y");
            break;
        case TRIERR_REGION_NO_ATTRIBUTE_OR_AREA_CON:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinocon", "REGION HAS NO ATTRIBUTE OR AREA CON");
            break;
        case TRIERR_CANNOT_CREATE_NODEFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinonodefile", "CANNOT CREATE NODEFILE");
            break;
        case TRIERR_CANNOT_CREATE_ELEFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinoelefile", "CANNOT CREATE ELEFILE");
            break;
        case TRIERR_CANNOT_CREATE_POLYFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinopolyfile", "CANNOT CREATE POLYFILE");
            break;
        case TRIERR_CANNOT_CREATE_EDGEFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinoedgefile", "CANNOT CREATE EDGEFILE");
            break;
        case TRIERR_CANNOT_CREATE_VNODEFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinovnodefile", "CANNOT CREATE VNODEFILE");
            break;
        case TRIERR_CANNOT_CREATE_VEDGEFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinovedgefile", "CANNOT CREATE VEDGEFILE");
            break;
        case TRIERR_CANNOT_CREATE_NEIGHBOURFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinoneighfile", "CANNOT CREATE NEIGHBOURFILE");
            break;
        case TRIERR_CANNOT_CREATE_OFFFILE:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:trinoofffile", "CANNOT CREATE OFFFILE");
            break;
        default:
            mexErrMsgIdAndTxt( "MFEMM:fmesher:unknownerr", "Unexpected error code");
            break;
    }

    plhs[0] = mxCreateDoubleScalar((double)tristatus);

}
