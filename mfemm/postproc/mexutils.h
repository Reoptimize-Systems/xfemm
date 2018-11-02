
#ifndef MEXUTILS_H
#define MEXUTILS_H

///////////////////        MEX UTILITIES        ///////////////////
//
// The following utility functions and classes are provided to assist with 
// parsing the input arguments to the mexfunction methods and returning 
// outputs.

namespace mexutils {

    
void mxtestnumeric (const mxArray* testMxArray, int argnum=0) {
 
   if (!mxIsNumeric(testMxArray))
   {
       if (argnum > 0)
       {
           mexErrMsgIdAndTxt("CPP:mxtestnumeric",
               "Input argument %i is not numeric.", argnum);
       }
       else
       {
           mexErrMsgIdAndTxt("CPP:mxtestnumeric",
               "Input argument is not numeric.");
       }
   } 
   
}

void mxtestchar (const mxArray* testMxArray, int argnum=0) {
 
   if (!mxIsChar(testMxArray))
   {
       if (argnum > 0)
       {
           mexErrMsgIdAndTxt("CPP:mxtestchar",
              "Input argument is not a string array.", argnum);
       }
       else
       {
           mexErrMsgIdAndTxt("CPP:mxtestchar",
                "Input argument is not a string array.");
       }
   } 
   
}

void mxtestlogical (const mxArray* testMxArray, int argnum=0) {
 
   if (!mxIsLogical(testMxArray))
   {
       if (argnum > 0)
       {
           mexErrMsgIdAndTxt("CPP:mxtestnumeric",
                "Input argument type is not logical.", argnum);
       }
       else
       {
           mexErrMsgIdAndTxt("CPP:mxtestnumeric",
                "Input argument type is not logical.");
       }
   } 
   
}

// check the number of input arguments provided
int mxnarginchk (int nargs, std::vector<int> nallowed, int offset=0)
{
  int offsetnargs = nargs-offset;
  
  if (nallowed.size () > 0)
  {
     for (int i = 0; i < nallowed.size (); i++)
     {
         if (nallowed[i] == offsetnargs)
         {
             // return as we have a matching number of arguments
             return offsetnargs;
         }
     }
  }
  else
  {
      mexErrMsgIdAndTxt("CPP:mxnarginchk",
           "No allowed number of arguments supplied.");
  }
  
  mexErrMsgIdAndTxt("CPP:mxnarginchk",
         "Incorrect number of input arguments. You supplied %i args with an offset of %i", nargs, offset);
  
  return offsetnargs;
}

void mxnaroutgchk (const int nlhs, int ntharg)
{
  
  if (ntharg <= nlhs)
  {
      // return as we have a matching number of arguments
      return;
  }
  
  // throw an error
  mexErrMsgIdAndTxt("CPP:mxnargoutchk",
         "Incorrect number of output arguments.");
  
  return;
}

// Get the n'th scalar input argument to a mexfunction
double mxnthargscalar (int nrhs, const mxArray *prhs[], int ntharg, int offset=0)
{
  
   ntharg = ntharg + offset;
  
   if (ntharg > nrhs)
   {
     mexErrMsgIdAndTxt("CPP:mxnthargscalar",
         "Requested argument is greater than total number of arguments.");
   }
   
   // check matrix is numeric
   mxtestnumeric (prhs[ntharg-1], ntharg); 
   
   if ((mxGetN(prhs[ntharg-1]) != 1) || (mxGetM(prhs[ntharg-1]) != 1))
   {
     mexErrMsgIdAndTxt("CPP:mxnthargscalar",
         "Input argument is not scalar.");
   }
   
   return mxGetScalar(prhs[ntharg-1]);
   
}

// Get the n'th scalar input argument to a mexfunction
bool mxnthargscalarbool (int nrhs, const mxArray *prhs[], int ntharg, int offset=0)
{
  
   ntharg = ntharg + offset;
  
   if (ntharg > nrhs)
   {
     mexErrMsgIdAndTxt("CPP:mxnthargscalar",
         "Requested argument is greater than total number of arguments.");
   }
   
   // check matrix is logical
   mxtestlogical (prhs[ntharg-1], ntharg); 
   
   if ((mxGetN(prhs[ntharg-1]) != 1) || (mxGetM(prhs[ntharg-1]) != 1))
   {
     mexErrMsgIdAndTxt("CPP:mxnthargscalar",
         "Input argument is not scalar.");
   }
   
   return (bool)mxGetScalar(prhs[ntharg-1]);
   
}

// Get the n'th input argument to a mexfunction whis is expected to be a 
// char array
char* mxnthargchar (int nrhs, const mxArray *prhs[], int ntharg, int offset=0)
{
  
   ntharg = ntharg + offset;
  
   if (ntharg > nrhs)
   {
     mexErrMsgIdAndTxt("CPP:mxnthargstringarray",
         "Requested argument is greater than total number of arguments.");
   }
   
   // check it's a string
   mxtestchar (prhs[ntharg-1], ntharg); 
   
//    if ((mxGetN(prhs[ntharg-1]) != 1) || (mxGetM(prhs[ntharg-1]) != 1))
//    {
//      mexErrMsgIdAndTxt("CPP:mxnthargstringarray",
//          "Input argument is not a char array.");
//    }
  char *buf;
  mwSize buflen; 
  
  /* Allocate enough memory to hold the converted string. */ 
  buflen = mxGetNumberOfElements(prhs[ntharg-1]) + 1;
  buf = (char*)mxCalloc(buflen, sizeof(char));
  
  /* Copy the string data from string_array_ptr and place it into buf. */ 
  if (mxGetString(prhs[ntharg-1], buf, buflen) != 0)
  {
      mexErrMsgIdAndTxt( "CPP:mxnthargstringarray",
            "Could not convert string data.");
  }
  
  return buf;
   
}

// Get the n'th input argument to a mexfunction whis is expected to be a 
// char array and return as a std::string
std::string mxnthargstring (int nrhs, const mxArray *prhs[], int ntharg, int offset=0)
{
  
  char* buf = mxnthargchar (nrhs, prhs, ntharg, offset);
  
  std::string str (buf);
  
  mxFree (buf);
  
  return str;
   
}


// return array of integers
void mxSetLHS (const int* const out, int argn, int size, const int nlhs, mxArray* plhs[])
{

    // check the argument position is possible
    mxnaroutgchk (nlhs, argn);
  
    // create the output matrix to hold the vector of numbers
    plhs[argn-1] = mxCreateNumericMatrix(1, size, mxINT32_CLASS, mxREAL);
    
    int * outArray = (int *) mxGetData(plhs[argn-1]);

    if (outArray)
    {
        // copy the data
        for (int i = 0; i < size; i++)
        {
            outArray[i] = *(out+i);
        }
    }
    else
    {
        mexErrMsgIdAndTxt("CPP:mxSetLHS",
         "Unable to set output.");
    }
}

// return integer
void mxSetLHS (const int out, int argn, const int nlhs, mxArray* plhs[])
{
    //int outcp = out;
  
    // call the function for returning a vector, with a pointer to the the 
    // output data
    mxSetLHS (&out, argn, 1, nlhs, plhs);
}


// return std::vector of integers
void mxSetLHS (const std::vector<int> out, int argn, const int nlhs, mxArray* plhs[])
{

    // check the argument position is possible
    mxnaroutgchk (nlhs, argn);
  
    // create the output matrix to hold the vector of numbers
    plhs[argn-1] = mxCreateNumericMatrix(1, out.size (), mxINT32_CLASS, mxREAL);
    
    int * outArray = (int *) mxGetData(plhs[argn-1]);

    if (outArray)
    {
        // copy the data
        for (int i = 0; i < out.size (); i++)
        {
            outArray[i] = out[i];
        }
    }
    else
    {
        mexErrMsgIdAndTxt("CPP:mxSetLHS",
         "Unable to set output.");
    }
}


// return array of floats
void mxSetLHS (const float* const out, int argn, int size, const int nlhs, mxArray* plhs[])
{
    // check the argument position is possible
    mxnaroutgchk (nlhs, argn);
  
    // create the output matrix to hold the vector of numbers
    plhs[argn-1] = mxCreateNumericMatrix(1, size, mxSINGLE_CLASS, mxREAL);
    
    float * outArray = (float *) mxGetData(plhs[argn-1]);

    if (outArray)
    {
        // copy the data
        for (int i = 0; i < size; i++)
        {
            outArray[i] = *(out+i);
        }
    }
    else
    {
        mexErrMsgIdAndTxt("CPP:mxSetLHS",
         "Unable to set output.");
    }
    
}

// return float
void mxSetLHS (const float out, int argn, const int nlhs, mxArray* plhs[])
{
    // call the function for returning a vector, with a pointer to the the 
    // output data
    mxSetLHS (&out, argn, 1, nlhs, plhs);
}

// return std::vector of floats
void mxSetLHS (const std::vector<float> out, int argn, const int nlhs, mxArray* plhs[])
{

    // check the argument position is possible
    mxnaroutgchk (nlhs, argn);
  
    // create the output matrix to hold the vector of numbers
    plhs[argn-1] = mxCreateNumericMatrix(1, out.size (), mxSINGLE_CLASS, mxREAL);
    
    float * outArray = (float *) mxGetData(plhs[argn-1]);

    if (outArray)
    {
        // copy the data
        for (int i = 0; i < out.size (); i++)
        {
            outArray[i] = out[i];
        }
    }
    else
    {
        mexErrMsgIdAndTxt("CPP:mxSetLHS",
         "Unable to set output.");
    }
}


// return array of doubles
void mxSetLHS (const double* const out, int argn, int size, const int nlhs, mxArray* plhs[])
{
    // check the argument position is possible
    mxnaroutgchk (nlhs, argn);
  
    // create the output matrix to hold the vector of numbers
    plhs[argn-1] = mxCreateNumericMatrix(1, size, mxDOUBLE_CLASS, mxREAL);
    
    double * outArray = (double *) mxGetData(plhs[argn-1]);

    if (outArray)
    {
        // copy the data
        for (int i = 0; i < size; i++)
        {
            outArray[i] = *(out+i);
        }
    }
    else
    {
        mexErrMsgIdAndTxt("CPP:mxSetLHS",
         "Unable to set output.");
    }
}

// return double
void mxSetLHS (const double out, int argn, const int nlhs, mxArray* plhs[])
{
    mxSetLHS (&out, argn, 1, nlhs, plhs);
}

// return std::vector of doubles
void mxSetLHS (const std::vector<double> out, int argn, const int nlhs, mxArray* plhs[])
{

    // check the argument position is possible
    mxnaroutgchk (nlhs, argn);
  
    // create the output matrix to hold the vector of numbers
    plhs[argn-1] = mxCreateNumericMatrix(1, out.size (), mxDOUBLE_CLASS, mxREAL);
    
    double * outArray = (double *) mxGetData(plhs[argn-1]);

    if (outArray)
    {
        // copy the data
        for (int i = 0; i < out.size (); i++)
        {
            outArray[i] = out[i];
        }
    }
    else
    {
        mexErrMsgIdAndTxt("CPP:mxSetLHS",
         "Unable to set output.");
    }
}


// wrapper class for mwArray, mainly to ease indexing
class mxNumericArrayWrapper
{
public:
  
  // constructor
  mxNumericArrayWrapper (const mxArray* wrappedMxArray)
  {
      mxtestnumeric (wrappedMxArray);
    
      wMxArray = wrappedMxArray;
    
      // get the number of dimensions
      mwSize ndims = mxGetNumberOfDimensions(wMxArray);
    
      const mwSize* dimspntr = mxGetDimensions(wMxArray);
    
      // get the dimensions and push them into the _dimensions vector
      for (int i = 0; i < ndims; i++)
      {
          mwSize dimsize = *(dimspntr+i);
          _dimensions.push_back(dimsize);
      }
      
  }
  
  // getDoubleValue
  // gets vale from matrix using matrix subscripts. Note matrix subscripts 
  // should be zero-based not one-based
  double getDoubleValue (std::vector<mwSize> index)
  {
      // check it's  double matrix
      if (!mxIsDouble (wMxArray))
      {
          mexErrMsgIdAndTxt("CPP:mxArrayWrapper:notdouble",
              "Double value requested for non-double matrix.");          
      }
    
      // get the linear index into the underlying data array
      mwIndex linindex = calcSingleSubscript (index);
      
      // get the data from the array
      double* data = mxGetPr(wMxArray);
      
      return data[(int)linindex];
      
  }
  
  // setDoubleValue
  // sets value from matrix using matrix subscripts. Note matrix subscripts 
  // should be zero-based not one-based
  void setDoubleValue (std::vector<mwSize> index, double value)
  {
      // check it's  double matrix
      if (!mxIsDouble (wMxArray))
      {
          mexErrMsgIdAndTxt("CPP:mxArrayWrapper:notdouble",
              "Attempted to set double value in non-double matrix.");          
      }

      // get the linear index into the underlying data array
      mwIndex linindex = calcSingleSubscript (index);
      
      // get the data from the array
      double* data = mxGetPr(wMxArray);
      
      data[(int)linindex] = value;
      
  }
  
  // calcSingleSubscript
  // gets linear index from matrix subscripts. Note matrix subscripts 
  // should be zero-based not one-based
  mwIndex calcSingleSubscript (std::vector<mwSize> index)
  {
      // check dimensions are within range
      checkDimensions (index);
      
      // make an array of the appropriate size to hold the indices
      mwIndex* subs = new mwIndex[_dimensions.size ()];
      // copy the index into the subs array
      for (int i=0; i < index.size (); i++) { *(subs+i) = index[i]; }
      // get the linear index into the underlying data array
      mwIndex linindex = mxCalcSingleSubscript(wMxArray, (mwSize)(_dimensions.size ()), subs);
      // delete the memory allocated for the index
      delete[] subs;
      
      return linindex;
  }
  
  void checkDimensions (const std::vector<mwSize> &index)
  {
      if (index.size () != _dimensions.size ())
      {
          mexErrMsgIdAndTxt("CPP:mxArrayWrapper:invalidindex",
              "Wrong number of dimensions specified.");
      }
      
      
      for (int i=0; i < index.size (); i++)
      {
          // check we are not outwith any dimensions
          if (index[i] > _dimensions[i])
          {
              mexErrMsgIdAndTxt("CPP:mxArrayWrapper:invalidindex",
                  "Index to dimension %i out of bounds, value %i out of bound %i.", 
                  i+1, index[i], _dimensions[i] );
          }
      }
  }
  
  std::vector<mwSize> getDimensions ()
  {
      return _dimensions;
  }
  
  mwSize getRows ()
  {
      return _dimensions[0];
  }
  
  mwSize getColumns ()
  {
      return _dimensions[1];
  }
  
private:
  
  const mxArray* wMxArray;
  std::vector<mwSize> _dimensions;
  
};
 
// Get the n'th scalar input argument to a mexfunction
mxNumericArrayWrapper mxnthargmatrix (int nrhs, const mxArray *prhs[], int ntharg, int offset=0)
{
  
   ntharg = ntharg + offset;
  
   if (ntharg > nrhs)
   {
     mexErrMsgIdAndTxt("CPP:mxnthargmatrix",
         "Requested argument is greater than total number of arguments.");
   }
   
   return mxNumericArrayWrapper (prhs[ntharg-1]);
   
}

} // namespace mexutils


#endif // MEXUTILS_H
