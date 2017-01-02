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

#include "CPointVals.h"
#include "FemmState.h"
#include "fpproc.h"
#include "LuaInstance.h"

#include <lua.h>

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
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addarc()}
 */
int femmcli::LuaMagneticsCommands::luaAddarc(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addbhpoint()}
 */
int femmcli::LuaMagneticsCommands::luaAddbhpoint(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addboundprop()}
 */
int femmcli::LuaMagneticsCommands::luaAddboundprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addcircuitprop()}
 */
int femmcli::LuaMagneticsCommands::luaAddcircuitprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_addcontour()}
 */
int femmcli::LuaMagneticsCommands::luaAddcontour(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addlabel()}
 */
int femmcli::LuaMagneticsCommands::luaAddlabel(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addline()}
 */
int femmcli::LuaMagneticsCommands::luaAddline(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addmatprop()}
 */
int femmcli::LuaMagneticsCommands::luaAddmatprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addnode()}
 */
int femmcli::LuaMagneticsCommands::luaAddnode(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_addpointprop()}
 */
int femmcli::LuaMagneticsCommands::luaAddpointprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
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
    std::shared_ptr<femm::MagneticsProblem> magneticsDoc = femmState->magneticsDocument;

    // check to see if all blocklabels are kosher...
    if (magneticsDoc->labellist.size()==0){
        std::string msg = "No block information has been defined\n"
                          "Cannot analyze the problem";
        lua_error(L, msg.c_str());
        return 0;
    }

    bool hasMissingBlockProps = false;
    for(int i=0; i<(int)magneticsDoc->labellist.size(); i++)
    {
        // note(ZaJ): this can be done better by break;ing from the k loop
        int j=0;
        for(int k=0; k<(int)magneticsDoc->blockproplist.size(); k++)
        {
            // FIXME: at this point we want a mesher doc, not a solver doc :-|
            if (magneticsDoc->labellist[i].BlockTypeName != magneticsDoc->blockproplist[k].BlockName)
                j++;
        }
        if ((j==(int)magneticsDoc->blockproplist.size())
                && (magneticsDoc->labellist[i].BlockTypeName!="<No Mesh>")
                )
        {
            // FIXME(ZaJ): check effects of OnBlockOp()
            //if(!hasMissingBlockProps) OnBlockOp();
            hasMissingBlockProps = true;
            magneticsDoc->labellist[i].IsSelected = true;
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


    if (magneticsDoc->ProblemType==AXISYMMETRIC)
    {
        // check to see if all of the input points are on r>=0 for axisymmetric problems.
        for (int k=0; k<(int)magneticsDoc->nodelist.size(); k++)
        {
            if (magneticsDoc->nodelist[k].x < -(1.e-6))
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
        for (int k=0; k<(int)magneticsDoc->labellist.size(); k++)
        {
            if (magneticsDoc->labellist[k].IsExternal)
            {
                if ((magneticsDoc->extRo==0) || (magneticsDoc->extRi==0))
                    hasExteriorProps = false;

                for(int i=0; i<(int)magneticsDoc->blockproplist.size(); i++)
                {
                    if (magneticsDoc->labellist[k].BlockTypeName == magneticsDoc->blockproplist[i].BlockName)
                    {
                        if (magneticsDoc->blockproplist[i].BHpoints!=0)
                            hasAnisotropicMaterial = true;
                        else if(magneticsDoc->blockproplist[i].mu_x != magneticsDoc->blockproplist[i].mu_y)
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

    // TODO(ZaJ) move data from magneticsDoc to mesherDoc
    std::string pathName = magneticsDoc->pathName;
    if (pathName.empty())
    {
        lua_error(L,"A data file must be loaded,\nor the current data must saved.");
        return 0;
    }
    std::shared_ptr<fmesher::FMesher> mesherDoc = femmState->getFMesher();
    if (!mesherDoc->SaveFEMFile(pathName))
        return 0;

    //BeginWaitCursor();
    if (mesherDoc->HasPeriodicBC()){
        if (!mesherDoc->DoPeriodicBCTriangulation(pathName))
        {
            //EndWaitCursor();
            mesherDoc->UnselectAll();
            return 0;
        }
    }
    else{
        if (!mesherDoc->DoNonPeriodicBCTriangulation(pathName))
        {
            //EndWaitCursor();
            return 0;
        }
    }
    //EndWaitCursor();

    // TODO FIXME CONTINUE HERE
    char CommandLine[512];
    CString rootname="\"" + pathName.Left(pathName.ReverseFind('.')) + "\"";

    if(bLinehook==FALSE)
        sprintf(CommandLine,"\"%sfkn.exe\" %s",BinDir,rootname);
    else
        sprintf(CommandLine,"\"%sfkn.exe\" %s bLinehook",BinDir,rootname);

    CString MyPath=pathName.Left(pathName.ReverseFind('\\'));

    STARTUPINFO StartupInfo2 = {0};
    PROCESS_INFORMATION ProcessInfo2;
    StartupInfo2.cb = sizeof(STARTUPINFO);
    if(bLinehook==HiddenLua){
        StartupInfo2.dwFlags = STARTF_USESHOWWINDOW;
        //<DP> SHOWNOACTIVATE doesn't steal focus to others </DP>
        StartupInfo2.wShowWindow =  SW_SHOWNOACTIVATE|SW_MINIMIZE;
    }
    if (CreateProcess(NULL,CommandLine, NULL, NULL, FALSE,
                      0, NULL, MyPath, &StartupInfo2, &ProcessInfo2))
    {
        if(bLinehook!=FALSE)
        {
            DWORD ExitCode;
            hProc=ProcessInfo2.hProcess;
            do{
                GetExitCodeProcess(ProcessInfo2.hProcess,&ExitCode);
                ((CFemmApp *)AfxGetApp())->line_hook(lua,NULL);
                Sleep(1);
            } while(ExitCode==STILL_ACTIVE);
            hProc=NULL;

            if (ExitCode==1)
                MsgBox("Material properties have not been defined for all regions");
            if (ExitCode==1)
                MsgBox("Material properties have not been defined for all regions");
            if (ExitCode==2)
                MsgBox("problem loading mesh");
            if (ExitCode==3)
                MsgBox("problem renumbering node points");
            if (ExitCode==4)
                MsgBox("couldn't allocate enough space for matrices");
            if (ExitCode==5)
                MsgBox("Couldn't solve the problem");
            if (ExitCode==6)
                MsgBox("couldn't write results to disk");
            if (ExitCode==7)
                MsgBox("problem loading input file");
        }
        CloseHandle(ProcessInfo2.hProcess);
        CloseHandle(ProcessInfo2.hThread);
    }
    else
    {
        MsgBox("Problem executing the solver");
        return;
    }

    return 0;

}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_attachdefault()}
 */
int femmcli::LuaMagneticsCommands::luaAttachdefault(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_attachouterspace()}
 */
int femmcli::LuaMagneticsCommands::luaAttachouterspace(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_bendcontour()}
 */
int femmcli::LuaMagneticsCommands::luaBendcontour(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_blockintegral()}
 */
int femmcli::LuaMagneticsCommands::luaBlockintegral(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_clearbhpoints()}
 */
int femmcli::LuaMagneticsCommands::luaClearbhpoints(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_clearblock()}
 */
int femmcli::LuaMagneticsCommands::luaClearblock(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_clearcontour()}
 */
int femmcli::LuaMagneticsCommands::luaClearcontour(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_clearselected()}
 */
int femmcli::LuaMagneticsCommands::luaClearselected(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_copy_rotate()}
 */
int femmcli::LuaMagneticsCommands::luaCopyRotate(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_copy_translate()}
 */
int femmcli::LuaMagneticsCommands::luaCopyTranslate(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_create_mesh()}
 */
int femmcli::LuaMagneticsCommands::luaCreateMesh(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_createradius()}
 */
int femmcli::LuaMagneticsCommands::luaCreateradius(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_defineouterspace()}
 */
int femmcli::LuaMagneticsCommands::luaDefineouterspace(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_delboundprop()}
 */
int femmcli::LuaMagneticsCommands::luaDelboundprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_delcircuitprop()}
 */
int femmcli::LuaMagneticsCommands::luaDelcircuitprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselectedarcsegments()}
 */
int femmcli::LuaMagneticsCommands::luaDeleteselectedarcsegments(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselectedlabels()}
 */
int femmcli::LuaMagneticsCommands::luaDeleteselectedlabels(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselected()}
 */
int femmcli::LuaMagneticsCommands::luaDeleteselected(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselectednodes()}
 */
int femmcli::LuaMagneticsCommands::luaDeleteselectednodes(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_deleteselectedsegments()}
 */
int femmcli::LuaMagneticsCommands::luaDeleteselectedsegments(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_delmatprop()}
 */
int femmcli::LuaMagneticsCommands::luaDelmatprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_delpointprop()}
 */
int femmcli::LuaMagneticsCommands::luaDelpointprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_detachdefault()}
 */
int femmcli::LuaMagneticsCommands::luaDetachdefault(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_detachouterspace()}
 */
int femmcli::LuaMagneticsCommands::luaDetachouterspace(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_exitpost()}
 */
int femmcli::LuaMagneticsCommands::luaExitpost(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_exitpre()}
 */
int femmcli::LuaMagneticsCommands::luaExitpre(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_getboundingbox()}
 */
int femmcli::LuaMagneticsCommands::luaGetboundingbox(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_getcircuitprops()}
 */
int femmcli::LuaMagneticsCommands::luaGetcircuitprops(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_getelement()}
 */
int femmcli::LuaMagneticsCommands::luaGetelement(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_getmaterial()}
 */
int femmcli::LuaMagneticsCommands::luaGetmaterial(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_getnode()}
 */
int femmcli::LuaMagneticsCommands::luaGetnode(lua_State *)
{
    return 0;
}

/**
 * @brief Implements mo_getpointvalues
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_getpointvals()}
 */
int femmcli::LuaMagneticsCommands::luaGetpointvals(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    // for compatibility with 4.0 and 4.1 Lua implementation
    //if (luaInstance->compatibilityMode())
    //    return ((CFemmviewDoc *)pFemmviewdoc)->old_lua_getpointvals(L);

    auto femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    auto fpproc = femmState->getFPProc();
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
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_getprobleminfo()}
 */
int femmcli::LuaMagneticsCommands::luaGetprobleminfo(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_gettitle()}
 */
int femmcli::LuaMagneticsCommands::luaGettitle(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_gradient()}
 */
int femmcli::LuaMagneticsCommands::luaGradient(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_gridsnap()}
 */
int femmcli::LuaMagneticsCommands::luaGridsnap(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_groupselectblock()}
 */
int femmcli::LuaMagneticsCommands::luaGroupselectblock(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_hidecountour()}
 */
int femmcli::LuaMagneticsCommands::luaHidecountour(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_hidedensity()}
 */
int femmcli::LuaMagneticsCommands::luaHidedensity(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_hidegrid()}
 */
int femmcli::LuaMagneticsCommands::luaHidegrid(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_hidemesh()}
 */
int femmcli::LuaMagneticsCommands::luaHidemesh(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_hidepoints()}
 */
int femmcli::LuaMagneticsCommands::luaHidepoints(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_lineintegral()}
 */
int femmcli::LuaMagneticsCommands::luaLineintegral(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_makeplot()}
 */
int femmcli::LuaMagneticsCommands::luaMakeplot(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,luaMaximize()}
 */
int femmcli::LuaMagneticsCommands::luaMaximize(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,luaMinimize()}
 */
int femmcli::LuaMagneticsCommands::luaMinimize(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_mirror()}
 */
int femmcli::LuaMagneticsCommands::luaMirror(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_modboundprop()}
 */
int femmcli::LuaMagneticsCommands::luaModboundprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_modcircprop()}
 */
int femmcli::LuaMagneticsCommands::luaModcircprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_modmatprop()}
 */
int femmcli::LuaMagneticsCommands::luaModmatprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_modpointprop()}
 */
int femmcli::LuaMagneticsCommands::luaModpointprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_move_rotate()}
 */
int femmcli::LuaMagneticsCommands::luaMoveRotate(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_move_translate()}
 */
int femmcli::LuaMagneticsCommands::luaMoveTranslate(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_newdocument()}
 */
int femmcli::LuaMagneticsCommands::luaNewdocument(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_numelements()}
 */
int femmcli::LuaMagneticsCommands::luaNumelements(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_numnodes()}
 */
int femmcli::LuaMagneticsCommands::luaNumnodes(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_previous()}
 */
int femmcli::LuaMagneticsCommands::luaPrevious(lua_State *)
{
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
    std::shared_ptr<femm::MagneticsProblem> magDoc = femmState->magneticsDocument;

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
int femmcli::LuaMagneticsCommands::luaPurgeMesh(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_readdxf()}
 */
int femmcli::LuaMagneticsCommands::luaReaddxf(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_refreshview()}
 */
int femmcli::LuaMagneticsCommands::luaRefreshview(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_reload()}
 */
int femmcli::LuaMagneticsCommands::luaReload(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,luaResize()}
 */
int femmcli::LuaMagneticsCommands::luaResize(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,luaRestore()}
 */
int femmcli::LuaMagneticsCommands::luaRestore(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_runpost()}
 */
int femmcli::LuaMagneticsCommands::luaRunpost(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_savebitmap()}
 */
int femmcli::LuaMagneticsCommands::luaSavebitmap(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,luaSaveDocument()}
 */
int femmcli::LuaMagneticsCommands::luaSaveDocument(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_savedxf()}
 */
int femmcli::LuaMagneticsCommands::luaSavedxf(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_saveWMF()}
 */
int femmcli::LuaMagneticsCommands::luaSaveWMF(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_scale()}
 */
int femmcli::LuaMagneticsCommands::luaScale(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectarcsegment()}
 */
int femmcli::LuaMagneticsCommands::luaSelectarcsegment(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_selectblock()}
 */
int femmcli::LuaMagneticsCommands::luaSelectblock(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectcircle()}
 */
int femmcli::LuaMagneticsCommands::luaSelectcircle(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectgroup()}
 */
int femmcli::LuaMagneticsCommands::luaSelectgroup(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectlabel()}
 */
int femmcli::LuaMagneticsCommands::luaSelectlabel(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_selectline()}
 */
int femmcli::LuaMagneticsCommands::luaSelectline(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectnode()}
 */
int femmcli::LuaMagneticsCommands::luaSelectnode(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectrectangle()}
 */
int femmcli::LuaMagneticsCommands::luaSelectrectangle(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_selectsegment()}
 */
int femmcli::LuaMagneticsCommands::luaSelectsegment(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setarcsegmentprop()}
 */
int femmcli::LuaMagneticsCommands::luaSetarcsegmentprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setblockprop()}
 */
int femmcli::LuaMagneticsCommands::luaSetblockprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_seteditmode()}
 */
int femmcli::LuaMagneticsCommands::luaSeteditmode(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setgrid()}
 */
int femmcli::LuaMagneticsCommands::luaSetgrid(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setgroup()}
 */
int femmcli::LuaMagneticsCommands::luaSetgroup(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setnodeprop()}
 */
int femmcli::LuaMagneticsCommands::luaSetnodeprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_setsegmentprop()}
 */
int femmcli::LuaMagneticsCommands::luaSetsegmentprop(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_showcountour()}
 */
int femmcli::LuaMagneticsCommands::luaShowcountour(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_showdensity()}
 */
int femmcli::LuaMagneticsCommands::luaShowdensity(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_showgrid()}
 */
int femmcli::LuaMagneticsCommands::luaShowgrid(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_show_mesh()}
 */
int femmcli::LuaMagneticsCommands::luaShowMesh(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_showmesh()}
 */
int femmcli::LuaMagneticsCommands::luaShowmesh(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_shownames()}
 */
int femmcli::LuaMagneticsCommands::luaShownames(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_showpoints()}
 */
int femmcli::LuaMagneticsCommands::luaShowpoints(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_smoothing()}
 */
int femmcli::LuaMagneticsCommands::luaSmoothing(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_switchfocus()}
 */
int femmcli::LuaMagneticsCommands::luaSwitchfocus(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_updatewindow()}
 */
int femmcli::LuaMagneticsCommands::luaUpdatewindow(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmviewLua.cpp,lua_vectorplot()}
 */
int femmcli::LuaMagneticsCommands::luaVectorplot(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_zoomin()}
 */
int femmcli::LuaMagneticsCommands::luaZoomin(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_zoom()}
 */
int femmcli::LuaMagneticsCommands::luaZoom(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_zoomnatural()}
 */
int femmcli::LuaMagneticsCommands::luaZoomnatural(lua_State *)
{
    return 0;
}

/**
 * @brief FIXME not implemented
 * @param L
 * @return 0
 * \ingroup LuaMM
 * \femm42{femm/femmeLua.cpp,lua_zoomout()}
 */
int femmcli::LuaMagneticsCommands::luaZoomout(lua_State *)
{
    return 0;
}

// vi:expandtab:tabstop=4 shiftwidth=4:
