/* Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
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

#include "LuaMagneticsCommands.h"

#include "CPointVals.h"
#include "FemmState.h"
#include "fpproc.h"
#include "LuaInstance.h"

#include <lua.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#ifdef DEBUG_FEMMLUA
#define debug std::cerr
#else
#define debug while(false) std::cerr
#endif

using namespace femm;

void femmcli::LuaMagneticsCommands::registerCommands(LuaInstance &li)
{
    li.addFunction("mi_add_arc", luaAddArc);
    li.addFunction("mi_addarc", luaAddArc);
    li.addFunction("mi_add_bh_point", luaAddBHPoint);
    li.addFunction("mi_addbhpoint", luaAddBHPoint);
    li.addFunction("mi_add_bound_prop", luaAddBoundaryProp);
    li.addFunction("mi_addboundprop", luaAddBoundaryProp);
    li.addFunction("mi_add_circ_prop", luaAddCircuitProp);
    li.addFunction("mi_addcircprop", luaAddCircuitProp);
    li.addFunction("mo_add_contour", luaAddcontourNOP);
    li.addFunction("mo_addcontour", luaAddcontourNOP);
    li.addFunction("mi_add_block_label", luaAddBlocklabel);
    li.addFunction("mi_addblocklabel", luaAddBlocklabel);
    li.addFunction("mi_add_segment", luaAddLine);
    li.addFunction("mi_addsegment", luaAddLine);
    li.addFunction("mi_add_material", luaAddMatProp);
    li.addFunction("mi_addmaterial", luaAddMatProp);
    li.addFunction("mi_add_node", luaAddNode);
    li.addFunction("mi_addnode", luaAddNode);
    li.addFunction("mi_add_point_prop", luaAddPointProp);
    li.addFunction("mi_addpointprop", luaAddPointProp);
    li.addFunction("mi_analyse", luaAnalyze);
    li.addFunction("mi_analyze", luaAnalyze);
    li.addFunction("mi_attach_default", luaAttachDefault);
    li.addFunction("mi_attachdefault", luaAttachDefault);
    li.addFunction("mi_attach_outer_space", luaAttachOuterSpace);
    li.addFunction("mi_attachouterspace", luaAttachOuterSpace);
    li.addFunction("mo_bend_contour", luaBendcontourNOP);
    li.addFunction("mo_bendcontour", luaBendcontourNOP);
    li.addFunction("mo_block_integral", luaBlockIntegral);
    li.addFunction("mo_blockintegral", luaBlockIntegral);
    li.addFunction("mi_clear_bh_points", luaClearBHPoints);
    li.addFunction("mi_clearbhpoints", luaClearBHPoints);
    li.addFunction("mo_clear_block", luaClearBlock);
    li.addFunction("mo_clearblock", luaClearBlock);
    li.addFunction("mo_clear_contour", luaClearcontourNOP);
    li.addFunction("mo_clearcontour", luaClearcontourNOP);
    li.addFunction("mi_clear_selected", luaClearSelected);
    li.addFunction("mi_clearselected", luaClearSelected);
    li.addFunction("mi_copy_rotate", luaCopyRotate);
    li.addFunction("mi_copyrotate", luaCopyRotate);
    li.addFunction("mi_copy_translate", luaCopyTranslate);
    li.addFunction("mi_copytranslate", luaCopyTranslate);
    li.addFunction("mi_create_mesh", luaCreateMesh);
    li.addFunction("mi_createmesh", luaCreateMesh);
    li.addFunction("mi_create_radius", luaCreateradiusNOP);
    li.addFunction("mi_createradius", luaCreateradiusNOP);
    li.addFunction("mi_define_outer_space", luaDefineOuterSpace);
    li.addFunction("mi_defineouterspace", luaDefineOuterSpace);
    li.addFunction("mi_delete_bound_prop", luaDeleteBoundaryProperty);
    li.addFunction("mi_deleteboundprop", luaDeleteBoundaryProperty);
    li.addFunction("mi_delete_circuit", luaDeleteCircuitProperty);
    li.addFunction("mi_deletecircuit", luaDeleteCircuitProperty);
    li.addFunction("mi_delete_selected_arcsegments", luaDeleteSelectedArcSegments);
    li.addFunction("mi_deleteselectedarcsegments", luaDeleteSelectedArcSegments);
    li.addFunction("mi_delete_selected_labels", luaDeleteSelectedBlockLabels);
    li.addFunction("mi_deleteselectedlabels", luaDeleteSelectedBlockLabels);
    li.addFunction("mi_delete_selected", luaDeleteSelected);
    li.addFunction("mi_deleteselected", luaDeleteSelected);
    li.addFunction("mi_delete_selected_nodes", luaDeleteSelectedNodes);
    li.addFunction("mi_deleteselectednodes", luaDeleteSelectedNodes);
    li.addFunction("mi_delete_selected_segments", luaDeleteSelectedSegments);
    li.addFunction("mi_deleteselectedsegments", luaDeleteSelectedSegments);
    li.addFunction("mi_delete_material", luaDeleteMaterial);
    li.addFunction("mi_deletematerial", luaDeleteMaterial);
    li.addFunction("mi_delete_point_prop", luaDeletePointProperty);
    li.addFunction("mi_deletepointprop", luaDeletePointProperty);
    li.addFunction("mi_detach_default", luaDetachDefault);
    li.addFunction("mi_detachdefault", luaDetachDefault);
    li.addFunction("mi_detach_outer_space", luaDetachOuterSpace);
    li.addFunction("mi_detachouterspace", luaDetachOuterSpace);
    li.addFunction("mo_close", luaExitPost);
    li.addFunction("mi_close", luaExitPre);
    li.addFunction("mi_getboundingbox", luaGetboundingboxNOP);
    li.addFunction("mo_get_circuit_properties", luaGetCircuitProperties);
    li.addFunction("mo_getcircuitproperties", luaGetCircuitProperties);
    li.addFunction("mo_get_element", luaGetElement);
    li.addFunction("mo_getelement", luaGetElement);
    li.addFunction("mi_get_material", luaGetmaterialNOP);
    li.addFunction("mi_getmaterial", luaGetmaterialNOP);
    li.addFunction("mo_get_node", luaGetnodeNOP);
    li.addFunction("mo_getnode", luaGetnodeNOP);
    li.addFunction("mo_get_point_values", luaGetPointVals);
    li.addFunction("mo_getpointvalues", luaGetPointVals);
    li.addFunction("mi_getprobleminfo", luaGetProblemInfo);
    li.addFunction("mo_get_problem_info", luaGetProblemInfo);
    li.addFunction("mo_getprobleminfo", luaGetProblemInfo);
    li.addFunction("mi_get_title", luaGettitleNOP);
    li.addFunction("mi_gettitle", luaGettitleNOP);
    li.addFunction("mo_get_title", luaGettitleNOP);
    li.addFunction("mo_gettitle", luaGettitleNOP);
    li.addFunction("mo_gradient", luaGradientNOP);
    li.addFunction("mi_grid_snap", LuaInstance::luaNOP);
    li.addFunction("mi_gridsnap", LuaInstance::luaNOP);
    li.addFunction("mo_grid_snap", LuaInstance::luaNOP);
    li.addFunction("mo_gridsnap", LuaInstance::luaNOP);
    li.addFunction("mo_group_select_block", luaGroupSelectBlock);
    li.addFunction("mo_groupselectblock", luaGroupSelectBlock);
    li.addFunction("mo_hide_contour_plot", LuaInstance::luaNOP);
    li.addFunction("mo_hidecontourplot", LuaInstance::luaNOP);
    li.addFunction("mo_hide_density_plot", LuaInstance::luaNOP);
    li.addFunction("mo_hidedensityplot", LuaInstance::luaNOP);
    li.addFunction("mi_hide_grid", LuaInstance::luaNOP);
    li.addFunction("mi_hidegrid", LuaInstance::luaNOP);
    li.addFunction("mo_hide_grid", LuaInstance::luaNOP);
    li.addFunction("mo_hidegrid", LuaInstance::luaNOP);
    li.addFunction("mo_hide_mesh", LuaInstance::luaNOP);
    li.addFunction("mo_hidemesh", LuaInstance::luaNOP);
    li.addFunction("mo_hide_points", LuaInstance::luaNOP);
    li.addFunction("mo_hidepoints", LuaInstance::luaNOP);
    li.addFunction("mo_line_integral", luaLineintegralNOP);
    li.addFunction("mo_lineintegral", luaLineintegralNOP);
    li.addFunction("mo_make_plot", luaMakeplotNOP);
    li.addFunction("mo_makeplot", luaMakeplotNOP);
    li.addFunction("mi_maximize", LuaInstance::luaNOP);
    li.addFunction("mo_maximize", LuaInstance::luaNOP);
    li.addFunction("mi_minimize", LuaInstance::luaNOP);
    li.addFunction("mo_minimize", LuaInstance::luaNOP);
    li.addFunction("mi_mirror", luaMirrorNOP);
    li.addFunction("mi_modify_bound_prop", luaModboundpropNOP);
    li.addFunction("mi_modifyboundprop", luaModboundpropNOP);
    li.addFunction("mi_modify_circ_prop", luaModifyCircuitProperty);
    li.addFunction("mi_modifycircprop", luaModifyCircuitProperty);
    li.addFunction("mi_modify_material", luaModmatpropNOP);
    li.addFunction("mi_modifymaterial", luaModmatpropNOP);
    li.addFunction("mi_modify_point_prop", luaModpointpropNOP);
    li.addFunction("mi_modifypointprop", luaModpointpropNOP);
    li.addFunction("mi_move_rotate", luaMoveRotate);
    li.addFunction("mi_moverotate", luaMoveRotate);
    li.addFunction("mi_move_translate", luaMoveTranslate);
    li.addFunction("mi_movetranslate", luaMoveTranslate);
    li.addFunction("mi_new_document", luaNewdocumentNOP);
    li.addFunction("mi_newdocument", luaNewdocumentNOP);
    li.addFunction("mo_num_elements", luaNumElements);
    li.addFunction("mo_numelements", luaNumElements);
    li.addFunction("mo_num_nodes", luaNumnodesNOP);
    li.addFunction("mo_numnodes", luaNumnodesNOP);
    li.addFunction("mi_setprevious", luaPreviousNOP);
    li.addFunction("mi_prob_def", luaProbDef);
    li.addFunction("mi_probdef", luaProbDef);
    li.addFunction("mi_purge_mesh", luaPurgeMeshNOP);
    li.addFunction("mi_purgemesh", luaPurgeMeshNOP);
    li.addFunction("mi_read_dxf", LuaInstance::luaNOP);
    li.addFunction("mi_readdxf", LuaInstance::luaNOP);
    li.addFunction("mo_refresh_view", LuaInstance::luaNOP);
    li.addFunction("mo_refreshview", LuaInstance::luaNOP);
    li.addFunction("mo_reload", luaReloadNOP);
    li.addFunction("mi_resize", LuaInstance::luaNOP);
    li.addFunction("mo_resize", LuaInstance::luaNOP);
    li.addFunction("mi_restore", LuaInstance::luaNOP);
    li.addFunction("mo_restore", LuaInstance::luaNOP);
    li.addFunction("mi_load_solution", luaLoadSolution);
    li.addFunction("mi_loadsolution", luaLoadSolution);
    li.addFunction("mi_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("mi_savebitmap", LuaInstance::luaNOP);
    li.addFunction("mo_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("mo_savebitmap", LuaInstance::luaNOP);
    li.addFunction("mi_save_as", luaSaveDocument);
    li.addFunction("mi_saveas", luaSaveDocument);
    li.addFunction("mi_save_dxf", LuaInstance::luaNOP);
    li.addFunction("mi_savedxf", LuaInstance::luaNOP);
    li.addFunction("mi_save_metafile", LuaInstance::luaNOP);
    li.addFunction("mi_savemetafile", LuaInstance::luaNOP);
    li.addFunction("mo_save_metafile", LuaInstance::luaNOP);
    li.addFunction("mo_savemetafile", LuaInstance::luaNOP);
    li.addFunction("mi_scale", luaScaleNOP);
    li.addFunction("mi_select_arcsegment", luaSelectArcsegment);
    li.addFunction("mi_selectarcsegment", luaSelectArcsegment);
    li.addFunction("mo_select_block", luaSelectBlockNOP);
    li.addFunction("mo_selectblock", luaSelectBlockNOP);
    li.addFunction("mi_select_circle", luaSelectcircleNOP);
    li.addFunction("mi_selectcircle", luaSelectcircleNOP);
    li.addFunction("mi_select_group", luaSelectGroup);
    li.addFunction("mi_selectgroup", luaSelectGroup);
    li.addFunction("mi_select_label", luaSelectBlocklabel);
    li.addFunction("mi_selectlabel", luaSelectBlocklabel);
    li.addFunction("mo_select_point", luaSelectlineNOP);
    li.addFunction("mo_selectpoint", luaSelectlineNOP);
    li.addFunction("mi_select_node", luaSelectnode);
    li.addFunction("mi_selectnode", luaSelectnode);
    li.addFunction("mi_select_rectangle", luaSelectrectangleNOP);
    li.addFunction("mi_selectrectangle", luaSelectrectangleNOP);
    li.addFunction("mi_select_segment", luaSelectSegment);
    li.addFunction("mi_selectsegment", luaSelectSegment);
    li.addFunction("mi_set_arcsegment_prop", luaSetArcsegmentProp);
    li.addFunction("mi_setarcsegmentprop", luaSetArcsegmentProp);
    li.addFunction("mi_set_block_prop", luaSetBlocklabelProp);
    li.addFunction("mi_setblockprop", luaSetBlocklabelProp);
    li.addFunction("mi_set_edit_mode", luaSetEditMode);
    li.addFunction("mi_seteditmode", luaSetEditMode);
    li.addFunction("mo_set_edit_mode", luaSetEditMode);
    li.addFunction("mo_seteditmode", luaSetEditMode);
    li.addFunction("mi_set_grid", LuaInstance::luaNOP);
    li.addFunction("mi_setgrid", LuaInstance::luaNOP);
    li.addFunction("mo_set_grid", LuaInstance::luaNOP);
    li.addFunction("mo_setgrid", LuaInstance::luaNOP);
    li.addFunction("mi_set_group", luaSetgroupNOP);
    li.addFunction("mi_setgroup", luaSetgroupNOP);
    li.addFunction("mi_set_node_prop", luaSetNodeProp);
    li.addFunction("mi_setnodeprop", luaSetNodeProp);
    li.addFunction("mi_set_segment_prop", luaSetSegmentProp);
    li.addFunction("mi_setsegmentprop", luaSetSegmentProp);
    li.addFunction("mo_show_contour_plot", LuaInstance::luaNOP);
    li.addFunction("mo_showcontourplot", LuaInstance::luaNOP);
    li.addFunction("mo_show_density_plot", LuaInstance::luaNOP);
    li.addFunction("mo_showdensityplot", LuaInstance::luaNOP);
    li.addFunction("mi_show_grid", LuaInstance::luaNOP);
    li.addFunction("mi_showgrid", LuaInstance::luaNOP);
    li.addFunction("mo_show_grid", LuaInstance::luaNOP);
    li.addFunction("mo_showgrid", LuaInstance::luaNOP);
    li.addFunction("mi_show_mesh", LuaInstance::luaNOP);
    li.addFunction("mi_showmesh", LuaInstance::luaNOP);
    li.addFunction("mo_show_mesh", LuaInstance::luaNOP);
    li.addFunction("mo_showmesh", LuaInstance::luaNOP);
    li.addFunction("mi_show_names", LuaInstance::luaNOP);
    li.addFunction("mi_shownames", LuaInstance::luaNOP);
    li.addFunction("mo_show_names", LuaInstance::luaNOP);
    li.addFunction("mo_shownames", LuaInstance::luaNOP);
    li.addFunction("mo_show_points", LuaInstance::luaNOP);
    li.addFunction("mo_showpoints", LuaInstance::luaNOP);
    li.addFunction("mo_smooth", LuaInstance::luaNOP);
    li.addFunction("mi_set_focus", LuaInstance::luaNOP);
    li.addFunction("mi_setfocus", LuaInstance::luaNOP);
    li.addFunction("mo_set_focus", LuaInstance::luaNOP);
    li.addFunction("mo_setfocus", LuaInstance::luaNOP);
    li.addFunction("mi_refresh_view", LuaInstance::luaNOP);
    li.addFunction("mi_refreshview", LuaInstance::luaNOP);
    li.addFunction("mo_show_vector_plot", LuaInstance::luaNOP);
    li.addFunction("mo_showvectorplot", LuaInstance::luaNOP);
    li.addFunction("mi_zoom_in", LuaInstance::luaNOP);
    li.addFunction("mi_zoomin", LuaInstance::luaNOP);
    li.addFunction("mo_zoom_in", LuaInstance::luaNOP);
    li.addFunction("mo_zoomin", LuaInstance::luaNOP);
    li.addFunction("mi_zoom", LuaInstance::luaNOP);
    li.addFunction("mo_zoom", LuaInstance::luaNOP);
    li.addFunction("mi_zoom_natural", LuaInstance::luaNOP);
    li.addFunction("mi_zoomnatural", LuaInstance::luaNOP);
    li.addFunction("mo_zoom_natural", LuaInstance::luaNOP);
    li.addFunction("mo_zoomnatural", LuaInstance::luaNOP);
    li.addFunction("mi_zoom_out", LuaInstance::luaNOP);
    li.addFunction("mi_zoomout", LuaInstance::luaNOP);
    li.addFunction("mo_zoom_out", LuaInstance::luaNOP);
    li.addFunction("mo_zoomout", LuaInstance::luaNOP);
}


/**
 * @brief Add a new arc segment.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addarc()}
 *
 * \internal
 * mi_addarc(x1,y1,x2,y2,angle,maxseg)
 * Add a new arc segment from the nearest node to (x1,y1) to the
 * nearest node to (x2,y2) with angle ‘angle’ divided into ‘maxseg’ segments.
 */
int femmcli::LuaMagneticsCommands::luaAddArc(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
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
 * @brief Add a B-H data point to the given material.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addbhpoint()}
 *
 * \internal
 * mi_addbhpoint("blockname",b,h)
 * Adds a B-H data point the the material specified by the string "blockname"
 *
 * \remark Note(ZaJ): xfemm has Bdata and Hdata in CMMaterialProp,
 * while femm42 has two forms of CMaterialProp: one with Bdata and Hdata, and
 * one with a single BHdata.
 * I didn't want to introduce the second form into xfemm, so I tried mapping the BHdata
 * in this function to Bdata+Hdata. Ideally, we'd have a single form BHdata in xfemm, though.
 */
int femmcli::LuaMagneticsCommands::luaAddBHPoint(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    // find the index of the material to modify;
    if (doc->blockproplist.empty())
        return 0; // Note(ZaJ): femm42 returns TRUE
    std::string BlockName = lua_tostring(L,1);

    // for(blockIdx=0;blockIdx<doc->blockproplist.GetSize();blockIdx++)
    //     if(BlockName==doc->blockproplist[blockIdx].BlockName) break;
    // // get out of here if there's no matching material
    // if(blockIdx==doc->blockproplist.GetSize()) return TRUE;
    int blockIdx;
    // search block name
    if (doc->blockMap.count(BlockName))
        blockIdx = doc->blockMap[BlockName];
    else
        return 0; // Note(ZaJ): femm42 returns TRUE

    // cast CMaterialProp to CMMaterialProp:
    CMMaterialProp *blockprop = dynamic_cast<CMMaterialProp*>(doc->blockproplist[blockIdx].get());
    // now, add the bhpoint for the specified material;
    int n = blockprop->BHpoints;
    if(n==0) // Make sure that (0,0) gets included as a data point;
    {
        n=1;
        blockprop->BHpoints=1;
        //blockprop->BHdata=(CComplex *)malloc(sizeof(CComplex));
        //blockprop->BHdata[0]=0;
        blockprop->Bdata.push_back(0);
        blockprop->Hdata.push_back(0);
    }
    //CComplex v = lua_tonumber(L,2)+I*lua_tonumber(L,3);
    double b = lua_tonumber(L,2).re;
    CComplex h = lua_tonumber(L,3);
    // make sure the point isn't a duplicate
    for(int i=0; i<n; i++)
    {
        if (blockprop->Bdata[i]==b && blockprop->Hdata[i]==h)
            return 0;
    }
    blockprop->BHpoints=n+1;
    blockprop->Bdata.push_back(b);
    blockprop->Hdata.push_back(h);

    // make sure that the BH points are in the right order;
    // Note(ZaJ): bubblesort anyone?
    for(int i=0; i<n; i++)
    {
        bool swapped=false;
        for(int j=0; j<n; j++)
        {
            if(blockprop->Bdata[j]>blockprop->Bdata[j+1])
            {
                swapped=true;
                std::swap(blockprop->Bdata[j],blockprop->Bdata[j+1]);
                std::swap(blockprop->Hdata[j],blockprop->Hdata[j+1]);
            }
        }
        if (!swapped) i=n;
    }
    return 0;
}

/**
 * @brief Add a new boundary property with a given name.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addboundprop()}
 *
 * \internal
 * mi_addboundprop("propname", A0, A1, A2, Phi, Mu, Sig, c0, c1, BdryFormat)
 * Adds a new boundary property with name "propname"
 */
int femmcli::LuaMagneticsCommands::luaAddBoundaryProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    std::unique_ptr<CMBoundaryProp> m = std::make_unique<CMBoundaryProp>();

    int n=lua_gettop(L);
    if (n>0) m->BdryName = lua_tostring(L,1);
    if (n>1) m->A0 = lua_todouble(L,2);
    if (n>2) m->A1 = lua_todouble(L,3);
    if (n>3) m->A2 = lua_todouble(L,4);
    if (n>4) m->phi = lua_todouble(L,5);
    if (n>5) m->Mu = lua_todouble(L,6);
    if (n>6) m->Sig = lua_todouble(L,7);
    if (n>7) m->c0 = lua_tonumber(L,8);
    if (n>8) m->c1 = lua_tonumber(L,9);
    if (n>9) m->BdryFormat = (int) lua_todouble(L,10);

    doc->lineproplist.push_back(std::move(m));
    doc->updateLineMap();
    return 0;
}

/**
 * @brief Add a new circuit property with a given name.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addcircuitprop()}
 *
 * \internal
 * mi_addcircprop("circuitname", i, circuittype)
 * Adds a new circuit property with name "circuitname"
 */
int femmcli::LuaMagneticsCommands::luaAddCircuitProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());

    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        //return ((CFemmeDoc *)pFemmeDoc)->old_lua_addcircuitprop(L);
        lua_error(L,"Compatibility mode for mi_addcircprop is not implemented!\n");
        return 0;
    }

    std::unique_ptr<CMCircuit> m = std::make_unique<CMCircuit>();
    int n=lua_gettop(L);

    if (n>0) m->CircName=lua_tostring(L,1);
    if (n>1) m->Amps=lua_tonumber(L,2);
    if (n>2) m->CircType=(int) lua_todouble(L,3);

    femmState->femmDocument->circproplist.push_back(std::move(m));
    femmState->femmDocument->updateCircuitMap();

    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_addcontour()}
 */
int femmcli::LuaMagneticsCommands::luaAddcontourNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Add new block label at given coordinates.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addlabel()}
 *
 * \internal
 * mi_addblocklabel(x,y)
 * Add a new block label at (x,y)
 */
int femmcli::LuaMagneticsCommands::luaAddBlocklabel(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
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
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addline()}
 *
 * \internal
 * mi_addsegment(x1,y1,x2,y2)
 * Add a new line segment from node closest to (x1,y1) to node closest to (x2,y2)
 */
int femmcli::LuaMagneticsCommands::luaAddLine(lua_State *L)
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
 * @brief Add a new material property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addmatprop()}
 *
 * \internal
 * mi addmaterial("materialname", mu x, mu y, H c, J, Cduct, Lam d, Phi hmax,
 *                lam fill, LamType, Phi hx, Phi hy, NStrands, WireD)
 * Adds a new material called "materialname" with the given material properties.
 */
int femmcli::LuaMagneticsCommands::luaAddMatProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());

    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        lua_error(L,"Compatibility mode for mi_addmatprop is not implemented!\n");
        //return ((CFemmeDoc *)pFemmeDoc)->old_lua_addmatprop(L);
        return 0;
    }

    std::unique_ptr<CMSolverMaterialProp> m = std::make_unique<CMSolverMaterialProp>();
    int n=lua_gettop(L);

    if (n>0)
    {
        m->BlockName = lua_tostring(L,1);
    }
    if (n>1){
        m->mu_x=lua_todouble(L,2);
        m->mu_y=m->mu_x;
    }
    if (n>2)  m->mu_y=lua_todouble(L,3);
    if (n>3)  m->H_c=lua_todouble(L,4);
    if (n>4)  m->J=lua_tonumber(L,5);
    if (n>5)  m->Cduct=lua_todouble(L,6);
    if (n>6)  m->Lam_d=lua_todouble(L,7);
    if (n>7)  m->Theta_hn=lua_todouble(L,8);
    if (n>8){
        m->LamFill=lua_todouble(L,9);
        if (m->LamFill<=0) m->LamFill=1;
        if (m->LamFill>1) m->LamFill=1;
    }
    if (n>9){
        m->LamType=(int) lua_todouble(L,10);
        if (m->LamType<0) m->LamType=0;
    }
    if(n>10)
    {
        m->Theta_hx=lua_todouble(L,11);
        m->Theta_hy=lua_todouble(L,12);
    }
    else{
        m->Theta_hx=m->Theta_hn;
        m->Theta_hy=m->Theta_hn;
    }
    if(n>12){
        m->NStrands=(int) lua_todouble(L,13);
        m->WireD=lua_todouble(L,14);
    }

    femmState->femmDocument->blockproplist.push_back(std::move(m));
    femmState->femmDocument->updateBlockMap();
    return 0;
}

/**
 * @brief Add a new node.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addnode()}
 *
 * \internal
 * mi_addnode(x,y) Add a new node at x,y
 */
int femmcli::LuaMagneticsCommands::luaAddNode(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

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
 * @brief Add a new point property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addpointprop()}
 *
 * \internal
 * mi_addpointprop("pointpropname",a,j)
 * Adds a new point property of name "pointpropname"
 * with either a specified potential a in units Webers/Meter or a point current j in units of Amps.
 * Set the unused parameter pairs to 0.
 */
int femmcli::LuaMagneticsCommands::luaAddPointProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        lua_error(L,"Compatibility mode for mi_addpointprop is not implemented!\n");
        //return ((CFemmeDoc *)pFemmeDoc)->old_lua_addpointprop(L);
        return 0;
    }

    std::unique_ptr<CMPointProp> m = std::make_unique<CMPointProp>();
    int n=lua_gettop(L);

    if (n>0) m->PointName = lua_tostring(L,1);
    if (n>1) m->A = lua_tonumber(L,2);
    if (n>2) m->J = lua_tonumber(L,3);

    doc->nodeproplist.push_back(std::move(m));
    doc->updateNodeMap();
    return 0;
}

/**
 * @brief Mesh the problem description, save it, and run the solver.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_analyze()}
 *
 * \internal
 * mi_analyze(flag) runs fkern to solve the problem.
 * Parameter flag (0,1) determines visibility of fkern window.
 *
 * Implementation notes:
 *  * \femm42{femm/femmeLua.cpp,lua_analyze()}: extracts thisDoc (=mesherDoc) and the accompanying FemmeViewDoc, calls CFemmeView::lnu_analyze(flag)
 *  * \femm42{femm/FemmeView.cpp,CFemmeView::OnMenuAnalyze()}: does the things we do here directly...
 */
int femmcli::LuaMagneticsCommands::luaAnalyze(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> doc = femmState->femmDocument;

    // check to see if all blocklabels are kosher...
    if (doc->labellist.size()==0){
        std::string msg = "No block information has been defined\n"
                          "Cannot analyze the problem";
        lua_error(L, msg.c_str());
        return 0;
    }

    bool hasMissingBlockProps = false;
    for(int i=0; i<(int)doc->labellist.size(); i++)
    {
        // note(ZaJ): this can be done better by break;ing from the k loop
        int j=0;
        for(int k=0; k<(int)doc->blockproplist.size(); k++)
        {
            if (doc->labellist[i]->BlockTypeName != doc->blockproplist[k]->BlockName)
                j++;
        }
        // if block type set but not found:
        if ((j==(int)doc->blockproplist.size())
                && (doc->labellist[i]->hasBlockType())
                )
        {
            //if(!hasMissingBlockProps) OnBlockOp();
            hasMissingBlockProps = true;
            doc->labellist[i]->IsSelected = true;
        }
    }

    if (hasMissingBlockProps)
    {
        //InvalidateRect(NULL);
        std::string ermsg = "Material properties have not\n"
                            "been defined for all block labels.\n"
                            "Cannot analyze the problem";
        lua_error(L,ermsg.c_str());
        return 0;
    }


    if (doc->ProblemType==AXISYMMETRIC)
    {
        // check to see if all of the input points are on r>=0 for axisymmetric problems.
        for (int k=0; k<(int)doc->nodelist.size(); k++)
        {
            if (doc->nodelist[k]->x < -(1.e-6))
            {
                //InvalidateRect(NULL);
                std::string ermsg = "The problem domain must lie in\n"
                                    "r>=0 for axisymmetric problems.\n"
                                    "Cannot analyze the problem.";
                lua_error(L,ermsg.c_str());
                return 0;
            }
        }

        // check to see if all block defined to be in an axisymmetric external region are linear.
        bool hasAnisotropicMaterial = false;
        bool hasExteriorProps = true;
        for (int k=0; k<(int)doc->labellist.size(); k++)
        {
            if (doc->labellist[k]->IsExternal)
            {
                if ((doc->extRo==0) || (doc->extRi==0))
                    hasExteriorProps = false;

                for(int i=0; i<(int)doc->blockproplist.size(); i++)
                {
                    if (doc->labellist[k]->BlockTypeName == doc->blockproplist[i]->BlockName)
                    {
                        CMMaterialProp *prop = dynamic_cast<CMMaterialProp*>(doc->blockproplist[i].get());
                        assert(prop);
                        if (prop->BHpoints!=0)
                            hasAnisotropicMaterial = true;
                        else if(prop->mu_x != prop->mu_y)
                            hasAnisotropicMaterial = true;
                    }
                }
            }
        }
        if (hasAnisotropicMaterial)
        {
            //InvalidateRect(NULL);
            std::string ermsg = "Only linear istropic materials are\n"
                                "allowed in axisymmetric external regions.\n"
                                "Cannot analyze the problem";
            lua_error(L,ermsg.c_str());
            return 0;
        }

        if (!hasExteriorProps)
        {
            //InvalidateRect(NULL);
            std::string ermsg = "Some block labels have been specific as placed in\n"
                                "an axisymmetric exterior region, but no properties\n"
                                "have been adequately defined for the exterior region\n"
                                "Cannot analyze the problem";
            lua_error(L,ermsg.c_str());
            return 0;
        }
    }

    std::string pathName = doc->pathName;
    if (pathName.empty())
    {
        lua_error(L,"A data file must be loaded,\nor the current data must saved.");
        return 0;
    }
    if (!doc->saveFEMFile(pathName))
    {
        lua_error(L, "mi_analyze(): Could not save fem file!\n");
        return 0;
    }
    if (!doc->consistencyCheckOK())
    {
        lua_error(L,"mi_analyze(): consistency check failed before meshing!\n");
        return 0;
    }

    //BeginWaitCursor();
    std::shared_ptr<fmesher::FMesher> mesherDoc = femmState->getMesher();
    if (mesherDoc->HasPeriodicBC()){
        if (mesherDoc->DoPeriodicBCTriangulation(pathName) != 0)
        {
            //EndWaitCursor();
            mesherDoc->UnselectAll();
            lua_error(L, "mi_analyze(): Periodic BC triangulation failed!\n");
            return 0;
        }
    }
    else{
        if (mesherDoc->DoNonPeriodicBCTriangulation(pathName) != 0)
        {
            //EndWaitCursor();
            lua_error(L, "mi_analyze(): Nonperiodic BC triangulation failed!\n");
            return 0;
        }
    }
    //EndWaitCursor();
    if (!doc->consistencyCheckOK())
    {
        lua_error(L,"mi_analyze(): consistency check failed after meshing!\n");
        return 0;
    }

    FSolver theFSolver;
    // filename.fem -> filename
    std::size_t dotpos = doc->pathName.find_last_of(".");
    theFSolver.PathName = doc->pathName.substr(0,dotpos);
    theFSolver.WarnMessage = &PrintWarningMsg;
    theFSolver.PrintMessage = &PrintWarningMsg;
    if (!theFSolver.LoadProblemFile())
    {
        lua_error(L, "mi_analyze(): problem initializing solver!");
        return 0;
    }
    assert( doc->ACSolver == theFSolver.ACSolver);
    assert( doc->Frequency == theFSolver.Frequency);
    assert( doc->lineproplist.size() == theFSolver.lineproplist.size());
    assert( doc->nodeproplist.size() == theFSolver.nodeproplist.size());
    assert( doc->blockproplist.size() == theFSolver.blockproplist.size());
    // this does not hold, because fsolver creates additional circprops
    // assert( doc->circproplist.size() == theFSolver.circproplist.size());
    assert( doc->labellist.size() == theFSolver.labellist.size());
    if (!theFSolver.runSolver())
    {
        lua_error(L, "solver failed.");
    }
    return 0;
}

/**
 * @brief Marks the first selected block label as the default block label.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_attachdefault()}
 *
 * \internal
 * mi_attachdefault()
 * Marks the selected block label as the default block label. This block
 * label is applied to any region that has not been explicitly labeled.
 */
int femmcli::LuaMagneticsCommands::luaAttachDefault(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

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
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_attachouterspace()}
 *
 * \internal
 * mi_attachouterspace()
 * Marks all selected block labels as members of the external region
 * used for modeling unbounded axisymmetric problems via the Kelvin Transformation.
 */
int femmcli::LuaMagneticsCommands::luaAttachOuterSpace(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    for (auto &label: doc->labellist)
    {
        if (label->IsSelected)
            label->IsExternal = true;
    }

    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_bendcontour()}
 */
int femmcli::LuaMagneticsCommands::luaBendcontourNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Calculate a block integral for the selected blocks.
 * @param L
 * @return 1 on success, 0 otherwise
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_blockintegral()}
 *
 * \internal
 * mo_blockintegral(type)
 * Type parameter is documented in femm42 manual.
 */
int femmcli::LuaMagneticsCommands::luaBlockIntegral(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = femmState->getFPProc();
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }
    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        lua_error(L,"Compatibility mode for mo_blockintegral is not implemented!\n");
        //return ((CFemmviewDoc *)pFemmviewdoc)->old_lua_blockintegral(L);
        return 0;
    }

    int type = (int) lua_todouble(L,1);

    bool hasSelectedBlocks = false;
    for (const auto &block: fpproc->blocklist )
    {
        if (block.IsSelected)
        {
            hasSelectedBlocks = true;
            break;
        }
    }

    if (!hasSelectedBlocks)
    {
        lua_error(L,"Cannot integrate\nNo area has been selected");
        return 0;
    }

    CComplex z = fpproc->BlockIntegral(type);

    lua_pushnumber(L,z);
    return 1;
}

/**
 * @brief Clear B-H data for a given material.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_clearbhpoints()}
 *
 * \internal
 * mi_clearbhpoints("blockname")
 * Clears all B-H data points associatied with the material
 * specified by "blockname".
 */
int femmcli::LuaMagneticsCommands::luaClearBHPoints(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    // find the index of the material to modify;
    if (doc->blockproplist.empty())
        return 0; // Note(ZaJ): femm42 returns TRUE
    std::string BlockName = lua_tostring(L,1);

    int blockIdx;
    // search block name
    if (doc->blockMap.count(BlockName))
        blockIdx = doc->blockMap[BlockName];
    else
        return 0; // Note(ZaJ): femm42 returns TRUE

    // cast CMaterialProp to CMMaterialProp:
    CMMaterialProp *blockprop = dynamic_cast<CMMaterialProp*>(doc->blockproplist[blockIdx].get());
    blockprop->BHpoints=0;
    blockprop->Bdata.clear();
    blockprop->Hdata.clear();

    return 0;
}

/**
 * @brief Clear block selection and reset some (fpproc) settings.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_clearblock()}
 *
 * \internal
 * mo_clearblock() Clear block selection
 */
int femmcli::LuaMagneticsCommands::luaClearBlock(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
    std::shared_ptr<FPProc> fpproc = femmState->getFPProc();
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    fpproc->bHasMask= false;
    for(auto &block: fpproc->blocklist)
    {
        block.IsSelected = false;
    }

    // FIXME(ZaJ): xfemm doesn't have this currently:
    //fpproc->EditAction=0;
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_clearcontour()}
 */
int femmcli::LuaMagneticsCommands::luaClearcontourNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Unselect all selected nodes, blocks, segments and arc segments.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_clearselected()}
 *
 * \internal
 * mi clearselected() Clear all selected nodes, blocks, segments and arc segments.
 */
int femmcli::LuaMagneticsCommands::luaClearSelected(lua_State *L)
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
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_copy_rotate()}
 *
 * \internal
 * mi_copyrotate(bx, by, angle, copies, (editaction) )
 * * bx, by – base point for rotation
 * * angle – angle by which the selected objects are incrementally shifted to make each copy. angle is measured in degrees.
 * * copies – number of copies to be produced from the selected objects.
 */
int femmcli::LuaMagneticsCommands::luaCopyRotate(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> doc = femmState->femmDocument;
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

    fmesher::FMesher::EditMode editAction;
    if (n==5) {
        editAction = mesher->intToEditMode((int)lua_todouble(L,5));
    } else {
        editAction = mesher->d_EditMode;
    }

    if (editAction == fmesher::FMesher::EditModeInvalid)
    {
        lua_error(L, "mi_copyrotate(): no editmode given and no default edit mode set!\n");
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
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_copy_translate()}
 *
 * \internal
 * mi_copytranslate(dx, dy, copies, (editaction))
 * * dx,dy – distance by which the selected objects are incrementally shifted.
 * * copies – number of copies to be produced from the selected objects.
 */
int femmcli::LuaMagneticsCommands::luaCopyTranslate(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> doc = femmState->femmDocument;
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

    fmesher::FMesher::EditMode editAction;
    if (n==4) {
        editAction = mesher->intToEditMode((int)lua_todouble(L,4));
    } else {
        editAction = mesher->d_EditMode;
    }

    if (editAction == fmesher::FMesher::EditModeInvalid)
    {
        lua_error(L, "mi_copytranslate(): no editmode given and no default edit mode set!\n");
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
 * @brief Explicitly calls the mesher.
 * As a side-effect, this method calls FMesher::LoadMesh() to count the number of mesh nodes.
 * This means that the memory consumption will be a little bit higher as when only luaAnalyze is called.
 * @param L
 * @return 1 on success, 0 otherwise.
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_create_mesh()}
 *
 * \internal
 * mi_createmesh() runs triangle to create a mesh.
 *
 * \remark The femm42 documentation states that "The number of elements in the mesh is pushed back onto the lua stack.", but the implementation does not do it.
 *
 * Implementation notes:
 *  * \femm42{femm/femmeLua.cpp,lua_create_mesh()}: extracts thisDoc (=mesherDoc) and the accompanying FemmeViewDoc, calls CFemmeView::lnuMakeMesh()
 *  * \femm42{femm/FemmeDoc.cpp,CFemmeView::lnuMakeMesh()}: calls OnMakeMesh
 *  * \femm42{femm/FemmeView.cpp,CFemmeView::OnMakeMesh()}: does the things we do here directly...
 */
int femmcli::LuaMagneticsCommands::luaCreateMesh(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> doc = femmState->femmDocument;
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    std::string pathName = doc->pathName;
    if (pathName.empty())
    {
        lua_error(L,"A data file must be loaded,\nor the current data must saved.");
        return 0;
    }
    if (!doc->saveFEMFile(pathName))
    {
        lua_error(L, "mi_createmesh(): Could not save fem file!\n");
        return 0;
    }
    if (!doc->consistencyCheckOK())
    {
        lua_error(L,"mi_createmesh(): consistency check failed before meshing!\n");
        return 0;
    }

    //BeginWaitCursor();
    if (mesher->HasPeriodicBC()){
        if (mesher->DoPeriodicBCTriangulation(pathName) != 0)
        {
            //EndWaitCursor();
            mesher->UnselectAll();
            lua_error(L, "mi_createmesh(): Periodic BC triangulation failed!\n");
            return 0;
        }
    } else {
        if (mesher->DoNonPeriodicBCTriangulation(pathName) != 0)
        {
            //EndWaitCursor();
            lua_error(L, "mi_createmesh(): Nonperiodic BC triangulation failed!\n");
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_createradius()}
 */
int femmcli::LuaMagneticsCommands::luaCreateradiusNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Define properties of external region.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_defineouterspace()}
 *
 * \internal
 * mi_defineouterspace(Zo,Ro,Ri)
 * Defines an axisymmetric external region to be used in
 * conjuction with the Kelvin Transformation method of modeling unbounded problems.
 */
int femmcli::LuaMagneticsCommands::luaDefineOuterSpace(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

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
 * @brief Delete the given boundary property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_delboundprop()}
 *
 * \internal
 * mi_deleteboundprop("propname") deletes the boundary property named "propname".
 */
int femmcli::LuaMagneticsCommands::luaDeleteBoundaryProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

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
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_delcircuitprop()}
 *
 * \internal
 * mi_deletecircuit("circuitname") deletes the circuit named circuitname.
 */
int femmcli::LuaMagneticsCommands::luaDeleteCircuitProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

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
 * @brief Delete selects arcs.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselectedarcsegments()}
 *
 * \internal
 * mi_deleteselectedarcsegments()
 */
int femmcli::LuaMagneticsCommands::luaDeleteSelectedArcSegments(lua_State *L)
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
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselectedlabels()}
 *
 * \internal
 * mi_deleteselectedlabels()
 */
int femmcli::LuaMagneticsCommands::luaDeleteSelectedBlockLabels(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    mesher->DeleteSelectedBlockLabels();
    return 0;
}

/**
 * @brief Delete selected objects.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselected()}
 *
 * \internal
 * mi_deleteselected()
 */
int femmcli::LuaMagneticsCommands::luaDeleteSelected(lua_State *L)
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
 * @brief Delete selected nodes
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselectednodes()}
 *
 * \internal
 * mi_deleteselectednodes()
 */
int femmcli::LuaMagneticsCommands::luaDeleteSelectedNodes(lua_State *L)
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
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselectedsegments()}
 *
 * \internal
 * mi_deleteselectedsegments()
 */
int femmcli::LuaMagneticsCommands::luaDeleteSelectedSegments(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    mesher->DeleteSelectedSegments();

    return 0;
}

/**
 * @brief Delete the given material property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_delmatprop()}
 *
 * \internal
 * mi deletematerial("materialname") deletes the material named "materialname".
 */
int femmcli::LuaMagneticsCommands::luaDeleteMaterial(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

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
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_delpointprop()}
 *
 * \internal
 * mi_deletepointprop("pointpropname") deletes the point property named "pointpropname"
 */
int femmcli::LuaMagneticsCommands::luaDeletePointProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

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
 * @brief Unset IsDefault for selected block labels.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_detachdefault()}
 *
 * \internal
 * mi_detachdefault()
 * Undefines the default attribute for the selected block labels.
 */
int femmcli::LuaMagneticsCommands::luaDetachDefault(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

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
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_detachouterspace()}
 *
 * \internal
 * mi_detachouterspace()
 * Undefines all selected block labels as members of the external
 * region used for modeling unbounded axisymmetric problems via the Kelvin Transformation.
 */
int femmcli::LuaMagneticsCommands::luaDetachOuterSpace(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    for (auto &label: doc->labellist)
    {
        if (label->IsSelected)
            label->IsExternal = false;
    }

    return 0;
}

/**
 * @brief Closes the current post-processor instance.
 * Invalidates the post-processor data of the FemmProblem.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_exitpost()}
 *
 * \internal
 * mo_close()
 */
int femmcli::LuaMagneticsCommands::luaExitPost(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    femmState->invalidateSolutionData();
    return 0;
}

/**
 * @brief Closes the current pre-processor instance.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_exitpre()}
 *
 * \internal
 * mi_close()
 * Closes current magnetics preprocessor document and destroys magnetics preprocessor window.
 */
int femmcli::LuaMagneticsCommands::luaExitPre(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    femmState->femmDocument.reset();
    // trigger a reset of the mesher object (to break its dependency on femmDocument)
    femmState->getMesher();
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_getboundingbox()}
 */
int femmcli::LuaMagneticsCommands::luaGetboundingboxNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Get information about a circuit property.
 * @param L
 * @return 0 on error, 3 on success.
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_getcircuitprops()}
 *
 * \internal
 * mo_getcircuitproperties("circuit")
 * Used primarily to obtain impedance information associated with circuit properties.
 *
 * Properties are returned for the circuit property named "circuit".
 * Three values are returned by the function:
 * 1. current Current carried by the circuit
 * 2. volts Voltage drop across the circuit
 * 3. flux_re Circuit’s flux linkage
 */
int femmcli::LuaMagneticsCommands::luaGetCircuitProperties(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
    std::shared_ptr<FPProc> fpproc = femmState->getFPProc();
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }
    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        lua_error(L,"Compatibility mode for mo_getcircuitproperties is not implemented!\n");
        //return ((CFemmviewDoc *)pFemmviewdoc)->old_lua_getcircuitprops(L);
        return 0;
    }

    std::string circuitname = lua_tostring(L,1);

    // ok we need to find the correct entry for the circuit name
    auto searchResult = doc->circuitMap.find(circuitname);
    // get out of here if there's no matching circuit
    if (searchResult == doc->circuitMap.end())
    {
        debug << "mo_getcircuitproperties(): No circuit of name " << circuitname << "\n";
        return 0;
    }
    int idx = searchResult->second;

    CComplex amps = fpproc->circproplist[idx].Amps;
    CComplex volts = fpproc->GetVoltageDrop(idx);
    CComplex fluxlinkage = fpproc->GetFluxLinkage(idx);

    lua_pushnumber(L,amps);
    lua_pushnumber(L,volts);
    lua_pushnumber(L,fluxlinkage);

    return 3;
}

/**
 * @brief Get data of indexed element.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_getelement()}
 *
 * \internal
 * mo_getelement(n)
 * MOGetElement[n] returns the following proprerties for the nth element:
 * 1. Index of first element node
 * 2. Index of second element node
 * 3. Index of third element node
 * 4. x (or r) coordinate of the element centroid
 * 5. y (or z) coordinate of the element centroid
 * 6. element area using the length unit defined for the problem
 * 7. group number associated with the element
 *
 * Note: in lua code, indices start at 1.
 */
int femmcli::LuaMagneticsCommands::luaGetElement(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    auto femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = femmState->getFPProc();
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    int idx=(int) lua_todouble(L,1);
    idx--;

    if ( idx<0 || idx>=(int)fpproc->meshelem.size())
        return 0;

    lua_pushnumber(L,fpproc->meshelem[idx].p[0]+1);
    lua_pushnumber(L,fpproc->meshelem[idx].p[1]+1);
    lua_pushnumber(L,fpproc->meshelem[idx].p[2]+1);
    lua_pushnumber(L,Re(fpproc->meshelem[idx].ctr));
    lua_pushnumber(L,Im(fpproc->meshelem[idx].ctr));
    lua_pushnumber(L,fpproc->ElmArea(idx));
    lua_pushnumber(L,fpproc->blocklist[fpproc->meshelem[idx].lbl].InGroup);

    return 7;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_getmaterial()}
 */
int femmcli::LuaMagneticsCommands::luaGetmaterialNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_getnode()}
 */
int femmcli::LuaMagneticsCommands::luaGetnodeNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Get the values for a point.
 * @param L
 * @return 0 on error, otherwise 14
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_getpointvals()}
 *
 * \internal
 * mo_getpointvalues(X,Y)
 * Get the values associated with the point at x,y return values in order.
 */
int femmcli::LuaMagneticsCommands::luaGetPointVals(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        lua_error(L,"Compatibility mode for mo_getpointvalues is not implemented!\n");
        //return ((CFemmviewDoc *)pFemmviewdoc)->old_lua_getpointvals(L);
        return 0;
    }

    auto femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = femmState->getFPProc();
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    double px,py;
    px=lua_tonumber(L,1).re;
    py=lua_tonumber(L,2).re;

    CPointVals u;

    if(fpproc->GetPointValues(px, py, u))
    {
        lua_pushnumber(L,u.A);
        lua_pushnumber(L,u.B1);
        lua_pushnumber(L,u.B2);
        lua_pushnumber(L,u.c);
        lua_pushnumber(L,u.E);
        lua_pushnumber(L,u.H1);
        lua_pushnumber(L,u.H2);
        lua_pushnumber(L,u.Je);
        lua_pushnumber(L,u.Js);
        lua_pushnumber(L,u.mu1);
        lua_pushnumber(L,u.mu2);
        lua_pushnumber(L,u.Pe);
        lua_pushnumber(L,u.Ph);
        lua_pushnumber(L,u.ff);
        return 14;
    }

    return 0;
}

/**
 * @brief Get information about the problem description.
 * @param L
 * @return 4
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_getprobleminfo()}
 *
 * \internal
 * mo_getprobleminfo()
 * Returns info on problem description. Returns four values
 * 1. problem type
 * 2. frequency in Hz
 * 3. depth assumed for planar problems in meters
 * 4. length unit used to draw the problem in meters
 */
int femmcli::LuaMagneticsCommands::luaGetProblemInfo(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    lua_pushnumber(L,doc->ProblemType);
    lua_pushnumber(L,doc->Frequency);
    lua_pushnumber(L,doc->Depth);
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

    return 4;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_gettitle()}
 */
int femmcli::LuaMagneticsCommands::luaGettitleNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_gradient()}
 */
int femmcli::LuaMagneticsCommands::luaGradientNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief (De)select blocks associated with block labels in a given group.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_groupselectblock()}
 *
 * \internal
 * mo_groupselectblock(n)
 * Selects all of the blocks that are labeled by block labels that are
 * members of group n.
 * If no number is specified (i.e. mo_groupselectblock() ), all blocks
 * are selected.
 */
int femmcli::LuaMagneticsCommands::luaGroupSelectBlock(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
    std::shared_ptr<FPProc> fpproc = femmState->getFPProc();
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    // FIXME(ZaJ): xfemm doesn't have this currently:
    //fpproc->EditAction=2;

    // Note(ZaJ): why does this depend on meshelem,
    //            when blocklist is affected?
    if (!fpproc->meshelem.empty())
    {
        int n = lua_gettop(L);
        int group = 0;
        if (n>0)
            group = (int)lua_todouble(L,1);

        for (auto &block: fpproc->blocklist)
        {
            if (group==0 || block.InGroup == group)
                block.ToggleSelect();
            fpproc->bHasMask = false;
        }

        //HDC myDCH=GetDC(theView->m_hWnd);
        //CDC tempDC;
        //CDC *pDC;
        //pDC=tempDC.FromHandle(myDCH);

        //CDC *pDC=GetDC(theView->m_hWnd);
        //theView->OnDraw(pDC);
        //theView->DrawSelected=-1;
        //theView->ReleaseDC(pDC);
        //fpproc->UpdateAllViews(theView);
    }

    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_lineintegral()}
 */
int femmcli::LuaMagneticsCommands::luaLineintegralNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_makeplot()}
 */
int femmcli::LuaMagneticsCommands::luaMakeplotNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_mirror()}
 */
int femmcli::LuaMagneticsCommands::luaMirrorNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_modboundprop()}
 */
int femmcli::LuaMagneticsCommands::luaModboundpropNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Modify the given circuit property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_modcircprop()}
 *
 * \internal
 * mi_modifycircprop("CircName",propnum,value)
 * This function allows for modification of a circuit property.
 *  * propnum:
 *    1. name
 *    2. Total current
 *    3. Circuit type (0=parallel or 1=series)
 */
int femmcli::LuaMagneticsCommands::luaModifyCircuitProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        //return ((CFemmeDoc *)pFemmeDoc)->old_lua_modcircprop(L);
        lua_error(L,"Compatibility mode for mi_modcircprop is not implemented!\n");
        return 0;
    }

    if (doc->circproplist.empty())
        return 0;

    // find the index of the material to modify;
    std::string CircName = lua_tostring(L,1);

    auto searchResult = doc->circuitMap.find(CircName);
    // get out of here if there's no matching circuit
    if (searchResult == doc->circuitMap.end())
    {
        debug << "mi_modcircprop(): No circuit of name " << CircName << "\n";
        return 0;
    }
    int idx = searchResult->second;

    int modprop=(int) lua_todouble(L,2);
    CMCircuit *prop = dynamic_cast<CMCircuit*>(doc->circproplist[idx].get());
    assert(prop);
    // now, modify the specified attribute...
    switch(modprop)
    {
    case 0:
    {
        std::string newName;
        if (!lua_isnil(L,3))
            newName = lua_tostring(L,3);
        prop->CircName = newName;
        break;
    }
    case 1:
        prop->Amps = lua_tonumber(L,3);
        break;
    case 2:
        prop->CircType = (int) lua_todouble(L,3);
        break;
    default:
    {
        std::string msg = "mi_modcircprop(): No property with index " + std::to_string(modprop) + "\n";
        lua_error(L,msg.c_str());
        break;
    }
    }

    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_modmatprop()}
 */
int femmcli::LuaMagneticsCommands::luaModmatpropNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_modpointprop()}
 */
int femmcli::LuaMagneticsCommands::luaModpointpropNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Rotate selected objects around a point by a given angle.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_move_rotate()}
 *
 * \internal
 * mi_moverotate(bx,by,shiftangle,(editaction))
 * * bx, by – base point for rotation
 * * shiftangle – angle in degrees by which the selected objects are rotated.
 * * editaction 0 –nodes, 1 – lines (segments), 2 –block labels, 3 – arc segments, 4- group
 */
int femmcli::LuaMagneticsCommands::luaMoveRotate(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
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

    fmesher::FMesher::EditMode editAction;
    if (n==4) {
        editAction = mesher->intToEditMode((int)lua_todouble(L,4));
    } else {
        editAction = mesher->d_EditMode;
    }
    if (editAction == fmesher::FMesher::EditModeInvalid)
    {
            lua_error(L, "mi_moverotate(): Invalid value of editaction!\n");
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
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_move_translate()}
 *
 * \internal
 * mi_movetranslate(dx,dy,(editaction))
 * * dx,dy – distance by which the selected objects are shifted.
 * * editaction:
 *   * 0: nodes
 *   * 1: lines (segments)
 *   * 2: block labels
 *   * 3: arc segments
 *   * 4: group
 */
int femmcli::LuaMagneticsCommands::luaMoveTranslate(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int n = lua_gettop(L);
    if( n<2 || n>3)
    {
        lua_error(L,"Invalid number of parameters for move translate!\n");
        return 0;
    }

    double x = lua_todouble(L,1);
    double y = lua_todouble(L,2);

    fmesher::FMesher::EditMode editAction;
    if (n==3) {
        editAction = mesher->intToEditMode((int)lua_todouble(L,3));
    } else {
        editAction = mesher->d_EditMode;
    }

    if (editAction == fmesher::FMesher::EditModeInvalid)
    {
        lua_error(L, "mi_movetranslate(): no editmode given and no default edit mode set!\n");
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_newdocument()}
 */
int femmcli::LuaMagneticsCommands::luaNewdocumentNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Return the number of elements in the output mesh.
 * @param L
 * @return 1
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_numelements()}
 *
 * \internal
 * mo_numelements() Returns the number of elements in the in focus magnets output mesh.
 */
int femmcli::LuaMagneticsCommands::luaNumElements(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    auto femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = femmState->getFPProc();
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }
    lua_pushnumber(L,(int) fpproc->meshelem.size());
    return 1;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_numnodes()}
 */
int femmcli::LuaMagneticsCommands::luaNumnodesNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_previous()}
 */
int femmcli::LuaMagneticsCommands::luaPreviousNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Change problem definition.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_prob_def()}
 * \internal
 * mi_probdef(frequency,units,type,precision,(depth),(minangle),(acsolver) changes the problem definition.
 *
 * Parameters:
 *  * frequency: number
 *  * units: "inches", "millimeters", "centimeters", "mils", "meters, and "micrometers"
 *  * type: "planar", "axi"
 *  * precision: in scientific notation (e.g. "1E-8")
 *  * depth: number
 *  * minangle: number
 *  * acsolver: number? ... solver type for AC problems
 *
 * All parameters except the first are optional.
 * Only the parameters that are set are changed.
 */
int femmcli::LuaMagneticsCommands::luaProbDef(lua_State * L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> magDoc = femmState->femmDocument;

    // argument count
    int n;
    n=lua_gettop(L);

    // Frequency
    double frequency = lua_tonumber(L,1).re;
    magDoc->Frequency = std::fabs(frequency);
    if(n==1) return 0;

    // Length Units
    std::string units (lua_tostring(L,2));
    if(units=="inches") magDoc->LengthUnits = LengthInches;
    else if(units=="millimeters") magDoc->LengthUnits = LengthMillimeters;
    else if(units=="centimeters") magDoc->LengthUnits = LengthCentimeters;
    else if(units=="meters") magDoc->LengthUnits = LengthMeters;
    else if(units=="mills") magDoc->LengthUnits = LengthMils;
    else if(units=="mils") magDoc->LengthUnits = LengthMils;
    else if(units=="micrometers") magDoc->LengthUnits = LengthMicrometers;
    else
    {
        std::string msg  = "Unknown length unit " + units;
        lua_error(L,msg.c_str());
        return 0;
    }
    if (n==2) return 0;

    // Problem type
    std::string type (lua_tostring(L,3));
    if(type=="planar") magDoc->ProblemType = PLANAR;
    else if(type=="axi") magDoc->ProblemType = AXISYMMETRIC;
    else
    {
        std::string msg =  "Unknown problem type " + type;
        lua_error(L,msg.c_str());
        return 0;
    }
    if (n==3) return 0;

    double precision = lua_tonumber(L,4).re;
    if (precision < 1.e-16 || precision >1.e-8)
    {
        std::string msg = "Invalid Precision " + std::to_string(precision);
        lua_error(L,msg.c_str());
        return 0;
    }
    magDoc->Precision = precision;
    if (n==4) return 0;

    magDoc->Depth = std::fabs(lua_tonumber(L,5).re);
    if (n==5) return 0;

    double minAngle = lua_tonumber(L,6).re;
    if ((minAngle>=1.) && (minAngle<=33.8))
    {
        magDoc->MinAngle = minAngle;
    }
    if (n==6) return 0;

    int acSolver = (int)lua_tonumber(L,7).re;
    if ((acSolver==0) || (acSolver==1))
    {
        magDoc->ACSolver=acSolver;
    }
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_purge_mesh()}
 */
int femmcli::LuaMagneticsCommands::luaPurgeMeshNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_reload()}
 */
int femmcli::LuaMagneticsCommands::luaReloadNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Load the solution and run the postprocessor on it.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_runpost()}
 *
 * \internal
 * mi_loadsolution()
 * Loads and displays the solution corresponding to the current geometry.
 */
int femmcli::LuaMagneticsCommands::luaLoadSolution(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    if (doc->pathName.empty())
    {
        lua_error(L,"No results to display");
        return 0;
    }

    std::size_t dotpos = doc->pathName.find_last_of(".");
    std::string solutionFile = doc->pathName.substr(0,dotpos);
    solutionFile += ".ans";

    femmState->invalidateSolutionData();
    auto fpproc = femmState->getFPProc();
    if (!fpproc->OpenDocument(solutionFile))
    {
        std::string msg = "mi_loadsolution(): error while loading solution file:\n";
        msg += solutionFile;
        lua_error(L, msg.c_str());
    }
    return 0;
}

/**
 * @brief Save the problem description into the given file.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,luaSaveDocument()}
 *
 * \internal
 * mi_saveas("filename")
 * Saves the file with name "filename".
 */
int femmcli::LuaMagneticsCommands::luaSaveDocument(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    if (!lua_isnil(L,1))
    {
        doc->pathName = lua_tostring(L,1);
        doc->saveFEMFile(doc->pathName);
    } else {
        lua_error(L, "mi_saveas(): no pathname given!");
    }

    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_scale()}
 */
int femmcli::LuaMagneticsCommands::luaScaleNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Select an arc segment near a given point.
 * @param L
 * @return 4 on success, 0 otherwise
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectarcsegment()}
 *
 * \internal
 * mi_selectarcsegment(x,y)
 * Select the arc segment closest to (x,y)
 */
int femmcli::LuaMagneticsCommands::luaSelectArcsegment(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_selectblock()}
 */
int femmcli::LuaMagneticsCommands::luaSelectBlockNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectcircle()}
 */
int femmcli::LuaMagneticsCommands::luaSelectcircleNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Select the given group of nodes, segments, arc segments and blocklabels.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectgroup()}
 *
 * \internal
 * mi_selectgroup(n)
 * Select the nth group of nodes, segments, arc segments and blocklabels.
 * This function will clear all previously selected elements and leave the editmode in 4 (group)
 */
int femmcli::LuaMagneticsCommands::luaSelectGroup(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    int group=(int) lua_todouble(L,1);

    if(group<0)
    {
        std::string msg = "Invalid group " + std::to_string(group);
        lua_error(L,msg.c_str());
        return 0;
    }


    // Note(ZaJ) this is also disabled in femm42:
    // doc->UnselectAll();

    // select nodes
    for (int i=0; i<(int)doc->nodelist.size(); i++)
    {
        if(doc->nodelist[i]->InGroup==group)
            doc->nodelist[i]->IsSelected=true;
    }

    // select segments
    for(int i=0; i<(int)doc->linelist.size(); i++)
    {
        if(doc->linelist[i]->InGroup==group)
            doc->linelist[i]->IsSelected=true;
    }

    // select arc segments
    for(int i=0; i<(int)doc->arclist.size(); i++)
    {
        if(doc->arclist[i]->InGroup==group)
            doc->arclist[i]->IsSelected=true;
    }

    // select blocks
    for(int i=0; i<(int)doc->labellist.size(); i++)
    {
        if(doc->labellist[i]->InGroup==group)
            doc->labellist[i]->IsSelected=true;
    }

    // set default edit mode
    mesher->d_EditMode = fmesher::FMesher::EditGroup;

    return 0;
}

/**
 * @brief Select the closest label to a given point.
 * @param L
 * @return 0 on error, 2 otherwise.
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectlabel()}
 *
 * \internal
 * mi_selectlabel(x,y)
 * Select the label closet to (x,y). Returns the coordinates of the selected label.
 */
