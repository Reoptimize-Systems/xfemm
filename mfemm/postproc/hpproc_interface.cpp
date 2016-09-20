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
#include "hpproc.h"
#include "hpproc_interface.h"


using namespace std;

void HPProcInterfaceWarning(const char* warningmsg)
{
    mexWarnMsgIdAndTxt("MFEMM:hpproc", warningmsg);
}

//////////////////      MEX Interface        //////////////////////

HPProc_interface::HPProc_interface()
{
    theHPProc.WarnMessage = &HPProcInterfaceWarning;
}

//HPProc_interface::HPProc_interface(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
//{
//    theHPProc.OpenDocument(filename);
//}

int HPProc_interface::opendocument(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char *input_buf;
    double *outpointer;
    int wasopened;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    /* 3rd input must be a string (first two are used for the class interface) */
    if ( mxIsChar(prhs[2]) != 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:inputNotString",
                           "Input must be a string containing the file name.");

    /* input must be a row vector */
    if (mxGetM(prhs[2])!=1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:inputNotVector",
                           "Input must be a row vector.");

    /* copy the string data from prhs[2] into a C string input_ buf.    */
    input_buf = mxArrayToString(prhs[2]);
    // create a std::string from the input C string
    string filename(input_buf);
    // free the char array as we are done with it
    mxFree(input_buf);

    // call the OpenDocument method with the input
    wasopened = theHPProc.OpenDocument(filename);

    if (!wasopened)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:inputNotVector",
                           "File could not be opened.");

    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);
    outpointer[0] = (double)(wasopened);

    // Generate error if any regions are multiply defined
    // (i.e. tagged by more than one block label)
    if (theHPProc.bMultiplyDefinedLabels)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:regionsmultiplydefined",
                           "Some regions in the problem have been defined by more than one block label.");

    return wasopened;
}

int HPProc_interface::temperaturebounds(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *outpointerRe; 

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(2), mxREAL);
    
    // get a pointer to the start of the actual output data array
    outpointerRe = mxGetPr(plhs[0]);
    
    outpointerRe[0] = theHPProc.A_Low;
    outpointerRe[1] = theHPProc.A_High;
    
    return 0;
}


int HPProc_interface::getpointvals(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *px, *py, *outpointerRe;
    size_t mxrows, myrows, nxcols, nycols;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "Two inputs required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
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
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "x and y must both be column vectors.");
    }

    if(myrows != myrows)
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "x and y must be column vectors of the same size.");
    }

   
#ifdef _MEX_DEBUG
        mexPrintf("Frequency was zero.\n");
#endif
    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(7), (mwSize)mxrows, mxREAL);
    // get a pointer to the start of the actual output data array
    outpointerRe = mxGetPr(plhs[0]);

    for(int i=0; i<(int)mxrows; i++)
    {
        CPointVals u;

        if(theHPProc.GetPointValues(px[i], py[i], u)==true)
        {
            // copy the point values to the matlab array at the
            // appropriate locations
#ifdef _MEX_DEBUG
            mexPrintf("row %i, theHPProc.GetPointValues was true.\n", i);
#endif
            outpointerRe[(i*7)] = u.T;
            outpointerRe[(i*7)+1] = u.F.Re();
            outpointerRe[(i*7)+2] = u.F.Im();
            outpointerRe[(i*7)+3] = u.G.Re();
            outpointerRe[(i*7)+4] = u.G.Im();
            outpointerRe[(i*7)+5] = u.K.Re();
            outpointerRe[(i*7)+6] = u.K.Im();
        }
        else
        {
            // we return nan values to alert the user
#ifdef _MEX_DEBUG
            mexPrintf("row %i, theHPProc.GetPointValues was false.\n", i);
#endif
            outpointerRe[(i*7)] = mxGetNaN();
            outpointerRe[(i*7)+1] = mxGetNaN();
            outpointerRe[(i*7)+2] = mxGetNaN();
            outpointerRe[(i*7)+3] = mxGetNaN();
            outpointerRe[(i*7)+4] = mxGetNaN();
            outpointerRe[(i*7)+5] = mxGetNaN();
            outpointerRe[(i*7)+6] = mxGetNaN();
        }
    }

    return 7;
}


