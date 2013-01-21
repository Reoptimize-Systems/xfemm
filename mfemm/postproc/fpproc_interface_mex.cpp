#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <typeinfo>
#include "mex.h"
#include "class_handle.hpp"
#include "fpproc_interface.h"

using namespace std;

// Value-Defintions of the different class methods available
enum ClassMethods { evNotDefined,
                    opendocument,
                    getpointvals };

// Map to associate the command strings with the class
// method enum values
std::map<std::string, ClassMethods> s_mapClassMethodStrs;

void Initialize()
{
  s_mapClassMethodStrs["opendocument"] = opendocument;
  s_mapClassMethodStrs["getpointvals"] = getpointvals;

//  cout << "s_mapClassMethodStrs contains "
//       << s_mapClassMethodStrs.size()
//       << " entries." << endl;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Get the command string
    char cmd[128];

    Initialize();

	if (nrhs < 1 || mxGetString(prhs[0], cmd, sizeof(cmd))){
	    mexErrMsgTxt("First input should be a command string less than 128 characters long.");
	}

    // New
    if (!strcmp("new", cmd)) {
        // Check parameters
        if (nlhs != 1)
            mexErrMsgTxt("New: One output expected.");
        // Return a handle to a new C++ instance
        plhs[0] = convertPtr2Mat<FPProc_interface>(new FPProc_interface);
        return;
    }

    // Check there is a second input, which should be the class instance handle
    if (nrhs < 2)
		mexErrMsgTxt("Second input should be a class instance handle.");

    // Delete
    if (!strcmp("delete", cmd)) {
        // Destroy the C++ object
        destroyObject<FPProc_interface>(prhs[1]);
        // Warn if other commands were ignored
        if (nlhs != 0 || nrhs != 2)
            mexWarnMsgTxt("Delete: Unexpected arguments ignored.");
        return;
    }

    // Get the class instance pointer from the second input
    FPProc_interface *FPProc_interface_instance = convertMat2Ptr<FPProc_interface>(prhs[1]);

    // Call the various class methods
    // Switch on the value
    switch(s_mapClassMethodStrs[cmd])
    {
      case opendocument:
        FPProc_interface_instance->opendocument(nlhs, plhs, nrhs, prhs);
        return;
      case getpointvals:
        FPProc_interface_instance->getpointvals(nlhs, plhs, nrhs, prhs);
        return;
      default:
        mexErrMsgTxt("Unrecognised class command string.");
        break;
    }


    // Train
//    if (!strcmp("train", cmd)) {
//        // Check parameters
//        if (nlhs < 0 || nrhs < 2)
//            mexErrMsgTxt("Train: Unexpected arguments.");
//        // Call the method
//        FPProc_interface_instance->train();
//        return;
//    }
//    // Test
//    if (!strcmp("test", cmd)) {
//        // Check parameters
//        if (nlhs < 0 || nrhs < 2)
//            mexErrMsgTxt("Test: Unexpected arguments.");
//        // Call the method
//        FPProc_interface_instance->test();
//        return;
//    }

    // Got here, so command not recognized
    mexErrMsgTxt("Command not recognized.");
}
