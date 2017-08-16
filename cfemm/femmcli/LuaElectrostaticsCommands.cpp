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

#include "LuaElectrostaticsCommands.h"
#include "LuaCommonCommands.h"

#include "CPointVals.h"
#include "esolver.h"
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

void femmcli::LuaElectrostaticsCommands::registerCommands(LuaInstance &li)
{
    li.addFunction("ei_add_arc", LuaCommonCommands::luaAddArc);
    li.addFunction("ei_addarc", LuaCommonCommands::luaAddArc);
    li.addFunction("ei_add_block_label", LuaCommonCommands::luaAddBlocklabel);
    li.addFunction("ei_addblocklabel", LuaCommonCommands::luaAddBlocklabel);
    li.addFunction("ei_add_bound_prop", luaAddBoundaryProp);
    li.addFunction("ei_addboundprop", luaAddBoundaryProp);
    li.addFunction("ei_add_conductor_prop", luaAddConductorProp);
    li.addFunction("ei_addconductorprop", luaAddConductorProp);
    li.addFunction("ei_add_material", luaAddMaterialProp);
    li.addFunction("ei_addmaterial", luaAddMaterialProp);
    li.addFunction("ei_add_node", LuaCommonCommands::luaAddNode);
    li.addFunction("ei_addnode", LuaCommonCommands::luaAddNode);
    li.addFunction("ei_add_point_prop", luaAddPointProp);
    li.addFunction("ei_addpointprop", luaAddPointProp);
    li.addFunction("ei_add_segment", LuaCommonCommands::luaAddLine);
    li.addFunction("ei_addsegment", LuaCommonCommands::luaAddLine);
    li.addFunction("ei_analyse", luaAnalyze);
    li.addFunction("ei_analyze", luaAnalyze);
    li.addFunction("ei_attach_default", LuaCommonCommands::luaAttachDefault);
    li.addFunction("ei_attachdefault", LuaCommonCommands::luaAttachDefault);
    li.addFunction("ei_attach_outer_space", LuaCommonCommands::luaAttachOuterSpace);
    li.addFunction("ei_attachouterspace", LuaCommonCommands::luaAttachOuterSpace);
    li.addFunction("ei_clear_selected", LuaCommonCommands::luaClearSelected);
    li.addFunction("ei_clearselected", LuaCommonCommands::luaClearSelected);
    li.addFunction("ei_close", LuaCommonCommands::luaExitPre);
    li.addFunction("ei_copy_rotate", LuaCommonCommands::luaCopyRotate);
    li.addFunction("ei_copyrotate", LuaCommonCommands::luaCopyRotate);
    li.addFunction("ei_copy_translate", LuaCommonCommands::luaCopyTranslate);
    li.addFunction("ei_copytranslate", LuaCommonCommands::luaCopyTranslate);
    li.addFunction("ei_create_mesh", LuaCommonCommands::luaCreateMesh);
    li.addFunction("ei_createmesh", LuaCommonCommands::luaCreateMesh);
    li.addFunction("ei_create_radius", LuaCommonCommands::luaCreateRadius);
    li.addFunction("ei_createradius", LuaCommonCommands::luaCreateRadius);
    li.addFunction("ei_define_outer_space", LuaCommonCommands::luaDefineOuterSpace);
    li.addFunction("ei_defineouterspace", LuaCommonCommands::luaDefineOuterSpace);
    li.addFunction("ei_delete_bound_prop", luaDeleteBoundaryProp);
    li.addFunction("ei_deleteboundprop", luaDeleteBoundaryProp);
    li.addFunction("ei_delete_conductor", luaDeleteCircuitProp);
    li.addFunction("ei_deleteconductor", luaDeleteCircuitProp);
    li.addFunction("ei_delete_material", luaDeleteMaterial);
    li.addFunction("ei_deletematerial", luaDeleteMaterial);
    li.addFunction("ei_delete_point_prop", luaDeletePointProp);
    li.addFunction("ei_deletepointprop", luaDeletePointProp);
    li.addFunction("ei_delete_selected_arcsegments", luaDeleteSelectedArcSegments);
    li.addFunction("ei_deleteselectedarcsegments", luaDeleteSelectedArcSegments);
    li.addFunction("ei_delete_selected_labels", luaDeleteSelectedBlockLabels);
    li.addFunction("ei_deleteselectedlabels", luaDeleteSelectedBlockLabels);
    li.addFunction("ei_delete_selected", luaDeleteSelected);
    li.addFunction("ei_deleteselected", luaDeleteSelected);
    li.addFunction("ei_delete_selected_nodes", luaDeleteSelectedNodes);
    li.addFunction("ei_deleteselectednodes", luaDeleteSelectedNodes);
    li.addFunction("ei_delete_selected_segments", luaDeleteSelectedSegments);
    li.addFunction("ei_deleteselectedsegments", luaDeleteSelectedSegments);
    li.addFunction("ei_detach_default", luaDetachDefault);
    li.addFunction("ei_detachdefault", luaDetachDefault);
    li.addFunction("ei_detach_outer_space", luaDetachOuterSpace);
    li.addFunction("ei_detachouterspace", luaDetachOuterSpace);
    li.addFunction("ei_getboundingbox", luaGetBoundingBox);
    li.addFunction("ei_get_material", luaGetMaterialFromLib);
    li.addFunction("ei_getmaterial", luaGetMaterialFromLib);
    li.addFunction("ei_getprobleminfo", luaGetProblemInfo);
    li.addFunction("ei_get_title", luaGetTitle);
    li.addFunction("ei_gettitle", luaGetTitle);
    li.addFunction("ei_grid_snap", LuaInstance::luaNOP);
    li.addFunction("ei_gridsnap", LuaInstance::luaNOP);
    li.addFunction("ei_hide_grid", LuaInstance::luaNOP);
    li.addFunction("ei_hidegrid", LuaInstance::luaNOP);
    li.addFunction("ei_load_solution", luaLoadSolution);
    li.addFunction("ei_loadsolution", luaLoadSolution);
    li.addFunction("ei_maximize", LuaInstance::luaNOP);
    li.addFunction("ei_minimize", LuaInstance::luaNOP);
    li.addFunction("ei_mirror", luaMirrorCopy);
    li.addFunction("ei_modify_bound_prop", luaModifyBoundaryProp);
    li.addFunction("ei_modifyboundprop", luaModifyBoundaryProp);
    li.addFunction("ei_modify_conductor_prop", luaModifyCircuitProp);
    li.addFunction("ei_modifyconductorprop", luaModifyCircuitProp);
    li.addFunction("ei_modify_material", luaModifyMaterialProp);
    li.addFunction("ei_modifymaterial", luaModifyMaterialProp);
    li.addFunction("ei_modify_point_prop", luaModifyPointProp);
    li.addFunction("ei_modifypointprop", luaModifyPointProp);
    li.addFunction("ei_move_rotate", luaMoveRotate);
    li.addFunction("ei_moverotate", luaMoveRotate);
    li.addFunction("ei_move_translate", luaMoveTranslate);
    li.addFunction("ei_movetranslate", luaMoveTranslate);
    li.addFunction("ei_new_document", luaNewDocument);
    li.addFunction("ei_newdocument", luaNewDocument);
    li.addFunction("ei_prob_def", luaProbDef);
    li.addFunction("ei_probdef", luaProbDef);
    li.addFunction("ei_purge_mesh", luaPurgeMesh);
    li.addFunction("ei_purgemesh", luaPurgeMesh);
    li.addFunction("ei_read_dxf", LuaInstance::luaNOP);
    li.addFunction("ei_readdxf", LuaInstance::luaNOP);
    li.addFunction("ei_refresh_view", LuaInstance::luaNOP);
    li.addFunction("ei_refreshview", LuaInstance::luaNOP);
    li.addFunction("ei_resize", LuaInstance::luaNOP);
    li.addFunction("ei_restore", LuaInstance::luaNOP);
    li.addFunction("ei_save_as", LuaCommonCommands::luaSaveDocument);
    li.addFunction("ei_saveas", LuaCommonCommands::luaSaveDocument);
    li.addFunction("ei_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("ei_savebitmap", LuaInstance::luaNOP);
    li.addFunction("ei_save_dxf", LuaInstance::luaNOP);
    li.addFunction("ei_savedxf", LuaInstance::luaNOP);
    li.addFunction("ei_save_metafile", LuaInstance::luaNOP);
    li.addFunction("ei_savemetafile", LuaInstance::luaNOP);
    li.addFunction("ei_scale", luaScaleMove);
    li.addFunction("ei_select_arcsegment", LuaCommonCommands::luaSelectArcsegment);
    li.addFunction("ei_selectarcsegment", LuaCommonCommands::luaSelectArcsegment);
    li.addFunction("ei_select_circle", luaSelectWithinCircle);
    li.addFunction("ei_selectcircle", luaSelectWithinCircle);
    li.addFunction("ei_select_group", luaSelectGroup);
    li.addFunction("ei_selectgroup", luaSelectGroup);
    li.addFunction("ei_select_label", LuaCommonCommands::luaSelectBlocklabel);
    li.addFunction("ei_selectlabel", LuaCommonCommands::luaSelectBlocklabel);
    li.addFunction("ei_select_node", LuaCommonCommands::luaSelectNode);
    li.addFunction("ei_selectnode", LuaCommonCommands::luaSelectNode);
    li.addFunction("ei_select_rectangle", luaSelectWithinRectangle);
    li.addFunction("ei_selectrectangle", luaSelectWithinRectangle);
    li.addFunction("ei_select_segment", LuaCommonCommands::luaSelectSegment);
    li.addFunction("ei_selectsegment", LuaCommonCommands::luaSelectSegment);
    li.addFunction("ei_set_arcsegment_prop", luaSetArcsegmentProp);
    li.addFunction("ei_setarcsegmentprop", luaSetArcsegmentProp);
    li.addFunction("ei_set_block_prop", luaSetBlocklabelProp);
    li.addFunction("ei_setblockprop", luaSetBlocklabelProp);
    li.addFunction("ei_set_edit_mode", LuaCommonCommands::luaSetEditMode);
    li.addFunction("ei_seteditmode", LuaCommonCommands::luaSetEditMode);
    li.addFunction("ei_set_focus", luaSetFocus);
    li.addFunction("ei_setfocus", luaSetFocus);
    li.addFunction("ei_set_grid", LuaInstance::luaNOP);
    li.addFunction("ei_setgrid", LuaInstance::luaNOP);
    li.addFunction("ei_set_group", LuaCommonCommands::luaSetGroup);
    li.addFunction("ei_setgroup", LuaCommonCommands::luaSetGroup);
    li.addFunction("ei_set_node_prop", luaSetNodeProp);
    li.addFunction("ei_setnodeprop", luaSetNodeProp);
    li.addFunction("ei_set_segment_prop", luaSetSegmentProp);
    li.addFunction("ei_setsegmentprop", luaSetSegmentProp);
    li.addFunction("ei_show_grid", LuaInstance::luaNOP);
    li.addFunction("ei_showgrid", LuaInstance::luaNOP);
    li.addFunction("ei_show_mesh", LuaInstance::luaNOP);
    li.addFunction("ei_showmesh", LuaInstance::luaNOP);
    li.addFunction("ei_show_names", LuaInstance::luaNOP);
    li.addFunction("ei_shownames", LuaInstance::luaNOP);
    li.addFunction("ei_zoom_in", LuaInstance::luaNOP);
    li.addFunction("ei_zoomin", LuaInstance::luaNOP);
    li.addFunction("ei_zoom", LuaInstance::luaNOP);
    li.addFunction("ei_zoom_natural", LuaInstance::luaNOP);
    li.addFunction("ei_zoomnatural", LuaInstance::luaNOP);
    li.addFunction("ei_zoom_out", LuaInstance::luaNOP);
    li.addFunction("ei_zoomout", LuaInstance::luaNOP);
    li.addFunction("eo_add_contour", luaAddContourPoint);
    li.addFunction("eo_addcontour", luaAddContourPoint);
    li.addFunction("eo_bend_contour", luaBendContourLine);
    li.addFunction("eo_bendcontour", luaBendContourLine);
    li.addFunction("eo_block_integral", luaBlockIntegral);
    li.addFunction("eo_blockintegral", luaBlockIntegral);
    li.addFunction("eo_clear_block", luaClearBlock);
    li.addFunction("eo_clearblock", luaClearBlock);
    li.addFunction("eo_clear_contour", luaClearContourPoint);
    li.addFunction("eo_clearcontour", luaClearContourPoint);
    li.addFunction("eo_close", luaExitPost);
    li.addFunction("eo_get_conductor_properties", luaGetCircuitProperties);
    li.addFunction("eo_getconductorproperties", luaGetCircuitProperties);
    li.addFunction("eo_get_element", luaGetElement);
    li.addFunction("eo_getelement", luaGetElement);
    li.addFunction("eo_get_node", luaGetMeshNode);
    li.addFunction("eo_getnode", luaGetMeshNode);
    li.addFunction("eo_get_point_values", luaGetPointVals);
    li.addFunction("eo_getpointvalues", luaGetPointVals);
    li.addFunction("eo_get_problem_info", luaGetProblemInfo);
    li.addFunction("eo_getprobleminfo", luaGetProblemInfo);
    li.addFunction("eo_get_title", luaGetTitle);
    li.addFunction("eo_gettitle", luaGetTitle);
    li.addFunction("eo_grid_snap", LuaInstance::luaNOP);
    li.addFunction("eo_gridsnap", LuaInstance::luaNOP);
    li.addFunction("eo_group_select_block", luaGroupSelectBlock);
    li.addFunction("eo_groupselectblock", luaGroupSelectBlock);
    li.addFunction("eo_hide_contour_plot", LuaInstance::luaNOP);
    li.addFunction("eo_hidecontourplot", LuaInstance::luaNOP);
    li.addFunction("eo_hide_density_plot", LuaInstance::luaNOP);
    li.addFunction("eo_hidedensityplot", LuaInstance::luaNOP);
    li.addFunction("eo_hide_grid", LuaInstance::luaNOP);
    li.addFunction("eo_hidegrid", LuaInstance::luaNOP);
    li.addFunction("eo_hide_mesh", LuaInstance::luaNOP);
    li.addFunction("eo_hidemesh", LuaInstance::luaNOP);
    li.addFunction("eo_hide_points", LuaInstance::luaNOP);
    li.addFunction("eo_hidepoints", LuaInstance::luaNOP);
    li.addFunction("eo_line_integral", luaLineIntegral);
    li.addFunction("eo_lineintegral", luaLineIntegral);
    li.addFunction("eo_make_plot", LuaInstance::luaNOP);
    li.addFunction("eo_makeplot", LuaInstance::luaNOP);
    li.addFunction("eo_maximize", LuaInstance::luaNOP);
    li.addFunction("eo_minimize", LuaInstance::luaNOP);
    li.addFunction("eo_num_elements", luaNumElements);
    li.addFunction("eo_numelements", luaNumElements);
    li.addFunction("eo_num_nodes", luaNumNodes);
    li.addFunction("eo_numnodes", luaNumNodes);
    li.addFunction("eo_refresh_view", LuaInstance::luaNOP);
    li.addFunction("eo_refreshview", LuaInstance::luaNOP);
    li.addFunction("eo_reload", luaLoadSolution);
    li.addFunction("eo_resize", LuaInstance::luaNOP);
    li.addFunction("eo_restore", LuaInstance::luaNOP);
    li.addFunction("eo_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("eo_savebitmap", LuaInstance::luaNOP);
    li.addFunction("eo_save_metafile", LuaInstance::luaNOP);
    li.addFunction("eo_savemetafile", LuaInstance::luaNOP);
    li.addFunction("eo_select_block", luaSelectOutputBlocklabel);
    li.addFunction("eo_selectblock", luaSelectOutputBlocklabel);
    li.addFunction("eo_select_point", luaAddContourPointFromNode);
    li.addFunction("eo_selectpoint", luaAddContourPointFromNode);
    li.addFunction("eo_set_edit_mode", LuaInstance::luaNOP);
    li.addFunction("eo_seteditmode", LuaInstance::luaNOP);
    li.addFunction("eo_set_focus", luaSetFocus);
    li.addFunction("eo_setfocus", luaSetFocus);
    li.addFunction("eo_set_grid", LuaInstance::luaNOP);
    li.addFunction("eo_setgrid", LuaInstance::luaNOP);
    li.addFunction("eo_show_contour_plot", LuaInstance::luaNOP);
    li.addFunction("eo_showcontourplot", LuaInstance::luaNOP);
    li.addFunction("eo_show_density_plot", LuaInstance::luaNOP);
    li.addFunction("eo_showdensityplot", LuaInstance::luaNOP);
    li.addFunction("eo_show_grid", LuaInstance::luaNOP);
    li.addFunction("eo_showgrid", LuaInstance::luaNOP);
    li.addFunction("eo_show_mesh", LuaInstance::luaNOP);
    li.addFunction("eo_showmesh", LuaInstance::luaNOP);
    li.addFunction("eo_show_names", LuaInstance::luaNOP);
    li.addFunction("eo_shownames", LuaInstance::luaNOP);
    li.addFunction("eo_show_points", LuaInstance::luaNOP);
    li.addFunction("eo_showpoints", LuaInstance::luaNOP);
    li.addFunction("eo_show_vector_plot", LuaInstance::luaNOP);
    li.addFunction("eo_showvectorplot", LuaInstance::luaNOP);
    li.addFunction("eo_smooth", LuaInstance::luaNOP);
    li.addFunction("eo_zoom_in", LuaInstance::luaNOP);
    li.addFunction("eo_zoomin", LuaInstance::luaNOP);
    li.addFunction("eo_zoom", LuaInstance::luaNOP);
    li.addFunction("eo_zoom_natural", LuaInstance::luaNOP);
    li.addFunction("eo_zoomnatural", LuaInstance::luaNOP);
    li.addFunction("eo_zoom_out", LuaInstance::luaNOP);
    li.addFunction("eo_zoomout", LuaInstance::luaNOP);
}

/**
 * @brief Add a new boundary property with a given name.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_addboundprop("boundpropname", Vs, qs, c0, c1, BdryFormat)}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_addboundprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddBoundaryProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    std::unique_ptr<CSBoundaryProp> m = std::make_unique<CSBoundaryProp>();

    int n=lua_gettop(L);
    if (n>0) m->BdryName = lua_tostring(L,1);
    if (n>1) m->V=lua_todouble(L,2);
    if (n>2) m->qs=lua_todouble(L,3);
    if (n>3) m->c0=lua_todouble(L,4);
    if (n>4) m->c1=lua_todouble(L,5);
    if (n>5) m->BdryFormat=(int) lua_todouble(L,6);

    doc->lineproplist.push_back(std::move(m));
    doc->updateLineMap();
    return 0;
}

/**
 * @brief  Add a new circuit property with a given name.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_add_conductor_prop("conductorname", Vc, qc, conductortype)}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_addcircuitprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddConductorProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::unique_ptr<CSCircuit> m = std::make_unique<CSCircuit>();
    int n=lua_gettop(L);

    if(n>0) m->CircName = lua_tostring(L,1);
    if(n>1) m->V = lua_todouble(L,2);
    if(n>2) m->q = lua_todouble(L,3);
    if(n>3) m->CircType = (int) lua_todouble(L,4);

    femmState->femmDocument()->circproplist.push_back(std::move(m));
    femmState->femmDocument()->updateCircuitMap();

    return 0;
}

/**
 * @brief Add a contour point.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_addcontour(x,y)}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_addcontour()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddContourPoint(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief Add a new material property.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_add_material("materialname", ex, ey, qv)}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_addmatprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddMaterialProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());

    std::unique_ptr<CSMaterialProp> m = std::make_unique<CSMaterialProp>();
    int n=lua_gettop(L);

    if (n>0) {
        m->BlockName = lua_tostring(L,1);
    }
    if (n>1) {
          m->ex = lua_todouble(L,2);
          m->ey = m->ex;
    }
    if(n>2) m->ey = lua_todouble(L,3);
    if(n>3) m->qv = lua_todouble(L,4);

    femmState->femmDocument()->blockproplist.push_back(std::move(m));
    femmState->femmDocument()->updateBlockMap();
    return 0;
}


/**
 * @brief Add a new point property.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_addpointprop("pointpropname",Vp,qp)}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_addpointprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddPointProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    std::unique_ptr<CSPointProp> m = std::make_unique<CSPointProp>();
    int n=lua_gettop(L);

    if (n>0) m->PointName = lua_tostring(L,1);
    if (n>1) m->V = lua_todouble(L,2);
    if (n>2) m->qp = lua_todouble(L,3);

    doc->nodeproplist.push_back(std::move(m));
    doc->updateNodeMap();
    return 0;
}

/**
 * @brief Mesh the problem description, save it, and run the solver.
 * If the global variable "XFEMM_VERBOSE" is set to 1, the mesher and solver is more verbose and prints statistics.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_analyse(flag)}
 *   Parameter flag (0,1) determines visibility of fkern window and is ignored on xfemm.
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_analyze()}
 * - \femm42{femm/beladrawView.cpp,CbeladrawView::OnMenuAnalyze()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAnalyze(lua_State *L)
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

        // check to see if all block defined to be in an axisymmetric external region are isotropic
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
                        CSMaterialProp *prop = dynamic_cast<CSMaterialProp*>(doc->blockproplist[i].get());
                        assert(prop);
                        if(prop->ex != prop->ey)
                            hasAnisotropicMaterial = true;
                        // TODO ZaJ: check if this can be merged with LuaMagneticsCommands::luaAnalze
                    }
                }
            }
        }
        if (hasAnisotropicMaterial)
        {
            //InvalidateRect(NULL);
            std::string ermsg = "Only istropic materials are\n"
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
        lua_error(L, "ei_analyze(): Could not save fem file!\n");
        return 0;
    }
    if (!doc->consistencyCheckOK())
    {
        lua_error(L,"ei_analyze(): consistency check failed before meshing!\n");
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
            mesherDoc->UnselectAll();
            lua_error(L, "ei_analyze(): Periodic BC triangulation failed!\n");
            return 0;
        }
    }
    else{
        if (mesherDoc->DoNonPeriodicBCTriangulation(pathName) != 0)
        {
            //EndWaitCursor();
            lua_error(L, "ei_analyze(): Nonperiodic BC triangulation failed!\n");
            return 0;
        }
    }
    //EndWaitCursor();
    if (!doc->consistencyCheckOK())
    {
        lua_error(L,"ei_analyze(): consistency check failed after meshing!\n");
        return 0;
    }

    ESolver theSolver;
    // filename.fee -> filename
    std::size_t dotpos = doc->pathName.find_last_of(".");
    theSolver.PathName = doc->pathName.substr(0,dotpos);
    theSolver.WarnMessage = &PrintWarningMsg;
    theSolver.PrintMessage = &PrintWarningMsg;
    if (!theSolver.LoadProblemFile())
    {
        lua_error(L, "ei_analyze(): problem initializing solver!");
        return 0;
    }
    assert( doc->ACSolver == theSolver.ACSolver);
    assert( doc->lineproplist.size() == theSolver.lineproplist.size());
    assert( doc->nodeproplist.size() == theSolver.nodeproplist.size());
    assert( doc->blockproplist.size() == theSolver.blockproplist.size());
    // the solver may create additional circprops upon loading:
    assert( doc->circproplist.size() <= theSolver.circproplist.size());
    // holes are not read by the solver, which means that the solver may have fewer blocklabels:
    assert( doc->labellist.size() >= theSolver.labellist.size());
    if (!theSolver.runSolver(verbose))
    {
        lua_error(L, "solver failed.");
    }
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_bend_contour}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_bendcontour()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaBendContourLine(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_block_integral}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_blockintegral()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaBlockIntegral(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_clear_block}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_clearblock()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaClearBlock(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_clear_contour}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_clearcontour()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaClearContourPoint(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_delete_bound_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_delboundprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDeleteBoundaryProp(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_delete_conductor}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_delcircuitprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDeleteCircuitProp(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_delete_selected_arcsegments}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselectedarcsegments()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDeleteSelectedArcSegments(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_delete_selected}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselected()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDeleteSelected(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_delete_selected_labels}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselectedlabels()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDeleteSelectedBlockLabels(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_delete_selected_nodes}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselectednodes()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDeleteSelectedNodes(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_delete_selected_segments}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_deleteselectedsegments()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDeleteSelectedSegments(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_delete_material}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_delmatprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDeleteMaterial(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_delete_point_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_delpointprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDeletePointProp(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_detach_default}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_detachdefault()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDetachDefault(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_detach_outer_space}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_detachouterspace()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDetachOuterSpace(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_close}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_exitpost()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaExitPost(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_getboundingbox}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_getboundingbox()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetBoundingBox(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_get_conductor_properties}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_getcircuitprops()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetCircuitProperties(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_get_element}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_getelement()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetElement(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_get_material}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_getmaterial()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetMaterialFromLib(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_get_node}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_getnode()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetMeshNode(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_get_point_values}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_getpointvals()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetPointVals(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_getprobleminfo}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_getprobleminfo()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetProblemInfo(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_get_title}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_gettitle()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetTitle(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_group_select_block}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_groupselectblock()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGroupSelectBlock(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_line_integral}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_lineintegral()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaLineIntegral(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_mirror}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_mirror()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaMirrorCopy(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_modify_bound_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_modboundprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaModifyBoundaryProp(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_modify_conductor_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_modcircprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaModifyCircuitProp(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_modify_material}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_modmatprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaModifyMaterialProp(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_modify_point_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_modpointprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaModifyPointProp(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_move_rotate}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_move_rotate()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaMoveRotate(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_move_translate}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_move_translate()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaMoveTranslate(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_new_document}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_newdocument()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaNewDocument(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_num_elements}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_numelements()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaNumElements(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_num_nodes}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_numnodes()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaNumNodes(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief Change problem definition.
 * Only the parameters that are set are changed.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_prob_def(units,type,precision,(depth),(minangle))}
 *   A negative depth is interpreted as 1.
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_prob_def()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaProbDef(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<femm::FemmProblem> esDoc = femmState->femmDocument();

    // argument count
    int n;
    n=lua_gettop(L);

    // Length Units
    std::string units (lua_tostring(L,1));
    if(units=="inches") esDoc->LengthUnits = LengthInches;
    else if(units=="millimeters") esDoc->LengthUnits = LengthMillimeters;
    else if(units=="centimeters") esDoc->LengthUnits = LengthCentimeters;
    else if(units=="meters") esDoc->LengthUnits = LengthMeters;
    else if(units=="mills") esDoc->LengthUnits = LengthMils;
    else if(units=="mils") esDoc->LengthUnits = LengthMils;
    else if(units=="micrometers") esDoc->LengthUnits = LengthMicrometers;
    else
    {
        std::string msg  = "Unknown length unit " + units;
        lua_error(L,msg.c_str());
        return 0;
    }

    // Problem type
    std::string type (lua_tostring(L,2));
    if(type=="planar") esDoc->ProblemType = PLANAR;
    else if(type=="axi") esDoc->ProblemType = AXISYMMETRIC;
    else
    {
        std::string msg =  "Unknown problem type " + type;
        lua_error(L,msg.c_str());
        return 0;
    }

    double precision = lua_tonumber(L,3).re;
    if (precision < 1.e-16 || precision >1.e-8)
    {
        std::string msg = "Invalid Precision " + std::to_string(precision);
        lua_error(L,msg.c_str());
        return 0;
    }
    esDoc->Precision = precision;
    if (n==3) return 0;

    // Note: mi_probdef does fabs(Depth) instead
    esDoc->Depth = lua_tonumber(L,4).re;
    if (esDoc->Depth < 0)
        esDoc->Depth = 1;
    if (n==4) return 0;

    double minAngle = lua_tonumber(L,5).re;
    if ((minAngle>=1.) && (minAngle<=33.8))
    {
        esDoc->MinAngle = minAngle;
    }

    return 0;
}



/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_purge_mesh}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_purge_mesh()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaPurgeMesh(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_load_solution}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_runpost()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaLoadSolution(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_scale}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_scale()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaScaleMove(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_select_block}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_selectblock()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectOutputBlocklabel(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_select_circle}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_selectcircle()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectWithinCircle(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_select_conductor}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_selectconductor()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectConductor(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_select_group}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_selectgroup()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectGroup(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_select_point}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_selectline()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddContourPointFromNode(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_select_rectangle}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_selectrectangle()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectWithinRectangle(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

/**
 * @brief Set properties for selected arc segments
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_set_arcsegment_prop(maxsegdeg, "propname", hide, group, "inconductor")}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_setarcsegmentprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetArcsegmentProp(lua_State *L)
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

    int inconductoridx = -1;
    std::string inconductor = "<None>";
    if (!lua_isnil(L,5))
    {
        inconductor = lua_tostring(L,5);
        if (doc->circuitMap.count(inconductor))
            inconductoridx = doc->circuitMap[inconductor];
        else
            debug << "Conductor " << inconductor << " has no index!\n";
    }

    for (int i=0; i<(int)doc->arclist.size(); i++)
    {
        if (doc->arclist[i]->IsSelected)
        {
            doc->arclist[i]->BoundaryMarker = boundpropidx;
            doc->arclist[i]->BoundaryMarkerName = boundprop;
            doc->arclist[i]->MaxSideLength = maxsegdeg;
            doc->arclist[i]->Hidden = hide;
            doc->arclist[i]->InGroup = group;
            doc->arclist[i]->InConductor = inconductoridx;
            doc->arclist[i]->InConductorName = inconductor;
        }
    }

    return 0;
}

/**
 * @brief Set properties for selected block labels
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_set_block_prop("blockname", automesh, meshsize, group)}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_setblockprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetBlocklabelProp(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    // default values
    int blocktypeidx = -1;
    std::string blocktype = "<None>";
    bool automesh = true;
    double meshsize = 0;
    int group = 0;

    int n=lua_gettop(L);

    // Note: blockname may be 0 (as in number 0, not string "0").
    //       In that case, the block labels have no block type.
    if (n>0 && !lua_isnil(L,1))
    {
        blocktype = lua_tostring(L,1);
        if (doc->blockMap.count(blocktype))
            blocktypeidx = doc->blockMap[blocktype];
    }
    if (n>1) automesh = (lua_todouble(L,2) != 0);
    if (n>2) meshsize = lua_todouble(L,3);
    if (n>3) group = (int) lua_todouble(L,4);

    for (int i=0; i<(int) doc->labellist.size(); i++)
    {
        CSBlockLabel *labelPtr = dynamic_cast<CSBlockLabel*>(doc->labellist[i].get());
        assert(labelPtr);
        if (labelPtr->IsSelected)
        {
            labelPtr->MaxArea = PI*meshsize*meshsize/4.;
            labelPtr->BlockTypeName = blocktype;
            labelPtr->BlockType = blocktypeidx;
            labelPtr->InGroup = group;
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
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_set_node_prop("propname",groupno, "inconductor")}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_setnodeprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetNodeProp(lua_State *L)
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

    int inconductoridx = -1;
    std::string inconductor = "<None>";
    if (!lua_isnil(L,3))
    {
        inconductor = lua_tostring(L,3);
        if (doc->circuitMap.count(inconductor))
            inconductoridx = doc->circuitMap[inconductor];
        else
            debug << "Conductor " << inconductor << " has no index!\n";
    }
    // check to see how many (if any) nodes are selected.
    for(int i=0; i<(int)doc->nodelist.size(); i++)
    {
        if(doc->nodelist[i]->IsSelected)
        {
            doc->nodelist[i]->InGroup = groupno;
            doc->nodelist[i]->BoundaryMarker = nodepropidx;
            doc->nodelist[i]->BoundaryMarkerName = nodeprop;
            doc->nodelist[i]->InConductor = inconductoridx;
            doc->nodelist[i]->InConductorName = inconductor;
        }
    }

    return 0;
}

/**
 * @brief Set properties for the selected segments.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_setsegmentprop("propname", elementsize, automesh, hide, group, "inconductor")}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_setsegmentprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetSegmentProp(lua_State *L)
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

    int inconductoridx = -1;
    std::string inconductor = "<None>";
    if (!lua_isnil(L,6))
    {
        inconductor = lua_tostring(L,6);
        if (doc->circuitMap.count(inconductor))
            inconductoridx = doc->circuitMap[inconductor];
        else
            debug << "Conductor " << inconductor << " has no index!\n";
    }

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
            doc->linelist[i]->InConductor = inconductoridx;
            doc->linelist[i]->InConductorName = inconductor;
        }
    }

    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_set_focus}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_switchfocus()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetFocus(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
}

