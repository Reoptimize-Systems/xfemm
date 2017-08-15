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
 * @brief Add a new arc segment.
 * Add a new arc segment from the nearest node to (x1,y1) to the
 * nearest node to (x2,y2) with angle ‘angle’ divided into ‘maxseg’ segments.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_addarc(x1,y1,x2,y2,angle,maxseg)}
 * - \lua{ei_add_arc(x1,y1,x2,y2,angle,maxseg)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_addarc()}
 * - \femm42{femm/beladrawLua.cpp,lua_addarc()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaAddArc(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    double sx = lua_todouble(L,1);
    double sy = lua_todouble(L,2);
    double ex = lua_todouble(L,3);
    double ey = lua_todouble(L,4);

    double angle = lua_todouble(L,5);
    double maxseg = lua_todouble(L,6);

    CArcSegment asegm;
    asegm.n0 = mesher->ClosestNode(sx,sy);
    asegm.n1 = mesher->ClosestNode(ex,ey);
    doc->nodelist[asegm.n1]->ToggleSelect();
    //theView->DrawPSLG();

    asegm.MaxSideLength = maxseg;
    asegm.ArcLength = angle;

    mesher->AddArcSegment(asegm);
    mesher->UnselectAll();
    //if(flag==TRUE){
    //    theView->MeshUpToDate=FALSE;
    //    if(theView->MeshFlag==TRUE) theView->lnu_show_mesh();
    //    else theView->DrawPSLG();
    //}
    //else theView->DrawPSLG();

    return 0;
}

/**
 * @brief Add new block label at given coordinates.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_addblocklabel(x,y)}
 * - \lua{ei_add_block_label(x,y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_addlabel()}
 * - \femm42{femm/beladrawLua.cpp,lua_addlabel()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaAddBlocklabel(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    double x = lua_todouble(L,1);
    double y = lua_todouble(L,2);

    double d;
    if (doc->nodelist.size()<2)
        d = 1.e-08;
    else{
        CComplex p0,p1,p2;
        p0 = doc->nodelist[0]->CC();
        p1 = p0;
        for (int i=1; i<(int)doc->nodelist.size(); i++)
        {
            p2 = doc->nodelist[i]->CC();
            if(p2.re<p0.re) p0.re = p2.re;
            if(p2.re>p1.re) p1.re = p2.re;
            if(p2.im<p0.im) p0.im = p2.im;
            if(p2.im>p1.im) p1.im = p2.im;
        }
        d = abs(p1-p0)*CLOSE_ENOUGH;
    }
    mesher->AddBlockLabel(x,y,d);

    //BOOL flag=thisDoc->AddBlockLabel(x,y,d);
    //if(flag==TRUE){
    //    theView->MeshUpToDate=FALSE;
    //    if(theView->MeshFlag==TRUE) theView->lnu_show_mesh();
    //    else theView->DrawPSLG();
    //}

    return 0;
}

/**
 * @brief Add a new line segment between two given points.
 * In other words, add a new line segment from node closest to (x1,y1) to node closest to (x2,y2)
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_addsegment(x1,y1,x2,y2)}
 * - \lua{ei_add_segment(x1,y1,x2,y2)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_addline()}
 * - \femm42{femm/beladrawLua.cpp,lua_addline()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaAddLine(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());

    double sx=lua_todouble(L,1);
    double sy=lua_todouble(L,2);

    double ex=lua_todouble(L,3);
    double ey=lua_todouble(L,4);

    femmState->getMesher()->AddSegment(
                femmState->getMesher()->ClosestNode(sx,sy),
                femmState->getMesher()->ClosestNode(ex,ey));

    //BOOL flag=thisDoc->AddSegment(thisDoc->ClosestNode(sx,sy),thisDoc->ClosestNode(ex,ey));
    //if(flag==TRUE)
    //{
    //    theView->MeshUpToDate=FALSE;
    //    if(theView->MeshFlag==TRUE) theView->lnu_show_mesh();
    //    else theView->DrawPSLG();
    //}
    //else theView->DrawPSLG();

    return 0;
}

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
 * @brief Marks the first selected block label as the default block label.
 *
 * This block label is applied to any region that has not been explicitly labeled.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_attachdefault()}
 * - \lua{ei_attachdefault()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_attachdefault()}
 * - \femm42{femm/beladrawLua.cpp,lua_attachdefault()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaAttachDefault(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    bool isFirstSelected = true;
    for (auto &label: doc->labellist)
    {
        label->IsDefault = (label->IsSelected && isFirstSelected);
        if (label->IsSelected)
            isFirstSelected = false;
    }

    return 0;
}

/**
 * @brief Mark selected block labels as members of the external region,
 * used for modeling unbounded axisymmetric problems via the Kelvin Transformation.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_attachouterspace()}
 * - \lua{ei_attachouterspace()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_attachouterspace()}
 * - \femm42{femm/beladrawLua.cpp,lua_attachouterspace()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaAttachOuterSpace(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    for (auto &label: doc->labellist)
    {
        if (label->IsSelected)
            label->IsExternal = true;
    }

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

/**
 * @brief Closes the current pre-processor instance.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_close()}
 * - \lua{ei_close()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_exitpre()}
 * - \femm42{femm/beladrawLua.cpp,lua_exitpre()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaExitPre(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    femmState->close();
    return 0;
}

/**
 * @brief Save the problem description into the given file.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_saveas("filename")}
 * - \lua{ei_saveas("filename")}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,luaSaveDocument()}
 * - \femm42{femm/beladrawLua.cpp,luaSaveDocument()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaSaveDocument(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    if (!lua_isnil(L,1))
    {
        doc->pathName = lua_tostring(L,1);
        doc->saveFEMFile(doc->pathName);
    } else {
        lua_error(L, "saveas(): no pathname given!");
    }

    return 0;
}

/**
 * @brief Select an arc segment near a given point.
 * @param L
 * @return 4 on success, 0 otherwise
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_selectarcsegment(x,y)}
 * - \lua{ei_select_arcsegment(x,y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_selectarcsegment()}
 * - \femm42{femm/beladrawLua.cpp,lua_selectarcsegment()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaSelectArcsegment(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    double mx = lua_todouble(L,1);
    double my = lua_todouble(L,2);

    if (doc->arclist.empty())
        return 0;

    int node = mesher->ClosestArcSegment(mx,my);
    doc->arclist[node]->ToggleSelect();

    lua_pushnumber(L,doc->nodelist[doc->arclist[node]->n0]->x);
    lua_pushnumber(L,doc->nodelist[doc->arclist[node]->n0]->y);
    lua_pushnumber(L,doc->nodelist[doc->arclist[node]->n1]->x);
    lua_pushnumber(L,doc->nodelist[doc->arclist[node]->n1]->y);

    return 4;
}

/**
 * @brief Select the closest label to a given point.
 * Select the label closet to (x,y). Returns the coordinates of the selected label.
 * @param L
 * @return 0 on error, 2 otherwise.
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_selectlabel(x,y)}
 * - \lua{ei_select_label(x,y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_selectlabel()}
 * - \femm42{femm/beladrawLua.cpp,lua_selectlabel()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaSelectBlocklabel(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    double mx = lua_todouble(L,1);
    double my = lua_todouble(L,2);

    if (doc->labellist.empty())
        return 0;

    int node = mesher->ClosestBlockLabel(mx,my);
    doc->labellist[node]->ToggleSelect();

    lua_pushnumber(L,doc->labellist[node]->x);
    lua_pushnumber(L,doc->labellist[node]->y);

    return 2;
}

/**
 * @brief Select the nearest node to given coordinates.
 * Returns the coordinates of the selected node.
 * @param L
 * @return 0 on error, 2 on success
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_selectnode(x,y)}
 * - \lua{ei_select_node(x,y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_selectnode()}
 * - \femm42{femm/beladrawLua.cpp,lua_selectnode()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaSelectNode(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    double mx = lua_todouble(L,1);
    double my = lua_todouble(L,2);

    if(mesher->problem->nodelist.size() == 0)
        return 0;

    int node = mesher->ClosestNode(mx,my);
    mesher->problem->nodelist[node]->ToggleSelect();

    lua_pushnumber(L,mesher->problem->nodelist[node]->x);
    lua_pushnumber(L,mesher->problem->nodelist[node]->y);

    return 2;
}

/**
 * @brief Select the line closest to a given point.
 * @param L
 * @return 0 on error, 4 on success
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_selectsegment(x,y)}
 * - \lua{ei_select_segment(x,y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_selectsegment()}
 * - \femm42{femm/beladrawLua.cpp,lua_selectsegment()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaSelectSegment(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> thisDoc = femmState->femmDocument();

    double mx = lua_todouble(L,1);
    double my = lua_todouble(L,2);

    if (thisDoc->linelist.empty())
        return 0;

    int node = femmState->getMesher()->ClosestSegment(mx,my);
    thisDoc->linelist[node]->ToggleSelect();

    lua_pushnumber(L,thisDoc->nodelist[thisDoc->linelist[node]->n0]->x);
    lua_pushnumber(L,thisDoc->nodelist[thisDoc->linelist[node]->n0]->y);
    lua_pushnumber(L,thisDoc->nodelist[thisDoc->linelist[node]->n1]->x);
    lua_pushnumber(L,thisDoc->nodelist[thisDoc->linelist[node]->n1]->y);

    return 4;
}
