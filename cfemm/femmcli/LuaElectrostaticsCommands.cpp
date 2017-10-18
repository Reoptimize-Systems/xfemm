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

#include "CSPointVals.h"
#include "epproc.h"
#include "esolver.h"
#include "femmconstants.h"
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
    li.addFunction("ei_add_bound_prop", luaAddBoundaryProperty);
    li.addFunction("ei_addboundprop", luaAddBoundaryProperty);
    li.addFunction("ei_add_conductor_prop", luaAddConductorProperty);
    li.addFunction("ei_addconductorprop", luaAddConductorProperty);
    li.addFunction("ei_add_material", luaAddMaterialProperty);
    li.addFunction("ei_addmaterial", luaAddMaterialProperty);
    li.addFunction("ei_add_node", LuaCommonCommands::luaAddNode);
    li.addFunction("ei_addnode", LuaCommonCommands::luaAddNode);
    li.addFunction("ei_add_point_prop", luaAddPointProperty);
    li.addFunction("ei_addpointprop", luaAddPointProperty);
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
    li.addFunction("ei_delete_bound_prop", LuaCommonCommands::luaDeleteBoundaryProperty);
    li.addFunction("ei_deleteboundprop", LuaCommonCommands::luaDeleteBoundaryProperty);
    li.addFunction("ei_delete_conductor", LuaCommonCommands::luaDeleteCircuitProperty);
    li.addFunction("ei_deleteconductor", LuaCommonCommands::luaDeleteCircuitProperty);
    li.addFunction("ei_delete_material", LuaCommonCommands::luaDeleteMaterial);
    li.addFunction("ei_deletematerial", LuaCommonCommands::luaDeleteMaterial);
    li.addFunction("ei_delete_point_prop", LuaCommonCommands::luaDeletePointProperty);
    li.addFunction("ei_deletepointprop", LuaCommonCommands::luaDeletePointProperty);
    li.addFunction("ei_delete_selected_arcsegments", LuaCommonCommands::luaDeleteSelectedArcSegments);
    li.addFunction("ei_deleteselectedarcsegments", LuaCommonCommands::luaDeleteSelectedArcSegments);
    li.addFunction("ei_delete_selected_labels", LuaCommonCommands::luaDeleteSelectedBlockLabels);
    li.addFunction("ei_deleteselectedlabels", LuaCommonCommands::luaDeleteSelectedBlockLabels);
    li.addFunction("ei_delete_selected", LuaCommonCommands::luaDeleteSelected);
    li.addFunction("ei_deleteselected", LuaCommonCommands::luaDeleteSelected);
    li.addFunction("ei_delete_selected_nodes", LuaCommonCommands::luaDeleteSelectedNodes);
    li.addFunction("ei_deleteselectednodes", LuaCommonCommands::luaDeleteSelectedNodes);
    li.addFunction("ei_delete_selected_segments", LuaCommonCommands::luaDeleteSelectedSegments);
    li.addFunction("ei_deleteselectedsegments", LuaCommonCommands::luaDeleteSelectedSegments);
    li.addFunction("ei_detach_default", LuaCommonCommands::luaDetachDefault);
    li.addFunction("ei_detachdefault", LuaCommonCommands::luaDetachDefault);
    li.addFunction("ei_detach_outer_space", LuaCommonCommands::luaDetachOuterSpace);
    li.addFunction("ei_detachouterspace", LuaCommonCommands::luaDetachOuterSpace);
    li.addFunction("ei_getboundingbox", LuaCommonCommands::luaGetBoundingBox);
    li.addFunction("ei_get_material", luaGetMaterialFromLib);
    li.addFunction("ei_getmaterial", luaGetMaterialFromLib);
    li.addFunction("ei_getprobleminfo", LuaCommonCommands::luaGetProblemInfo);
    li.addFunction("ei_get_title", LuaCommonCommands::luaGetTitle);
    li.addFunction("ei_gettitle", LuaCommonCommands::luaGetTitle);
    li.addFunction("ei_grid_snap", LuaInstance::luaNOP);
    li.addFunction("ei_gridsnap", LuaInstance::luaNOP);
    li.addFunction("ei_hide_grid", LuaInstance::luaNOP);
    li.addFunction("ei_hidegrid", LuaInstance::luaNOP);
    li.addFunction("ei_load_solution", LuaCommonCommands::luaLoadSolution);
    li.addFunction("ei_loadsolution", LuaCommonCommands::luaLoadSolution);
    li.addFunction("ei_maximize", LuaInstance::luaNOP);
    li.addFunction("ei_minimize", LuaInstance::luaNOP);
    li.addFunction("ei_mirror", LuaCommonCommands::luaMirrorCopy);
    li.addFunction("ei_modify_bound_prop", luaModifyBoundaryProperty);
    li.addFunction("ei_modifyboundprop", luaModifyBoundaryProperty);
    li.addFunction("ei_modify_conductor_prop", luaModifyConductorProperty);
    li.addFunction("ei_modifyconductorprop", luaModifyConductorProperty);
    li.addFunction("ei_modify_material", luaModifyMaterialProperty);
    li.addFunction("ei_modifymaterial", luaModifyMaterialProperty);
    li.addFunction("ei_modify_point_prop", luaModifyPointProperty);
    li.addFunction("ei_modifypointprop", luaModifyPointProperty);
    li.addFunction("ei_move_rotate", LuaCommonCommands::luaMoveRotate);
    li.addFunction("ei_moverotate", LuaCommonCommands::luaMoveRotate);
    li.addFunction("ei_move_translate", LuaCommonCommands::luaMoveTranslate);
    li.addFunction("ei_movetranslate", LuaCommonCommands::luaMoveTranslate);
    li.addFunction("ei_new_document", luaNewDocument);
    li.addFunction("ei_newdocument", luaNewDocument);
    li.addFunction("ei_prob_def", luaProblemDefinition);
    li.addFunction("ei_probdef", luaProblemDefinition);
    li.addFunction("ei_purge_mesh", LuaCommonCommands::luaPurgeMesh);
    li.addFunction("ei_purgemesh", LuaCommonCommands::luaPurgeMesh);
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
    li.addFunction("ei_scale", LuaCommonCommands::luaScaleMove);
    li.addFunction("ei_select_arcsegment", LuaCommonCommands::luaSelectArcsegment);
    li.addFunction("ei_selectarcsegment", LuaCommonCommands::luaSelectArcsegment);
    li.addFunction("ei_select_circle", LuaCommonCommands::luaSelectWithinCircle);
    li.addFunction("ei_selectcircle", LuaCommonCommands::luaSelectWithinCircle);
    li.addFunction("ei_select_group", LuaCommonCommands::luaSelectGroup);
    li.addFunction("ei_selectgroup", LuaCommonCommands::luaSelectGroup);
    li.addFunction("ei_select_label", LuaCommonCommands::luaSelectBlocklabel);
    li.addFunction("ei_selectlabel", LuaCommonCommands::luaSelectBlocklabel);
    li.addFunction("ei_select_node", LuaCommonCommands::luaSelectNode);
    li.addFunction("ei_selectnode", LuaCommonCommands::luaSelectNode);
    li.addFunction("ei_select_rectangle", LuaCommonCommands::luaSelectWithinRectangle);
    li.addFunction("ei_selectrectangle", LuaCommonCommands::luaSelectWithinRectangle);
    li.addFunction("ei_select_segment", LuaCommonCommands::luaSelectSegment);
    li.addFunction("ei_selectsegment", LuaCommonCommands::luaSelectSegment);
    li.addFunction("ei_set_arcsegment_prop", luaSetArcsegmentProperty);
    li.addFunction("ei_setarcsegmentprop", luaSetArcsegmentProperty);
    li.addFunction("ei_set_block_prop", LuaCommonCommands::luaSetBlocklabelProperty);
    li.addFunction("ei_setblockprop", LuaCommonCommands::luaSetBlocklabelProperty);
    li.addFunction("ei_set_edit_mode", LuaCommonCommands::luaSetEditMode);
    li.addFunction("ei_seteditmode", LuaCommonCommands::luaSetEditMode);
    li.addFunction("ei_set_focus", LuaCommonCommands::luaSetFocus);
    li.addFunction("ei_setfocus", LuaCommonCommands::luaSetFocus);
    li.addFunction("ei_set_grid", LuaInstance::luaNOP);
    li.addFunction("ei_setgrid", LuaInstance::luaNOP);
    li.addFunction("ei_set_group", LuaCommonCommands::luaSetGroup);
    li.addFunction("ei_setgroup", LuaCommonCommands::luaSetGroup);
    li.addFunction("ei_set_node_prop", luaSetNodeProperty);
    li.addFunction("ei_setnodeprop", luaSetNodeProperty);
    li.addFunction("ei_set_segment_prop", luaSetSegmentProperty);
    li.addFunction("ei_setsegmentprop", luaSetSegmentProperty);
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
    li.addFunction("eo_add_contour", LuaCommonCommands::luaAddContourPoint);
    li.addFunction("eo_addcontour", LuaCommonCommands::luaAddContourPoint);
    li.addFunction("eo_bend_contour", LuaCommonCommands::luaBendContourLine);
    li.addFunction("eo_bendcontour", LuaCommonCommands::luaBendContourLine);
    li.addFunction("eo_block_integral", luaBlockIntegral);
    li.addFunction("eo_blockintegral", luaBlockIntegral);
    li.addFunction("eo_clear_block", LuaCommonCommands::luaClearBlockSelection);
    li.addFunction("eo_clearblock", LuaCommonCommands::luaClearBlockSelection);
    li.addFunction("eo_clear_contour", LuaCommonCommands::luaClearContourPoint);
    li.addFunction("eo_clearcontour", LuaCommonCommands::luaClearContourPoint);
    li.addFunction("eo_close", LuaCommonCommands::luaExitPost);
    li.addFunction("eo_get_conductor_properties", LuaCommonCommands::luaGetConductorProperties);
    li.addFunction("eo_getconductorproperties", LuaCommonCommands::luaGetConductorProperties);
    li.addFunction("eo_get_element", LuaCommonCommands::luaGetElement);
    li.addFunction("eo_getelement", LuaCommonCommands::luaGetElement);
    li.addFunction("eo_get_node", LuaCommonCommands::luaGetMeshNode);
    li.addFunction("eo_getnode", LuaCommonCommands::luaGetMeshNode);
    li.addFunction("eo_get_point_values", luaGetPointValues);
    li.addFunction("eo_getpointvalues", luaGetPointValues);
    li.addFunction("eo_get_problem_info", LuaCommonCommands::luaGetProblemInfo);
    li.addFunction("eo_getprobleminfo", LuaCommonCommands::luaGetProblemInfo);
    li.addFunction("eo_get_title", LuaCommonCommands::luaGetTitle);
    li.addFunction("eo_gettitle", LuaCommonCommands::luaGetTitle);
    li.addFunction("eo_grid_snap", LuaInstance::luaNOP);
    li.addFunction("eo_gridsnap", LuaInstance::luaNOP);
    li.addFunction("eo_group_select_block", LuaCommonCommands::luaGroupSelectBlock);
    li.addFunction("eo_groupselectblock", LuaCommonCommands::luaGroupSelectBlock);
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
    li.addFunction("eo_num_elements", LuaCommonCommands::luaNumElements);
    li.addFunction("eo_numelements", LuaCommonCommands::luaNumElements);
    li.addFunction("eo_num_nodes", LuaCommonCommands::luaNumNodes);
    li.addFunction("eo_numnodes", LuaCommonCommands::luaNumNodes);
    li.addFunction("eo_refresh_view", LuaInstance::luaNOP);
    li.addFunction("eo_refreshview", LuaInstance::luaNOP);
    li.addFunction("eo_reload", LuaCommonCommands::luaLoadSolution);
    li.addFunction("eo_resize", LuaInstance::luaNOP);
    li.addFunction("eo_restore", LuaInstance::luaNOP);
    li.addFunction("eo_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("eo_savebitmap", LuaInstance::luaNOP);
    li.addFunction("eo_save_metafile", LuaInstance::luaNOP);
    li.addFunction("eo_savemetafile", LuaInstance::luaNOP);
    li.addFunction("eo_select_block", LuaCommonCommands::luaSelectOutputBlocklabel);
    li.addFunction("eo_selectblock", LuaCommonCommands::luaSelectOutputBlocklabel);
    li.addFunction("eo_select_conductor", luaSelectConductor);
    li.addFunction("eo_selectconductor", luaSelectConductor);
    li.addFunction("eo_select_point", LuaCommonCommands::luaAddContourPointFromNode);
    li.addFunction("eo_selectpoint", LuaCommonCommands::luaAddContourPointFromNode);
    li.addFunction("eo_set_edit_mode", LuaInstance::luaNOP);
    li.addFunction("eo_seteditmode", LuaInstance::luaNOP);
    li.addFunction("eo_set_focus", LuaCommonCommands::luaSetFocus);
    li.addFunction("eo_setfocus", LuaCommonCommands::luaSetFocus);
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
    li.addFunction("eo_smooth", LuaCommonCommands::luaSetSmoothing);
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
int femmcli::LuaElectrostaticsCommands::luaAddBoundaryProperty(lua_State *L)
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
int femmcli::LuaElectrostaticsCommands::luaAddConductorProperty(lua_State *L)
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
int femmcli::LuaElectrostaticsCommands::luaAddMaterialProperty(lua_State *L)
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
int femmcli::LuaElectrostaticsCommands::luaAddPointProperty(lua_State *L)
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
            mesherDoc->problem->unselectAll();
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
 * @brief Calculate a block integral for the selected blocks.
 * @param L
 * @return 1 or 2 on success, 0 otherwise
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_blockintegral(type)}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_blockintegral()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaBlockIntegral(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<ElectrostaticsPostProcessor> pproc = std::dynamic_pointer_cast<ElectrostaticsPostProcessor>(femmState->getPostProcessor());
    if (!pproc)
    {
        lua_error(L,"No electrostatics output in focus");
        return 0;
    }

    int type = (int) lua_todouble(L,1);
    if((type<0) || (type>6))
    {
        lua_error(L, "Invalid block integral type selected");
        return 0;
    }
    bool hasSelectedItems=false;
    for (const auto &block: pproc->getProblem()->labellist )
    {
        if (block->IsSelected)
        {
            hasSelectedItems = true;
            break;
        }
    }
    if (type>=5 && !hasSelectedItems)
        for (const auto &node: pproc->getMeshNodes())
        {
            const femmsolver::CSMeshNode *snode = dynamic_cast<femmsolver::CSMeshNode*>(node.get());
            assert(snode);
            if (snode->IsSelected)
            {
                hasSelectedItems=true;
                break;
            }
        }
    if(!hasSelectedItems)
    {
        lua_error(L,"Cannot integrate\nNo area has been selected");
        return 0;
    }
    if(type>=5)
        pproc->makeMask();
    CComplex z=pproc->blockIntegral(type);

    lua_pushnumber(L,z.re);
    lua_pushnumber(L,z.im);

    return 2;
}

/**
 * @brief Read the file matlib.dat and extract a named material property.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_getmaterial("materialname")}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_getmaterial()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetMaterialFromLib(lua_State *L)
{
    // TODO(ZaJ): merge this with LuaMagneticsCommands::luaGetMaterialFromLib
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

    std::ifstream input;
    input.open(matlib.c_str(), std::ifstream::in);
    if (!input.is_open())
    {
        std::string msg = "Couldn't open " + matlib + "\n";
        lua_error(L,msg.c_str());
        return 0;
    }

    std::stringstream err;
    while (input && err.str().empty())
    {
        std::unique_ptr<CMaterialProp> prop;
        prop = std::make_unique<CSMaterialProp>(CSMaterialProp::fromStream(input, err));
        if (prop->BlockName == matname)
        {
            doc->blockproplist.push_back(std::move(prop));
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
 * @brief Get the solution values for a point.
 * @param L
 * @return 0 on error (e.g. point not in triangle), otherwise 8
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_getpointvalues(X,Y)}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_getpointvals()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaGetPointValues(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);

    auto femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<ElectrostaticsPostProcessor> pproc = std::dynamic_pointer_cast<ElectrostaticsPostProcessor>(femmState->getPostProcessor());
    if (!pproc)
    {
        lua_error(L,"No electrostatics output in focus");
        return 0;
    }

    double px,py;
    px=lua_tonumber(L,1).re;
    py=lua_tonumber(L,2).re;

    CSPointVals u;
    if(pproc->getPointValues(px, py, u))
    {
        lua_pushnumber(L,u.V);
        lua_pushnumber(L,u.D.re);
        lua_pushnumber(L,u.D.im);
        lua_pushnumber(L,u.E.re);
        lua_pushnumber(L,u.E.im);
        lua_pushnumber(L,u.e.re);
        lua_pushnumber(L,u.e.im);
        lua_pushnumber(L,u.nrg);
        return 8;
    }
    if (luaInstance->getGlobal("XFEMM_VERBOSE") != 0)
    {
        std::string msg = "No point value at " + std::to_string(px) + ", " + std::to_string(py) + "\n";
        PrintWarningMsg(msg.c_str());
    }

    return 0;
}

/**
 * @brief  Calculate the line integral for the defined contour.
 * @param L
 * @return 0 on error, otherwise 1 or 2 (depending on integral type)
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_lineintegral(type)}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_lineintegral()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaLineIntegral(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<ElectrostaticsPostProcessor> pproc = std::dynamic_pointer_cast<ElectrostaticsPostProcessor>(femmState->getPostProcessor());
    if (!pproc)
    {
        lua_error(L,"No electrostatics output in focus");
        return 0;
    }

    int type=(int) lua_todouble(L,1);
    // 0 - E.t
    // 1 - D.n
    // 2 - Cont length
    // 3 - Force from stress tensor
    // 4 - Torque from stress tensor

    if (type<0 || type >4)
    {
        std::string msg = "Invalid line integral selected " + std::to_string(type) + "\n";
        lua_error(L,msg.c_str());
        return 0;

    }

    double z[2] = {0,0};
    pproc->lineIntegral(type,z);

    lua_pushnumber(L,z[0]);
    if (type==0 || type==4)
        return 1;

    lua_pushnumber(L,z[1]);
    return 2;
}

/**
 * @brief Modify a field of a boundary property.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_modifyboundprop("BdryName",propnum,value)}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_modboundprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaModifyBoundaryProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    // find the index of the boundary property to modify;
    std::string BdryName = lua_tostring(L,1);
    CSBoundaryProp *m = nullptr;
    for (auto &prop: doc->lineproplist)
    {
        if (BdryName==prop->BdryName) {
            m = dynamic_cast<CSBoundaryProp*>(prop.get());
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
        m->V = lua_todouble(L,3);
        break;
    case 2:
        m->qs = lua_todouble(L,3);
        break;
    case 3:
        m->c0 = lua_todouble(L,3);
        break;
    case 4:
        m->c1 = lua_todouble(L,3);
        break;
    case 5:
        m->BdryFormat = (int)lua_todouble(L,3);
        break;
    default:
        break;
    }
    return 0;
}

/**
 * @brief Modify the given conductor property.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{mi_modifyconductorprop("CondName",propnum,value)}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_modcircprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaModifyConductorProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    if (doc->circproplist.empty())
        return 0;

    // find the index of the material to modify;
    std::string CircName = lua_tostring(L,1);

    auto searchResult = doc->circuitMap.find(CircName);
    // get out of here if there's no matching circuit
    if (searchResult == doc->circuitMap.end())
    {
        debug << "ei_modcondprop(): No conductor of name " << CircName << "\n";
        return 0;
    }
    int idx = searchResult->second;

    int modprop=(int) lua_todouble(L,2);
    CSCircuit *prop = dynamic_cast<CSCircuit*>(doc->circproplist[idx].get());
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
        prop->V = lua_todouble(L,3);
        break;
    case 2:
        prop->q = lua_todouble(L,3);
        break;
    case 3:
        prop->CircType = (int) lua_todouble(L,3);
        break;
    default:
    {
        std::string msg = "ei_modcondprop(): No property with index " + std::to_string(modprop) + "\n";
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
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_modifymaterial("BlockName",propnum,value)}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_modmatprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaModifyMaterialProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    // find the index of the material to modify;
    std::string BlockName = lua_tostring(L,1);
    CSMaterialProp *m = nullptr;
    for (auto &prop: doc->blockproplist)
    {
        if (BlockName==prop->BlockName) {
            m = dynamic_cast<CSMaterialProp*>(prop.get());
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
        m->ex = lua_todouble(L,3);
        break;
    case 2:
        m->ey = lua_todouble(L,3);
        break;
    case 3:
        m->qv = lua_todouble(L,3);
        break;
    default:
        break;
    }

    return 0;
}

/**
 * @brief Modify a field of a point property.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_modifypointprop("PointName",propnum,value)}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_modpointprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaModifyPointProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    // find the index of the material to modify;
    std::string PointName = lua_tostring(L,1);
    CSPointProp *p = nullptr;
    for (const auto &prop:doc->nodeproplist)
    {
        if (PointName==prop->PointName) {
            p = dynamic_cast<CSPointProp*>(prop.get());
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
        p->V = lua_todouble(L,3);
        break;
    case 2:
        p->qp = lua_todouble(L,3);
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
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_newdocument()}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_newdocument()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaNewDocument(lua_State *L)
{
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(LuaInstance::instance(L)->femmState());
    femmState->setDocument(std::make_shared<femm::FemmProblem>(femm::FileType::ElectrostaticsFile));
    return 0;
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
int femmcli::LuaElectrostaticsCommands::luaProblemDefinition(lua_State *L)
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
    if(type=="planar") esDoc->problemType = PLANAR;
    else if(type=="axi") esDoc->problemType = AXISYMMETRIC;
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
 * @brief Selects all nodes, segments, and arc segments that are part
 * of the conductor specified by the string ("name").
 *
 * This command is used to select conductors for the purposes of the
 * “weighted stress tensor” force and torque integrals, where the
 * conductors are points or surfaces, rather than regions (i.e. can’t
 * be selected with eo_selectblock).
 *
 * This command is specific to electrostatics problems.
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_selectconductor}
 *
 * ### FEMM sources:
 * - \femm42{femm/belaviewLua.cpp,lua_selectconductor()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectConductor(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<ElectrostaticsPostProcessor> pproc = std::dynamic_pointer_cast<ElectrostaticsPostProcessor>(femmState->getPostProcessor());
    if (!pproc)
    {
        lua_error(L,"No electrostatics output in focus");
        return 0;
    }
    int n=lua_gettop(L);
    if (n==0)
        return 0;

    std::string conductorName = lua_tostring(L,1);

    // find conductor index
    const auto doc = pproc->getProblem();
    auto searchResult = doc->circuitMap.find(conductorName);
    if ( searchResult == doc->circuitMap.end())
        return 0;
    int idx = searchResult->second;
    pproc->selectConductor(idx);

    return 0;
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
int femmcli::LuaElectrostaticsCommands::luaSetArcsegmentProperty(lua_State *L)
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
int femmcli::LuaElectrostaticsCommands::luaSetNodeProperty(lua_State *L)
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
int femmcli::LuaElectrostaticsCommands::luaSetSegmentProperty(lua_State *L)
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

