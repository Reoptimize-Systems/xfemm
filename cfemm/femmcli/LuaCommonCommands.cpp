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
 * @brief Copy selected objects and rotate the copy around a point.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_copyrotate(bx, by, angle, copies, (editaction) )}
 * - \lua{ei_copyrotate(bx, by, angle, copies, (editaction) )}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_copy_rotate()}
 * - \femm42{femm/beladrawLua.cpp,lua_copy_rotate()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaCopyRotate(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int n = lua_gettop(L);
    if(n!=4 && n!=5)
    {
        lua_error(L,"Invalid number of parameters for copy rotate\n");
        return 0;
    }

    double x = lua_todouble(L,1);
    double y = lua_todouble(L,2);
    double angle = lua_todouble(L,3);
    int copies = (int) lua_todouble(L,4);

    EditMode editAction;
    if (n==5) {
        editAction = intToEditMode((int)lua_todouble(L,5));
    } else {
        editAction = mesher->d_EditMode;
    }

    if (editAction == EditMode::Invalid)
    {
        lua_error(L, "copyrotate(): no editmode given and no default edit mode set!\n");
        return 0;
    }


    // Note(ZaJ): why is mesher->UpdateUndo called in mi_copytranslate but not here?
    mesher->RotateCopy(CComplex(x,y),angle,copies,editAction);
    // Note(ZaJ): shouldn't the invalidation be done by RotateCopy?
    doc->invalidateMesh();
    mesher->meshline.clear();
    mesher->meshnode.clear();
    mesher->greymeshline.clear();

    return 0;
}

/**
 * @brief Copy selected objects and translate each copy by a given offset.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_copytranslate(dx, dy, copies, (editaction))}
 * - \lua{ei_copytranslate(dx, dy, copies, (editaction))}
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_copy_translate()}
 * - \femm42{femm/beladrawLua.cpp,lua_copy_translate()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaCopyTranslate(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int n = lua_gettop(L);
    if(n!=3 && n!=4)
    {
        lua_error(L,"Invalid number of parameters for copy translate\n");
        return 0;
    }

    double x = lua_todouble(L,1);
    double y = lua_todouble(L,2);
    int copies = (int) lua_todouble(L,3);

    EditMode editAction;
    if (n==4) {
        editAction = intToEditMode((int)lua_todouble(L,4));
    } else {
        editAction = mesher->d_EditMode;
    }

    if (editAction == EditMode::Invalid)
    {
        lua_error(L, "copytranslate(): no editmode given and no default edit mode set!\n");
        return 0;
    }


    mesher->UpdateUndo();
    mesher->TranslateCopy(x,y,copies,editAction);
    // Note(ZaJ): shouldn't the invalidation be done by TranslateCopy?
    doc->invalidateMesh();
    mesher->meshline.clear();
    mesher->meshnode.clear();
    mesher->greymeshline.clear();

    return 0;
}

/**
 * @brief Delete the given boundary property.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_deleteboundprop("propname")}
 * - \lua{ei_deleteboundprop("propname")}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_delboundprop()}
 * - \femm42{femm/beladrawLua.cpp,lua_delboundprop()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDeleteBoundaryProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    std::string propName = lua_tostring(L,1);
    doc->lineproplist.erase(
                std::remove_if(doc->lineproplist.begin(),doc->lineproplist.end(),
                               [&propName](const auto& prop){ return prop->BdryName == propName; } ),
                doc->lineproplist.end()
                );
    doc->lineproplist.shrink_to_fit();
    doc->updateLineMap();

    return 0;
}

/**
 * @brief Delete the given circuit property.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_deletecircuit("circuitname")}
 * - \lua{ei_deleteconductor("circuitname")}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_delcircuitprop()}
 * - \femm42{femm/beladrawLua.cpp,lua_delcircuitprop()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDeleteCircuitProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    std::string propName = lua_tostring(L,1);
    doc->circproplist.erase(
                std::remove_if(doc->circproplist.begin(),doc->circproplist.end(),
                               [&propName](const auto& prop){ return prop->CircName == propName; } ),
                doc->circproplist.end()
                );
    doc->circproplist.shrink_to_fit();
    doc->updateCircuitMap();

    return 0;
}

/**
 * @brief Delete the given material property.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_deletematerial("materialname")} deletes the material named "materialname".
 * - \lua{ei_deletematerial("materialname")} deletes the material named "materialname".

 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_delmatprop()}
 * - \femm42{femm/beladrawLua.cpp,lua_delmatprop()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDeleteMaterial(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    std::string propName = lua_tostring(L,1);
    doc->blockproplist.erase(
                std::remove_if(doc->blockproplist.begin(),doc->blockproplist.end(),
                               [&propName](const auto& mat){ return mat->BlockName == propName; } ),
                doc->blockproplist.end()
                );
    doc->blockproplist.shrink_to_fit();
    doc->updateBlockMap();

    return 0;
}

/**
 * @brief Delete the given point property.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_deletepointprop("pointpropname")} deletes the point property named "pointpropname"
 * - \lua{ei_deletepointprop("pointpropname")} deletes the point property named "pointpropname"
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_delpointprop()}
 * - \femm42{femm/beladrawLua.cpp,lua_delpointprop()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDeletePointProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    std::string propName = lua_tostring(L,1);
    doc->nodeproplist.erase(
                std::remove_if(doc->nodeproplist.begin(),doc->nodeproplist.end(),
                               [&propName](const auto& prop){ return prop->PointName == propName; } ),
                doc->nodeproplist.end()
                );
    doc->nodeproplist.shrink_to_fit();
    doc->updateNodeMap();

    return 0;
}

/**
 * @brief Delete selected objects.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_deleteselected()}
 * - \lua{ei_deleteselected()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_deleteselected()}
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselected()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDeleteSelected(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    mesher->DeleteSelectedSegments();
    mesher->DeleteSelectedArcSegments();
    mesher->DeleteSelectedNodes();
    mesher->DeleteSelectedBlockLabels();

    return 0;
}

/**
 * @brief Delete selects arcs.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_deleteselectedarcsegments()}
 * - \lua{ei_deleteselectedarcsegments()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_deleteselectedarcsegments()}
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselectedarcsegments()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDeleteSelectedArcSegments(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    mesher->DeleteSelectedArcSegments();
    return 0;
}

/**
 * @brief Delete selected block labels.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_deleteselectedlabels()}
 * - \lua{ei_deleteselectedlabels()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_deleteselectedlabels()}
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselectedlabels()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDeleteSelectedBlockLabels(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    mesher->DeleteSelectedBlockLabels();
    return 0;
}

/**
 * @brief Delete selected nodes
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_deleteselectednodes()}
 * - \lua{ei_deleteselectednodes()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_deleteselectednodes()}
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselectednodes()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDeleteSelectedNodes(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    mesher->DeleteSelectedNodes();

    return 0;
}

/**
 * @brief Delete selected segments.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_deleteselectedsegments()}
 * - \lua{ei_deleteselectedsegments()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_deleteselectedsegments()}
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselectedsegments()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDeleteSelectedSegments(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    mesher->DeleteSelectedSegments();

    return 0;
}

/**
 * @brief Unset IsDefault for selected block labels.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_detachdefault()}
 * - \lua{ei_detachdefault()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_detachdefault()}
 * - \femm42{femm/beladrawLua.cpp,lua_detachdefault()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDetachDefault(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    for (auto &label: doc->labellist)
    {
        if (label->IsSelected)
            label->IsDefault = false;
    }

    return 0;
}

/**
 * @brief Mark selected block labels as not belonging to the external region.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_detachouterspace()}
 * - \lua{ei_detachouterspace()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_detachouterspace()}
 * - \femm42{femm/beladrawLua.cpp,lua_detachouterspace()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDetachOuterSpace(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    for (auto &label: doc->labellist)
    {
        if (label->IsSelected)
            label->IsExternal = false;
    }

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
 * @brief Compute a bounding box for the problem.
 * @param L
 * @return 4 on success, 0 on failure
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_getboundingbox()}<br> \b undocumented in manual42
 * - \lua{ei_getboundingbox()}<br> \b undocumented in manual42
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_getboundingbox()}
 * - \femm42{femm/beladrawLua.cpp,lua_getboundingbox()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaGetBoundingBox(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    double x[2],y[2];
    if (doc->getBoundingBox(x,y))
    {
        lua_pushnumber(L,x[0]);
        lua_pushnumber(L,x[1]);
        lua_pushnumber(L,y[0]);
        lua_pushnumber(L,y[1]);
        return 4;
    }
    else return 0;
}

/**
 * @brief Get information about the problem description.
 * Returns info on problem description. Returns four values
 * 1. problem type
 * 2. frequency in Hz (only for magnetics problems)
 * 3. depth assumed for planar problems in meters
 * 4. length unit used to draw the problem in meters
 * @param L
 * @return 3 or 4 (depending on document type)
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_getprobleminfo()}
 * - \lua{mo_getprobleminfo()}
 * - \lua{ei_getprobleminfo()}
 * - \lua{eo_getprobleminfo()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_getprobleminfo()}
 * - \femm42{femm/beladrawLua.cpp,lua_getprobleminfo()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaGetProblemInfo(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    int num=0;

    lua_pushnumber(L,doc->ProblemType);
    num++;
    if (doc->filetype == FileType::MagneticsFile)
    {
        lua_pushnumber(L,doc->Frequency);
        num++;
    }
    lua_pushnumber(L,doc->Depth);
    num++;
    switch (doc->LengthUnits)
    {
    case LengthMillimeters:
        lua_pushnumber(L,0.001);
        break;
    case LengthCentimeters:
        lua_pushnumber(L,0.01);
        break;
    case LengthMeters:
        lua_pushnumber(L,1.0);
        break;
    case LengthMils:
        lua_pushnumber(L,2.54e-05);
        break;
    case LengthMicrometers:
        lua_pushnumber(L,1.0e-06);
        break;
    case LengthInches:
    default:// inches
        lua_pushnumber(L,0.0254);
        break;
    }
    num++;
    return num;
}

/**
 * @brief Get the document title
 * @param L
 * @return 1
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_gettitle()}
 * - \lua{mo_gettitle()}
 * - \lua{ei_gettitle()}
 * - \lua{eo_gettitle()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_gettitle()}
 * - \femm42{femm/beladrawLua.cpp,lua_gettitle()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaGetTitle(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    lua_pushstring(L, doc->getTitle().c_str());
    return 1;
}

/**
 * @brief Load the solution and run the postprocessor on it.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_loadsolution()}
 * - \lua{mo_reload()}
 * - \lua{ei_loadsolution()}
 * - \lua{eo_reload()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_runpost()}
 * - \femm42{femm/beladrawLua.cpp,lua_runpost()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaLoadSolution(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    if (doc->pathName.empty())
    {
        lua_error(L,"No results to display");
        return 0;
    }

    std::size_t dotpos = doc->pathName.find_last_of(".");
    std::string solutionFile = doc->pathName.substr(0,dotpos);
    solutionFile += femm::outputExtensionForFileType(doc->filetype);

    femmState->invalidateSolutionData();
    auto fpproc = femmState->getFPProc();
    if (!fpproc->OpenDocument(solutionFile))
    {
        std::string msg = "loadsolution(): error while loading solution file:\n";
        msg += solutionFile;
        lua_error(L, msg.c_str());
    }
    return 0;
}

/**
 * @brief Mirror a copy of the selected objects about a line.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_mirror(x1,y1,x2,y2,(editaction))}
 * - \lua{ei_mirror(x1,y1,x2,y2,(editaction))}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_mirror()}
 * - \femm42{femm/beladrawLua.cpp,lua_mirror()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaMirrorCopy(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int n = lua_gettop(L);

    if (n!=4 && n!=5)
    {
        lua_error(L,"Invalid number of parameters for mirror\n");
        return 0;
    }

    double m_pax = lua_todouble(L,1);
    double m_pay = lua_todouble(L,2);
    double m_pbx = lua_todouble(L,3);
    double m_pby = lua_todouble(L,4);

    EditMode editAction;
    if (n==5) {
        editAction = intToEditMode((int)lua_todouble(L,5));
    } else {
        editAction = mesher->d_EditMode;
    }

    if (editAction == EditMode::Invalid)
    {
        lua_error(L, "mirror(): no editmode given and no default edit mode set!\n");
        return 0;
    }

    mesher->UpdateUndo();
    mesher->MirrorCopy(m_pax,m_pay,m_pbx,m_pby,editAction);

    return 0;
}

/**
 * @brief Rotate selected objects around a point by a given angle.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_moverotate(bx,by,shiftangle,(editaction))}
 * - \lua{ei_moverotate(bx,by,shiftangle,(editaction))}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_move_rotate()}
 * - \femm42{femm/beladrawLua.cpp,lua_move_rotate()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaMoveRotate(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int n;
    n=lua_gettop(L);

    if(n!=4 && n!=3)
    {
        lua_error(L,"Invalid number of parameters for move rotate!\n");
        return 0;
    }

    double x = lua_todouble(L,1);
    double y = lua_todouble(L,2);
    double shiftangle = lua_todouble(L,3);

    EditMode editAction;
    if (n==4) {
        editAction = intToEditMode((int)lua_todouble(L,4));
    } else {
        editAction = mesher->d_EditMode;
    }
    if (editAction == EditMode::Invalid)
    {
            lua_error(L, "moverotate(): Invalid value of editaction!\n");
            return 0;
    }

    mesher->UpdateUndo();
    mesher->RotateMove(CComplex(x,y),shiftangle,editAction);
    // Note(ZaJ): shouldn't the invalidation be done by translateMove?
    doc->invalidateMesh();
    mesher->meshline.clear();
    mesher->meshnode.clear();
    mesher->greymeshline.clear();

    return 0;
}

/**
 * @brief Translate selected objects by a given vector.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_movetranslate(dx,dy,(editaction))}
 * - \lua{ei_movetranslate(dx,dy,(editaction))}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_move_translate()}
 * - \femm42{femm/beladrawLua.cpp,lua_move_translate()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaMoveTranslate(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int n = lua_gettop(L);
    if( n<2 || n>3)
    {
        lua_error(L,"Invalid number of parameters for move translate!\n");
        return 0;
    }

    double x = lua_todouble(L,1);
    double y = lua_todouble(L,2);

    EditMode editAction;
    if (n==3) {
        editAction = intToEditMode((int)lua_todouble(L,3));
    } else {
        editAction = mesher->d_EditMode;
    }

    if (editAction == EditMode::Invalid)
    {
        lua_error(L, "movetranslate(): no editmode given and no default edit mode set!\n");
        return 0;
    }

    mesher->UpdateUndo();
    mesher->TranslateMove(x,y,editAction);
    // Note(ZaJ): shouldn't the invalidation be done by translateMove?
    doc->invalidateMesh();
    mesher->meshline.clear();
    mesher->meshnode.clear();
    mesher->greymeshline.clear();

    return 0;
}

/**
 * @brief Clear mesh data.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_purgemesh()} clears the mesh out of both the screen and memory.
 * - \lua{ei_purgemesh()} clears the mesh out of both the screen and memory.
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_purge_mesh()}
 * - \femm42{femm/beladrawLua.cpp,lua_purge_mesh()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaPurgeMesh(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    doc->invalidateMesh();
    mesher->meshline.clear();
    mesher->meshnode.clear();
    mesher->greymeshline.clear();

    return 0;
}

/**
 * @brief Explicitly calls the mesher.
 * As a side-effect, this method calls FMesher::LoadMesh() to count the number of mesh nodes.
 * This means that the memory consumption will be a little bit higher as when only luaAnalyze is called.
 *
 * \remark The femm42 documentation states that "The number of elements in the mesh is pushed back onto the lua stack.", but the implementation does not do it.
 * @param L
 * @return 1 on success, 0 otherwise.
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_createmesh()} runs triangle to create a mesh.
 * - \lua{ei_createmesh()} runs triangle to create a mesh.
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_create_mesh()}
 *
 * #### Additional source:
 * - \femm42{femm/femmeLua.cpp,lua_create_mesh()}: extracts thisDoc (=mesherDoc) and the accompanying FemmeViewDoc, calls CFemmeView::lnuMakeMesh()
 * - \femm42{femm/beladrawLua.cpp,lua_create_mesh()}
 * - \femm42{femm/FemmeDoc.cpp,CFemmeView::lnuMakeMesh()}: calls OnMakeMesh
 * - \femm42{femm/FemmeView.cpp,CFemmeView::OnMakeMesh()}: does the things we do here directly...
 * - \femm42{femm/beladrawView.cpp,CbeladrawView::OnMakeMesh()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaCreateMesh(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    std::string pathName = doc->pathName;
    if (pathName.empty())
    {
        lua_error(L,"A data file must be loaded,\nor the current data must saved.");
        return 0;
    }
    if (!doc->saveFEMFile(pathName))
    {
        lua_error(L, "createmesh(): Could not save fem file!\n");
        return 0;
    }
    if (!doc->consistencyCheckOK())
    {
        lua_error(L,"createmesh(): consistency check failed before meshing!\n");
        return 0;
    }

    //BeginWaitCursor();
    if (mesher->HasPeriodicBC()){
        if (mesher->DoPeriodicBCTriangulation(pathName) != 0)
        {
            //EndWaitCursor();
            mesher->UnselectAll();
            lua_error(L, "createmesh(): Periodic BC triangulation failed!\n");
            return 0;
        }
    } else {
        if (mesher->DoNonPeriodicBCTriangulation(pathName) != 0)
        {
            //EndWaitCursor();
            lua_error(L, "createmesh(): Nonperiodic BC triangulation failed!\n");
            return 0;
        }
    }
    bool LoadMesh=mesher->LoadMesh(pathName);
    //EndWaitCursor();

    if (LoadMesh)
    {
        //MeshUpToDate=TRUE;
        //if(MeshFlag==FALSE) OnShowMesh();
        //else InvalidateRect(NULL);
        //CString s;
        //s.Format("Created mesh with %i nodes",mesher->meshnode.GetSize());
        //if (mesher->greymeshline.GetSize()!=0)
        //    s+="\nGrey mesh lines denote regions\nthat have no block label.";
        //if(bLinehook==FALSE) AfxMessageBox(s,MB_ICONINFORMATION);
        //else lua_pushnumber(lua,(int) mesher->meshnode.GetSize());

        lua_pushnumber(L,(int) mesher->meshnode.size());
        // Note(ZaJ): femm42 returns 0 - I think that's a bug
        return 1;
    }

    return 0;
}

/**
 * @brief Turn a corner into a curve of given radius.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_createradius(x,y,r)}
 * - \lua{ei_createradius(x,y,r)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_createradius()}
 * - \femm42{femm/beladrawLua.cpp,lua_createradius()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaCreateRadius(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int n = lua_gettop(L);
    if (n!=3)
        return 0;

    double x = lua_todouble(L,1);
    double y = lua_todouble(L,2);
    double r = fabs(lua_todouble(L,3));

    int node = mesher->ClosestNode(x,y);
    if (node<0)
        return 0; // catch case where no nodes have been drawn yet;

    if (!mesher->CanCreateRadius(node))
    {
        lua_error(L, "The specified point is not suitable for conversion into a radius\n");
        return 0;
    }

    if (!mesher->CreateRadius(node,r))
    {
        lua_error(L, "Could not make a radius of the prescribed dimension\n");
        return 0;
    }
    doc->invalidateMesh();

    return 0;
}

/**
 * @brief Define properties of external region.
 * Defines an axisymmetric external region to be used in
 * conjuction with the Kelvin Transformation method of modeling unbounded problems.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_defineouterspace(Zo,Ro,Ri)}
 * - \lua{ei_defineouterspace(Zo,Ro,Ri)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_defineouterspace()}
 * - \femm42{femm/beladrawLua.cpp,lua_defineouterspace()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaDefineOuterSpace(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    int n = lua_gettop(L);
    if (n!=3)
        return 0;

    doc->extZo = fabs(lua_todouble(L,1));
    doc->extRo = fabs(lua_todouble(L,2));
    doc->extRi = fabs(lua_todouble(L,3));

    if((doc->extRo==0) || (doc->extRi==0))
    {
        doc->extZo = 0;
        doc->extRo = 0;
        doc->extRi = 0;
    }

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
 * @brief Scale the selected objects
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_scale(bx,by,scalefactor,(editaction))}
 * - \lua{ei_scale(bx,by,scalefactor,(editaction))}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_scale()}
 * - \femm42{femm/beladrawLua.cpp,lua_scale()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaScaleMove(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int n = lua_gettop(L);

    double x=lua_todouble(L,1);
    double y=lua_todouble(L,2);
    double scalefactor=lua_todouble(L,3);

    EditMode editAction;
    if (n==4) {
        editAction = intToEditMode((int)lua_todouble(L,4));
    } else {
        editAction = mesher->d_EditMode;
    }

    if (editAction == EditMode::Invalid)
    {
        lua_error(L, "scale(): no editmode given and no default edit mode set!\n");
        return 0;
    }

    if (n!=4 && n!=3)
    {
        lua_error(L, "Invalid number of parameters for scale");
        return 0;
    }


    mesher->UpdateUndo();
    mesher->ScaleMove(x,y,scalefactor,editAction);

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

/**
 * @brief Select objects in a given radius around a point.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_selectcircle(x,y,R,(editmode))}
 * - \lua{ei_selectcircle(x,y,R,(editmode))}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_selectcircle()}
 * - \femm42{femm/beladrawLua.cpp,lua_selectcircle()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaSelectWithinCircle(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int n=lua_gettop(L);
    if (n<3)
        return 0;

    CComplex c=lua_tonumber(L,1)+I*lua_tonumber(L,2);
    double R=lua_todouble(L,3);

    EditMode editAction;
    if (n>3) {
        editAction = intToEditMode((int)lua_todouble(L,4));
    } else {
        editAction = mesher->d_EditMode;
    }

    if (editAction == EditMode::Invalid)
    {
        lua_error(L, "selectcircle(): no editmode given and no default edit mode set!\n");
        return 0;
    }

    if((editAction==EditMode::EditNodes) || (editAction==EditMode::EditGroup))
    {
        for (auto &node: doc->nodelist)
        {
            CComplex q = node->CC();
            if (abs(q-c)<=R)
                node->IsSelected = true;
        }
    }

    if((editAction==EditMode::EditLabels) || (editAction==EditMode::EditGroup))
    {
        for (auto &label: doc->labellist)
        {
            CComplex q (label->x,label->y);
            if (abs(q-c)<=R)
                label->IsSelected = true;
        }
    }
    if((editAction==EditMode::EditLines) || (editAction==EditMode::EditGroup))
    {
        for (auto &line: doc->linelist)
        {
            CComplex q0 = doc->nodelist[line->n0]->CC();
            CComplex q1 = doc->nodelist[line->n1]->CC();

            if (abs(q0-c)<=R && abs(q1-c)<=R)
                line->IsSelected = true;
        }
    }

    if((editAction==EditMode::EditArcs) || (editAction==EditMode::EditGroup))
    {
        for (auto &arc: doc->arclist)
        {
            CComplex q0 = doc->nodelist[arc->n0]->CC();
            CComplex q1 = doc->nodelist[arc->n1]->CC();

            if (abs(q0-c)<=R && abs(q1-c)<=R)
                arc->IsSelected = true;
        }
    }
    return 0;
}

/**
 * @brief Set the default mesher EditMode.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_seteditmode(editmode)}
 * - \lua{ei_seteditmode(editmode)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_seteditmode()}
 * - \femm42{femm/beladrawLua.cpp,lua_seteditmode()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaSetEditMode(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    EditMode mode;
    std::string modeString (lua_tostring(L,1));
    if (modeString == "nodes")
        mode = EditMode::EditNodes;
    else if (modeString == "segments")
        mode = EditMode::EditLines;
    else if (modeString == "blocks")
        mode = EditMode::EditLabels;
    else if (modeString == "arcsegments")
        mode = EditMode::EditArcs;
    else if (modeString == "group")
        mode = EditMode::EditGroup;
    else {
        lua_error(L, "mi_seteditmode(): Invalid value of editmode!\n");
        return 0;
    }

    mesher->d_EditMode = mode;
    return 0;
}

/**
 * @brief Set the group of selected items and unselect them.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_setgroup(n)} Set the group associated of the selected items to n
 * - \lua{ei_setgroup(n)} Set the group associated of the selected items to n
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_setgroup()}
 * - \femm42{femm/beladrawLua.cpp,lua_setgroup()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaSetGroup(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    int n=lua_gettop(L);
    int grp;
    if (n>0)
        grp =(int) lua_todouble(L,1);
    else
        return 0;

    for(auto &node: doc->nodelist)
    {
        if(node->IsSelected)
            node->InGroup=grp;
    }

    for(auto &line: doc->linelist)
    {
        if(line->IsSelected)
            line->InGroup=grp;
    }

    for(auto &arc: doc->arclist)
    {
        if(arc->IsSelected)
            arc->InGroup=grp;
    }

    for(auto &label: doc->labellist)
    {
        if(label->IsSelected)
            label->InGroup=grp;
    }

    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();
    mesher->UnselectAll();

    return 0;
}
