// femmviewDoc.h : interface of the FPProc class
//
/////////////////////////////////////////////////////////////////////////////

#include "mex.h"
//#include "luadebug.h"
//#include "luaconsoledlg.h"
#include "fpproc.h"

#ifndef FPPROC_INTERFACE_H
#define FPPROC_INTERFACE_H

class FPProc_interface
{

public:

    // constructor
    FPProc_interface();
    //~FPProc_interface();

    int opendocument(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
	int getpointvals(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

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

    // The one and only FPProc object to which we are interfacing
    FPProc theFPProc;

};

#endif

