/* Copyright 2017 Johannes Zarl-Zierl <johannes@zarl-zierl.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of
 * Mechatronics GmbH (LCM)
 * Copyright 1998-2016 David Meeker <dmeeker@ieee.org>
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#include "LuaCommonCommands.h"

#include "CPointVals.h"
#include "femmenums.h"
#include "FemmState.h"
#include "LuaInstance.h"

#include <lua.h>

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifdef DEBUG_FEMMLUA
#define debug std::cerr
#else
#define debug while(false) std::cerr
#endif

using namespace femm;
using std::swap;

/**
 * @brief Add a new node.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_addnode(x,y)}
 * - \lua{ei_add_node(x,y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_addnode()}
 * - \femm42{femm/beladrawLua.cpp,lua_addnode()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaAddNode(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    double x=lua_todouble(L,1);
    double y=lua_todouble(L,2);

    double d;
    if ((int)doc->nodelist.size()<2) {
        d=1.e-08;
    } else {
        CComplex p0,p1,p2;
        p0=doc->nodelist[0]->CC();
        p1=p0;
        for(int i=1; i< (int)doc->nodelist.size(); i++)
        {
            p2=doc->nodelist[i]->CC();
            if(p2.re<p0.re) p0.re=p2.re;
            if(p2.re>p1.re) p1.re=p2.re;
            if(p2.im<p0.im) p0.im=p2.im;
            if(p2.im>p1.im) p1.im=p2.im;
        }
        d=abs(p1-p0)*CLOSE_ENOUGH;
    }
    femmState->getMesher()->AddNode(x,y,d);

    //BOOL flag=doc->AddNode(x,y,d);
    //if(flag==TRUE){
    //    theView->MeshUpToDate=FALSE;
    //    if(theView->MeshFlag==TRUE) theView->lnu_show_mesh();
    //    else theView->DrawPSLG();
    //}

    return 0;
}

/**
 * @brief Unselect all selected nodes, blocks, segments and arc segments.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_clearselected()}
 * - \lua{ei_clearselected()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_clearselected()}
 * - \femm42{femm/beladrawLua.cpp,lua_clearselected()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaClearSelected(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());

    femmState->getMesher()->UnselectAll();
    return 0;
}
