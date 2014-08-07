// HPProc_interface.h : interface of the HPProc class
//
/////////////////////////////////////////////////////////////////////////////

#include "mex.h"
//#include "luadebug.h"
//#include "luaconsoledlg.h"
#include "hpproc.h"

#ifndef HPProc_INTERFACE_H
#define HPProc_INTERFACE_H

class HPProc_interface
{

public:

    // constructor
    HPProc_interface();
    //~HPProc_interface();

    int opendocument(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int temperaturebounds(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	int getpointvals(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	int addcontour(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	int clearcontour();
	int lineintegral(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	int selectblock(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	int groupselectblock(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	int clearblock();
	int blockintegral(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	int smoothon();
	int smoothoff();
	int getprobleminfo(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	int getconductorprops(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int numnodes (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int numelements (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int getelements (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int getvertices (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int getcentroids (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int getareas (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int numgroupelements (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int getgroupelements (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int getgroupvertices (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int getgroupcentroids (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    int getgroupareas (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
// To be created
//	int addcontour(lua_State * L);
//	int clearcontour(lua_State * L);
//	int lineintegral(lua_State * L);
//	int selectblock(lua_State * L);
//	int groupselectblock(lua_State * L);
//	int blockintegral(lua_State * L);
//	int clearblock(lua_State * L);
//	int smoothing(lua_State * L);
//	int getprobleminfo(lua_State * L);
//	int selectline(lua_State *L);
//	int seteditmode(lua_State *L);
//	int bendcontour(lua_State *L);
//	int shownames(lua_State *L);
//	int vectorplot(lua_State *L);
//	int gradient(lua_State *L);

	// commands to access low-level information through Lua
//	int numnodes(lua_State *L);
//	int numelements(lua_State *L);
//	int getnode(lua_State *L);
//	int getelement(lua_State *L);

	// functions used for compatibility with older versions
//	int old_lua_lineintegral(lua_State * L);
//	int old_lua_getpointvals(lua_State * L);
//	int old_lua_blockintegral(lua_State * L);
//	int old_lua_getcircuitprops(lua_State *L);

	// Junk
    //int dumpheader(lua_State * L);
	//int exitpost(lua_State * L);
	//int reload(lua_State *L);

private:

    // The one and only HPProc object to which we are interfacing
    HPProc theHPProc;
    
    int countGroupElements (int);

};

// function to display warnings
void HPProcInterfaceWarning(const char*);

#endif