int femmcli::LuaMagneticsCommands::luaSelectBlocklabel(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_selectline()}
 */
int femmcli::LuaMagneticsCommands::luaSelectlineNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Select the nearest node to given coordinates.
 * @param L
 * @return 0 on error, 2 on success
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectnode()}
 *
 * \internal
 * mi_selectnode(x,y) Select the node closest to (x,y).
 * Returns the coordinates of the selected node.
 */
int femmcli::LuaMagneticsCommands::luaSelectnode(lua_State *L)
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectrectangle()}
 */
int femmcli::LuaMagneticsCommands::luaSelectrectangleNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Select the line closest to a given point.
 * @param L
 * @return 0 on error, 4 on success
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectsegment()}
 *
 * \internal
 * mi_selectsegment(x,y)
 * Select the line segment closest to (x,y)
 */
int femmcli::LuaMagneticsCommands::luaSelectSegment(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> thisDoc = femmState->femmDocument;

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
 * @brief Set properties for selected arc segments
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setarcsegmentprop()}
 *
 * \internal
 * mi_setarcsegmentprop(maxsegdeg, "propname", hide, group)
 *
 * Note: propname may be 0 (as in number 0, not string "0").
 *       In that case, the arc segments have no boundary property.
 */
int femmcli::LuaMagneticsCommands::luaSetArcsegmentProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    double maxsegdeg = lua_todouble(L,1);
    std::string boundprop;
    int boundpropidx = -1;
    if (!lua_isnil(L,2))
    {
        boundprop = lua_tostring(L,2);
        if (doc->nodeMap.count(boundprop))
            boundpropidx = doc->nodeMap[boundprop];
    }
    bool hide = (lua_todouble(L,3)!=0);
    int group = (int) lua_todouble(L,4);

    for (int i=0; i<(int)doc->arclist.size(); i++)
    {
        if (doc->arclist[i]->IsSelected)
        {
            doc->arclist[i]->BoundaryMarker = boundpropidx;
            doc->arclist[i]->BoundaryMarkerName = boundprop;
            doc->arclist[i]->MaxSideLength = maxsegdeg;
            doc->arclist[i]->Hidden = hide;
            doc->arclist[i]->InGroup = group;
        }
    }

    return 0;
}

/**
 * @brief Set properties for selected block labels
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setblockprop()}
 *
 * \internal
 * mi_setblockprop("blockname", automesh, meshsize, "incircuit", magdirection, group, turns)
 * Set the selected block labels to have the properties
 *
 * Note: blockname and/or incircuit may be 0 (as in number 0, not string "0").
 *       In that case, the block labels have no block type and/or are not in a circuit.
 *
 * magdirection can either be a number, or a string containing a lua expression.
 */
int femmcli::LuaMagneticsCommands::luaSetBlocklabelProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    // default values
    int blocktypeidx = -1;
    std::string blocktype = "<None>";
    bool automesh = true;
    double meshsize = 0;
    int incircuitidx = -1;
    std::string incircuit = "<None>";
    double magdirection = 0;
    std::string magdirfctn;
    int group = 0;
    int turns = 1;

    int n=lua_gettop(L);

    if (n>0 && !lua_isnil(L,1))
    {
        blocktype = lua_tostring(L,1);
        if (doc->blockMap.count(blocktype))
            blocktypeidx = doc->blockMap[blocktype];
    }
    if (n>1) automesh = (lua_todouble(L,2) != 0);
    if (n>2) meshsize = lua_todouble(L,3);
    if (n>3 && !lua_isnil(L,4))
    {
        incircuit = lua_tostring(L,4);
        if (doc->circuitMap.count(incircuit))
            incircuitidx = doc->circuitMap[incircuit];
    }
    if (n>4)
    {
        if (lua_isnumber(L,5))
            magdirection = lua_todouble(L,5);
        else if (!lua_isnil(L,5))
            magdirfctn = lua_tostring(L,5);
    }
    if (n>5) group = (int) lua_todouble(L,6);
    if (n>6)
    {
        turns = (int) lua_todouble(L,7);
        if (turns==0) turns = 1;
    }

    for (int i=0; i<(int) doc->labellist.size(); i++)
    {
        if (doc->labellist[i]->IsSelected)
        {
            doc->labellist[i]->MaxArea = PI*meshsize*meshsize/4.;
            doc->labellist[i]->MagDir = magdirection;
            doc->labellist[i]->BlockTypeName = blocktype;
            doc->labellist[i]->BlockType = blocktypeidx;
            doc->labellist[i]->InCircuitName = incircuit;
            doc->labellist[i]->InCircuit = incircuitidx;
            doc->labellist[i]->InGroup = group;
            doc->labellist[i]->Turns = turns;
            doc->labellist[i]->MagDirFctn = magdirfctn;
            if(automesh)
                doc->labellist[i]->MaxArea = 0;
        }
    }

    return 0;
}

/**
 * @brief Set the default mesher EditMode.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_seteditmode()}
 *
 * \internal
 * mi_seteditmode(editmode)
 */
int femmcli::LuaMagneticsCommands::luaSetEditMode(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<fmesher::FMesher> mesher = femmState->getMesher();

    fmesher::FMesher::EditMode mode;
    std::string modeString (lua_tostring(L,1));
    if (modeString == "nodes")
        mode = fmesher::FMesher::EditNodes;
    else if (modeString == "segments")
        mode = fmesher::FMesher::EditLines;
    else if (modeString == "blocks")
        mode = fmesher::FMesher::EditLabels;
    else if (modeString == "arcsegments")
        mode = fmesher::FMesher::EditArcs;
    else if (modeString == "group")
        mode = fmesher::FMesher::EditGroup;
    else {
        lua_error(L, "mi_seteditmode(): Invalid value of editmode!\n");
        return 0;
    }

    mesher->d_EditMode = mode;
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setgroup()}
 */
int femmcli::LuaMagneticsCommands::luaSetgroupNOP(lua_State *L)
{
    lua_error(L, "Not implemented.");
    return 0;
}

/**
 * @brief Set the nodal property for selected nodes.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setnodeprop()}
 *
 * \internal
 * mi_setnodeprop("propname",groupno)
 * Set the selected nodes to have the nodal property mi_"propname" and group number groupno.
 *
 * Note: propname may be 0 (as in number 0, not string "0").
 *       In that case, the arc segments have no boundary property.
 */
int femmcli::LuaMagneticsCommands::luaSetNodeProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    int nodepropidx = -1;
    std::string nodeprop = "<None>";
    if (!lua_isnil(L,1))
    {
        nodeprop = lua_tostring(L,1);
        if (doc->nodeMap.count(nodeprop))
            nodepropidx = doc->nodeMap[nodeprop];
    }
    int groupno=(int) lua_todouble(L,2);

    if (groupno<0)
    {
        std::string msg = "Invalid group no " + std::to_string(groupno);
        lua_error(L,msg.c_str());
        return 0;
    }

    // check to see how many (if any) nodes are selected.
    for(int i=0; i<(int)doc->nodelist.size(); i++)
    {
        if(doc->nodelist[i]->IsSelected)
        {
            doc->nodelist[i]->InGroup = groupno;
            doc->nodelist[i]->BoundaryMarker = nodepropidx;
            doc->nodelist[i]->BoundaryMarkerName = nodeprop;
        }
    }

    return 0;
}

/**
 * @brief Set properties for the selected segments.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setsegmentprop()}
 *
 * \internal
 * mi_setsegmentprop("propname", elementsize, automesh, hide, group)
 * Set the selected segments to have:
 * * Boundary property "propname"
 *   Note: propname may be 0 (as in number 0, not string "0").
 *   In that case, the arc segments have no boundary property.
 * * Local element size along segment no greater than elementsize
 * * automesh:
 *   0 = mesher defers to the element constraint defined by elementsize,
 *   1 = mesher automatically chooses mesh size along the selected segments
 * * hide: 0 = not hidden in post-processor, 1 == hidden in post processor
 * * A member of group number group
 */
int femmcli::LuaMagneticsCommands::luaSetSegmentProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument;

    int nodepropidx = -1;
    std::string nodeprop = "<None>";
    if (!lua_isnil(L,1))
    {
        nodeprop = lua_tostring(L,1);
        if (doc->nodeMap.count(nodeprop))
            nodepropidx = doc->nodeMap[nodeprop];
    }
    double elesize = lua_todouble(L,2);
    bool automesh = (lua_todouble(L,3) != 0);
    bool hide = (lua_todouble(L,4) != 0);
    int group = (int) lua_todouble(L,5);

    for (int i=0; i<(int)doc->linelist.size(); i++)
    {
        if (doc->linelist[i]->IsSelected)
        {
            if (automesh)
                doc->linelist[i]->MaxSideLength = -1;
            else{
                if (elesize>0)
                    doc->linelist[i]->MaxSideLength = elesize;
                else elesize = -1;
            }
            doc->linelist[i]->BoundaryMarker = nodepropidx;
            doc->linelist[i]->BoundaryMarkerName = nodeprop;
            doc->linelist[i]->Hidden = hide;
            doc->linelist[i]->InGroup = group;
        }
    }

    return 0;
}


// vi:expandtab:tabstop=4 shiftwidth=4:
