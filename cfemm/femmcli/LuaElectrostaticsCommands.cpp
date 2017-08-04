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
    li.addFunction("ei_add_arc", luaAddArc);
    li.addFunction("ei_addarc", luaAddArc);
    li.addFunction("ei_add_block_label", luaAddBlocklabel);
    li.addFunction("ei_addblocklabel", luaAddBlocklabel);
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
    li.addFunction("ei_add_segment", luaAddLine);
    li.addFunction("ei_addsegment", luaAddLine);
    li.addFunction("ei_analyse", luaAnalyze);
    li.addFunction("ei_analyze", luaAnalyze);
    li.addFunction("ei_attach_default", luaAttachDefault);
    li.addFunction("ei_attachdefault", luaAttachDefault);
    li.addFunction("ei_attach_outer_space", luaAttachOuterSpace);
    li.addFunction("ei_attachouterspace", luaAttachOuterSpace);
    li.addFunction("ei_clear_selected", LuaCommonCommands::luaClearSelected);
    li.addFunction("ei_clearselected", LuaCommonCommands::luaClearSelected);
    li.addFunction("ei_close", luaExitPre);
    li.addFunction("ei_copy_rotate", luaCopyRotate);
    li.addFunction("ei_copyrotate", luaCopyRotate);
    li.addFunction("ei_copy_translate", luaCopyTranslate);
    li.addFunction("ei_copytranslate", luaCopyTranslate);
    li.addFunction("ei_create_mesh", luaCreateMesh);
    li.addFunction("ei_createmesh", luaCreateMesh);
    li.addFunction("ei_create_radius", luaCreateRadius);
    li.addFunction("ei_createradius", luaCreateRadius);
    li.addFunction("ei_define_outer_space", luaDefineOuterSpace);
    li.addFunction("ei_defineouterspace", luaDefineOuterSpace);
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
    li.addFunction("ei_save_as", luaSaveDocument);
    li.addFunction("ei_saveas", luaSaveDocument);
    li.addFunction("ei_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("ei_savebitmap", LuaInstance::luaNOP);
    li.addFunction("ei_save_dxf", LuaInstance::luaNOP);
    li.addFunction("ei_savedxf", LuaInstance::luaNOP);
    li.addFunction("ei_save_metafile", LuaInstance::luaNOP);
    li.addFunction("ei_savemetafile", LuaInstance::luaNOP);
    li.addFunction("ei_scale", luaScaleMove);
    li.addFunction("ei_select_arcsegment", luaSelectArcsegment);
    li.addFunction("ei_selectarcsegment", luaSelectArcsegment);
    li.addFunction("ei_select_circle", luaSelectWithinCircle);
    li.addFunction("ei_selectcircle", luaSelectWithinCircle);
    li.addFunction("ei_select_group", luaSelectGroup);
    li.addFunction("ei_selectgroup", luaSelectGroup);
    li.addFunction("ei_select_label", luaSelectBlocklabel);
    li.addFunction("ei_selectlabel", luaSelectBlocklabel);
    li.addFunction("ei_select_node", luaSelectnode);
    li.addFunction("ei_selectnode", luaSelectnode);
    li.addFunction("ei_select_rectangle", luaSelectWithinRectangle);
    li.addFunction("ei_selectrectangle", luaSelectWithinRectangle);
    li.addFunction("ei_select_segment", luaSelectSegment);
    li.addFunction("ei_selectsegment", luaSelectSegment);
    li.addFunction("ei_set_arcsegment_prop", luaSetArcsegmentProp);
    li.addFunction("ei_setarcsegmentprop", luaSetArcsegmentProp);
    li.addFunction("ei_set_block_prop", luaSetBlocklabelProp);
    li.addFunction("ei_setblockprop", luaSetBlocklabelProp);
    li.addFunction("ei_set_edit_mode", luaSetEditMode);
    li.addFunction("ei_seteditmode", luaSetEditMode);
    li.addFunction("ei_set_focus", luaSetFocus);
    li.addFunction("ei_setfocus", luaSetFocus);
    li.addFunction("ei_set_grid", LuaInstance::luaNOP);
    li.addFunction("ei_setgrid", LuaInstance::luaNOP);
    li.addFunction("ei_set_group", luaSetGroup);
    li.addFunction("ei_setgroup", luaSetGroup);
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_add_arc}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_addarc()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddArc(lua_State *L)
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
 * - \lua{ei_add_bound_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_addboundprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddBoundaryProp(lua_State *L)
{
    lua_error(L, "Not implemented"); return 0;
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{eo_add_contour}
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_add_block_label}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_addlabel()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddBlocklabel(lua_State *L)
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
 * - \lua{ei_add_segment}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_addline()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddLine(lua_State *L)
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_add_point_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_addpointprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAddPointProp(lua_State *L)
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
 * - \lua{ei_analyse}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_analyze()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAnalyze(lua_State *L)
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
 * - \lua{ei_attach_default}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_attachdefault()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAttachDefault(lua_State *L)
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
 * - \lua{ei_attach_outer_space}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_attachouterspace()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaAttachOuterSpace(lua_State *L)
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
 * - \lua{ei_copy_rotate}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_copy_rotate()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaCopyRotate(lua_State *L)
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
 * - \lua{ei_copy_translate}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_copy_translate()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaCopyTranslate(lua_State *L)
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
 * - \lua{ei_create_mesh}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_create_mesh()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaCreateMesh(lua_State *L)
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
 * - \lua{ei_create_radius}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_createradius()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaCreateRadius(lua_State *L)
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
 * - \lua{ei_define_outer_space}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_defineouterspace()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaDefineOuterSpace(lua_State *L)
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
 * - \lua{ei_close}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_exitpre()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaExitPre(lua_State *L)
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
 * - \lua{ei_save_as}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,luaSaveDocument()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSaveDocument(lua_State *L)
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
 * - \lua{ei_select_arcsegment}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_selectarcsegment()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectArcsegment(lua_State *L)
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
 * - \lua{ei_select_label}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_selectlabel()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectBlocklabel(lua_State *L)
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
 * - \lua{ei_select_node}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_selectnode()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectnode(lua_State *L)
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaES
 *
 * \internal
 * ### Implements:
 * - \lua{ei_select_segment}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_selectsegment()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSelectSegment(lua_State *L)
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
 * - \lua{ei_set_arcsegment_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_setarcsegmentprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetArcsegmentProp(lua_State *L)
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
 * - \lua{ei_set_block_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_setblockprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetBlocklabelProp(lua_State *L)
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
 * - \lua{ei_set_edit_mode}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_seteditmode()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetEditMode(lua_State *L)
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
 * - \lua{ei_set_group}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_setgroup()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetGroup(lua_State *L)
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
 * - \lua{ei_set_node_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_setnodeprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetNodeProp(lua_State *L)
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
 * - \lua{ei_set_segment_prop}
 *
 * ### FEMM sources:
 * - \femm42{femm/beladrawLua.cpp,lua_setsegmentprop()}
 * \endinternal
 */
int femmcli::LuaElectrostaticsCommands::luaSetSegmentProp(lua_State *L)
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