int HPProc_interface::addcontour(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    CComplex z;
    int nconpts;
    double *px, *py, *outpointer;
    size_t mxrows, myrows, nxcols, nycols;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "Two inputs required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
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
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "x and y must both be row vectors.");
    }

    if(mxrows != myrows)
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "x and y must be row vectors of the same size.");
    }

    for (int ind=0; ind<mxrows; ind++)
    {

        z.Set(px[ind],py[ind]);

        nconpts = theHPProc.contour.size();

        if(nconpts>0)
        {
            if (z != theHPProc.contour[nconpts-1])
            {
                theHPProc.contour.push_back(z);
            }
        }
        else
        {
            theHPProc.contour.push_back(z);
        }

    }

    // now return the actual points in the current contour
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(theHPProc.contour.size()), (mwSize)(2), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);
    nconpts = theHPProc.contour.size();
    for (int ind=0; ind<nconpts; ind++)
    {
        outpointer[ind] = theHPProc.contour[ind].re;
        outpointer[ind+nconpts] = theHPProc.contour[ind].im;
    }

    return 0;
}

int HPProc_interface::clearcontour()
{
    theHPProc.contour.clear();

    return 0;
}

int HPProc_interface::lineintegral(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int type;
    double *z;
    double *ptype, *outpointerRe, *outpointerIm;
    size_t mrows, ncols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    /*  get the dimensions of the matrix input x */
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    // check dimensions are allowed
    if((mrows!=1) | (ncols!=1))
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "type must be a scalar.");
    }

    // get a pointer to the actual data
    ptype = mxGetPr(prhs[2]);

    // cast the value in the matlab matrix to an integer
    type = (int) ptype[0];
    // 0 - Temperature difference (Gt)
    // 1 - Heat flux through the contour (Fn)
    // 2 - Contour length and surface area
    // 3 - Average Temperature

    if (type<0 || type >3)
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidinttype",
                           "Invalid line integral selected %d",type);
        return 0;
    }

    // Create an array of CComplex to hold the results of the integral
    z = (double *)calloc(2,sizeof(double));
    theHPProc.LineIntegral(type,z);

    switch(type)
    {
    case 0: // Temperature difference bewteen points
      
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z[0]; // contour length
        free(z);
        return 0;

    case 1: // Heat Flux

        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z[0];
        outpointerRe[1] = z[1];

        free(z);
        return 1;


    case 2: // contour length and surface area
      
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z[0];
        outpointerRe[1] = z[1];

        free(z);
        return 2;
    
    case 3: // Temperature
      
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z[0];
        outpointerRe[1] = z[1];

        free(z);
        return 2;
      

    default:
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidinttype",
                           "Invalid line integral selected %d",type);
        return 0;
    }

    free(z);
    return 0;
}

int HPProc_interface::selectblock(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    //CatchNullDocument();//
    double *px, *py, *outpointer;
    size_t mxrows, myrows, nxcols, nycols;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
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
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "x and y must both be scalars.");
    }

    int k;

    if (theHPProc.meshelem.size()>0)
    {
        k = theHPProc.InTriangle(px[0],py[0]);

        if(k>=0)
        {
            theHPProc.bHasMask = false;
            theHPProc.blocklist[theHPProc.meshelem[k].lbl].ToggleSelect();
        }
    }

    return 0;
}

int HPProc_interface::groupselectblock(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int j,k,n;
    double *pgroup;
    size_t mrows, ncols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    /*  get the dimensions of the matrix input x */
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    // check dimensions are allowed
    if((mrows>1) | (ncols>1))
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "group number must be a scalar.");
    }

    if ((mrows==0) | (ncols==0))
    {
        // select all the blocks
        theHPProc.bHasMask = false;

        for(int i=0; i<theHPProc.blocklist.size(); i++)
        {
            if (theHPProc.blocklist[i].IsSelected == false)
            {
                theHPProc.blocklist[i].IsSelected = true;
            }
        }
    }
    else
    {
        // select blocks in the specified group

        // get a pointer to the actual data
        pgroup = mxGetPr(prhs[2]);

        if (theHPProc.meshelem.size()>0)
        {
            k = 0;

            if (pgroup[0] > 0)
            {
                k = (int)pgroup[0];
            }

            for(j=0; j<theHPProc.blocklist.size(); j++)
            {
                if ((theHPProc.blocklist[j].InGroup==k) || (pgroup[0]==0))
                {
                    theHPProc.blocklist[j].ToggleSelect();
                }

                theHPProc.bHasMask = false;
            }
        }
    }

    return 0;
}

int HPProc_interface::clearblock()
{
    theHPProc.bHasMask = false;

    for(int i=0; i<theHPProc.blocklist.size(); i++)
    {
        if (theHPProc.blocklist[i].IsSelected == true)
        {
            theHPProc.blocklist[i].IsSelected = false;
        }
    }

    return 0;
}

int HPProc_interface::blockintegral(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int type;

    double *ptype, *outpointerRe, *outpointerIm;
    size_t mrows, ncols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs != 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Wrong number of output arguments.");

    /*  get the dimensions of the matrix input x */
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    // check dimensions are allowed
    if((mrows!=1) | (ncols!=1))
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "type must be a scalar.");
    }

    // get a pointer to the actual data
    ptype = mxGetPr(prhs[2]);

    // cast the value in the matlab matrix to an integer
    type = (int) ptype[0];

    CComplex z;

    bool flg = false;
    for(unsigned int i=0; i<theHPProc.blocklist.size(); i++)
    {
        if (theHPProc.blocklist[i].IsSelected==true)
        {
            flg = true;
        }
    }

    if(flg==false)
    {
        mexErrMsgIdAndTxt("MFEMM:hpproc:noblockselected",
                          "Block integral failed, no area has been selected");
    }

    if((type<0) || (type>4))
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidinttype",
                           "Invalid block integral selected %d",type);
        return 0;
    }
    
//    if((type>=18) && (type<=23))
//    {
//        theHPProc.MakeMask();
//    }

    z = theHPProc.BlockIntegral(type);

    switch (type)
    {
      case 0: // T
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z.Re();
        break;
      case 1: // Area
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z.Re();
        break;
      case 2: // Volume
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z.Re();
        break;
      case 3: // F
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxCOMPLEX);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z.Re();
        outpointerRe[1] = z.Im();
        break;
      case 4: // G
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxCOMPLEX);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z.Re();
        outpointerRe[1] = z.Im();
        break;
      default:
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidinttype",
                           "Invalid block integral selected %d",type);
        return 0;
    
    }

    return 1;
}


int HPProc_interface::smoothon()
{
    theHPProc.Smooth = true;
    return 0;
}

int HPProc_interface::smoothoff()
{
    theHPProc.Smooth = false;
    return 0;
}

int HPProc_interface::getprobleminfo(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *outpointer;

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(3), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);
    outpointer[0] = theHPProc.problemType;
    outpointer[1] = theHPProc.Depth;
    outpointer[2] = theHPProc.LengthConv[theHPProc.LengthUnits];

    return 3;
}

int HPProc_interface::getconductorprops(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int NumCircuits,k;
    char circuitname[1024];
    k=-1;

    if (nrhs < 3 || mxGetString(prhs[2], circuitname, sizeof(circuitname)))
    {
        mexErrMsgTxt("Input should be a string containng a circuit name less than 1024 characters long.");
    }

    // ok we need to find the correct entry for the circuit name
    NumCircuits = theHPProc.circproplist.size();

    for(int i=0; i<NumCircuits; i++)
    {
        if(strcmp(theHPProc.circproplist[i].CircName.c_str(), circuitname) == 0)
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

    double temperature = theHPProc.circproplist[k].V;
    double flux = theHPProc.circproplist[k].q;

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    double *outpointerRe = mxGetPr(plhs[0]);
    outpointerRe[0] = temperature;
    outpointerRe[1] = flux;

    return 2;
}


int HPProc_interface::numnodes(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *outpointer;

    /* check for proper number of arguments */
    if(nrhs!=2)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "No input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

	outpointer[0] = (double)theHPProc.meshnode.size();

	return 1;
}

int HPProc_interface::numelements(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *outpointer;

    /* check for proper number of arguments */
    if(nrhs!=2)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "No input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

	outpointer[0] = (double)theHPProc.meshelem.size();

	return 1;
}


int HPProc_interface::getelements(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_elmnos, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    p_elmnos = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if (nxcols>1)
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "element nos, must be a column vector.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)mxrows, (mwSize)(7), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    int numelms = theHPProc.meshelem.size();
    int i = 0;

    // check no invalid element numbers have been requested
    for (i = 0; i < (int)mxrows; i++)
    {
        if (std::floor(p_elmnos[i]) < 1.0)
        {
            mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidelmno",
                               "Element numbers start from 1.");
        }

        if (std::floor(p_elmnos[i]) > numelms)
        {
            mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidelmno",
                               "Invalid element number (bigger than number of elements).");
        }
    }

    for(i=0; i<(int)mxrows; i++)
    {
        // copy the element info to the matlab array at the
        // appropriate locations
        int n = (int)std::floor(p_elmnos[i]) - 1;
        outpointer[i] = theHPProc.meshelem[n].p[0]+1;
        outpointer[i+(int)mxrows] = theHPProc.meshelem[n].p[1]+1;
        outpointer[i+2*(int)mxrows] = theHPProc.meshelem[n].p[2]+1;
        outpointer[i+3*(int)mxrows] = Re(theHPProc.meshelem[n].ctr);
        outpointer[i+4*(int)mxrows] = Im(theHPProc.meshelem[n].ctr);
        outpointer[i+5*(int)mxrows] = theHPProc.ElmArea(n);
        outpointer[i+6*(int)mxrows] = theHPProc.blocklist[theHPProc.meshelem[n].lbl].InGroup;
    }

	return 7;
}

int HPProc_interface::getcentroids(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_elmnos, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    p_elmnos = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if (nxcols>1)
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "element nos, must be a column vector.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)mxrows, (mwSize)(2), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    int numelms = theHPProc.meshelem.size();
    int i = 0;

    // check no invalid element numbers have been requested
    for (i = 0; i < (int)mxrows; i++)
    {
        if (std::floor(p_elmnos[i]) < 1.0)
        {
            mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidelmno",
                               "Element numbers start from 1.");
        }

        if (std::floor(p_elmnos[i]) > numelms)
        {
            mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidelmno",
                               "Invalid element number (bigger than number of elements).");
        }
    }

    for(i=0; i<(int)mxrows; i++)
    {
        // copy the element info to the matlab array at the
        // appropriate locations
        int n = (int)std::floor(p_elmnos[i]) - 1;
        outpointer[i] = Re(theHPProc.meshelem[n].ctr);
        outpointer[i+(int)mxrows] = Im(theHPProc.meshelem[n].ctr);
    }

	return 0;
}

int HPProc_interface::getareas(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_elmnos, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    p_elmnos = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if (nxcols>1)
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "element nos, must be a column vector.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)mxrows, (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    int numelms = theHPProc.meshelem.size();
    int i = 0;

    // check no invalid element numbers have been requested
    for (i = 0; i < (int)mxrows; i++)
    {
        if (std::floor(p_elmnos[i]) < 1.0)
        {
            mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidelmno",
                               "Element numbers start from 1.");
        }

        if (std::floor(p_elmnos[i]) > numelms)
        {
            mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidelmno",
                               "Invalid element number (bigger than number of elements).");
        }
    }

    for(i=0; i<(int)mxrows; i++)
    {
        // copy the element info to the matlab array at the
        // appropriate locations
        int n = (int)std::floor(p_elmnos[i]) - 1;
        outpointer[i] = theHPProc.ElmArea(n);
    }

	return 0;
}


int HPProc_interface::getvertices(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_elmnos, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    p_elmnos = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if (nxcols>1)
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "element nos, must be a column vector.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)mxrows, (mwSize)(6), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);

    int numelms = theHPProc.meshelem.size();
    int i = 0;

    // check no invalid element numbers have been requested
    for (i = 0; i < (int)mxrows; i++)
    {
        if (std::floor(p_elmnos[i]) < 1.0)
        {
            mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidelmno",
                               "Element numbers start from 1.");
        }

        if (std::floor(p_elmnos[i]) > numelms)
        {
            mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidelmno",
                               "Invalid element number (bigger than number of elements).");
        }
    }

    for(i=0; i<(int)mxrows; i++)
    {
        // copy the element info to the matlab array at the
        // appropriate locations
        int n = (int)std::floor(p_elmnos[i]) - 1;
        outpointer[i] = theHPProc.meshnode[theHPProc.meshelem[n].p[0]].x;
        outpointer[i+(int)mxrows] = theHPProc.meshnode[theHPProc.meshelem[n].p[0]].y;
        outpointer[i+2*(int)mxrows] = theHPProc.meshnode[theHPProc.meshelem[n].p[1]].x;
        outpointer[i+3*(int)mxrows] = theHPProc.meshnode[theHPProc.meshelem[n].p[1]].y;
        outpointer[i+4*(int)mxrows] = theHPProc.meshnode[theHPProc.meshelem[n].p[2]].x;
        outpointer[i+5*(int)mxrows] = theHPProc.meshnode[theHPProc.meshelem[n].p[2]].y;
    }

	return 0;
}



int HPProc_interface::numgroupelements (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (nxcols != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int groupno = (int)p_gpno[0];
    int numingroup = countGroupElements (groupno);

    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
    outpointer = mxGetPr(plhs[0]);
    outpointer[0] = (double)numingroup;

    return 1;
}



int HPProc_interface::countGroupElements (int groupno)
{
    int numingroup = 0;

    // count group elements
    for (int i = 0; i < theHPProc.meshelem.size (); i++)
    {
        if (theHPProc.blocklist[theHPProc.meshelem[i].lbl].InGroup == groupno)
        {
            numingroup++;
        }
    }

    return numingroup;
}


int HPProc_interface::getgroupelements(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (nxcols != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int numelms = theHPProc.meshelem.size();
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
            if (theHPProc.blocklist[theHPProc.meshelem[i].lbl].InGroup == groupno)
            {
                // copy the element info to the matlab array at the
                // appropriate locations
                outpointer[n] = theHPProc.meshelem[i].p[0]+1;
                outpointer[n+numingroup] = theHPProc.meshelem[i].p[1]+1;
                outpointer[n+2*numingroup] = theHPProc.meshelem[i].p[2]+1;
                outpointer[n+3*numingroup] = Re(theHPProc.meshelem[i].ctr);
                outpointer[n+4*numingroup] = Im(theHPProc.meshelem[i].ctr);
                outpointer[n+5*numingroup] = theHPProc.ElmArea(i);
                outpointer[n+6*numingroup] = theHPProc.blocklist[theHPProc.meshelem[i].lbl].InGroup;

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

int HPProc_interface::getgroupcentroids(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (mxrows != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int numelms = theHPProc.meshelem.size();
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
            if (theHPProc.blocklist[theHPProc.meshelem[i].lbl].InGroup == groupno)
            {
                // copy the element info to the matlab array at the
                // appropriate locations
                outpointer[n] = Re(theHPProc.meshelem[i].ctr);
                outpointer[n+numingroup] = Im(theHPProc.meshelem[i].ctr);

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

int HPProc_interface::getgroupareas(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (mxrows != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int numelms = theHPProc.meshelem.size();
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
            if (theHPProc.blocklist[theHPProc.meshelem[i].lbl].InGroup == groupno)
            {
                // copy the element info to the matlab array at the
                // appropriate locations
                outpointer[n] = theHPProc.ElmArea(i);

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

int HPProc_interface::getgroupvertices(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *p_gpno, *outpointer;
    size_t mxrows, nxcols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MFEMM:hpproc:maxlhs",
                           "Too many output arguments.");

    p_gpno = mxGetPr(prhs[2]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    nxcols = mxGetN(prhs[2]);

    if ((nxcols != 1) | (mxrows != 1))
    {
        mexErrMsgIdAndTxt( "MFEMM:hpproc:invalidSizeInputs",
                           "group no, must be a scalar.");
    }

    int numelms = theHPProc.meshelem.size();
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
            if (theHPProc.blocklist[theHPProc.meshelem[i].lbl].InGroup == groupno)
            {

                outpointer[n] = theHPProc.meshnode[theHPProc.meshelem[i].p[0]].x;
                outpointer[n+numingroup] = theHPProc.meshnode[theHPProc.meshelem[i].p[0]].y;
                outpointer[n+2*numingroup] = theHPProc.meshnode[theHPProc.meshelem[i].p[1]].x;
                outpointer[n+3*numingroup] = theHPProc.meshnode[theHPProc.meshelem[i].p[1]].y;
                outpointer[n+4*numingroup] = theHPProc.meshnode[theHPProc.meshelem[i].p[2]].x;
                outpointer[n+5*numingroup] = theHPProc.meshnode[theHPProc.meshelem[i].p[2]].y;

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
//int HPProc_interface::selectline(lua_State *L)
//{
//		//CatchNullDocument();//
//        HPProc_interface * pDoc; // note normally thisdoc
//        CFemmviewView * theView;
//        pDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::seteditmode(lua_State *L)
//{
//    //CatchNullDocument();//
//	CString EditAction;
//
//    EditAction.Format("%s",lua_tostring(L,1));
//    EditAction.MakeLower();
//
//	HPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::bendcontour(lua_State *L)
//{
//	//CatchNullDocument();//
//	HPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::gradient(lua_State *L)
//{
//	// computes the gradients of the B field by differentiating
//	// the shape functions that are used to represent the smoothed
//	// B in an element.
//
//    //CatchNullDocument();//
//    HPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::gradient(lua_State *L)
//{
//	// computes the gradients of the B field by differentiating
//	// the shape functions that are used to represent the smoothed
//	// B in an element.
//
//    //CatchNullDocument();//
//    HPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(HPProc_interface *)pFemmviewdoc;
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

//int HPProc_interface::old_lua_lineintegral(lua_State * L)
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
//	HPProc_interface * thisDoc;
//	CFemmviewView * theView;
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::old_lua_getpointvals(lua_State * L)
//{
//	//CatchNullDocument();//
//	double px,py;
//
//	px=lua_todouble(L,1);
//	py=lua_todouble(L,2);
//
//	HPProc_interface * thisDoc;
//	CFemmviewView * theView;
//
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::old_lua_blockintegral(lua_State * L)
//{
//	//CatchNullDocument();//
//	int type;
//	type=(int) lua_todouble(L,1);
//
//	HPProc_interface *thisDoc;
//	CFemmviewView * theView;
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::old_lua_getcircuitprops(lua_State *L)
//{
//	//CatchNullDocument();//
//	HPProc_interface * TheDoc; // note normally thisdoc
//	CFemmviewView * theView;
//	TheDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::gettitle(lua_State *L)
//{
//	//CatchNullDocument();//
//	HPProc_interface * thisDoc;
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::numnodes(lua_State *L)
//{
//	//CatchNullDocument();//
//	HPProc_interface * thisDoc;
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
//
//	lua_pushnumber(L,thisDoc->meshnode.GetSize());
//
//	return 1;
//}
//
//int HPProc_interface::numelements(lua_State *L)
//{
//	//CatchNullDocument();//
//	HPProc_interface * thisDoc;
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
//
//	lua_pushnumber(L,thisDoc->meshelem.GetSize());
//
//	return 1;
//}
//
//int HPProc_interface::getnode(lua_State *L)
//{
//	//CatchNullDocument();//
//	HPProc_interface * thisDoc;
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
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
//int HPProc_interface::getelement(lua_State *L)
//{
//	//CatchNullDocument();//
//	HPProc_interface * thisDoc;
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
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

//int HPProc_interface::dumpheader(lua_State * L)
//{
//// 	CatchNullDocument();
//	HPProc_interface * thisDoc;
//	CFemmviewView * theView;
//
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	return 0;
//}


// void HPProc_interface::initalise_lua()
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


//int HPProc_interface::reload(lua_State * L)
//{
//	HPProc_interface * thisDoc;
//	CFemmviewView * theView;
//	thisDoc=(HPProc_interface *)pFemmviewdoc;
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

