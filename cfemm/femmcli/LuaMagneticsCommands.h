/* Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
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

    static int luaAddArc(lua_State *L);
    static int luaAddBHPoint(lua_State *L);
    static int luaAddBoundaryProp(lua_State *L);
    static int luaAddCircuitProp(lua_State *L);
    static int luaAddcontourNOP(lua_State *L);
    static int luaAddBlocklabel(lua_State *L);
    static int luaAddLine(lua_State *L);
    static int luaAddMatProp(lua_State *L);
    static int luaAddNode(lua_State *L);
    static int luaAddPointProp(lua_State *L);
    static int luaAnalyze(lua_State *L);
    static int luaAttachDefault(lua_State *L);
    static int luaAttachOuterSpace(lua_State *L);
    static int luaBendcontourNOP(lua_State *L);
    static int luaBlockIntegral(lua_State *L);
    static int luaClearBHPoints(lua_State *L);
    static int luaClearBlock(lua_State *L);
    static int luaClearcontourNOP(lua_State *L);
    static int luaClearSelected(lua_State *L);
    static int luaCopyRotate(lua_State *L);
    static int luaCopyTranslate(lua_State *L);
    static int luaCreateMesh(lua_State *L);
    static int luaCreateradiusNOP(lua_State *L);
    static int luaDefineOuterSpace(lua_State *L);
    static int luaDeleteBoundaryProperty(lua_State *L);
    static int luaDeleteCircuitProperty(lua_State *L);
    static int luaDeleteSelectedArcSegments(lua_State *L);
    static int luaDeleteSelectedBlockLabels(lua_State *L);
    static int luaDeleteSelected(lua_State *L);
    static int luaDeleteSelectedNodes(lua_State *L);
    static int luaDeleteSelectedSegments(lua_State *L);
    static int luaDeleteMaterial(lua_State *L);
    static int luaDeletePointProperty(lua_State *L);
    static int luaDetachDefault(lua_State *L);
    static int luaDetachOuterSpace(lua_State *L);
    static int luaExitPost(lua_State *L);
    static int luaExitPre(lua_State *L);
    static int luaGetBoundingBox(lua_State *L);
    static int luaGetCircuitProperties(lua_State *L);
    static int luaGetElement(lua_State *L);
    static int luaGetMaterialFromLib(lua_State *L);
    static int luaGetnodeNOP(lua_State *L);
    static int luaGetPointVals(lua_State *L);
    static int luaGetProblemInfo(lua_State *L);
    static int luaGetTitle(lua_State *L);
    static int luaGradientNOP(lua_State *L);
    static int luaGroupSelectBlock(lua_State *L);
    static int luaLineintegralNOP(lua_State *L);
    static int luaMakeplotNOP(lua_State *L);
    static int luaMirrorNOP(lua_State *L);
    static int luaModboundpropNOP(lua_State *L);
    static int luaModifyCircuitProperty(lua_State *L);
    static int luaModmatpropNOP(lua_State *L);
    static int luaModpointpropNOP(lua_State *L);
    static int luaMoveRotate(lua_State *L);
    static int luaMoveTranslate(lua_State *L);
    static int luaNewDocument(lua_State *L);
    static int luaNumElements(lua_State *L);
    static int luaNumNodes(lua_State *L);
    static int luaPreviousNOP(lua_State *L);
    static int luaProbDef(lua_State *L);
    static int luaPurgeMeshNOP(lua_State *L);
    static int luaReloadNOP(lua_State *L);
    static int luaLoadSolution(lua_State *L);
    static int luaSaveDocument(lua_State *L);
    static int luaScaleNOP(lua_State *L);
    static int luaSelectArcsegment(lua_State *L);
    static int luaSelectBlockNOP(lua_State *L);
    static int luaSelectcircleNOP(lua_State *L);
    static int luaSelectGroup(lua_State *L);
    static int luaSelectBlocklabel(lua_State *L);
    static int luaSelectlineNOP(lua_State *L);
    static int luaSelectnode(lua_State *L);
    static int luaSelectrectangleNOP(lua_State *L);
    static int luaSelectSegment(lua_State *L);
    static int luaSetArcsegmentProp(lua_State *L);
    static int luaSetBlocklabelProp(lua_State *L);
    static int luaSetEditMode(lua_State *L);
    static int luaSetgroupNOP(lua_State *L);
    static int luaSetNodeProp(lua_State *L);
    static int luaSetSegmentProp(lua_State *L);
};

} /* namespace FemmLua*/

#endif /* LUAMAGNETICSCOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4:

