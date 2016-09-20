/*
   This code is a modified version of an algorithm
   forming part of the software program Finite
   Element Method Magnetics (FEMM), authored by
   David Meeker. The original software code is
   subject to the Aladdin Free Public Licence
   version 8, November 18, 1999. For more information
   on FEMM see www.femm.info. This modified version
   is not endorsed in any way by the original
   authors of FEMM.

   This software has been modified to use the C++
   standard template libraries and remove all Microsoft (TM)
   MFC dependent code to allow easier reuse across
   multiple operating system platforms.

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

// fsolver.cpp : implementation of the FSolver class
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <malloc.h>
#include "lua.h"
#include "femmcomplex.h"
#include "mesh.h"
#include "spars.h"
#include "fparse.h"
#include "feasolver.h"

#ifndef _MSC_VER
#define _strnicmp strncasecmp
#endif

extern lua_State *lua; // the main lua object

extern void lua_baselibopen (lua_State *L);
extern void lua_iolibopen (lua_State *L);
extern void lua_strlibopen (lua_State *L);
extern void lua_mathlibopen (lua_State *L);
extern void lua_dblibopen (lua_State *L);
extern int bLinehook;
extern int lua_byebye;

lua_State *lua;
int bLinehook;
int lua_byebye;

using namespace std;
using namespace femm;

/////////////////////////////////////////////////////////////////////////////
// FEASolver construction/destruction

FEASolver::FEASolver()
{
    Precision = 0;
    LengthUnits = 0;
    ProblemType = 0;
    DoForceMaxMeshArea = false;
    Coords = 0;
    BandWidth = 0;
    NumNodes = 0;
    NumEls = 0;
    NumBlockProps = 0;
    NumPBCs = 0;
    NumLineProps = 0;
    NumPointProps = 0;
    NumCircProps = 0;
    NumBlockLabels = 0;

    meshele = NULL;
    lineproplist = NULL;
    pbclist = NULL;

    extRo = extRi = extZo = 0.0;

    // initialise the warning message box function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;

    // Initialize Lua
    bLinehook = false;
    bMultiplyDefinedLabels = false;
    lua = lua_open(4096);
    lua_baselibopen(lua);
    lua_strlibopen(lua);
    lua_mathlibopen(lua);
    lua_iolibopen(lua);
}

FEASolver::~FEASolver()
{

    lua_close(lua);

    if (lineproplist != NULL)
    {
        free(lineproplist);
    }

    if (meshele != NULL)
    {
        free(meshele);
    }

    if (pbclist!=NULL)
    {
        free(pbclist);
    }

}

