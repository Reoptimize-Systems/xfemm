/* Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
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

#include "LuaMagneticsCommands.h"
#include "LuaCommonCommands.h"

#include "CMPointVals.h"
#include "femmconstants.h"
#include "femmenums.h"
#include "FemmState.h"
#include "fpproc.h"
#include "LuaInstance.h"
#include "MatlibReader.h"
#include "stringTools.h"
#include "make_unique.h"

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

void femmcli::LuaMagneticsCommands::registerCommands(LuaInstance &li)
{
    li.addFunction("mi_add_arc", LuaCommonCommands::luaAddArc);
    li.addFunction("mi_addarc", LuaCommonCommands::luaAddArc);
    li.addFunction("mi_add_bh_point", luaAddBHPoint);
    li.addFunction("mi_addbhpoint", luaAddBHPoint);
    li.addFunction("mi_add_bound_prop", luaAddBoundaryProperty);
    li.addFunction("mi_addboundprop", luaAddBoundaryProperty);
    li.addFunction("mi_add_circ_prop", luaAddCircuitProperty);
    li.addFunction("mi_addcircprop", luaAddCircuitProperty);
    li.addFunction("mo_add_contour", luaAddContourPoint);
    li.addFunction("mo_addcontour", luaAddContourPoint);
    li.addFunction("mi_add_block_label", LuaCommonCommands::luaAddBlocklabel);
    li.addFunction("mi_addblocklabel", LuaCommonCommands::luaAddBlocklabel);
    li.addFunction("mi_add_segment", LuaCommonCommands::luaAddLine);
    li.addFunction("mi_addsegment", LuaCommonCommands::luaAddLine);
    li.addFunction("mi_add_material", luaAddMatProperty);
    li.addFunction("mi_addmaterial", luaAddMatProperty);
    li.addFunction("mi_add_node", LuaCommonCommands::luaAddNode);
    li.addFunction("mi_addnode", LuaCommonCommands::luaAddNode);
    li.addFunction("mi_add_point_prop", luaAddPointProperty);
    li.addFunction("mi_addpointprop", luaAddPointProperty);
    li.addFunction("mi_analyse", luaAnalyze);
    li.addFunction("mi_analyze", luaAnalyze);
    li.addFunction("mi_attach_default", LuaCommonCommands::luaAttachDefault);
    li.addFunction("mi_attachdefault", LuaCommonCommands::luaAttachDefault);
    li.addFunction("mi_attach_outer_space", LuaCommonCommands::luaAttachOuterSpace);
    li.addFunction("mi_attachouterspace", LuaCommonCommands::luaAttachOuterSpace);
    li.addFunction("mo_bend_contour", luaBendContourLine);
    li.addFunction("mo_bendcontour", luaBendContourLine);
    li.addFunction("mo_block_integral", luaBlockIntegral);
    li.addFunction("mo_blockintegral", luaBlockIntegral);
    li.addFunction("mi_clear_bh_points", luaClearBHPoints);
    li.addFunction("mi_clearbhpoints", luaClearBHPoints);
    li.addFunction("mo_clear_block", luaClearBlock);
    li.addFunction("mo_clearblock", luaClearBlock);
    li.addFunction("mo_clear_contour", luaClearContourPoint);
    li.addFunction("mo_clearcontour", luaClearContourPoint);
    li.addFunction("mi_clear_selected", LuaCommonCommands::luaClearSelected);
    li.addFunction("mi_clearselected", LuaCommonCommands::luaClearSelected);
    li.addFunction("mi_copy_rotate", LuaCommonCommands::luaCopyRotate);
    li.addFunction("mi_copyrotate", LuaCommonCommands::luaCopyRotate);
    li.addFunction("mi_copy_translate", LuaCommonCommands::luaCopyTranslate);
    li.addFunction("mi_copytranslate", LuaCommonCommands::luaCopyTranslate);
    li.addFunction("mi_create_mesh", LuaCommonCommands::luaCreateMesh);
    li.addFunction("mi_createmesh", LuaCommonCommands::luaCreateMesh);
    li.addFunction("mi_create_radius", LuaCommonCommands::luaCreateRadius);
    li.addFunction("mi_createradius", LuaCommonCommands::luaCreateRadius);
    li.addFunction("mi_define_outer_space", LuaCommonCommands::luaDefineOuterSpace);
    li.addFunction("mi_defineouterspace", LuaCommonCommands::luaDefineOuterSpace);
    li.addFunction("mi_delete_bound_prop", LuaCommonCommands::luaDeleteBoundaryProperty);
    li.addFunction("mi_deleteboundprop", LuaCommonCommands::luaDeleteBoundaryProperty);
    li.addFunction("mi_delete_circuit", LuaCommonCommands::luaDeleteCircuitProperty);
    li.addFunction("mi_deletecircuit", LuaCommonCommands::luaDeleteCircuitProperty);
    li.addFunction("mi_delete_selected_arcsegments", LuaCommonCommands::luaDeleteSelectedArcSegments);
    li.addFunction("mi_deleteselectedarcsegments", LuaCommonCommands::luaDeleteSelectedArcSegments);
    li.addFunction("mi_delete_selected_labels", LuaCommonCommands::luaDeleteSelectedBlockLabels);
    li.addFunction("mi_deleteselectedlabels", LuaCommonCommands::luaDeleteSelectedBlockLabels);
    li.addFunction("mi_delete_selected", LuaCommonCommands::luaDeleteSelected);
    li.addFunction("mi_deleteselected", LuaCommonCommands::luaDeleteSelected);
    li.addFunction("mi_delete_selected_nodes", LuaCommonCommands::luaDeleteSelectedNodes);
    li.addFunction("mi_deleteselectednodes", LuaCommonCommands::luaDeleteSelectedNodes);
    li.addFunction("mi_delete_selected_segments", LuaCommonCommands::luaDeleteSelectedSegments);
    li.addFunction("mi_deleteselectedsegments", LuaCommonCommands::luaDeleteSelectedSegments);
    li.addFunction("mi_delete_material", LuaCommonCommands::luaDeleteMaterial);
    li.addFunction("mi_deletematerial", LuaCommonCommands::luaDeleteMaterial);
    li.addFunction("mi_delete_point_prop", LuaCommonCommands::luaDeletePointProperty);
    li.addFunction("mi_deletepointprop", LuaCommonCommands::luaDeletePointProperty);
    li.addFunction("mi_detach_default", LuaCommonCommands::luaDetachDefault);
    li.addFunction("mi_detachdefault", LuaCommonCommands::luaDetachDefault);
    li.addFunction("mi_detach_outer_space", LuaCommonCommands::luaDetachOuterSpace);
    li.addFunction("mi_detachouterspace", LuaCommonCommands::luaDetachOuterSpace);
    li.addFunction("mo_close", LuaCommonCommands::luaExitPost);
    li.addFunction("mi_close", LuaCommonCommands::luaExitPre);
    li.addFunction("mi_getboundingbox", LuaCommonCommands::luaGetBoundingBox);
    li.addFunction("mo_get_circuit_properties", luaGetCircuitProperties);
    li.addFunction("mo_getcircuitproperties", luaGetCircuitProperties);
    li.addFunction("mo_get_element", luaGetElement);
    li.addFunction("mo_getelement", luaGetElement);
    li.addFunction("mi_get_material", luaGetMaterialFromLib);
    li.addFunction("mi_getmaterial", luaGetMaterialFromLib);
    li.addFunction("mo_get_node", luaGetMeshNode);
    li.addFunction("mo_getnode", luaGetMeshNode);
    li.addFunction("mo_get_point_values", luaGetPointValues);
    li.addFunction("mo_getpointvalues", luaGetPointValues);
    li.addFunction("mi_getprobleminfo", LuaCommonCommands::luaGetProblemInfo);
    li.addFunction("mo_get_problem_info", LuaCommonCommands::luaGetProblemInfo);
    li.addFunction("mo_getprobleminfo", LuaCommonCommands::luaGetProblemInfo);
    li.addFunction("mi_get_title", LuaCommonCommands::luaGetTitle);
    li.addFunction("mi_gettitle", LuaCommonCommands::luaGetTitle);
    li.addFunction("mo_get_title", LuaCommonCommands::luaGetTitle);
    li.addFunction("mo_gettitle", LuaCommonCommands::luaGetTitle);
    li.addFunction("mo_gradient", luaBGradient);
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
    li.addFunction("mo_line_integral", luaLineIntegral);
    li.addFunction("mo_lineintegral", luaLineIntegral);
    li.addFunction("mo_make_plot", LuaInstance::luaNOP);
    li.addFunction("mo_makeplot", LuaInstance::luaNOP);
    li.addFunction("mi_maximize", LuaInstance::luaNOP);
    li.addFunction("mo_maximize", LuaInstance::luaNOP);
    li.addFunction("mi_minimize", LuaInstance::luaNOP);
    li.addFunction("mo_minimize", LuaInstance::luaNOP);
    li.addFunction("mi_mirror", LuaCommonCommands::luaMirrorCopy);
    li.addFunction("mi_modify_bound_prop", luaModifyBoundaryProperty);
    li.addFunction("mi_modifyboundprop", luaModifyBoundaryProperty);
    li.addFunction("mi_modify_circ_prop", luaModifyCircuitProperty);
    li.addFunction("mi_modifycircprop", luaModifyCircuitProperty);
    li.addFunction("mi_modify_material", luaModifyMaterialProperty);
    li.addFunction("mi_modifymaterial", luaModifyMaterialProperty);
    li.addFunction("mi_modify_point_prop", luaModifyPointProperty);
    li.addFunction("mi_modifypointprop", luaModifyPointProperty);
    li.addFunction("mi_move_rotate", LuaCommonCommands::luaMoveRotate);
    li.addFunction("mi_moverotate", LuaCommonCommands::luaMoveRotate);
    li.addFunction("mi_move_translate", LuaCommonCommands::luaMoveTranslate);
    li.addFunction("mi_movetranslate", LuaCommonCommands::luaMoveTranslate);
    li.addFunction("mi_new_document", luaNewDocument);
    li.addFunction("mi_newdocument", luaNewDocument);
    li.addFunction("mo_num_elements", LuaCommonCommands::luaNumElements);
    li.addFunction("mo_numelements", LuaCommonCommands::luaNumElements);
    li.addFunction("mo_num_nodes", LuaCommonCommands::luaNumNodes);
    li.addFunction("mo_numnodes", LuaCommonCommands::luaNumNodes);
    li.addFunction("mi_setprevious", luaSetPrevious);
    li.addFunction("mi_prob_def", luaProblemDefinition);
    li.addFunction("mi_probdef", luaProblemDefinition);
    li.addFunction("mi_purge_mesh", LuaCommonCommands::luaPurgeMesh);
    li.addFunction("mi_purgemesh", LuaCommonCommands::luaPurgeMesh);
    li.addFunction("mi_read_dxf", LuaInstance::luaNOP);
    li.addFunction("mi_readdxf", LuaInstance::luaNOP);
    li.addFunction("mo_refresh_view", LuaInstance::luaNOP);
    li.addFunction("mo_refreshview", LuaInstance::luaNOP);
    li.addFunction("mo_reload", LuaCommonCommands::luaLoadSolution);
    li.addFunction("mi_resize", LuaInstance::luaNOP);
    li.addFunction("mo_resize", LuaInstance::luaNOP);
    li.addFunction("mi_restore", LuaInstance::luaNOP);
    li.addFunction("mo_restore", LuaInstance::luaNOP);
    li.addFunction("mi_load_solution", LuaCommonCommands::luaLoadSolution);
    li.addFunction("mi_loadsolution", LuaCommonCommands::luaLoadSolution);
    li.addFunction("mi_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("mi_savebitmap", LuaInstance::luaNOP);
    li.addFunction("mo_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("mo_savebitmap", LuaInstance::luaNOP);
    li.addFunction("mi_save_as", LuaCommonCommands::luaSaveDocument);
    li.addFunction("mi_saveas", LuaCommonCommands::luaSaveDocument);
    li.addFunction("mi_save_dxf", LuaInstance::luaNOP);
    li.addFunction("mi_savedxf", LuaInstance::luaNOP);
    li.addFunction("mi_save_metafile", LuaInstance::luaNOP);
    li.addFunction("mi_savemetafile", LuaInstance::luaNOP);
    li.addFunction("mo_save_metafile", LuaInstance::luaNOP);
    li.addFunction("mo_savemetafile", LuaInstance::luaNOP);
    li.addFunction("mi_scale", LuaCommonCommands::luaScaleMove);
    li.addFunction("mi_select_arcsegment", LuaCommonCommands::luaSelectArcsegment);
    li.addFunction("mi_selectarcsegment", LuaCommonCommands::luaSelectArcsegment);
    li.addFunction("mo_select_block", luaSelectOutputBlocklabel);
    li.addFunction("mo_selectblock", luaSelectOutputBlocklabel);
    li.addFunction("mi_select_circle", LuaCommonCommands::luaSelectWithinCircle);
    li.addFunction("mi_selectcircle", LuaCommonCommands::luaSelectWithinCircle);
    li.addFunction("mi_select_group", LuaCommonCommands::luaSelectGroup);
    li.addFunction("mi_selectgroup", LuaCommonCommands::luaSelectGroup);
    li.addFunction("mi_select_label", LuaCommonCommands::luaSelectBlocklabel);
    li.addFunction("mi_selectlabel", LuaCommonCommands::luaSelectBlocklabel);
    li.addFunction("mo_select_point", luaAddContourPointFromNode);
    li.addFunction("mo_selectpoint", luaAddContourPointFromNode);
    li.addFunction("mi_select_node", LuaCommonCommands::luaSelectNode);
    li.addFunction("mi_selectnode", LuaCommonCommands::luaSelectNode);
    li.addFunction("mi_select_rectangle", LuaCommonCommands::luaSelectWithinRectangle);
    li.addFunction("mi_selectrectangle", LuaCommonCommands::luaSelectWithinRectangle);
    li.addFunction("mi_select_segment", LuaCommonCommands::luaSelectSegment);
    li.addFunction("mi_selectsegment", LuaCommonCommands::luaSelectSegment);
    li.addFunction("mi_set_arcsegment_prop", luaSetArcsegmentProperty);
    li.addFunction("mi_setarcsegmentprop", luaSetArcsegmentProperty);
    li.addFunction("mi_set_block_prop", luaSetBlocklabelProperty);
    li.addFunction("mi_setblockprop", luaSetBlocklabelProperty);
    li.addFunction("mi_set_edit_mode", LuaCommonCommands::luaSetEditMode);
    li.addFunction("mi_seteditmode", LuaCommonCommands::luaSetEditMode);
    li.addFunction("mo_set_edit_mode", LuaInstance::luaNOP);
    li.addFunction("mo_seteditmode", LuaInstance::luaNOP);
    li.addFunction("mi_set_grid", LuaInstance::luaNOP);
    li.addFunction("mi_setgrid", LuaInstance::luaNOP);
    li.addFunction("mo_set_grid", LuaInstance::luaNOP);
    li.addFunction("mo_setgrid", LuaInstance::luaNOP);
    li.addFunction("mi_set_group", LuaCommonCommands::luaSetGroup);
    li.addFunction("mi_setgroup", LuaCommonCommands::luaSetGroup);
    li.addFunction("mi_set_node_prop", luaSetNodeProperty);
    li.addFunction("mi_setnodeprop", luaSetNodeProperty);
    li.addFunction("mi_set_segment_prop", luaSetSegmentProperty);
    li.addFunction("mi_setsegmentprop", luaSetSegmentProperty);
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
    li.addFunction("mo_smooth", luaSetSmoothing);
    li.addFunction("mi_set_focus", LuaCommonCommands::luaSetFocus);
    li.addFunction("mi_setfocus", LuaCommonCommands::luaSetFocus);
    li.addFunction("mo_set_focus", LuaCommonCommands::luaSetFocus);
    li.addFunction("mo_setfocus", LuaCommonCommands::luaSetFocus);
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
 * @brief Add a B-H data point to the given material.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_addbhpoint("blockname",b,h)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_addbhpoint()}
 *
 * \remark Note(ZaJ): xfemm has Bdata and Hdata in CMMaterialProp,
 * while femm42 has two forms of CMaterialProp: one with Bdata and Hdata, and
 * one with a single BHdata.
 * I didn't want to introduce the second form into xfemm, so I tried mapping the BHdata
 * in this function to Bdata+Hdata. Ideally, we'd have a single form BHdata in xfemm, though.
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaAddBHPoint(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

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
                swap(blockprop->Bdata[j],blockprop->Bdata[j+1]);
                swap(blockprop->Hdata[j],blockprop->Hdata[j+1]);
            }
        }
        if (!swapped) break;
    }
    return 0;
}

/**
 * @brief Add a new boundary property with a given name.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_addboundprop("propname", A0, A1, A2, Phi, Mu, Sig, c0, c1, BdryFormat)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_addboundprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaAddBoundaryProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    std::unique_ptr<CMBoundaryProp> m = MAKE_UNIQUE<CMBoundaryProp>();

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
 *
 * \internal
 * ### Implements:
 * - \lua{mi_addcircprop("circuitname", i, circuittype)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_addcircuitprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaAddCircuitProperty(lua_State *L)
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

    std::unique_ptr<CMCircuit> m = MAKE_UNIQUE<CMCircuit>();
    int n=lua_gettop(L);

    if (n>0) m->CircName=lua_tostring(L,1);
    if (n>1) m->Amps=lua_tonumber(L,2);
    if (n>2) m->CircType=(int) lua_todouble(L,3);

    femmState->femmDocument()->circproplist.push_back(std::move(m));
    femmState->femmDocument()->updateCircuitMap();

    return 0;
}

/**
 * @brief Add a contour point.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_addcontour(x,y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_addcontour()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaAddContourPoint(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    CComplex z(lua_todouble(L,1),lua_todouble(L,2));

    if (!fpproc->contour.empty())
    {
        // Note(ZaJ): this seems like a rather arbitrary check:
        if (z != fpproc->contour.back())
            fpproc->contour.push_back(z);
    } else
        fpproc->contour.push_back(z);

    //theView->DrawUserContour(FALSE);

    return 0;
}

/**
 * @brief Add a new material property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_addmaterial("materialname", mu x, mu y, H c, J, Cduct, Lam d, Phi hmax, lam fill, LamType, Phi hx, Phi hy, NStrands, WireD)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_addmatprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaAddMatProperty(lua_State *L)
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

    std::unique_ptr<CMSolverMaterialProp> m = MAKE_UNIQUE<CMSolverMaterialProp>();
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

    femmState->femmDocument()->blockproplist.push_back(std::move(m));
    femmState->femmDocument()->updateBlockMap();
    return 0;
}

/**
 * @brief Add a new point property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_addpointprop("pointpropname",a,j)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_addpointprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaAddPointProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        lua_error(L,"Compatibility mode for mi_addpointprop is not implemented!\n");
        //return ((CFemmeDoc *)pFemmeDoc)->old_lua_addpointprop(L);
        return 0;
    }

    std::unique_ptr<CMPointProp> m = MAKE_UNIQUE<CMPointProp>();
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
 * If the global variable "XFEMM_VERBOSE" is set to 1, the mesher and solver is more verbose and prints statistics.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_analyze(flag)}
 *   Parameter flag (0,1) determines visibility of fkern window and is ignored on xfemm.
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_analyze()}
 *
 * #### Additional source:
 * - \femm42{femm/femmeLua.cpp,lua_analyze()}: extracts thisDoc (=mesherDoc) and the accompanying FemmeViewDoc, calls CFemmeView::lnu_analyze(flag)
 * - \femm42{femm/FemmeView.cpp,CFemmeView::OnMenuAnalyze()}: does the things we do here directly...
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaAnalyze(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> doc = femmState->femmDocument();

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


    if (doc->problemType==AXISYMMETRIC)
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
    // allow setting verbosity from lua:
    const bool verbose = (luaInstance->getGlobal("XFEMM_VERBOSE") != 0);
    mesherDoc->Verbose = verbose;
    if (mesherDoc->HasPeriodicBC()){
        if (mesherDoc->DoPeriodicBCTriangulation(pathName) != 0)
        {
            //EndWaitCursor();
            mesherDoc->problem->unselectAll();
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
    // not supported yet, but set the previous solution so that we can detect this case afterwards:
    theFSolver.previousSolutionFile = doc->PrevSoln;
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
    // the solver may create additional circprops upon loading:
    assert( doc->circproplist.size() <= theFSolver.circproplist.size());
    // holes are not read by the solver, which means that the solver may have fewer blocklabels:
    assert( doc->labellist.size() >= theFSolver.labellist.size());
    if (!theFSolver.runSolver(verbose))
    {
        lua_error(L, "solver failed.");
    }
    return 0;
}

/**
 * @brief Bend the end of the contour line.
 * Replaces the straight line formed by the last two
 * points in the contour by an arc that spans angle degrees. The arc is actually composed
 * of many straight lines, each of which is constrained to span no more than anglestep
 * degrees.
 *
 * The angle parameter can take on values from -180 to 180 degrees.
 * The anglestep parameter must be greater than zero.
 * If there are less than two points defined in the contour, this command is ignored.
 *
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_bendcontour(angle,anglestep)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_bendcontour()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaBendContourLine(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    fpproc->BendContour(lua_todouble(L,1),lua_todouble(L,2));
    return 0;
}

/**
 * @brief Calculate a block integral for the selected blocks.
 * @param L
 * @return 1 on success, 0 otherwise
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_blockintegral(type)}
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_blockintegral()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaBlockIntegral(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
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
    if((type<0) || (type>24))
    {
        lua_error(L, "Invalid block integral type selected");
        return 0;
    }

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

    if ((type>=18) && (type<=23))
    {
        fpproc->MakeMask();
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
 *
 * \internal
 * ### Implements:
 * - \lua{mi_clearbhpoints("blockname")}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_clearbhpoints()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaClearBHPoints(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

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
 * @brief Clear output block label selection and reset some (fpproc) settings.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_clearblock()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_clearblock()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaClearBlock(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
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

    //fpproc->d_EditMode = EditNodes;
    return 0;
}

/**
 * @brief Clear the contour line.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_clearcontour()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_clearcontour()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaClearContourPoint(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    //theView->EraseUserContour(TRUE);
    fpproc->contour.clear();

    return 0;
}

/**
 * @brief Get information about a circuit property.
 * Used primarily to obtain impedance information associated with circuit properties.
 *
 * Properties are returned for the circuit property named "circuit".
 * Three values are returned by the function:
 * 1. current Current carried by the circuit
 * 2. volts Voltage drop across the circuit
 * 3. flux_re Circuit’s flux linkage
 * @param L
 * @return 0 on error, 3 on success.
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_getcircuitproperties("circuit")}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_getcircuitprops()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaGetCircuitProperties(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
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
 * MOGetElement[n] returns the following proprerties for the nth element:
 * 1. Index of first element node
 * 2. Index of second element node
 * 3. Index of third element node
 * 4. x (or r) coordinate of the element centroid
 * 5. y (or z) coordinate of the element centroid
 * 6. element area using the length unit defined for the problem
 * 7. group number associated with the element
 * @param L
 * @return 0 on invalid index, 7 otherwise.
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_getelement(n)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_getelement()}
 *
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaGetElement(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    auto femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    // Note: in lua code, indices start at 1.
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
 * @brief Read the file matlib.dat and extract a named material property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_getmaterial("materialname")}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_getmaterial()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaGetMaterialFromLib(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    int n=lua_gettop(L);
    std::string matname;
    if (n>0)
        matname=lua_tostring(L,1);
    else
        return 0;

    std::string matlib = luaInstance->getBaseDir() + "matlib.dat";

    MatlibReader reader( femm::FileType::MagneticsFile );
    std::stringstream err;
    if ( reader.parse(matlib, err, matname) == MatlibParseResult::OK )
    {
        CMaterialProp *prop;
        prop = reader.takeMaterial(matname);
        if (prop != nullptr)
        {
            doc->blockproplist.push_back(std::unique_ptr<CMaterialProp>(prop));
            doc->updateBlockMap();
            return 0;
        }
    }
    std::string msg = "Couldn't load \"" + matname + "\" from the materials library\n";
    msg.append(err.str());
    lua_error(L, msg.c_str());
    return 0;
}

/**
 * @brief Get position of a mesh node.
 * @param L
 * @return 2 if the node was found, 0 otherwise
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_getnode(n)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_getnode()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaGetMeshNode(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    auto femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());

    int idx = (int)lua_todouble(L,1);
    idx--; // convert to 0-based indexing

    if (idx<0 || idx>=(int)fpproc->meshnode.size())
        return 0;

    lua_pushnumber(L, fpproc->meshnode[idx].x);
    lua_pushnumber(L, fpproc->meshnode[idx].y);
    return 2;
}

/**
 * @brief Get the values for a point.
 * @param L
 * @return 0 on error, otherwise 14
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_getpointvalues(X,Y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_getpointvals()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaGetPointValues(lua_State *L)
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
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    double px,py;
    px=lua_tonumber(L,1).re;
    py=lua_tonumber(L,2).re;

    CMPointVals u;

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
 * @brief Compute the gradients of the B field.
 *
 * Femm42 source documentation:
 * Computes the gradients of the B field by differentiating
 * the shape functions that are used to represent the smoothed
 * B in an element.
 * @param L
 * @return 8
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_gradient(xo,yo)}<br> \b undocumented in manual42.
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_gradient()}
 *
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaBGradient(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    double xo=lua_todouble(L,1);
    double yo=lua_todouble(L,2);

    CComplex dbxdx = 0;
    CComplex dbxdy = 0;
    CComplex dbydx = 0;
    CComplex dbydy = 0;

    for(int i=0; i<(int)fpproc->meshelem.size(); i++)
    {
        CComplex Mx;
        CComplex My;
        fpproc->GetMagnetization(i,Mx,My);

        double da = muo*fpproc->ElmArea(i)/fpproc->LengthConv[fpproc->LengthUnits];
        double x = Re(fpproc->meshelem[i].ctr);
        double y = Im(fpproc->meshelem[i].ctr);

        double p0 = PI*pow(pow(x - xo,2.) + pow(y - yo,2.),3.);
        double p1 = (-3.*pow(x - xo,2.) + pow(y - yo,2.))*(y - yo);
        double p2 = (x - xo)*(pow(x - xo,2.) - 3.*pow(y - yo,2.));
        dbxdx += (da*(-(My*p1) + Mx*p2))/p0;
        dbydx += (da*(-(Mx*p1) - My*p2))/p0;
        dbxdy += (da*(-(Mx*p1) - My*p2))/p0;
        dbydy += -(da*(-(My*p1) + Mx*p2))/p0;
    }
    lua_pushnumber(L,Re(dbxdx));
    lua_pushnumber(L,Im(dbxdx));
    lua_pushnumber(L,Re(dbxdy));
    lua_pushnumber(L,Im(dbxdy));
    lua_pushnumber(L,Re(dbydx));
    lua_pushnumber(L,Im(dbydx));
    lua_pushnumber(L,Re(dbydy));
    lua_pushnumber(L,Im(dbydy));

    return 8;
}

/**
 * @brief (De)select output block labels associated with block labels in a given group.
 * Selects all of the blocks that are labeled by block labels that are
 * members of group n.
 * If no number is specified (i.e. mo_groupselectblock() ), all blocks
 * are selected.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_groupselectblock(n)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_groupselectblock()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaGroupSelectBlock(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    //fpproc->d_EditMode = EditLabels;

    if (!fpproc->meshelem.empty())
    {
        int n = lua_gettop(L);
        int group = 0;
        if (n>0)
            group = (int)lua_todouble(L,1);

        for (auto &block: fpproc->blocklist)
        {
            if (group==0 || block.InGroup == group)
            {
                block.ToggleSelect();
            }
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
 * @brief Calculate the line integral for the defined contour.
 * @param L
 * @return Depending on integral type: 1, 2, or 4, or 0 on error.
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_lineintegral(type)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_lineintegral()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaLineIntegral(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        //return ((CFemmviewDoc *)pFemmviewdoc)->old_lua_lineintegral(L);
        lua_error(L,"Compatibility mode for mo_lineintegral is not implemented!\n");
        return 0;
    }

    int type=(int) lua_todouble(L,1);
    // 0- B.n
    // 1 - H.t
    // 2 - Cont length
    // 3 - Force from stress tensor
    // 4 - Torque from stress tensor
    // 5 - (B.n)^2

    if (type<0 || type >5)
    {
        std::string msg = "Invalid line integral selected " + std::to_string(type) + "\n";
        lua_error(L,msg.c_str());
        return 0;
    }

    CComplex z[4];
    fpproc->LineIntegral(type,z);

    switch(type)
    {
    case 0: // B.n
    case 1: // H.t
    case 5: // (B.n)^2
        lua_pushnumber(L,z[0]); // total
        lua_pushnumber(L,z[1]); // avg
        return 2;

    case 2: // length result
        lua_pushnumber(L,z[0].re); // contour length
        lua_pushnumber(L,z[0].im); // swept area
        return 2;

    case 3: // force results
        if (fpproc->Frequency!=0)
        {
            lua_pushnumber(L,z[2].re);
            lua_pushnumber(L,z[3].re);
            lua_pushnumber(L,z[0]);
            lua_pushnumber(L,z[1]);
            return 4;
        } else {
            lua_pushnumber(L,z[0].re);
            lua_pushnumber(L,z[1].re);
            return 2;
        }

    case 4: // torque results
        if (fpproc->Frequency!=0)
        {
            lua_pushnumber(L,z[1].re);
            lua_pushnumber(L,z[0]);
            return 2;
        } else {
            lua_pushnumber(L,z[0].re);
            lua_pushnumber(L,0);
            return 2;
        }
    default:
        assert(false);
    }
    return 0;
}

/**
 * @brief Modify a field of a boundary property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_modifyboundprop("BdryName",propnum,value)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_modboundprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaModifyBoundaryProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    // find the index of the boundary property to modify;
    std::string BdryName = lua_tostring(L,1);
    CMBoundaryProp *m = nullptr;
    for (auto &prop: doc->lineproplist)
    {
        if (BdryName==prop->BdryName) {
            m = dynamic_cast<CMBoundaryProp*>(prop.get());
            break;
        }
    }
    // get out of here if there's no matching material
    if (m==nullptr)
        return 0;

    // now, modify the specified attribute...
    int modprop=(int) lua_todouble(L,2);
    switch(modprop)
    {
    case 0:
        m->BdryName = lua_tostring(L,3);
        doc->updateLineMap();
        break;
    case 1:
        m->A0 = lua_todouble(L,3);
        break;
    case 2:
        m->A1 = lua_todouble(L,3);
        break;
    case 3:
        m->A2 = lua_todouble(L,3);
        break;
    case 4:
        m->phi = lua_todouble(L,3);
        break;
    case 5:
        m->Mu = lua_todouble(L,3);
        break;
    case 6:
        m->Sig = lua_todouble(L,3);
        break;
    case 7:
        m->c0 = lua_tonumber(L,3);
        break;
    case 8:
        m->c1 = lua_tonumber(L,3);
        break;
    case 9:
        m->BdryFormat = (int)lua_todouble(L,3);
        break;
    default:
        break;
    }
    return 0;
}

/**
 * @brief Modify the given circuit property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_modifycircprop("CircName",propnum,value)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_modcircprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaModifyCircuitProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

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
 * @brief Modify a field of a material.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_modifymaterial("BlockName",propnum,value)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_modmatprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaModifyMaterialProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        //return ((CFemmeDoc *)pFemmeDoc)->old_lua_modmatprop(L);
        lua_error(L,"Compatibility mode for mi_modmatprop is not implemented!\n");
        return 0;
    }

    // find the index of the material to modify;
    std::string BlockName = lua_tostring(L,1);
    CMSolverMaterialProp *m = nullptr;
    for (auto &prop: doc->blockproplist)
    {
        if (BlockName==prop->BlockName) {
            m = dynamic_cast<CMSolverMaterialProp*>(prop.get());
            break;
        }
    }

    // get out of here if there's no matching material
    if (m==nullptr)
        return 0;

    int modprop=(int) lua_todouble(L,2);
    // now, modify the specified attribute...
    switch(modprop)
    {
    case 0:
        m->BlockName = lua_tostring(L,3);
        doc->updateBlockMap();
        break;
    case 1:
        m->mu_x = lua_todouble(L,3);
        break;
    case 2:
        m->mu_y = lua_todouble(L,3);
        break;
    case 3:
        m->H_c = lua_todouble(L,3);
        break;
    case 4:
        m->J = lua_tonumber(L,3);
        break;
    case 5:
        m->Cduct = lua_todouble(L,3);
        break;
    case 6:
        m->Lam_d = lua_todouble(L,3);
        break;
    case 7:
        m->Theta_hn = lua_todouble(L,3);
        break;
    case 8:
        m->LamFill = lua_todouble(L,3);
        break;
    case 9:
        m->LamType = (int) lua_todouble(L,3);
        break;
    case 10:
        m->Theta_hx = lua_todouble(L,3);
        break;
    case 11:
        m->Theta_hy = lua_todouble(L,3);
        break;
    case 12:
        m->NStrands = (int) lua_todouble(L,3);
        break;
    case 13:
        m->WireD = lua_todouble(L,4);
    default:
        break;
    }

    return 0;
}

/**
 * @brief Modify a field of a point property.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_modifypointprop("PointName",propnum,value)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_modpointprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaModifyPointProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    // for compatibility with 4.0 and 4.1 Lua implementation
    if (luaInstance->compatibilityMode())
    {
        //return ((CFemmeDoc *)pFemmeDoc)->old_lua_modpointprop(L);
        lua_error(L,"Compatibility mode for mi_modifypointprop is not implemented!\n");
        return 0;
    }

    // find the index of the material to modify;
    std::string PointName = lua_tostring(L,1);
    CMPointProp *p = nullptr;
    for (const auto &prop:doc->nodeproplist)
    {
        if (PointName==prop->PointName) {
            p = dynamic_cast<CMPointProp*>(prop.get());
            break;
        }
    }
    // get out of here if there's no matching material
    if (p==nullptr)
        return 0;

    int modprop=(int) lua_todouble(L,2);
    // now, modify the specified attribute...
    switch(modprop)
    {
    case 0:
        p->PointName = lua_tostring(L,3);
        break;
    case 1:
        p->A = lua_tonumber(L,3);
        break;
    case 2:
        p->J = lua_tonumber(L,3);
        break;
    default:
        break;
    }

    return 0;
}

/**
 * @brief Create a new magnetics document.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_newdocument()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_newdocument()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaNewDocument(lua_State *L)
{
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(LuaInstance::instance(L)->femmState());
    femmState->setDocument(std::make_shared<femm::FemmProblem>(femm::FileType::MagneticsFile));
    return 0;
}

/**
 * @brief Set file name of previous solution file.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_setprevious(filename)} defines the previous solution to be used as the basis for an
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_previous()}
 * AC incremental permeability solution. The filename should include the .ans extension
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaSetPrevious(lua_State *L)
{
    int n = lua_gettop(L);

    if (n>0)
    {
        auto luaInstance = LuaInstance::instance(L);
        std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
        std::shared_ptr<femm::FemmProblem> doc = femmState->femmDocument();

        std::string prev = lua_tostring(L,n);
        doc->PrevSoln=prev;
    }

    return 0;
}

/**
 * @brief Change problem definition.
 * Only the parameters that are set are changed.
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \internal
 * ### Implements:
 * - \lua{mi_probdef(frequency,(units),(type),(precision),(depth),(minangle),(acsolver))}
 *   A negative depth is interpreted as positive depth.
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_prob_def()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaProblemDefinition(lua_State * L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> magDoc = femmState->femmDocument();

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
    if(type=="planar") magDoc->problemType = PLANAR;
    else if(type=="axi") magDoc->problemType = AXISYMMETRIC;
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

    // Note: ei_probdef sets depth to 1 if Depth < 0
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
 * @brief Select the postprocessor block label containing the given point
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_selectblock(x,y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_selectblock()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaSelectOutputBlocklabel(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    double px=lua_todouble(L,1);
    double py=lua_todouble(L,2);

    //fpproc->d_EditMode = EditLabels;

    if (!fpproc->meshelem.empty()){
        int k=fpproc->InTriangle(px,py);
        if(k>=0)
        {
            fpproc->bHasMask=false;
            fpproc->blocklist[fpproc->meshelem[k].lbl].ToggleSelect();
            // RedrawView();
            // theView->DrawSelected=fpproc->meshelem[k].lbl;
            // HDC myDCH=GetDC(theView->m_hWnd);
            // CDC tempDC;
            // CDC *pDC;
            // pDC=tempDC.FromHandle(myDCH);

            // //CDC *pDC=GetDC(theView->m_hWnd);
            // theView->OnDraw(pDC);
            // theView->DrawSelected=-1;
            // theView->ReleaseDC(pDC);
            // fpproc->UpdateAllViews(theView);
        }
    }

    return 0;
}

/**
 * @brief Adds a contour point at the closest input point to (x,y).
 *
 * If the
 * selected point and a previous selected points lie at the ends of an arcsegment,
 * a contour is added that traces along the arcsegment.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_selectpoint(x,y)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmviewLua.cpp,lua_selectline()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaAddContourPointFromNode(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }
    // Note(ZaJ): editAction should not be relevant to anything this method does
    //theView->EditAction=1; // make sure things update OK

    double mx = lua_todouble(L,1);
    double my = lua_todouble(L,2);

    if (!fpproc->nodelist.empty())
    {
        int n0=fpproc->ClosestNode(mx,my);

        int lineno=-1;
        int arcno=-1;
        CComplex z(fpproc->nodelist[n0].x,fpproc->nodelist[n0].y);
        if (fpproc->contour.empty())
        {
            fpproc->contour.push_back(z);
            //theView->DrawUserContour(FALSE);
            return 0;
        }
        //check to see if point is the same as last point in the contour;
        CComplex y = fpproc->contour.back();

        if ((y.re==z.re) && (y.im==z.im))
            return 0;

        int n1 = fpproc->ClosestNode(y.re,y.im);
        CComplex x(fpproc->nodelist[n1].x,fpproc->nodelist[n1].y);

        //check to see if this point and the last point are ends of an
        //input segment;
        double d1=1.e08;

        if (abs(x-y)<1.e-08)
        {
            for(int k=0; k<(int)fpproc->linelist.size(); k++)
            {
                if((fpproc->linelist[k].n0==n1) && (fpproc->linelist[k].n1==n0))
                {
                    double d2=fabs(fpproc->ShortestDistanceFromSegment(mx,my,k));
                    if(d2<d1)
                    {
                        lineno=k;
                        d1=d2;
                    }
                }
                if((fpproc->linelist[k].n0==n0) && (fpproc->linelist[k].n1==n1))
                {
                    double d2=fabs(fpproc->ShortestDistanceFromSegment(mx,my,k));
                    if(d2<d1){
                        lineno=k;
                        d1=d2;
                    }
                }
            }
        }
        bool reverseOrder = false;
        //check to see if this point and last point are ends of an
        // arc segment; if so, add entire arc to the contour;
        if (abs(x-y)<1.e-08)
        {
            for(int k=0;k<(int)fpproc->arclist.size();k++)
            {
                if((fpproc->arclist[k].n0==n1) && (fpproc->arclist[k].n1==n0))
                {
                    double d2=fpproc->ShortestDistanceFromArc(CComplex(mx,my),
                                                              fpproc->arclist[k]);
                    if(d2<d1){
                        arcno=k;
                        lineno=-1;
                        reverseOrder=true;
                        d1=d2;
                    }
                }
                if((fpproc->arclist[k].n0==n0) && (fpproc->arclist[k].n1==n1))
                {
                    double d2=fpproc->ShortestDistanceFromArc(CComplex(mx,my),
                                                              fpproc->arclist[k]);
                    if(d2<d1){
                        arcno=k;
                        lineno=-1;
                        reverseOrder=false;
                        d1=d2;
                    }
                }
            }
        }
        if((lineno<0) && (arcno<0))
        {
            fpproc->contour.push_back(z);
            //theView->DrawUserContour(FALSE);
        }
        if(lineno>=0)
        {
            int size=(int) fpproc->contour.size();
            if(size>1)
            {
                if(abs(fpproc->contour[size-2]-z)<1.e-08)
                    return 0;
            }
            fpproc->contour.push_back(z);
            //theView->DrawUserContour(FALSE);
        }
        if(arcno>=0){
            int k=arcno;
            double R;
            fpproc->GetCircle(fpproc->arclist[k],x,R);
            int arcsegments=(int) ceil(fpproc->arclist[k].ArcLength/fpproc->arclist[k].MaxSideLength);
            if(reverseOrder)
                z=exp(I*fpproc->arclist[k].ArcLength*PI/(180.*((double) arcsegments)) );
            else
                z=exp(-I*fpproc->arclist[k].ArcLength*PI/(180.*((double) arcsegments)) );

            for(int i=0; i<arcsegments; i++)
            {
                y=(y-x)*z+x;
                int size=(int) fpproc->contour.size();
                if(size>1)
                {
                    if(abs(fpproc->contour[size-2]-y)<1.e-08)
                        return 0;
                }
                fpproc->contour.push_back(y);
                //theView->DrawUserContour(FALSE);
            }
        }
    }

    return 0;
}

/**
 * @brief Set properties for selected arc segments
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_setarcsegmentprop(maxsegdeg, "propname", hide, group)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_setarcsegmentprop()}
 *
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaSetArcsegmentProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    double maxsegdeg = lua_todouble(L,1);
    std::string boundprop;
    int boundpropidx = -1;
    // Note: propname may be 0 (as in number 0, not string "0").
    //       In that case, the arc segments have no boundary property.
    if (!lua_isnil(L,2))
    {
        boundprop = lua_tostring(L,2);
        if (doc->lineMap.count(boundprop))
            boundpropidx = doc->lineMap[boundprop];
        else
            debug << "Property " << boundprop << " has no index!\n";
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
 *
 * \internal
 * ### Implements:
 * - \lua{mi_setblockprop("blockname", automesh, meshsize, "incircuit", magdirection, group, turns)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_setblockprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaSetBlocklabelProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

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

    // Note: blockname and/or incircuit may be 0 (as in number 0, not string "0").
    //       In that case, the block labels have no block type and/or are not in a circuit.
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
        // magdirection can either be a number, or a string containing a lua expression.
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
        CMBlockLabel *labelPtr = dynamic_cast<CMBlockLabel*>(doc->labellist[i].get());
        assert(labelPtr);
        if (labelPtr->IsSelected)
        {
            labelPtr->MaxArea = PI*meshsize*meshsize/4.;
            labelPtr->MagDir = magdirection;
            labelPtr->BlockTypeName = blocktype;
            labelPtr->BlockType = blocktypeidx;
            labelPtr->InCircuitName = incircuit;
            labelPtr->InCircuit = incircuitidx;
            labelPtr->InGroup = group;
            labelPtr->Turns = turns;
            labelPtr->MagDirFctn = magdirfctn;
            if(automesh)
                labelPtr->MaxArea = 0;
        }
    }

    return 0;
}

/**
 * @brief Set the nodal property for selected nodes.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mi_setnodeprop("propname",groupno)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_setnodeprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaSetNodeProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    int nodepropidx = -1;
    std::string nodeprop = "<None>";
    // Note: propname may be 0 (as in number 0, not string "0").
    //       In that case, the arc segments have no boundary property.
    if (!lua_isnil(L,1))
    {
        nodeprop = lua_tostring(L,1);
        if (doc->nodeMap.count(nodeprop))
            nodepropidx = doc->nodeMap[nodeprop];
        else
            debug << "Property " << nodeprop << " has no index!\n";
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
 *
 * \internal
 * ### Implements:
 * - \lua{mi_setsegmentprop("propname", elementsize, automesh, hide, group)}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_setsegmentprop()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaSetSegmentProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    int boundpropidx = -1;
    std::string boundprop = "<None>";
    if (!lua_isnil(L,1))
    {
        boundprop = lua_tostring(L,1);
        if (doc->lineMap.count(boundprop))
            boundpropidx = doc->lineMap[boundprop];
        else
            debug << "Property " << boundprop << " has no index!\n";
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
            doc->linelist[i]->BoundaryMarker = boundpropidx;
            doc->linelist[i]->BoundaryMarkerName = boundprop;
            doc->linelist[i]->Hidden = hide;
            doc->linelist[i]->InGroup = group;
        }
    }

    return 0;
}


/**
 * @brief This function controls whether or not smoothing is applied to the F and G/D and E/B and H fields.
 * Setting flag equal to "on" turns on smoothing, and setting flag to "off" turns off smoothing.
 * @param L
 * @return 0
 * \ingroup LuaMM
 *
 * \internal
 * ### Implements:
 * - \lua{mo_smooth}
 *
 * ### FEMM sources:
 * - \femm42{femm/femmviewLua.cpp,lua_smoothing()}
 * \endinternal
 */
int femmcli::LuaMagneticsCommands::luaSetSmoothing(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FPProc> fpproc = std::dynamic_pointer_cast<FPProc>(femmState->getPostProcessor());
    if (!fpproc)
    {
        lua_error(L,"No magnetics output in focus");
        return 0;
    }

    std::string flag (lua_tostring(L,1));
    to_lower(flag);
    if (flag == "on")
    {
        fpproc->Smooth = true;
    } else if (flag == "off")
    {
        fpproc->Smooth = false;
    } else {
        lua_error(L, "Unknown option for smoothing");
    }
    return 0;
}


// vi:expandtab:tabstop=4 shiftwidth=4:
