//extern lua_State *lua;
//extern BOOL bLinehook;
//extern void *pFemmviewdoc;
//extern CLuaConsoleDlg *LuaConsole;
//extern BOOL lua_byebye;
//extern int m_luaWindowStatus;

#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include "mex.h"
#include "fpproc.h"
#include "problem.h"
#include "fpproc_interface.h"


using namespace std;

void FPProcInterfaceWarning(const char* warningmsg)
{
    mexWarnMsgIdAndTxt("MFEMM:fpproc", warningmsg);
}

//////////////////      MEX Interface        //////////////////////

FPProc_interface::FPProc_interface()
{
    theFPProc.WarnMessage = &FPProcInterfaceWarning;
}

//FPProc_interface::FPProc_interface(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
//{
//    theFPProc.OpenDocument(filename);
//}

int FPProc_interface::opendocument(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char *input_buf;
    double *outpointer;
    int wasopened;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    /* 3rd input must be a string (first two are used for the class interface) */
    if ( mxIsChar(prhs[2]) != 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:inputNotString",
                           "Input must be a string containing the file name.");

    /* input must be a row vector */
    if (mxGetM(prhs[2])!=1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:inputNotVector",
                           "Input must be a row vector.");

    /* copy the string data from prhs[2] into a C string input_ buf.    */
    input_buf = mxArrayToString(prhs[2]);
    // create a std::string from the input C string
    string filename(input_buf);
    // free the char array as we are done with it
    mxFree(input_buf);

    // call the OpenDocument method with the input
    wasopened = theFPProc.OpenDocument(filename);

    if (!wasopened)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:inputNotVector",
                           "File could not be opened.");

    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);
    outpointer[0] = (double)(wasopened);

    // Generate error if any regions are multiply defined
    // (i.e. tagged by more than one block label)
    if (theFPProc.bMultiplyDefinedLabels)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:regionsmultiplydefined",
                           "Some regions in the problem have been defined by more than one block label.");

    return wasopened;
}

int FPProc_interface::getpointvals(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *px, *py, *outpointerRe, *outpointerIm;
    size_t mxrows, myrows, nxcols, nycols;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "Two inputs required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    px = mxGetPr(prhs[2]);
    py = mxGetPr(prhs[3]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    myrows = mxGetM(prhs[3]);

    nxcols = mxGetN(prhs[2]);
    nycols = mxGetN(prhs[3]);
    if((nxcols>1) | (nycols>1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "x and y must both be column vectors.");
    }

    if(myrows != myrows)
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "x and y must be column vectors of the same size.");
    }

    if (theFPProc.Frequency!=0)
    {
#ifdef _MEX_DEBUG
        mexPrintf("Frequency was not equal to zero.\n");
#endif
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(14), (mwSize)mxrows, mxCOMPLEX);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerIm = mxGetPi(plhs[0]);

        for(int i=0; i<(int)mxrows; i++)
        {
            CPointVals u;

            if(theFPProc.GetPointValues(px[i], py[i], u)==true)
            {
                // copy the point values to the matlab array at the
                // appropriate locations
                outpointerRe[(i*14)] = u.A.Re();
                outpointerIm[(i*14)] = u.A.Im();

                outpointerRe[(i*14)+1] = u.B1.Re();
                outpointerIm[(i*14)+1] = u.B1.Im();

                outpointerRe[(i*14)+2] = u.B2.Re();
                outpointerIm[(i*14)+2] = u.B2.Im();

                outpointerRe[(i*14)+3] = u.c;
                outpointerIm[(i*14)+3] = 0.0;

                outpointerRe[(i*14)+4] = u.E;
                outpointerIm[(i*14)+4] = 0.0;

                outpointerRe[(i*14)+5] = u.H1.Re();
                outpointerIm[(i*14)+5] = u.H1.Im();

                outpointerRe[(i*14)+6] = u.H2.Re();
                outpointerIm[(i*14)+6] = u.H2.Im();

                outpointerRe[(i*14)+7] = u.Je.Re();
                outpointerIm[(i*14)+7] = u.Je.Im();

                outpointerRe[(i*14)+8] = u.Js.Re();
                outpointerIm[(i*14)+8] = u.Js.Im();

                outpointerRe[(i*14)+9] = u.mu1.Re();
                outpointerIm[(i*14)+9] = u.mu1.Im();

                outpointerRe[(i*14)+10] = u.mu2.Re();
                outpointerIm[(i*14)+10] = u.mu2.Im();

                outpointerRe[(i*14)+11] = u.Pe;
                outpointerIm[(i*14)+11] = 0.0;

                outpointerRe[(i*14)+12] = u.Ph;
                outpointerIm[(i*14)+12] = 0.0;

                outpointerRe[(i*14)+13] = u.ff;
                outpointerIm[(i*14)+13] = 0.0;
            }
            else
            {
                // we return nan values to alert the user
                outpointerRe[(i*14)] = mxGetNaN();
                outpointerIm[(i*14)] = mxGetNaN();

                outpointerRe[(i*14)+1] = mxGetNaN();
                outpointerIm[(i*14)+1] = mxGetNaN();

                outpointerRe[(i*14)+2] = mxGetNaN();
                outpointerIm[(i*14)+2] = mxGetNaN();

                outpointerRe[(i*14)+3] = mxGetNaN();
                outpointerIm[(i*14)+3] = mxGetNaN();

                outpointerRe[(i*14)+4] = mxGetNaN();
                outpointerIm[(i*14)+4] = mxGetNaN();

                outpointerRe[(i*14)+5] = mxGetNaN();
                outpointerIm[(i*14)+5] = mxGetNaN();

                outpointerRe[(i*14)+6] = mxGetNaN();
                outpointerIm[(i*14)+6] = mxGetNaN();

                outpointerRe[(i*14)+7] = mxGetNaN();
                outpointerIm[(i*14)+7] = mxGetNaN();

                outpointerRe[(i*14)+8] = mxGetNaN();
                outpointerIm[(i*14)+8] = mxGetNaN();

                outpointerRe[(i*14)+9] = mxGetNaN();
                outpointerIm[(i*14)+9] = mxGetNaN();

                outpointerRe[(i*14)+10] = mxGetNaN();
                outpointerIm[(i*14)+10] = mxGetNaN();

                outpointerRe[(i*14)+11] = mxGetNaN();
                outpointerIm[(i*14)+11] = mxGetNaN();

                outpointerRe[(i*14)+12] = mxGetNaN();
                outpointerIm[(i*14)+12] = mxGetNaN();

                outpointerRe[(i*14)+13] = mxGetNaN();
                outpointerIm[(i*14)+13] = mxGetNaN();
            }
        }
    }
    else
    {
#ifdef _MEX_DEBUG
        mexPrintf("Frequency was zero.\n");
#endif
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(14), (mwSize)mxrows, mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);

        for(int i=0; i<(int)mxrows; i++)
        {
            CPointVals u;

            if(theFPProc.GetPointValues(px[i], py[i], u)==true)
            {
                // copy the point values to the matlab array at the
                // appropriate locations
#ifdef _MEX_DEBUG
                mexPrintf("row %i, theFPProc.GetPointValues was true.\n", i);
#endif
                outpointerRe[(i*14)] = u.A.Re();
                outpointerRe[(i*14)+1] = u.B1.Re();
                outpointerRe[(i*14)+2] = u.B2.Re();
                outpointerRe[(i*14)+3] = u.c;
                outpointerRe[(i*14)+4] = u.E;
                outpointerRe[(i*14)+5] = u.H1.Re();
                outpointerRe[(i*14)+6] = u.H2.Re();
                outpointerRe[(i*14)+7] = u.Je.Re();
                outpointerRe[(i*14)+8] = u.Js.Re();
                outpointerRe[(i*14)+9] = u.mu1.Re();
                outpointerRe[(i*14)+10] = u.mu2.Re();
                outpointerRe[(i*14)+11] = u.Pe;
                outpointerRe[(i*14)+12] = u.Ph;
                outpointerRe[(i*14)+13] = u.ff;
            }
            else
            {
                // we return nan values to alert the user
#ifdef _MEX_DEBUG
                mexPrintf("row %i, theFPProc.GetPointValues was false.\n", i);
#endif
                outpointerRe[(i*14)] = mxGetNaN();
                outpointerRe[(i*14)+1] = mxGetNaN();
                outpointerRe[(i*14)+2] = mxGetNaN();
                outpointerRe[(i*14)+3] = mxGetNaN();
                outpointerRe[(i*14)+4] = mxGetNaN();
                outpointerRe[(i*14)+5] = mxGetNaN();
                outpointerRe[(i*14)+6] = mxGetNaN();
                outpointerRe[(i*14)+7] = mxGetNaN();
                outpointerRe[(i*14)+8] = mxGetNaN();
                outpointerRe[(i*14)+9] = mxGetNaN();
                outpointerRe[(i*14)+10] = mxGetNaN();
                outpointerRe[(i*14)+11] = mxGetNaN();
                outpointerRe[(i*14)+12] = mxGetNaN();
                outpointerRe[(i*14)+13] = mxGetNaN();
            }
        }
    }

    return 14;
}


int FPProc_interface::addcontour(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    CComplex z;
    int nconpts;
    double *px, *py, *outpointer;
    size_t mxrows, myrows, nxcols, nycols;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "Two inputs required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    px = mxGetPr(prhs[2]);
    py = mxGetPr(prhs[3]);

    /*  get the no. of rows of the matrix input x and y */
    mxrows = mxGetM(prhs[2]);
    myrows = mxGetM(prhs[3]);
    /*  get the no. of cols of the matrix input x and y */
    nxcols = mxGetN(prhs[2]);
    nycols = mxGetN(prhs[3]);
    // check dimensions are allowed
    if((nxcols>1) | (nycols>1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "x and y must both be row vectors.");
    }

    if(mxrows != myrows)
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "x and y must be row vectors of the same size.");
    }

    for (int ind=0; ind<mxrows; ind++)
    {

        z.Set(px[ind],py[ind]);

        nconpts = theFPProc.contour.size();

        if(nconpts>0)
        {
            if (z != theFPProc.contour[nconpts-1])
            {
                theFPProc.contour.push_back(z);
            }
        }
        else
        {
            theFPProc.contour.push_back(z);
        }

    }

    // now return the actual points in the current contour
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(theFPProc.contour.size()), (mwSize)(2), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);
    nconpts = theFPProc.contour.size();
    for (int ind=0; ind<nconpts; ind++)
    {
        outpointer[ind] = theFPProc.contour[ind].re;
        outpointer[ind+nconpts] = theFPProc.contour[ind].im;
    }

    return 0;
}

int FPProc_interface::clearcontour()
{
    theFPProc.contour.clear();

    return 0;
}

int FPProc_interface::lineintegral(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int type;
    CComplex *z;
    double *ptype, *outpointerRe, *outpointerIm;
    size_t mrows, ncols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    /*  get the dimensions of the matrix input x */
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    // check dimensions are allowed
    if((mrows!=1) | (ncols!=1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "type must be a scalar.");
    }

    // get a pointer to the actual data
    ptype = mxGetPr(prhs[2]);

    // cast the value in the matlab matrix to an integer
    type = (int) ptype[0];
    // 0- B.n
    // 1 - H.t
    // 2 - Cont length
    // 3 - Force from stress tensor
    // 4 - Torque from stress tensor
    // 5 - (B.n)^2

    if (type<0 || type >5)
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidinttype",
                           "Invalid line integral type selected %d",type);
        return 0;
    }

    // Create an array of CComplex to hold the results of the integral
    z = (CComplex *)calloc(4,sizeof(CComplex));
    theFPProc.LineIntegral(type,z);
mexPrintf ("%f %f %f %f %f %f %f %f %f %f\n", z[0].re, z[0].im, z[1].re, z[1].im, z[2].re, z[2].im, z[3].re, z[3].im);
    switch(type)
    {
    case 2: // length result
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z[0].re; // contour length
        outpointerRe[1] = z[0].im; // swept area
        free(z);
        return 2;

    case 3: // force results
        if (theFPProc.Frequency!=0)
        {
            // Stress Tensor Force
            // 1: DC r/x force
            // 2: DC y/z force
            // 3: 2× r/x force
            // 4: 2× y/z force

            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(4), (mwSize)(1), mxCOMPLEX );
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerIm = mxGetPi(plhs[0]);
            
            outpointerRe[0] = z[2].re; outpointerIm[0] = 0.0;
            outpointerRe[1] = z[3].re; outpointerIm[1] = 0.0;
            outpointerRe[2] = z[0].re;
            outpointerIm[2] = z[0].im;
            outpointerRe[3] = z[1].re;
            outpointerIm[3] = z[1].im;

            free(z);
            return 4;
        }
        else
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxREAL);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            
            outpointerRe[0] = z[0].re;
            outpointerRe[1] = z[1].re;

            free(z);
            return 2;
        }


    case 4: // torque results
        if(theFPProc.Frequency!=0)
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxCOMPLEX);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerIm = mxGetPi(plhs[0]);
            outpointerRe[0] = z[1].re;
            outpointerRe[1] = z[0].re;
            outpointerIm[1] = z[0].im;

            free(z);
            return 2;
        }
        else
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerRe[0] = z[0].re;

            free(z);
            return 2;
        }

    default:
        
        if (theFPProc.Frequency!=0)
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxCOMPLEX);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerIm = mxGetPi(plhs[0]);
            outpointerRe[0] = z[0].re;
            outpointerIm[0] = z[0].im;
            outpointerRe[1] = z[1].re;
            outpointerIm[1] = z[1].im;
        }
        else
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxREAL);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerRe[0] = z[0].re;
            outpointerRe[1] = z[1].re;
        }
        free(z);
        return 2;
    }

    free(z);
    return 0;
}

int FPProc_interface::selectblock(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    //CatchNullDocument();//
    double *px, *py, *outpointer;
    size_t mxrows, myrows, nxcols, nycols;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.", nlhs);

    px = mxGetPr(prhs[2]);
    py = mxGetPr(prhs[3]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    myrows = mxGetM(prhs[3]);

    nxcols = mxGetN(prhs[2]);
    nycols = mxGetN(prhs[3]);
    if((mxrows != 1) | (myrows != 1) | (nxcols != 1) | (nycols != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "x and y must both be scalars.");
    }

    int k;

    if (theFPProc.meshelem.size()>0)
    {
        k = theFPProc.InTriangle(px[0],py[0]);

        if(k>=0)
        {
            theFPProc.bHasMask = false;
            theFPProc.blocklist[theFPProc.meshelem[k].lbl].ToggleSelect();
        }
    }

    return 0;
}

int FPProc_interface::groupselectblock(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int j,k,n;
    double *pgroup;
    size_t mrows, ncols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    /*  get the dimensions of the matrix input x */
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    // check dimensions are allowed
    if((mrows>1) | (ncols>1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "group number must be a scalar.");
    }

    if ((mrows==0) | (ncols==0))
    {
        // select all the blocks
        theFPProc.bHasMask = false;

        for(int i=0; i<theFPProc.blocklist.size(); i++)
        {
            if (theFPProc.blocklist[i].IsSelected == false)
            {
                theFPProc.blocklist[i].IsSelected = true;
            }
        }
    }
    else
    {
        // select blocks in the specified group

        // get a pointer to the actual data
        pgroup = mxGetPr(prhs[2]);

        if (theFPProc.meshelem.size()>0)
        {
            k = 0;

            if (pgroup[0] > 0)
            {
                k = (int)pgroup[0];
            }

            for(j=0; j<theFPProc.blocklist.size(); j++)
            {
                if ((theFPProc.blocklist[j].InGroup==k) || (pgroup[0]==0))
                {
                    theFPProc.blocklist[j].ToggleSelect();
                }

                theFPProc.bHasMask = false;
            }
        }
    }

    return 0;
}

int FPProc_interface::clearblock()
{
    theFPProc.bHasMask = false;

    for(int i=0; i<theFPProc.blocklist.size(); i++)
    {
        if (theFPProc.blocklist[i].IsSelected == true)
        {
            theFPProc.blocklist[i].IsSelected = false;
        }
    }

    return 0;
}

int FPProc_interface::blockintegral(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int type;

    double *ptype, *outpointerRe, *outpointerIm;
    size_t mrows, ncols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs != 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Wrong number of output arguments.");

    /*  get the dimensions of the matrix input x */
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    // check dimensions are allowed
    if((mrows!=1) | (ncols!=1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "type must be a scalar.");
    }

    // get a pointer to the actual data
    ptype = mxGetPr(prhs[2]);

    // cast the value in the matlab matrix to an integer
    type = (int) ptype[0];

    CComplex z;

    bool flg = false;
    for(unsigned int i=0; i<theFPProc.blocklist.size(); i++)
    {
        if (theFPProc.blocklist[i].IsSelected==true)
        {
            flg = true;
        }
    }

    if(flg==false)
    {
        mexErrMsgIdAndTxt("MFEMM:fpproc:noblockselected",
                          "Block integral failed, no area has been selected");
    }
    
    if((type<0) || (type>25))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidinttype",
                           "Invalid block integral type selected %d",type);
        return 0;
    }

    if((type>=18) && (type<=23))
    {
        theFPProc.MakeMask();
    }

    z = theFPProc.BlockIntegral(type);

    if (type == 25)
    {
        // 2D block centroid
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(2), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z.Re();
        outpointerRe[1] = z.Im();
    }
    else
    {
        if (z.Im() == 0.0)
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerRe[0] = z.Re();
        }
        else
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxCOMPLEX);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerIm = mxGetPi(plhs[0]);
            outpointerRe[0] = z.Re();
            outpointerIm[0] = z.Im();
        }
    }

    return 1;
}


int FPProc_interface::smoothon()
{
    theFPProc.Smooth = true;
    return 0;
}

int FPProc_interface::smoothoff()
{
    theFPProc.Smooth = false;
    return 0;
}

int FPProc_interface::getprobleminfo(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *outpointer;

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(4), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);
    outpointer[0] = theFPProc.problemType;
    outpointer[1] = theFPProc.Frequency;
    outpointer[2] = theFPProc.Depth;
    outpointer[3] = theFPProc.LengthConv[theFPProc.LengthUnits];

    return 4;
}

int FPProc_interface::getcircuitprops(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int NumCircuits,k;
    char circuitname[1024];
    k=-1;

    if (nrhs < 3 || mxGetString(prhs[2], circuitname, sizeof(circuitname)))
    {
        mexErrMsgTxt("Input should be a string containng a circuit name less than 1024 characters long.");
    }

    // ok we need to find the correct entry for the circuit name
    NumCircuits = theFPProc.circproplist.size();

    for(int i=0; i<NumCircuits; i++)
    {
        if(strcmp(theFPProc.circproplist[i].CircName.c_str(), circuitname) == 0)
        {
            k = i;
            i = NumCircuits; // that will break it
        }
    }

// trap errors

    if(k==-1)
    {
        mexErrMsgTxt("Unknown circuit");
    }

    CComplex amps,volts,fluxlinkage;

    amps = theFPProc.circproplist[k].Amps;
    volts = theFPProc.GetVoltageDrop(k);
    fluxlinkage = theFPProc.GetFluxLinkage(k);

    if (theFPProc.Frequency != 0)
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(3), (mwSize)(1), mxCOMPLEX);
        // get a pointer to the start of the actual output data array
        double *outpointerRe = mxGetPr(plhs[0]);
        double *outpointerIm = mxGetPi(plhs[0]);
        outpointerRe[0] = amps.re;
        outpointerIm[0] = amps.im;
        outpointerRe[1] = volts.re;
        outpointerIm[1] = volts.im;
        outpointerRe[2] = fluxlinkage.re;
        outpointerIm[2] = fluxlinkage.im;
    }
    else
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(3), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        double *outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = amps.re;
        outpointerRe[1] = volts.re;
        outpointerRe[2] = fluxlinkage.re;
    }

    return 3;
}


int FPProc_interface::numnodes(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *outpointer;

    /* check for proper number of arguments */
    if(nrhs!=2)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "No input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

	outpointer[0] = (double)theFPProc.meshnode.size();

	return 1;
}

int FPProc_interface::numelements(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *outpointer;

    /* check for proper number of arguments */
    if(nrhs!=2)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "No input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

	outpointer[0] = (double)theFPProc.meshelem.size();

	return 1;
}


int FPProc_interface::getelements(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_elmnos, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_elmnos = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if (nxcols>1)
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "element nos, must be a column vector.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)mxrows, (mwSize)(7), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    int numelms = theFPProc.meshelem.size();
    int i = 0;

    // check no invalid element numbers have been requested
    for (i = 0; i < (int)mxrows; i++)
    {
        if (std::floor(p_elmnos[i]) < 1.0)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Element numbers start from 1.");
        }

        if (std::floor(p_elmnos[i]) > numelms)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Invalid element number (bigger than number of elements).");
        }
    }

    for(i=0; i<(int)mxrows; i++)
    {
        // copy the element info to the matlab array at the
        // appropriate locations
        int n = (int)std::floor(p_elmnos[i]) - 1;
        outpointer[i] = theFPProc.meshelem[n].p[0]+1;
        outpointer[i+(int)mxrows] = theFPProc.meshelem[n].p[1]+1;
        outpointer[i+2*(int)mxrows] = theFPProc.meshelem[n].p[2]+1;
        outpointer[i+3*(int)mxrows] = Re(theFPProc.meshelem[n].ctr);
        outpointer[i+4*(int)mxrows] = Im(theFPProc.meshelem[n].ctr);
        outpointer[i+5*(int)mxrows] = theFPProc.ElmArea(n);
        outpointer[i+6*(int)mxrows] = theFPProc.blocklist[theFPProc.meshelem[n].lbl].InGroup;
    }

	return 7;
}

int FPProc_interface::getcentroids(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_elmnos, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_elmnos = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if (nxcols>1)
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "element nos, must be a column vector.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)mxrows, (mwSize)(2), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    int numelms = theFPProc.meshelem.size();
    int i = 0;

    // check no invalid element numbers have been requested
    for (i = 0; i < (int)mxrows; i++)
    {
        if (std::floor(p_elmnos[i]) < 1.0)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Element numbers start from 1.");
        }

        if (std::floor(p_elmnos[i]) > numelms)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Invalid element number (bigger than number of elements).");
        }
    }

    for(i=0; i<(int)mxrows; i++)
    {
        // copy the element info to the matlab array at the
        // appropriate locations
        int n = (int)std::floor(p_elmnos[i]) - 1;
        outpointer[i] = Re(theFPProc.meshelem[n].ctr);
        outpointer[i+(int)mxrows] = Im(theFPProc.meshelem[n].ctr);
    }

	return 0;
}

int FPProc_interface::getareas(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_elmnos, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_elmnos = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if (nxcols>1)
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "element nos, must be a column vector.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)mxrows, (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    int numelms = theFPProc.meshelem.size();
    int i = 0;

    // check no invalid element numbers have been requested
    for (i = 0; i < (int)mxrows; i++)
    {
        if (std::floor(p_elmnos[i]) < 1.0)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Element numbers start from 1.");
        }

        if (std::floor(p_elmnos[i]) > numelms)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Invalid element number (bigger than number of elements).");
        }
    }

    for(i=0; i<(int)mxrows; i++)
    {
        // copy the element info to the matlab array at the
        // appropriate locations
        int n = (int)std::floor(p_elmnos[i]) - 1;
        outpointer[i] = theFPProc.ElmArea(n);
    }

	return 0;
}

int FPProc_interface::getvolumes(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_elmnos, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_elmnos = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if (nxcols>1)
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "element nos, must be a column vector.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)mxrows, (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    int numelms = theFPProc.meshelem.size();
    int i = 0;

    // check no invalid element numbers have been requested
    for (i = 0; i < (int)mxrows; i++)
    {
        if (std::floor(p_elmnos[i]) < 1.0)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Element numbers start from 1.");
        }

        if (std::floor(p_elmnos[i]) > numelms)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Invalid element number (bigger than number of elements).");
        }
    }

    for(i=0; i<(int)mxrows; i++)
    {
        // copy the element info to the matlab array at the
        // appropriate locations
        int n = (int)std::floor(p_elmnos[i]) - 1;
        outpointer[i] = theFPProc.ElmVolume(n);
    }

	return 0;
}

int FPProc_interface::getvertices(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_elmnos, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_elmnos = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if (nxcols>1)
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "element nos, must be a column vector.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)mxrows, (mwSize)(6), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    int numelms = theFPProc.meshelem.size();
    int i = 0;

    // check no invalid element numbers have been requested
    for (i = 0; i < (int)mxrows; i++)
    {
        if (std::floor(p_elmnos[i]) < 1.0)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Element numbers start from 1.");
        }

        if (std::floor(p_elmnos[i]) > numelms)
        {
            mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidelmno",
                               "Invalid element number (bigger than number of elements).");
        }
    }

    for(i=0; i<(int)mxrows; i++)
    {
        // copy the element info to the matlab array at the
        // appropriate locations
        int n = (int)std::floor(p_elmnos[i]) - 1;
        outpointer[i] = theFPProc.meshnode[theFPProc.meshelem[n].p[0]].x;
        outpointer[i+(int)mxrows] = theFPProc.meshnode[theFPProc.meshelem[n].p[0]].y;
        outpointer[i+2*(int)mxrows] = theFPProc.meshnode[theFPProc.meshelem[n].p[1]].x;
        outpointer[i+3*(int)mxrows] = theFPProc.meshnode[theFPProc.meshelem[n].p[1]].y;
        outpointer[i+4*(int)mxrows] = theFPProc.meshnode[theFPProc.meshelem[n].p[2]].x;
        outpointer[i+5*(int)mxrows] = theFPProc.meshnode[theFPProc.meshelem[n].p[2]].y;
    }

	return 0;
}



int FPProc_interface::numgroupelements (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (nxcols != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int groupno = (int)p_gpno[0];
    int numingroup = countGroupElements (groupno);

    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
    outpointer = mxGetPr(plhs[0]);
    outpointer[0] = (double)numingroup;

    return 1;
}



int FPProc_interface::countGroupElements (int groupno)
{
    int numingroup = 0;

    // count group elements
    for (int i = 0; i < theFPProc.meshelem.size (); i++)
    {
        if (theFPProc.blocklist[theFPProc.meshelem[i].lbl].InGroup == groupno)
        {
            numingroup++;
        }
    }

    return numingroup;
}


int FPProc_interface::getgroupelements(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (nxcols != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int numelms = theFPProc.meshelem.size();
    int groupno = (int)p_gpno[0];
    int numingroup = countGroupElements (groupno);
    int i = 0;

    if (numingroup > 0)
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)numingroup, (mwSize)(7), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointer = mxGetPr(plhs[0]);
        int n = 0;

        for(i=0; i < numelms; i++)
        {
            if (theFPProc.blocklist[theFPProc.meshelem[i].lbl].InGroup == groupno)
            {
                // copy the element info to the matlab array at the
                // appropriate locations
                outpointer[n] = theFPProc.meshelem[i].p[0]+1;
                outpointer[n+numingroup] = theFPProc.meshelem[i].p[1]+1;
                outpointer[n+2*numingroup] = theFPProc.meshelem[i].p[2]+1;
                outpointer[n+3*numingroup] = Re(theFPProc.meshelem[i].ctr);
                outpointer[n+4*numingroup] = Im(theFPProc.meshelem[i].ctr);
                outpointer[n+5*numingroup] = theFPProc.ElmArea(i);
                outpointer[n+6*numingroup] = theFPProc.blocklist[theFPProc.meshelem[i].lbl].InGroup;

                n++;
            }
        }
    }
    else
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(0), (mwSize)(0), mxREAL);
    }

	return 7;
}

int FPProc_interface::getgroupcentroids(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (mxrows != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int numelms = theFPProc.meshelem.size();
    int groupno = (int)p_gpno[0];
    int numingroup = countGroupElements (groupno);
    int i = 0;

    if (numingroup > 0)
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)numingroup, (mwSize)(2), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointer = mxGetPr(plhs[0]);

        int n = 0;


        for(i = 0; i < numelms; i++)
        {
            if (theFPProc.blocklist[theFPProc.meshelem[i].lbl].InGroup == groupno)
            {
                // copy the element info to the matlab array at the
                // appropriate locations
                outpointer[n] = Re(theFPProc.meshelem[i].ctr);
                outpointer[n+numingroup] = Im(theFPProc.meshelem[i].ctr);

                n++;
            }
        }
    }
    else
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(0), (mwSize)(0), mxREAL);
    }

	return 0;
}

int FPProc_interface::getgroupareas(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (mxrows != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int numelms = theFPProc.meshelem.size();
    int groupno = (int)p_gpno[0];
    int numingroup = countGroupElements (groupno);
    int i = 0;

    if (numingroup > 0)
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)numingroup, (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointer = mxGetPr(plhs[0]);

        int n = 0;


        for(i = 0; i < numelms; i++)
        {
            if (theFPProc.blocklist[theFPProc.meshelem[i].lbl].InGroup == groupno)
            {
                // copy the element info to the matlab array at the
                // appropriate locations
                outpointer[n] = theFPProc.ElmArea(i);

                n++;
            }
        }
    }
    else
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(0), (mwSize)(0), mxREAL);
    }

	return 1;
}


int FPProc_interface::getgroupvolumes(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (mxrows != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int numelms = theFPProc.meshelem.size();
    int groupno = (int)p_gpno[0];
    int numingroup = countGroupElements (groupno);
    int i = 0;

    if (numingroup > 0)
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)numingroup, (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointer = mxGetPr(plhs[0]);

        int n = 0;


        for(i = 0; i < numelms; i++)
        {
            if (theFPProc.blocklist[theFPProc.meshelem[i].lbl].InGroup == groupno)
            {
                // copy the element info to the matlab array at the
                // appropriate locations
                outpointer[n] = theFPProc.ElmVolume(i);

                n++;
            }
        }
    }
    else
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(0), (mwSize)(0), mxREAL);
    }

	return 1;
}


int FPProc_interface::getgroupvertices(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:fpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (mxrows != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:fpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int numelms = theFPProc.meshelem.size();
    int groupno = (int)p_gpno[0];
    int numingroup = countGroupElements (groupno);
    int i = 0;

    if (numingroup > 0)
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)numingroup, (mwSize)(6), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointer = mxGetPr(plhs[0]);

        int n = 0;

        for(i = 0; i < numelms; i++)
        {
            if (theFPProc.blocklist[theFPProc.meshelem[i].lbl].InGroup == groupno)
            {

                outpointer[n] = theFPProc.meshnode[theFPProc.meshelem[i].p[0]].x;
                outpointer[n+numingroup] = theFPProc.meshnode[theFPProc.meshelem[i].p[0]].y;
                outpointer[n+2*numingroup] = theFPProc.meshnode[theFPProc.meshelem[i].p[1]].x;
                outpointer[n+3*numingroup] = theFPProc.meshnode[theFPProc.meshelem[i].p[1]].y;
                outpointer[n+4*numingroup] = theFPProc.meshnode[theFPProc.meshelem[i].p[2]].x;
                outpointer[n+5*numingroup] = theFPProc.meshnode[theFPProc.meshelem[i].p[2]].y;

                n++;
            }
        }
    }
    else
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(0), (mwSize)(0), mxREAL);
    }

    return 0;
}


///////////////////////////////////////////////////////////////
/////////////      FUNCTIONS TO BE CONVERTED     //////////////
///////////////////////////////////////////////////////////////


//
//
//int FPProc_interface::selectline(lua_State *L)
//{
//		//CatchNullDocument();//
//        FPProc_interface * pDoc; // note normally thisdoc
//        CFemmviewView * theView;
//        pDoc=(FPProc_interface *)pFemmviewdoc;
//        POSITION pos;
//        pos=pDoc->GetFirstViewPosition();
//        theView=(CFemmviewView *)pDoc->GetNextView(pos);
//
//        theView->EditAction=1; // make sure things update OK
//
//        double mx,my;
//        int i,j,k,m;
//
//        mx=lua_todouble(L,1);
//        my=lua_todouble(L,2);
//
//
////***************
//        {
//                if (pDoc->nodelist.GetSize()>0){
//                        i=pDoc->ClosestNode(mx,my);
//                        CComplex x,y,z;
//                        double R,d1,d2;
//                        int lineno,arcno,flag=0;
//                        z.Set(pDoc->nodelist[i].x,pDoc->nodelist[i].y);
//
//                        if (pDoc->contour.GetSize()>0){
//
//                                //check to see if point is the same as last point in the contour;
//                                y=pDoc->contour[pDoc->contour.GetSize()-1];
//                                if ((y.re==z.re) && (y.im==z.im)) return 0;
//
//                                j=pDoc->ClosestNode(y.re,y.im);
//                                x.Set(pDoc->nodelist[j].x,pDoc->nodelist[j].y);
//
//                                //check to see if this point and the last point are ends of an
//                                //input segment;
//                                lineno=-1;
//                                d1=1.e08;
//
//                                if (abs(x-y)<1.e-08){
//                                        for(k=0;k<pDoc->linelist.GetSize();k++){
//                                                if((pDoc->linelist[k].n0==j) && (pDoc->linelist[k].n1==i))
//                                                {
//                                                        d2=fabs(pDoc->ShortestDistanceFromSegment(mx,my,k));
//                                                        if(d2<d1){
//                                                                lineno=k;
//                                                                d1=d2;
//                                                        }
//                                                }
//                                                if((pDoc->linelist[k].n0==i) && (pDoc->linelist[k].n1==j))
//                                                {
//                                                        d2=fabs(pDoc->ShortestDistanceFromSegment(mx,my,k));
//                                                        if(d2<d1){
//                                                                lineno=k;
//                                                                d1=d2;
//                                                        }
//                                                }
//                                        }
//                                }
//
//                                //check to see if this point and last point are ends of an
//                                // arc segment; if so, add entire arc to the contour;
//                                arcno=-1;
//                                if (abs(x-y)<1.e-08){
//                                        for(k=0;k<pDoc->arclist.GetSize();k++){
//                                                if((pDoc->arclist[k].n0==j) && (pDoc->arclist[k].n1==i))
//                                                {
//                                                        d2=pDoc->ShortestDistanceFromArc(CComplex(mx,my),
//                                                           pDoc->arclist[k]);
//                                                        if(d2<d1){
//                                                                arcno=k;
//                                                                lineno=-1;
//                                                                flag=true;
//                                                                d1=d2;
//                                                        }
//                                                }
//                                                if((pDoc->arclist[k].n0==i) && (pDoc->arclist[k].n1==j))
//                                                {
//                                                        d2=pDoc->ShortestDistanceFromArc(CComplex(mx,my),
//                                                           pDoc->arclist[k]);
//                                                        if(d2<d1){
//                                                                arcno=k;
//                                                                lineno=-1;
//                                                                flag=false;
//                                                                d1=d2;
//                                                        }
//                                                }
//                                        }
//                                }
//
//                        }
//                        else{
//                                pDoc->contour.Add(z);
//                                theView->DrawUserContour(false);
//                                return 0;
//                        }
//
//                        if((lineno<0) && (arcno<0)){
//                                pDoc->contour.Add(z);
//                                theView->DrawUserContour(FALSE);
//                        }
//
//                        if(lineno>=0){
//                                j=pDoc->contour.GetSize();
//                                if(j>1){
//                                        if(abs(pDoc->contour[j-2]-z)<1.e-08){
//                                                //CView::OnLButtonDown(nFlags, point);
//                                                return 0;
//                                        }
//                                }
//                                pDoc->contour.Add(z);
//                                theView->DrawUserContour(FALSE);
//                        }
//                        if(arcno>=0){
//                                k=arcno;
//                                pDoc->GetCircle(pDoc->arclist[k],x,R);
//                                j=(int) ceil(pDoc->arclist[k].ArcLength/pDoc->arclist[k].MaxSideLength);
//                                if(flag==true)
//                                        z=exp(I*pDoc->arclist[k].ArcLength*PI/(180.*((double) j)) );
//                                else
//                                        z=exp(-I*pDoc->arclist[k].ArcLength*PI/(180.*((double) j)) );
//                                for(i=0;i<j;i++){
//                                        y=(y-x)*z+x;
//                                        m=pDoc->contour.GetSize();
//                                        if(m>1){
//                                                if(abs(pDoc->contour[m-2]-y)<1.e-08){
//                                                        //CView::OnLButtonDown(nFlags, point);
//                                                        return 0;
//                                                }
//                                        }
//                                        pDoc->contour.Add(y);
//                                        theView->DrawUserContour(FALSE);
//                                }
//                        }
//                }
//        }
//
//
////*************
//        return 0;
//}
//
//int FPProc_interface::seteditmode(lua_State *L)
//{
//    //CatchNullDocument();//
//	CString EditAction;
//
//    EditAction.Format("%s",lua_tostring(L,1));
//    EditAction.MakeLower();
//
//	FPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(FPProc_interface *)pFemmviewdoc;
//    POSITION pos;
//    pos=thisDoc->GetFirstViewPosition();
//    theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	    if (EditAction=="point")
//		{
//			if (theView->EditAction==1){
//				theView->EraseUserContour(TRUE);
//				thisDoc->contour.RemoveAll();
//			}
//			if (theView->EditAction==2){
//				int i;
//				bool flg=FALSE;
//				thisDoc->bHasMask=FALSE;
//				for(i=0;i<thisDoc->blocklist.GetSize();i++)
//				{
//					if (thisDoc->blocklist[i].IsSelected==true)
//					{
//						thisDoc->blocklist[i].IsSelected=FALSE;
//						flg=true;
//					}
//				}
//				if(flg==true) theView->RedrawView();
//			}
//			theView->EditAction=0;
//	}
//    if (EditAction=="contour")
//	{
//		if(theView->EditAction==2){
//			int i;
//			bool flg=FALSE;
//			thisDoc->bHasMask=FALSE;
//			for(i=0;i<thisDoc->blocklist.GetSize();i++){
//				if (thisDoc->blocklist[i].IsSelected==true)
//				{
//					thisDoc->blocklist[i].IsSelected=FALSE;
//					flg=true;
//				}
//			}
//
//			if(flg==true) theView->RedrawView();
//		}
//		theView->EditAction=1;
//	}
//    if (EditAction=="area")
//	{
//		if(theView->EditAction==1){
//			theView->EraseUserContour(true);
//			thisDoc->contour.RemoveAll();
//		}
//		theView->EditAction=2;
//	}
//
//	return 0;
//}
//
//int FPProc_interface::bendcontour(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(FPProc_interface *)pFemmviewdoc;
//    POSITION pos;
//    pos=thisDoc->GetFirstViewPosition();
//    theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	thisDoc->BendContour(lua_todouble(L,1),lua_todouble(L,2));
//	theView->InvalidateRect(NULL);
//
//	return 0;
//}


///*
//int FPProc_interface::gradient(lua_State *L)
//{
//	// computes the gradients of the B field by differentiating
//	// the shape functions that are used to represent the smoothed
//	// B in an element.
//
//    //CatchNullDocument();//
//    FPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(FPProc_interface *)pFemmviewdoc;
//    POSITION pos;
//    pos=thisDoc->GetFirstViewPosition();
//    theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//    double x,y;
//    int i,n[3];
//    double b[3],c[3],da;
//    CComplex dbxdx,dbxdy,dbydx,dbydy;
//    CElement elm;
//
//    x=lua_todouble(L,1);
//    y=lua_todouble(L,2);
//    i=thisDoc->InTriangle(x,y);
//    if(i<0){
//        for(i=0;i<8;i++) lua_pushnumber(L,0);
//        return 8;
//    }
//
//    elm=thisDoc->meshelem[i];
//    for(i=0;i<3;i++) n[i]=elm.p[i];
//    b[0]=thisDoc->meshnode[n[1]].y - thisDoc->meshnode[n[2]].y;
//    b[1]=thisDoc->meshnode[n[2]].y - thisDoc->meshnode[n[0]].y;
//    b[2]=thisDoc->meshnode[n[0]].y - thisDoc->meshnode[n[1]].y;
//    c[0]=thisDoc->meshnode[n[2]].x - thisDoc->meshnode[n[1]].x;
//    c[1]=thisDoc->meshnode[n[0]].x - thisDoc->meshnode[n[2]].x;
//    c[2]=thisDoc->meshnode[n[1]].x - thisDoc->meshnode[n[0]].x;
//    da=(b[0]*c[1]-b[1]*c[0])*thisDoc->LengthConv[thisDoc->LengthUnits];
//
//	dbxdx=0; dbxdy=0; dbydx=0; dbydy=0;
//    for(i=0;i<3;i++)
//    {
//		dbxdx+=elm.b1[i]*b[i]/da;
//		dbxdy+=elm.b1[i]*c[i]/da;
//		dbydx+=elm.b2[i]*b[i]/da;
//		dbydy+=elm.b2[i]*c[i]/da;
//	}
//
//	if (thisDoc->ProblemType==PLANAR)
//	{
//		// in a source-free region, we can get a little bit
//		// of smoothing by enforcing the dependencies that
//		// are implied by the differential equation.
//		dbxdx=(dbxdx-dbydy)/2.;
//		dbydy=-dbxdx;
//		dbydx=(dbydx+dbxdy)/2;
//		dbxdy=dbydx;
//	}
//
//    lua_pushnumber(L,Re(dbxdx));
//    lua_pushnumber(L,Im(dbxdx));
//	lua_pushnumber(L,Re(dbxdy));
//    lua_pushnumber(L,Im(dbxdy));
//	lua_pushnumber(L,Re(dbydx));
//    lua_pushnumber(L,Im(dbydx));
//	lua_pushnumber(L,Re(dbydy));
//    lua_pushnumber(L,Im(dbydy));
//
//    return 8;
//}
//*/
//
//int FPProc_interface::gradient(lua_State *L)
//{
//	// computes the gradients of the B field by differentiating
//	// the shape functions that are used to represent the smoothed
//	// B in an element.
//
//    //CatchNullDocument();//
//    FPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(FPProc_interface *)pFemmviewdoc;
//    POSITION pos;
//    pos=thisDoc->GetFirstViewPosition();
//    theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//    int i;
//	double xo,yo,x,y,da,p0,p1,p2;
//    CComplex dbxdx,dbxdy,dbydx,dbydy,Mx,My;
//
//    xo=lua_todouble(L,1);
//    yo=lua_todouble(L,2);
//
//	dbxdx=0;
//	dbxdy=0;
//	dbydx=0;
//	dbydy=0;
//
//	for(i=0;i<thisDoc->meshelem.GetSize();i++)
//	{
//		thisDoc->GetMagnetization(i,Mx,My);
//		da=muo*thisDoc->ElmArea(i)/thisDoc->LengthConv[thisDoc->LengthUnits];
//		x=Re(thisDoc->meshelem[i].ctr);
//		y=Im(thisDoc->meshelem[i].ctr);
//
//		p0=PI*pow(pow(x - xo,2.) + pow(y - yo,2.),3.);
//		p1=(-3.*pow(x - xo,2.) + pow(y - yo,2.))*(y - yo);
//		p2=(x - xo)*(pow(x - xo,2.) - 3.*pow(y - yo,2.));
//		dbxdx+=(da*(-(My*p1) + Mx*p2))/p0;
//		dbydx+=(da*(-(Mx*p1) - My*p2))/p0;
//		dbxdy+=(da*(-(Mx*p1) - My*p2))/p0;
//		dbydy+=-(da*(-(My*p1) + Mx*p2))/p0;
//	}
//
//    lua_pushnumber(L,Re(dbxdx));
//    lua_pushnumber(L,Im(dbxdx));
//	lua_pushnumber(L,Re(dbxdy));
//    lua_pushnumber(L,Im(dbxdy));
//	lua_pushnumber(L,Re(dbydx));
//    lua_pushnumber(L,Im(dbydx));
//	lua_pushnumber(L,Re(dbydy));
//    lua_pushnumber(L,Im(dbydy));
//
//    return 8;
//}


////////////////////////////////////////////////////////////////////////////////////
//
// old versions for compatibility
//
////////////////////////////////////////////////////////////////////////////////////

//int FPProc_interface::old_lua_lineintegral(lua_State * L)
//{
//	//CatchNullDocument();//
//	int type;
//	CComplex *z;
//	z=(CComplex *)calloc(4,sizeof(CComplex));
//
//	type=(int) lua_todouble(L,1);
//	// 0- B.n
//	// 1 - H.t
//	// 2 - Cont length
//	// 3 - Force from stress tensor
//	// 4 - Torque from stress tensor
//	// 5 - (B.n)^2
//
//	if (type<0 || type >5)
//	{
//		CString msg;
//		msg.Format("Invalid line integral selected %d",type);
//		lua_error(L,msg.GetBuffer(1));
//		return 0;
//
//	}
//
//	FPProc_interface * thisDoc;
//	CFemmviewView * theView;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	thisDoc->LineIntegral(type,z);
//
//	switch(type)
//	{
//		case 3: // force results
//			if (thisDoc->Frequency!=0){
//				lua_pushnumber(lua,z[2].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,z[3].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,z[0].re);
//				lua_pushnumber(lua,z[0].im);
//				lua_pushnumber(lua,z[1].re);
//				lua_pushnumber(lua,z[1].im);
//				free(z);
//				return 8;
//			}
//			else{
//				lua_pushnumber(lua,z[0].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,z[1].re);
//				lua_pushnumber(lua,0);
//				free(z);
//				return 4;
//			}
//
//
//		case 4: // torque results
//			if(thisDoc->Frequency!=0){
//				lua_pushnumber(lua,z[1].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,z[0].re);
//				lua_pushnumber(lua,z[0].im);
//				free(z);
//				return 4;
//			}
//			else
//			{
//				lua_pushnumber(lua,z[0].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,0);
//
//				free(z);
//				return 4;
//			}
//
//		default:
//			lua_pushnumber(lua,z[0].re);
//			lua_pushnumber(lua,z[0].im);
//
//			lua_pushnumber(lua,z[1].re);
//			lua_pushnumber(lua,z[1].im);
//
//			free(z);
//			return 4;
//	}
//
//	free(z);
//	return 0;
//}
//
//int FPProc_interface::old_lua_getpointvals(lua_State * L)
//{
//	//CatchNullDocument();//
//	double px,py;
//
//	px=lua_todouble(L,1);
//	py=lua_todouble(L,2);
//
//	FPProc_interface * thisDoc;
//	CFemmviewView * theView;
//
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	CPointVals u;
//	CComplex Jtot;
//
//	if(thisDoc->GetPointValues(px, py, u)==true)
//	{
//		lua_pushnumber(L,u.A.re);
//		lua_pushnumber(L,u.A.im);
//		lua_pushnumber(L,u.B1.re);
//		lua_pushnumber(L,u.B1.im);
//		lua_pushnumber(L,u.B2.re);
//		lua_pushnumber(L,u.B2.im);
//		lua_pushnumber(L,u.c);
//		lua_pushnumber(L,u.E);
//		lua_pushnumber(L,u.H1.re);
//		lua_pushnumber(L,u.H1.im);
//		lua_pushnumber(L,u.H2.re);
//		lua_pushnumber(L,u.H2.im);
//		lua_pushnumber(L,u.Je.re);
//		lua_pushnumber(L,u.Je.im);
//		lua_pushnumber(L,u.Js.re);
//		lua_pushnumber(L,u.Js.im);
//		lua_pushnumber(L,u.mu1.re);
//		lua_pushnumber(L,u.mu1.im);
//		lua_pushnumber(L,u.mu2.re);
//		lua_pushnumber(L,u.mu2.im);
//		lua_pushnumber(L,u.Pe);
//		lua_pushnumber(L,u.Ph);
//		lua_pushnumber(L,u.ff);
//		return 23;
//	}
//
//	return 0;
//}
//
//int FPProc_interface::old_lua_blockintegral(lua_State * L)
//{
//	//CatchNullDocument();//
//	int type;
//	type=(int) lua_todouble(L,1);
//
//	FPProc_interface *thisDoc;
//	CFemmviewView * theView;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	//CatchNullDocument();//
//
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	CComplex z;
//
//	int i;
//	bool flg=FALSE;
//	for(i=0;i<thisDoc->blocklist.GetSize();i++)
//		if (thisDoc->blocklist[i].IsSelected==true) flg=TRUE;
//	if(flg==FALSE)
//	{
//		CString msg="Cannot integrate\nNo area has been selected";
//		lua_error(L,msg.GetBuffer(1));
//		return 0;
//	}
//	if((type>=18) && (type<=23)) thisDoc->MakeMask();
//	z=thisDoc->BlockIntegral(type);
//
//	lua_pushnumber(L,z.re);
//	lua_pushnumber(L,z.im);
//
//	return 2;
//}
//
//int FPProc_interface::old_lua_getcircuitprops(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * TheDoc; // note normally thisdoc
//	CFemmviewView * theView;
//	TheDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=TheDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)TheDoc->GetNextView(pos);
//
//
//	int NumCircuits,k;
//	CString circuitname;
//	circuitname=lua_tostring(L,1);
//	k=-1;
//
//	// ok we need to find the correct entry for the circuit name
//	NumCircuits=TheDoc->circproplist.GetSize();
//	for(int i=0;i<NumCircuits;i++)
//		{
//			if(TheDoc->circproplist[i].CircName==circuitname)
//			{
//				k=i;
//				i=NumCircuits; // that will break it
//			}
//		}
//
//	// trap errors
//
//	if(k==-1)
//	{
//		CString msg="Unknown circuit";
//		lua_error(L,msg.GetBuffer(1));
//		return 0;
//	}
//
//	CComplex amps,volts,fluxlinkage;
//
//	amps =TheDoc->circproplist[k].Amps;
//	volts=TheDoc->GetVoltageDrop(k);
//	fluxlinkage=TheDoc->GetFluxLinkage(k);
//
//	lua_pushnumber(L,Re(amps));
//	lua_pushnumber(L,Im(amps));
//	lua_pushnumber(L,Re(volts));
//	lua_pushnumber(L,Im(volts));
//	lua_pushnumber(L,Re(fluxlinkage));
//	lua_pushnumber(L,Im(fluxlinkage));
//
//	return 6;
//}
//
//int FPProc_interface::gettitle(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	lua_pushstring(L,thisDoc->GetTitle());
//
//	return 1;
//}
//
//
/////////////////////////////////////////////////////////
//// Functions that provide low-level access to the mesh
/////////////////////////////////////////////////////////
//
//int FPProc_interface::numnodes(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	lua_pushnumber(L,thisDoc->meshnode.GetSize());
//
//	return 1;
//}
//
//int FPProc_interface::numelements(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	lua_pushnumber(L,thisDoc->meshelem.GetSize());
//
//	return 1;
//}
//
//int FPProc_interface::getnode(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	int k=(int) lua_todouble(L,1);
//	k--;
//	if ((k<0) || (k>=thisDoc->meshnode.GetSize())) return 0;
//
//	lua_pushnumber(L,thisDoc->meshnode[k].x);
//	lua_pushnumber(L,thisDoc->meshnode[k].y);
//
//	return 2;
//}
//
//int FPProc_interface::getelement(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	int k=(int) lua_todouble(L,1);
//	k--;
//	if ((k<0) || (k>=thisDoc->meshelem.GetSize())) return 0;
//
//	lua_pushnumber(L,thisDoc->meshelem[k].p[0]+1);
//	lua_pushnumber(L,thisDoc->meshelem[k].p[1]+1);
//	lua_pushnumber(L,thisDoc->meshelem[k].p[2]+1);
//	lua_pushnumber(L,Re(thisDoc->meshelem[k].ctr));
//	lua_pushnumber(L,Im(thisDoc->meshelem[k].ctr));
//	lua_pushnumber(L,thisDoc->ElmArea(k));
//	lua_pushnumber(L,thisDoc->blocklist[thisDoc->meshelem[k].lbl].InGroup);
//
//	return 7;
//}


/////////////////////////  JUNK //////////////////////////////

//int FPProc_interface::dumpheader(lua_State * L)
//{
//// 	CatchNullDocument();
//	FPProc_interface * thisDoc;
//	CFemmviewView * theView;
//
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	return 0;
//}


// void FPProc_interface::initalise_lua()
// {
// 	// use a messagebox to display errors
// 	lua_register(lua,"mo_setfocus",lua_switchfocus);
// 	lua_register(lua,"mo_getpointvalues",lua_getpointvals);
// 	lua_register(lua,"mo_close",lua_exitpost);
// 	lua_register(lua,"mo_addcontour",lua_addcontour);
// 	lua_register(lua,"mo_clearcontour",lua_clearcontour);
// 	lua_register(lua,"mo_lineintegral",lua_lineintegral);
// 	lua_register(lua,"mo_selectblock",lua_selectblock);
// 	lua_register(lua,"mo_groupselectblock",lua_groupselectblock);
// 	lua_register(lua,"mo_clearblock",lua_clearblock);
// 	lua_register(lua,"mo_blockintegral",lua_blockintegral);
// 	lua_register(lua,"mo_zoomnatural",lua_zoomnatural);
// 	lua_register(lua,"mo_zoomin",lua_zoomin);
// 	lua_register(lua,"mo_zoomout",lua_zoomout);
// 	lua_register(lua,"mo_zoom",lua_zoom);
// 	lua_register(lua,"mo_showgrid",lua_showgrid);
// 	lua_register(lua,"mo_hidegrid",lua_hidegrid);
// 	lua_register(lua,"mo_gridsnap",lua_gridsnap);
// 	lua_register(lua,"mo_setgrid",lua_setgrid);
// 	lua_register(lua,"mo_showmesh",lua_showmesh);
// 	lua_register(lua,"mo_hidemesh",lua_hidemesh);
// 	lua_register(lua,"mo_hidedensityplot",lua_hidedensity);
// 	lua_register(lua,"mo_showdensityplot",lua_showdensity);
// 	lua_register(lua,"mo_hidecontourplot",lua_hidecountour);
// 	lua_register(lua,"mo_showcontourplot",lua_showcountour);
// 	lua_register(lua,"mo_showvectorplot",lua_vectorplot);
// 	lua_register(lua,"mo_smooth",lua_smoothing);
// 	lua_register(lua,"mo_showpoints",lua_showpoints);
// 	lua_register(lua,"mo_hidepoints",lua_hidepoints);
// 	lua_register(lua,"mo_shownames",lua_shownames);
// 	lua_register(lua,"mo_getprobleminfo",lua_getprobleminfo);
// 	lua_register(lua,"mo_savebitmap",lua_savebitmap);
// 	lua_register(lua,"mo_getcircuitproperties",lua_getcircuitprops);
// 	lua_register(lua,"mo_savemetafile",lua_saveWMF);
//     lua_register(lua,"mo_refreshview",lua_refreshview);
// 	lua_register(lua,"mo_selectpoint",lua_selectline);
// 	lua_register(lua,"mo_seteditmode",lua_seteditmode);
// 	lua_register(lua,"mo_bendcontour",lua_bendcontour);
// 	lua_register(lua,"mo_makeplot",lua_makeplot);
// 	lua_register(lua,"mo_reload",lua_reload);
// 	lua_register(lua,"mo_gradient",lua_gradient);
// 	lua_register(lua,"mo_gettitle",lua_gettitle);
// 	lua_register(lua,"mo_get_title",lua_gettitle);
//
// 	lua_register(lua,"mo_resize",luaResize);
// 	lua_register(lua,"mo_minimize",luaMinimize);
// 	lua_register(lua,"mo_maximize",luaMaximize);
// 	lua_register(lua,"mo_restore", luaRestore);
//
// 	// compatibility with alternate lua function naming convention
// 	lua_register(lua,"mo_set_focus",lua_switchfocus);
// 	lua_register(lua,"mo_get_point_values",lua_getpointvals);
// 	lua_register(lua,"mo_add_contour",lua_addcontour);
// 	lua_register(lua,"mo_clear_contour",lua_clearcontour);
// 	lua_register(lua,"mo_line_integral",lua_lineintegral);
// 	lua_register(lua,"mo_select_block",lua_selectblock);
// 	lua_register(lua,"mo_group_select_block",lua_groupselectblock);
// 	lua_register(lua,"mo_clear_block",lua_clearblock);
// 	lua_register(lua,"mo_block_integral",lua_blockintegral);
// 	lua_register(lua,"mo_zoom_natural",lua_zoomnatural);
// 	lua_register(lua,"mo_zoom_in",lua_zoomin);
// 	lua_register(lua,"mo_zoom_out",lua_zoomout);
// 	lua_register(lua,"mo_show_grid",lua_showgrid);
// 	lua_register(lua,"mo_hide_grid",lua_hidegrid);
// 	lua_register(lua,"mo_show_mesh",lua_showmesh);
// 	lua_register(lua,"mo_hide_mesh",lua_hidemesh);
// 	lua_register(lua,"mo_hide_density_plot",lua_hidedensity);
// 	lua_register(lua,"mo_show_density_plot",lua_showdensity);
// 	lua_register(lua,"mo_hide_contour_plot",lua_hidecountour);
// 	lua_register(lua,"mo_show_contour_plot",lua_showcountour);
// 	lua_register(lua,"mo_show_vector_plot",lua_vectorplot);
// 	lua_register(lua,"mo_show_points",lua_showpoints);
// 	lua_register(lua,"mo_hide_points",lua_hidepoints);
// 	lua_register(lua,"mo_grid_snap",lua_gridsnap);
// 	lua_register(lua,"mo_set_grid",lua_setgrid);
// 	lua_register(lua,"mo_get_problem_info",lua_getprobleminfo);
// 	lua_register(lua,"mo_save_bitmap",lua_savebitmap);
// 	lua_register(lua,"mo_get_circuit_properties",lua_getcircuitprops);
// 	lua_register(lua,"mo_save_metafile",lua_saveWMF);
//     lua_register(lua,"mo_refresh_view",lua_refreshview);
// 	lua_register(lua,"mo_select_point",lua_selectline);
// 	lua_register(lua,"mo_set_edit_mode",lua_seteditmode);
// 	lua_register(lua,"mo_bend_contour",lua_bendcontour);
// 	lua_register(lua,"mo_make_plot",lua_makeplot);
// 	lua_register(lua,"mo_show_names",lua_shownames);
//
// 	// functions to access low-level mesh info
// 	lua_register(lua,"mo_numnodes",lua_numnodes);
// 	lua_register(lua,"mo_numelements",lua_numelements);
// 	lua_register(lua,"mo_getnode",lua_getnode);
// 	lua_register(lua,"mo_getelement",lua_getelement);
//
// 	lua_register(lua,"mo_num_nodes",lua_numnodes);
// 	lua_register(lua,"mo_num_elements",lua_numelements);
// 	lua_register(lua,"mo_get_node",lua_getnode);
// 	lua_register(lua,"mo_get_element",lua_getelement);
//
// 	pFemmviewdoc=this;
//
// }


//int FPProc_interface::reload(lua_State * L)
//{
//	FPProc_interface * thisDoc;
//	CFemmviewView * theView;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	theView->BeginWaitCursor();
//	thisDoc->OnReload();
//	theView->EndWaitCursor();
//	if (theView->d_ResetOnReload==FALSE) thisDoc->FirstDraw=FALSE;
//	theView->RedrawView();
//	theView->LuaViewInfo();
//	return 0;
//}

