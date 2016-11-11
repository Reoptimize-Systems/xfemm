/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
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

#include "LuaInstance.h"

#include <lua.h>

#include <iostream>
#include <string>

#ifdef DEBUG_FEMMLUA
#define debug std::cerr
#else
#define debug while(false) std::cerr
#endif

void femm::LuaMagneticsCommands::registerCommands(LuaInstance &li)
{
    li.addFunction("mi_add_arc", luaAddarc);
    li.addFunction("mi_addarc", luaAddarc);
    li.addFunction("mi_add_bh_point", luaAddbhpoint);
    li.addFunction("mi_addbhpoint", luaAddbhpoint);
    li.addFunction("mi_add_bound_prop", luaAddboundprop);
    li.addFunction("mi_addboundprop", luaAddboundprop);
    li.addFunction("mi_add_circ_prop", luaAddcircuitprop);
    li.addFunction("mi_addcircprop", luaAddcircuitprop);
    li.addFunction("mo_add_contour", luaAddcontour);
    li.addFunction("mo_addcontour", luaAddcontour);
    li.addFunction("mi_add_block_label", luaAddlabel);
    li.addFunction("mi_addblocklabel", luaAddlabel);
    li.addFunction("mi_add_segment", luaAddline);
    li.addFunction("mi_addsegment", luaAddline);
    li.addFunction("mi_add_material", luaAddmatprop);
    li.addFunction("mi_addmaterial", luaAddmatprop);
    li.addFunction("mi_add_node", luaAddnode);
    li.addFunction("mi_addnode", luaAddnode);
    li.addFunction("mi_add_point_prop", luaAddpointprop);
    li.addFunction("mi_addpointprop", luaAddpointprop);
    li.addFunction("mi_analyse", luaAnalyze);
    li.addFunction("mi_analyze", luaAnalyze);
    li.addFunction("mi_attach_default", luaAttachdefault);
    li.addFunction("mi_attachdefault", luaAttachdefault);
    li.addFunction("mi_attach_outer_space", luaAttachouterspace);
    li.addFunction("mi_attachouterspace", luaAttachouterspace);
    li.addFunction("mo_bend_contour", luaBendcontour);
    li.addFunction("mo_bendcontour", luaBendcontour);
    li.addFunction("mo_block_integral", luaBlockintegral);
    li.addFunction("mo_blockintegral", luaBlockintegral);
    li.addFunction("mi_clear_bh_points", luaClearbhpoints);
    li.addFunction("mi_clearbhpoints", luaClearbhpoints);
    li.addFunction("mo_clear_block", luaClearblock);
    li.addFunction("mo_clearblock", luaClearblock);
    li.addFunction("mo_clear_contour", luaClearcontour);
    li.addFunction("mo_clearcontour", luaClearcontour);
    li.addFunction("mi_clear_selected", luaClearselected);
    li.addFunction("mi_clearselected", luaClearselected);
    li.addFunction("mi_copy_rotate", luaCopyRotate);
    li.addFunction("mi_copyrotate", luaCopyRotate);
    li.addFunction("mi_copy_translate", luaCopyTranslate);
    li.addFunction("mi_copytranslate", luaCopyTranslate);
    li.addFunction("mi_create_mesh", luaCreateMesh);
    li.addFunction("mi_createmesh", luaCreateMesh);
    li.addFunction("mi_create_radius", luaCreateradius);
    li.addFunction("mi_createradius", luaCreateradius);
    li.addFunction("mi_define_outer_space", luaDefineouterspace);
    li.addFunction("mi_defineouterspace", luaDefineouterspace);
    li.addFunction("mi_delete_bound_prop", luaDelboundprop);
    li.addFunction("mi_deleteboundprop", luaDelboundprop);
    li.addFunction("mi_delete_circuit", luaDelcircuitprop);
    li.addFunction("mi_deletecircuit", luaDelcircuitprop);
    li.addFunction("mi_delete_selected_arcsegments", luaDeleteselectedarcsegments);
    li.addFunction("mi_deleteselectedarcsegments", luaDeleteselectedarcsegments);
    li.addFunction("mi_delete_selected_labels", luaDeleteselectedlabels);
    li.addFunction("mi_deleteselectedlabels", luaDeleteselectedlabels);
    li.addFunction("mi_delete_selected", luaDeleteselected);
    li.addFunction("mi_deleteselected", luaDeleteselected);
    li.addFunction("mi_delete_selected_nodes", luaDeleteselectednodes);
    li.addFunction("mi_deleteselectednodes", luaDeleteselectednodes);
    li.addFunction("mi_delete_selected_segments", luaDeleteselectedsegments);
    li.addFunction("mi_deleteselectedsegments", luaDeleteselectedsegments);
    li.addFunction("mi_delete_material", luaDelmatprop);
    li.addFunction("mi_deletematerial", luaDelmatprop);
    li.addFunction("mi_delete_point_prop", luaDelpointprop);
    li.addFunction("mi_deletepointprop", luaDelpointprop);
    li.addFunction("mi_detach_default", luaDetachdefault);
    li.addFunction("mi_detachdefault", luaDetachdefault);
    li.addFunction("mi_detach_outer_space", luaDetachouterspace);
    li.addFunction("mi_detachouterspace", luaDetachouterspace);
    li.addFunction("mo_close", luaExitpost);
    li.addFunction("mi_close", luaExitpre);
    li.addFunction("mi_getboundingbox", luaGetboundingbox);
    li.addFunction("mo_get_circuit_properties", luaGetcircuitprops);
    li.addFunction("mo_getcircuitproperties", luaGetcircuitprops);
    li.addFunction("mo_get_element", luaGetelement);
    li.addFunction("mo_getelement", luaGetelement);
    li.addFunction("mi_get_material", luaGetmaterial);
    li.addFunction("mi_getmaterial", luaGetmaterial);
    li.addFunction("mo_get_node", luaGetnode);
    li.addFunction("mo_getnode", luaGetnode);
    li.addFunction("mo_get_point_values", luaGetpointvals);
    li.addFunction("mo_getpointvalues", luaGetpointvals);
    li.addFunction("mi_getprobleminfo", luaGetprobleminfo);
    li.addFunction("mo_get_problem_info", luaGetprobleminfo);
    li.addFunction("mo_getprobleminfo", luaGetprobleminfo);
    li.addFunction("mi_get_title", luaGettitle);
    li.addFunction("mi_gettitle", luaGettitle);
    li.addFunction("mo_get_title", luaGettitle);
    li.addFunction("mo_gettitle", luaGettitle);
    li.addFunction("mo_gradient", luaGradient);
    li.addFunction("mi_grid_snap", luaGridsnap);
    li.addFunction("mi_gridsnap", luaGridsnap);
    li.addFunction("mo_grid_snap", luaGridsnap);
    li.addFunction("mo_gridsnap", luaGridsnap);
    li.addFunction("mo_group_select_block", luaGroupselectblock);
    li.addFunction("mo_groupselectblock", luaGroupselectblock);
    li.addFunction("mo_hide_contour_plot", luaHidecountour);
    li.addFunction("mo_hidecontourplot", luaHidecountour);
    li.addFunction("mo_hide_density_plot", luaHidedensity);
    li.addFunction("mo_hidedensityplot", luaHidedensity);
    li.addFunction("mi_hide_grid", luaHidegrid);
    li.addFunction("mi_hidegrid", luaHidegrid);
    li.addFunction("mo_hide_grid", luaHidegrid);
    li.addFunction("mo_hidegrid", luaHidegrid);
    li.addFunction("mo_hide_mesh", luaHidemesh);
    li.addFunction("mo_hidemesh", luaHidemesh);
    li.addFunction("mo_hide_points", luaHidepoints);
    li.addFunction("mo_hidepoints", luaHidepoints);
    li.addFunction("mo_line_integral", luaLineintegral);
    li.addFunction("mo_lineintegral", luaLineintegral);
    li.addFunction("mo_make_plot", luaMakeplot);
    li.addFunction("mo_makeplot", luaMakeplot);
    li.addFunction("mi_maximize", luaMaximize);
    li.addFunction("mo_maximize", luaMaximize);
    li.addFunction("mi_minimize", luaMinimize);
    li.addFunction("mo_minimize", luaMinimize);
    li.addFunction("mi_mirror", luaMirror);
    li.addFunction("mi_modify_bound_prop", luaModboundprop);
    li.addFunction("mi_modifyboundprop", luaModboundprop);
    li.addFunction("mi_modify_circ_prop", luaModcircprop);
    li.addFunction("mi_modifycircprop", luaModcircprop);
    li.addFunction("mi_modify_material", luaModmatprop);
    li.addFunction("mi_modifymaterial", luaModmatprop);
    li.addFunction("mi_modify_point_prop", luaModpointprop);
    li.addFunction("mi_modifypointprop", luaModpointprop);
    li.addFunction("mi_move_rotate", luaMoveRotate);
    li.addFunction("mi_moverotate", luaMoveRotate);
    li.addFunction("mi_move_translate", luaMoveTranslate);
    li.addFunction("mi_movetranslate", luaMoveTranslate);
    li.addFunction("mi_new_document", luaNewdocument);
    li.addFunction("mi_newdocument", luaNewdocument);
    li.addFunction("mo_num_elements", luaNumelements);
    li.addFunction("mo_numelements", luaNumelements);
    li.addFunction("mo_num_nodes", luaNumnodes);
    li.addFunction("mo_numnodes", luaNumnodes);
    li.addFunction("mi_setprevious", luaPrevious);
    li.addFunction("mi_prob_def", luaProbDef);
    li.addFunction("mi_probdef", luaProbDef);
    li.addFunction("mi_purge_mesh", luaPurgeMesh);
    li.addFunction("mi_purgemesh", luaPurgeMesh);
    li.addFunction("mi_read_dxf", luaReaddxf);
    li.addFunction("mi_readdxf", luaReaddxf);
    li.addFunction("mo_refresh_view", luaRefreshview);
    li.addFunction("mo_refreshview", luaRefreshview);
    li.addFunction("mo_reload", luaReload);
    li.addFunction("mi_resize", luaResize);
    li.addFunction("mo_resize", luaResize);
    li.addFunction("mi_restore", luaRestore);
    li.addFunction("mo_restore", luaRestore);
    li.addFunction("mi_load_solution", luaRunpost);
    li.addFunction("mi_loadsolution", luaRunpost);
    li.addFunction("mi_save_bitmap", luaSavebitmap);
    li.addFunction("mi_savebitmap", luaSavebitmap);
    li.addFunction("mo_save_bitmap", luaSavebitmap);
    li.addFunction("mo_savebitmap", luaSavebitmap);
    li.addFunction("mi_save_as", luaSaveDocument);
    li.addFunction("mi_saveas", luaSaveDocument);
    li.addFunction("mi_save_dxf", luaSavedxf);
    li.addFunction("mi_savedxf", luaSavedxf);
    li.addFunction("mi_save_metafile", luaSaveWMF);
    li.addFunction("mi_savemetafile", luaSaveWMF);
    li.addFunction("mo_save_metafile", luaSaveWMF);
    li.addFunction("mo_savemetafile", luaSaveWMF);
    li.addFunction("mi_scale", luaScale);
    li.addFunction("mi_select_arcsegment", luaSelectarcsegment);
    li.addFunction("mi_selectarcsegment", luaSelectarcsegment);
    li.addFunction("mo_select_block", luaSelectblock);
    li.addFunction("mo_selectblock", luaSelectblock);
    li.addFunction("mi_select_circle", luaSelectcircle);
    li.addFunction("mi_selectcircle", luaSelectcircle);
    li.addFunction("mi_select_group", luaSelectgroup);
    li.addFunction("mi_selectgroup", luaSelectgroup);
    li.addFunction("mi_select_label", luaSelectlabel);
    li.addFunction("mi_selectlabel", luaSelectlabel);
    li.addFunction("mo_select_point", luaSelectline);
    li.addFunction("mo_selectpoint", luaSelectline);
    li.addFunction("mi_select_node", luaSelectnode);
    li.addFunction("mi_selectnode", luaSelectnode);
    li.addFunction("mi_select_rectangle", luaSelectrectangle);
    li.addFunction("mi_selectrectangle", luaSelectrectangle);
    li.addFunction("mi_select_segment", luaSelectsegment);
    li.addFunction("mi_selectsegment", luaSelectsegment);
    li.addFunction("mi_set_arcsegment_prop", luaSetarcsegmentprop);
    li.addFunction("mi_setarcsegmentprop", luaSetarcsegmentprop);
    li.addFunction("mi_set_block_prop", luaSetblockprop);
    li.addFunction("mi_setblockprop", luaSetblockprop);
    li.addFunction("mi_set_edit_mode", luaSeteditmode);
    li.addFunction("mi_seteditmode", luaSeteditmode);
    li.addFunction("mo_set_edit_mode", luaSeteditmode);
    li.addFunction("mo_seteditmode", luaSeteditmode);
    li.addFunction("mi_set_grid", luaSetgrid);
    li.addFunction("mi_setgrid", luaSetgrid);
    li.addFunction("mo_set_grid", luaSetgrid);
    li.addFunction("mo_setgrid", luaSetgrid);
    li.addFunction("mi_set_group", luaSetgroup);
    li.addFunction("mi_setgroup", luaSetgroup);
    li.addFunction("mi_set_node_prop", luaSetnodeprop);
    li.addFunction("mi_setnodeprop", luaSetnodeprop);
    li.addFunction("mi_set_segment_prop", luaSetsegmentprop);
    li.addFunction("mi_setsegmentprop", luaSetsegmentprop);
    li.addFunction("mo_show_contour_plot", luaShowcountour);
    li.addFunction("mo_showcontourplot", luaShowcountour);
    li.addFunction("mo_show_density_plot", luaShowdensity);
    li.addFunction("mo_showdensityplot", luaShowdensity);
    li.addFunction("mi_show_grid", luaShowgrid);
    li.addFunction("mi_showgrid", luaShowgrid);
    li.addFunction("mo_show_grid", luaShowgrid);
    li.addFunction("mo_showgrid", luaShowgrid);
    li.addFunction("mi_show_mesh", luaShowMesh);
    li.addFunction("mi_showmesh", luaShowMesh);
    li.addFunction("mo_show_mesh", luaShowmesh);
    li.addFunction("mo_showmesh", luaShowmesh);
    li.addFunction("mi_show_names", luaShownames);
    li.addFunction("mi_shownames", luaShownames);
    li.addFunction("mo_show_names", luaShownames);
    li.addFunction("mo_shownames", luaShownames);
    li.addFunction("mo_show_points", luaShowpoints);
    li.addFunction("mo_showpoints", luaShowpoints);
    li.addFunction("mo_smooth", luaSmoothing);
    li.addFunction("mi_set_focus", luaSwitchfocus);
    li.addFunction("mi_setfocus", luaSwitchfocus);
    li.addFunction("mo_set_focus", luaSwitchfocus);
    li.addFunction("mo_setfocus", luaSwitchfocus);
    li.addFunction("mi_refresh_view", luaUpdatewindow);
    li.addFunction("mi_refreshview", luaUpdatewindow);
    li.addFunction("mo_show_vector_plot", luaVectorplot);
    li.addFunction("mo_showvectorplot", luaVectorplot);
    li.addFunction("mi_zoom_in", luaZoomin);
    li.addFunction("mi_zoomin", luaZoomin);
    li.addFunction("mo_zoom_in", luaZoomin);
    li.addFunction("mo_zoomin", luaZoomin);
    li.addFunction("mi_zoom", luaZoom);
    li.addFunction("mo_zoom", luaZoom);
    li.addFunction("mi_zoom_natural", luaZoomnatural);
    li.addFunction("mi_zoomnatural", luaZoomnatural);
    li.addFunction("mo_zoom_natural", luaZoomnatural);
    li.addFunction("mo_zoomnatural", luaZoomnatural);
    li.addFunction("mi_zoom_out", luaZoomout);
    li.addFunction("mi_zoomout", luaZoomout);
    li.addFunction("mo_zoom_out", luaZoomout);
    li.addFunction("mo_zoomout", luaZoomout);
}


/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_addarc()
 */
int femm::LuaMagneticsCommands::luaAddarc(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_addbhpoint()
 */
int femm::LuaMagneticsCommands::luaAddbhpoint(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_addboundprop()
 */
int femm::LuaMagneticsCommands::luaAddboundprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_addcircuitprop()
 */
int femm::LuaMagneticsCommands::luaAddcircuitprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_addcontour()
 */
int femm::LuaMagneticsCommands::luaAddcontour(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_addlabel()
 */
int femm::LuaMagneticsCommands::luaAddlabel(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_addline()
 */
int femm::LuaMagneticsCommands::luaAddline(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_addmatprop()
 */
int femm::LuaMagneticsCommands::luaAddmatprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_addnode()
 */
int femm::LuaMagneticsCommands::luaAddnode(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_addpointprop()
 */
int femm::LuaMagneticsCommands::luaAddpointprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_analyze()
 */
int femm::LuaMagneticsCommands::luaAnalyze(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_attachdefault()
 */
int femm::LuaMagneticsCommands::luaAttachdefault(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_attachouterspace()
 */
int femm::LuaMagneticsCommands::luaAttachouterspace(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_bendcontour()
 */
int femm::LuaMagneticsCommands::luaBendcontour(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_blockintegral()
 */
int femm::LuaMagneticsCommands::luaBlockintegral(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_clearbhpoints()
 */
int femm::LuaMagneticsCommands::luaClearbhpoints(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_clearblock()
 */
int femm::LuaMagneticsCommands::luaClearblock(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_clearcontour()
 */
int femm::LuaMagneticsCommands::luaClearcontour(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_clearselected()
 */
int femm::LuaMagneticsCommands::luaClearselected(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_copy_rotate()
 */
int femm::LuaMagneticsCommands::luaCopyRotate(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_copy_translate()
 */
int femm::LuaMagneticsCommands::luaCopyTranslate(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_create_mesh()
 */
int femm::LuaMagneticsCommands::luaCreateMesh(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_createradius()
 */
int femm::LuaMagneticsCommands::luaCreateradius(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_defineouterspace()
 */
int femm::LuaMagneticsCommands::luaDefineouterspace(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_delboundprop()
 */
int femm::LuaMagneticsCommands::luaDelboundprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_delcircuitprop()
 */
int femm::LuaMagneticsCommands::luaDelcircuitprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_deleteselectedarcsegments()
 */
int femm::LuaMagneticsCommands::luaDeleteselectedarcsegments(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_deleteselectedlabels()
 */
int femm::LuaMagneticsCommands::luaDeleteselectedlabels(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_deleteselected()
 */
int femm::LuaMagneticsCommands::luaDeleteselected(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_deleteselectednodes()
 */
int femm::LuaMagneticsCommands::luaDeleteselectednodes(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_deleteselectedsegments()
 */
int femm::LuaMagneticsCommands::luaDeleteselectedsegments(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_delmatprop()
 */
int femm::LuaMagneticsCommands::luaDelmatprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_delpointprop()
 */
int femm::LuaMagneticsCommands::luaDelpointprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_detachdefault()
 */
int femm::LuaMagneticsCommands::luaDetachdefault(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_detachouterspace()
 */
int femm::LuaMagneticsCommands::luaDetachouterspace(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_exitpost()
 */
int femm::LuaMagneticsCommands::luaExitpost(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_exitpre()
 */
int femm::LuaMagneticsCommands::luaExitpre(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_getboundingbox()
 */
int femm::LuaMagneticsCommands::luaGetboundingbox(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_getcircuitprops()
 */
int femm::LuaMagneticsCommands::luaGetcircuitprops(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_getelement()
 */
int femm::LuaMagneticsCommands::luaGetelement(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_getmaterial()
 */
int femm::LuaMagneticsCommands::luaGetmaterial(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_getnode()
 */
int femm::LuaMagneticsCommands::luaGetnode(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_getpointvals()
 */
int femm::LuaMagneticsCommands::luaGetpointvals(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_getprobleminfo()
 */
int femm::LuaMagneticsCommands::luaGetprobleminfo(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_gettitle()
 */
int femm::LuaMagneticsCommands::luaGettitle(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_gradient()
 */
int femm::LuaMagneticsCommands::luaGradient(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_gridsnap()
 */
int femm::LuaMagneticsCommands::luaGridsnap(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_groupselectblock()
 */
int femm::LuaMagneticsCommands::luaGroupselectblock(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_hidecountour()
 */
int femm::LuaMagneticsCommands::luaHidecountour(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_hidedensity()
 */
int femm::LuaMagneticsCommands::luaHidedensity(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_hidegrid()
 */
int femm::LuaMagneticsCommands::luaHidegrid(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_hidemesh()
 */
int femm::LuaMagneticsCommands::luaHidemesh(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_hidepoints()
 */
int femm::LuaMagneticsCommands::luaHidepoints(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_lineintegral()
 */
int femm::LuaMagneticsCommands::luaLineintegral(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_makeplot()
 */
int femm::LuaMagneticsCommands::luaMakeplot(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:luaMaximize()
 */
int femm::LuaMagneticsCommands::luaMaximize(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:luaMinimize()
 */
int femm::LuaMagneticsCommands::luaMinimize(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_mirror()
 */
int femm::LuaMagneticsCommands::luaMirror(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_modboundprop()
 */
int femm::LuaMagneticsCommands::luaModboundprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_modcircprop()
 */
int femm::LuaMagneticsCommands::luaModcircprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_modmatprop()
 */
int femm::LuaMagneticsCommands::luaModmatprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_modpointprop()
 */
int femm::LuaMagneticsCommands::luaModpointprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_move_rotate()
 */
int femm::LuaMagneticsCommands::luaMoveRotate(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_move_translate()
 */
int femm::LuaMagneticsCommands::luaMoveTranslate(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_newdocument()
 */
int femm::LuaMagneticsCommands::luaNewdocument(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_numelements()
 */
int femm::LuaMagneticsCommands::luaNumelements(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_numnodes()
 */
int femm::LuaMagneticsCommands::luaNumnodes(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_previous()
 */
int femm::LuaMagneticsCommands::luaPrevious(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_prob_def()
 */
int femm::LuaMagneticsCommands::luaProbDef(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_purge_mesh()
 */
int femm::LuaMagneticsCommands::luaPurgeMesh(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_readdxf()
 */
int femm::LuaMagneticsCommands::luaReaddxf(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_refreshview()
 */
int femm::LuaMagneticsCommands::luaRefreshview(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_reload()
 */
int femm::LuaMagneticsCommands::luaReload(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:luaResize()
 */
int femm::LuaMagneticsCommands::luaResize(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:luaRestore()
 */
int femm::LuaMagneticsCommands::luaRestore(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_runpost()
 */
int femm::LuaMagneticsCommands::luaRunpost(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_savebitmap()
 */
int femm::LuaMagneticsCommands::luaSavebitmap(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:luaSaveDocument()
 */
int femm::LuaMagneticsCommands::luaSaveDocument(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_savedxf()
 */
int femm::LuaMagneticsCommands::luaSavedxf(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_saveWMF()
 */
int femm::LuaMagneticsCommands::luaSaveWMF(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_scale()
 */
int femm::LuaMagneticsCommands::luaScale(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_selectarcsegment()
 */
int femm::LuaMagneticsCommands::luaSelectarcsegment(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_selectblock()
 */
int femm::LuaMagneticsCommands::luaSelectblock(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_selectcircle()
 */
int femm::LuaMagneticsCommands::luaSelectcircle(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_selectgroup()
 */
int femm::LuaMagneticsCommands::luaSelectgroup(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_selectlabel()
 */
int femm::LuaMagneticsCommands::luaSelectlabel(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_selectline()
 */
int femm::LuaMagneticsCommands::luaSelectline(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_selectnode()
 */
int femm::LuaMagneticsCommands::luaSelectnode(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_selectrectangle()
 */
int femm::LuaMagneticsCommands::luaSelectrectangle(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_selectsegment()
 */
int femm::LuaMagneticsCommands::luaSelectsegment(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_setarcsegmentprop()
 */
int femm::LuaMagneticsCommands::luaSetarcsegmentprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_setblockprop()
 */
int femm::LuaMagneticsCommands::luaSetblockprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_seteditmode()
 */
int femm::LuaMagneticsCommands::luaSeteditmode(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_setgrid()
 */
int femm::LuaMagneticsCommands::luaSetgrid(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_setgroup()
 */
int femm::LuaMagneticsCommands::luaSetgroup(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_setnodeprop()
 */
int femm::LuaMagneticsCommands::luaSetnodeprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_setsegmentprop()
 */
int femm::LuaMagneticsCommands::luaSetsegmentprop(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_showcountour()
 */
int femm::LuaMagneticsCommands::luaShowcountour(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_showdensity()
 */
int femm::LuaMagneticsCommands::luaShowdensity(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_showgrid()
 */
int femm::LuaMagneticsCommands::luaShowgrid(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_show_mesh()
 */
int femm::LuaMagneticsCommands::luaShowMesh(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_showmesh()
 */
int femm::LuaMagneticsCommands::luaShowmesh(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_shownames()
 */
int femm::LuaMagneticsCommands::luaShownames(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_showpoints()
 */
int femm::LuaMagneticsCommands::luaShowpoints(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_smoothing()
 */
int femm::LuaMagneticsCommands::luaSmoothing(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_switchfocus()
 */
int femm::LuaMagneticsCommands::luaSwitchfocus(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_updatewindow()
 */
int femm::LuaMagneticsCommands::luaUpdatewindow(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmviewLua.cpp:lua_vectorplot()
 */
int femm::LuaMagneticsCommands::luaVectorplot(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_zoomin()
 */
int femm::LuaMagneticsCommands::luaZoomin(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_zoom()
 */
int femm::LuaMagneticsCommands::luaZoom(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_zoomnatural()
 */
int femm::LuaMagneticsCommands::luaZoomnatural(lua_State *L)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup mm
 * \sa FEMM42/femmeLua.cpp:lua_zoomout()
 */
int femm::LuaMagneticsCommands::luaZoomout(lua_State *L)
{
    return 0;
}

// vi:expandtab:tabstop=4 shiftwidth=4:
