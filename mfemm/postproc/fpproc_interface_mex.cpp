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
                    getpointvals,
                    clearcontour,
                    addcontour,
                    selectblock,
                    groupselectblock,
                    clearblock,
                    blockintegral,
                    lineintegral,
                    smoothon,
                    smoothoff,
                    getprobleminfo,
                    getcircuitprops,
                    numnodes,
                    numelements,
                    getelements,
                    getvertices,
                    getcentroids,
                    numgroupelements,
                    getareas,
                    getvolumes,
                    getgroupelements,
                    getgroupvertices,
                    getgroupcentroids,
                    getgroupareas,
                    getgroupvolumes
                  };

// Map to associate the command strings with the class
// method enum values
std::map<std::string, ClassMethods> s_mapClassMethodStrs;

void Initialize()
{
    // Set up the class methods map
    s_mapClassMethodStrs["opendocument"]      = opendocument;
    s_mapClassMethodStrs["getpointvals"]      = getpointvals;
    s_mapClassMethodStrs["clearcontour"]      = clearcontour;
    s_mapClassMethodStrs["addcontour"]        = addcontour;
    s_mapClassMethodStrs["selectblock"]       = selectblock;
    s_mapClassMethodStrs["groupselectblock"]  = groupselectblock;
    s_mapClassMethodStrs["clearblock"]        = clearblock;
    s_mapClassMethodStrs["blockintegral"]     = blockintegral;
    s_mapClassMethodStrs["lineintegral"]      = lineintegral;
    s_mapClassMethodStrs["smoothon"]          = smoothon;
    s_mapClassMethodStrs["smoothoff"]         = smoothoff;
    s_mapClassMethodStrs["getprobleminfo"]    = getprobleminfo;
    s_mapClassMethodStrs["getcircuitprops"]   = getcircuitprops;
    s_mapClassMethodStrs["numnodes"]          = numnodes;
    s_mapClassMethodStrs["numelements"]       = numelements;
    s_mapClassMethodStrs["getelements"]       = getelements;
    s_mapClassMethodStrs["getvertices"]       = getvertices;
    s_mapClassMethodStrs["getcentroids"]      = getcentroids;
    s_mapClassMethodStrs["getareas"]          = getareas;
    s_mapClassMethodStrs["getvolumes"]        = getvolumes;
    s_mapClassMethodStrs["numgroupelements"]  = numgroupelements;
    s_mapClassMethodStrs["getgroupelements"]  = getgroupelements;
    s_mapClassMethodStrs["getgroupvertices"]  = getgroupvertices;
    s_mapClassMethodStrs["getgroupcentroids"] = getgroupcentroids;
    s_mapClassMethodStrs["getgroupareas"]     = getgroupareas;
    s_mapClassMethodStrs["getgroupvolumes"]   = getgroupvolumes;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Get the command string
    char cmd[128];

    Initialize();

    if (nrhs < 1 || mxGetString(prhs[0], cmd, sizeof(cmd)))
    {
        mexErrMsgTxt("First input should be a command string less than 128 characters long.");
    }

    // New
    if (!strcmp("new", cmd))
    {
        // Check parameters
        if (nlhs != 1)
            mexErrMsgTxt("New: One output expected.");
        // Return a handle to a new C++ instance
        plhs[0] = convertPtr2Mat<FPProc_interface>(new FPProc_interface);
        return;
    }

    // Check there is a second input, which should be the class instance handle
    if (nrhs < 2)
    {
        mexErrMsgTxt("Second input should be a class instance handle.");
    }

    // Delete
    if (!strcmp("delete", cmd))
    {
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
    case clearcontour:
        FPProc_interface_instance->clearcontour();
        return;
    case addcontour:
        FPProc_interface_instance->addcontour(nlhs, plhs, nrhs, prhs);
        return;
    case selectblock:
        FPProc_interface_instance->selectblock(nlhs, plhs, nrhs, prhs);
        return;
    case groupselectblock:
        FPProc_interface_instance->groupselectblock(nlhs, plhs, nrhs, prhs);
        return;
    case clearblock:
        FPProc_interface_instance->clearblock();
        return;
    case blockintegral:
        FPProc_interface_instance->blockintegral(nlhs, plhs, nrhs, prhs);
        return;
    case lineintegral:
        FPProc_interface_instance->lineintegral(nlhs, plhs, nrhs, prhs);
        return;
    case smoothon:
        FPProc_interface_instance->smoothon();
        return;
    case smoothoff:
        FPProc_interface_instance->smoothoff();
        return;
    case getprobleminfo:
        FPProc_interface_instance->getprobleminfo(nlhs, plhs, nrhs, prhs);
        return;
    case getcircuitprops:
        FPProc_interface_instance->getcircuitprops(nlhs, plhs, nrhs, prhs);
        return;
    case numnodes:
        FPProc_interface_instance->numnodes(nlhs, plhs, nrhs, prhs);
        return;
    case numelements:
        FPProc_interface_instance->numelements(nlhs, plhs, nrhs, prhs);
        return;
    case getelements:
        FPProc_interface_instance->getelements(nlhs, plhs, nrhs, prhs);
        return;
    case getvertices:
        FPProc_interface_instance->getvertices(nlhs, plhs, nrhs, prhs);
        return;
    case getcentroids:
        FPProc_interface_instance->getcentroids(nlhs, plhs, nrhs, prhs);
        return;
	case getareas:
        FPProc_interface_instance->getareas(nlhs, plhs, nrhs, prhs);
        return;
    case getvolumes:
        FPProc_interface_instance->getvolumes(nlhs, plhs, nrhs, prhs);
        return;
    case numgroupelements:
        FPProc_interface_instance->numgroupelements(nlhs, plhs, nrhs, prhs);
        return;    
    case getgroupelements:
        FPProc_interface_instance->getgroupelements(nlhs, plhs, nrhs, prhs);
        return;
    case getgroupvertices:
        FPProc_interface_instance->getgroupvertices(nlhs, plhs, nrhs, prhs);
        return;
    case getgroupcentroids:
        FPProc_interface_instance->getgroupcentroids(nlhs, plhs, nrhs, prhs);
        return;
    case getgroupareas:
        FPProc_interface_instance->getgroupareas(nlhs, plhs, nrhs, prhs);
        return;
	case getgroupvolumes:
        FPProc_interface_instance->getgroupvolumes(nlhs, plhs, nrhs, prhs);
        return;
    default:
        mexErrMsgTxt("Unrecognised class command string.");
        break;
    }

    // Got here, so command not recognized
    //mexErrMsgTxt("Command not recognized.");
}
