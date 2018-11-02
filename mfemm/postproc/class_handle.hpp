/*
   class_handle.hpp
   
   A C++ mex class interface for Matlab/Octave
 
   Copyright (c) 2012, Oliver Woodford
   Copyright (c) 2014-2016, Richard Crozier
   All rights reserved.

*/


#ifndef CLASS_HANDLE_HPP
#define CLASS_HANDLE_HPP
#include "mex.h"
#include <stdint.h>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <typeinfo>

// define a signature to recognise the class at runtime, ideally
// you should DEFINE THIS WITH A DIFFERENT VALUE than the default before 
// #including this header in the mex function file with  unique number 
// for the class. This define is intended to uniquely identify the wrapped
// class type so you don't mix up pointers to different class types
#ifndef CLASS_HANDLE_SIGNATURE
#define CLASS_HANDLE_SIGNATURE 0xFF00F0A5
#endif

template<class base> class class_handle
{
public:
    
    class_handle(base *ptr) : ptr_m(ptr), name_m(typeid(base).name()) 
    { 
        signature_m = CLASS_HANDLE_SIGNATURE; 
    }
    
    ~class_handle() 
    { 
        signature_m = 0; 
        delete ptr_m; 
    }
    
    // function for checking if the wrapped object is still valid
    bool isValid() 
    { 
        return ( (signature_m == CLASS_HANDLE_SIGNATURE) 
                 && !strcmp (name_m.c_str(), typeid (base).name()) ); 
    }
    
    base *ptr() { return ptr_m; }

private:
    
    uint32_t signature_m;
    std::string name_m;
    base *ptr_m;
    
};

template<class base> inline mxArray *convertPtr2Mat(base *ptr)
{
    // lock the memory used in this function so it is not automatically 
    // cleaned up by matlab/octave
    mexLock();
    // create a 64 bit integer array to return a pointer to the class for
    // storage in a normal matlab variable
    mxArray *out = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    // now create a new instance of a class_handle class wrapping the c++ 
    // class to be wrapped, convert the pointer to the class to a uint64 
    // and place it in the array created to hold it
    *((uint64_t *)mxGetData(out)) = reinterpret_cast<uint64_t>(new class_handle<base>(ptr));

    return out;
}

template<class base> inline class_handle<base> *convertMat2HandlePtr(const mxArray *in)
{
    if (mxGetNumberOfElements(in) != 1 || mxGetClassID(in) != mxUINT64_CLASS || mxIsComplex(in))
    {
        mexErrMsgTxt("Input must be a real uint64 scalar.");
    }
    
    class_handle<base> *ptr = reinterpret_cast<class_handle<base> *>(*((uint64_t *)mxGetData(in)));
    
    if (!ptr->isValid())
    {
        mexErrMsgTxt("Handle not valid.");
    }
    
    return ptr;
}

template<class base> inline base *convertMat2Ptr(const mxArray *in)
{
    return convertMat2HandlePtr<base>(in)->ptr();
}

template<class base> inline void destroyObject(const mxArray *in)
{
    // delete the object pointed to by the pointer
    delete convertMat2HandlePtr<base>(in);
    
    // unlock the memory so matlab is free to clean up
    mexUnlock();
}

///////////////////        HELPER MACROS        ///////////////////

// This macro simply defines the standard mex input args, for convenience 
// for use in wrapper class methods
#define MEX_INPUT_ARGS int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]

//
// The following macros allow easy creation of a map to the wrapped class
// methods. You must first create a wrapper class for the c++ class to which you
// are interfacing. Every method of this interface class which will be called by
// the mex interface must have the following signature:
//
// void methodname (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
//
// You could use the previous macro to replace this with
//
// void methodname (MEX_INPUT_ARGS)
//
// Then in your mexfunction, use the macros: BEGIN_MEX_CLASS_WRAPPER, 
// REGISTER_CLASS_METHOD and END_MEX_CLASS_WRAPPER to register the methods
// and create the interface function like so:
//
// void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
// {
//     BEGIN_MEX_CLASS_WRAPPER(interfaceClassName)
//       REGISTER_CLASS_METHOD(interfaceClassName,method1name)
//       REGISTER_CLASS_METHOD(interfaceClassName,method2name)
//     END_MEX_CLASS_WRAPPER(interfaceClassName)
// }
//
// For information, the instance of the wrapped class will then be named 
// interfaceClassName_instance where interfaceClassName should be the name of 
// the class which you previously will have passed into the 
// BEGIN_MEX_CLASS_WRAPPER macro
//
// When called, your methods will be passed all the input arguments passed
// to the mexfunction. The first two of these arguments will be a string
// and integer containing the name of the method to be called and the 
// object handle used to find the pointer to the object in memory.
//
//

// BEGIN_MEX_CLASS_WRAPPER
#define BEGIN_MEX_CLASS_WRAPPER(WRAPPEDCLASS)                                                                \
    typedef void(WRAPPEDCLASS::*classMethod)(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);    \
                                                                                                             \
    std::map<std::string, classMethod> s_map_mex_wrapped_ClassMethodStrs;                                    \

// REGISTER_CLASS_METHOD 
#define REGISTER_CLASS_METHOD(WRAPPEDCLASS,METHOD)  s_map_mex_wrapped_ClassMethodStrs[#METHOD] = &WRAPPEDCLASS::METHOD;

// END_MEX_CLASS_WRAPPER 
#define END_MEX_CLASS_WRAPPER(WRAPPEDCLASS)                                                                  \
    char mex_wrapped_class_cmd_str[1024];                                                                    \
                                                                                                             \
    if (nrhs < 1 || mxGetString(prhs[0], mex_wrapped_class_cmd_str, sizeof(mex_wrapped_class_cmd_str)))      \
    {                                                                                                        \
        mexErrMsgTxt("First input should be a command string less than 128 characters long.");               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    if (!strcmp("new", mex_wrapped_class_cmd_str))                                                           \
    {                                                                                                        \
                                                                                                             \
        if (nlhs != 1)                                                                                       \
            mexErrMsgTxt("New: One output expected.");                                                       \
                                                                                                             \
        plhs[0] = convertPtr2Mat<WRAPPEDCLASS>(new WRAPPEDCLASS);                                            \
        return;                                                                                              \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    if (nrhs < 2)                                                                                            \
    {                                                                                                        \
        mexErrMsgTxt("Second input should be a class instance handle.");                                     \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    if (!strcmp("delete", mex_wrapped_class_cmd_str))                                                        \
    {                                                                                                        \
                                                                                                             \
        destroyObject<WRAPPEDCLASS>(prhs[1]);                                                                \
                                                                                                             \
        if (nlhs != 0 || nrhs != 2)                                                                          \
            mexWarnMsgTxt("Delete: Unexpected arguments ignored.");                                          \
        return;                                                                                              \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    WRAPPEDCLASS* WRAPPEDCLASS ## _instance = convertMat2Ptr<WRAPPEDCLASS>(prhs[1]);                         \
                                                                                                             \
                                                                                                             \
    if (s_map_mex_wrapped_ClassMethodStrs.count(mex_wrapped_class_cmd_str) > 0)                              \
    {                                                                                                        \
        classMethod theMethodpntr = s_map_mex_wrapped_ClassMethodStrs[mex_wrapped_class_cmd_str];            \
                                                                                                             \
        (WRAPPEDCLASS ## _instance->*theMethodpntr)(nlhs, plhs, nrhs, prhs);                                 \
    }                                                                                                        \
    else                                                                                                     \
    {                                                                                                        \
        mexErrMsgTxt("Unrecognised class command string.");                                                  \
    }                                                                                                        \


#endif // __CLASS_HANDLE_HPP__