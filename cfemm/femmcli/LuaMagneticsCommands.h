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

#ifndef LUAMAGNETICSCOMMANDS_H
#define LUAMAGNETICSCOMMANDS_H

class lua_State;

namespace femm {
class LuaInstance;
}

namespace femmcli
{

/**
 * LuaMagneticsCommands registers the lua commands related to magnetics.
 * The Lua magnetics command set is described in sections 3.3 and 3.4 of the FEMM manual.
 *
 */
class LuaMagneticsCommands
{
public:
    /**
     * @brief Register the common command set with the given LuaInstance
     * @param li a LuaInstance
     */
    static void registerCommands(femm::LuaInstance &li );

protected:

    static int luaAddarc(lua_State *L);
    static int luaAddbhpoint(lua_State *L);
    static int luaAddboundprop(lua_State *L);
    static int luaAddcircuitprop(lua_State *L);
    static int luaAddcontour(lua_State *L);
    static int luaAddlabel(lua_State *L);
    static int luaAddline(lua_State *L);
    static int luaAddmatprop(lua_State *L);
    static int luaAddnode(lua_State *L);
    static int luaAddpointprop(lua_State *L);
    static int luaAnalyze(lua_State *L);
    static int luaAttachdefault(lua_State *L);
    static int luaAttachouterspace(lua_State *L);
    static int luaBendcontour(lua_State *L);
    static int luaBlockintegral(lua_State *L);
    static int luaClearbhpoints(lua_State *L);
    static int luaClearblock(lua_State *L);
    static int luaClearcontour(lua_State *L);
    static int luaClearselected(lua_State *L);
    static int luaCopyRotate(lua_State *L);
    static int luaCopyTranslate(lua_State *L);
    static int luaCreateMesh(lua_State *L);
    static int luaCreateradius(lua_State *L);
    static int luaDefineouterspace(lua_State *L);
    static int luaDelboundprop(lua_State *L);
    static int luaDelcircuitprop(lua_State *L);
    static int luaDeleteselectedarcsegments(lua_State *L);
    static int luaDeleteselectedlabels(lua_State *L);
    static int luaDeleteselected(lua_State *L);
    static int luaDeleteselectednodes(lua_State *L);
    static int luaDeleteselectedsegments(lua_State *L);
    static int luaDelmatprop(lua_State *L);
    static int luaDelpointprop(lua_State *L);
    static int luaDetachdefault(lua_State *L);
    static int luaDetachouterspace(lua_State *L);
    static int luaExitpost(lua_State *L);
    static int luaExitpre(lua_State *L);
    static int luaGetboundingbox(lua_State *L);
    static int luaGetcircuitprops(lua_State *L);
    static int luaGetelement(lua_State *L);
    static int luaGetmaterial(lua_State *L);
    static int luaGetnode(lua_State *L);
    static int luaGetpointvals(lua_State *L);
    static int luaGetprobleminfo(lua_State *L);
    static int luaGettitle(lua_State *L);
    static int luaGradient(lua_State *L);
    static int luaGridsnap(lua_State *L);
    static int luaGroupselectblock(lua_State *L);
    static int luaHidecountour(lua_State *L);
    static int luaHidedensity(lua_State *L);
    static int luaHidegrid(lua_State *L);
    static int luaHidemesh(lua_State *L);
    static int luaHidepoints(lua_State *L);
    static int luaLineintegral(lua_State *L);
    static int luaMakeplot(lua_State *L);
    static int luaMaximize(lua_State *L);
    static int luaMinimize(lua_State *L);
    static int luaMirror(lua_State *L);
    static int luaModboundprop(lua_State *L);
    static int luaModcircprop(lua_State *L);
    static int luaModmatprop(lua_State *L);
    static int luaModpointprop(lua_State *L);
    static int luaMoveRotate(lua_State *L);
    static int luaMoveTranslate(lua_State *L);
    static int luaNewdocument(lua_State *L);
    static int luaNumelements(lua_State *L);
    static int luaNumnodes(lua_State *L);
    static int luaPrevious(lua_State *L);
    static int luaProbDef(lua_State *L);
    static int luaPurgeMesh(lua_State *L);
    static int luaReaddxf(lua_State *L);
    static int luaRefreshview(lua_State *L);
    static int luaReload(lua_State *L);
    static int luaResize(lua_State *L);
    static int luaRestore(lua_State *L);
    static int luaRunpost(lua_State *L);
    static int luaSavebitmap(lua_State *L);
    static int luaSaveDocument(lua_State *L);
    static int luaSavedxf(lua_State *L);
    static int luaSaveWMF(lua_State *L);
    static int luaScale(lua_State *L);
    static int luaSelectarcsegment(lua_State *L);
    static int luaSelectblock(lua_State *L);
    static int luaSelectcircle(lua_State *L);
    static int luaSelectgroup(lua_State *L);
    static int luaSelectlabel(lua_State *L);
    static int luaSelectline(lua_State *L);
    static int luaSelectnode(lua_State *L);
    static int luaSelectrectangle(lua_State *L);
    static int luaSelectsegment(lua_State *L);
    static int luaSetarcsegmentprop(lua_State *L);
    static int luaSetblockprop(lua_State *L);
    static int luaSeteditmode(lua_State *L);
    static int luaSetgrid(lua_State *L);
    static int luaSetgroup(lua_State *L);
    static int luaSetnodeprop(lua_State *L);
    static int luaSetsegmentprop(lua_State *L);
    static int luaShowcountour(lua_State *L);
    static int luaShowdensity(lua_State *L);
    static int luaShowgrid(lua_State *L);
    static int luaShowMesh(lua_State *L);
    static int luaShowmesh(lua_State *L);
    static int luaShownames(lua_State *L);
    static int luaShowpoints(lua_State *L);
    static int luaSmoothing(lua_State *L);
    static int luaSwitchfocus(lua_State *L);
    static int luaUpdatewindow(lua_State *L);
    static int luaVectorplot(lua_State *L);
    static int luaZoomin(lua_State *L);
    static int luaZoom(lua_State *L);
    static int luaZoomnatural(lua_State *L);
    static int luaZoomout(lua_State *L);
};

} /* namespace FemmLua*/

#endif /* LUAMAGNETICSCOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4:
