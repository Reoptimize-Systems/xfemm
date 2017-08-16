/* Copyright 2017 Johannes Zarl-Zierl <johannes@zarl-zierl.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of 
 * Mechatronics GmbH (LCM)
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

#ifndef LUAELECTROSTATICSCOMMANDS_H
#define LUAELECTROSTATICSCOMMANDS_H

class lua_State;

namespace femm {
class LuaInstance;
}

namespace femmcli
{

/**
 * LuaElectrostaticsCommands provides the lua commands related to electrostatics.
 * The Lua magnetics command set is described in sections 3.5 and 3.6 of the FEMM manual.
 *
 */
namespace LuaElectrostaticsCommands
{
/**
 * @brief Register the common command set with the given LuaInstance
 * @param li a LuaInstance
 */
void registerCommands(femm::LuaInstance &li );

int luaAddBoundaryProp(lua_State *L);
int luaAddConductorProp(lua_State *L);
int luaAddContourPointFromNode(lua_State *L);
int luaAddContourPoint(lua_State *L);
int luaAddMaterialProp(lua_State *L);
int luaAddPointProp(lua_State *L);
int luaAnalyze(lua_State *L);
int luaBendContourLine(lua_State *L);
int luaBlockIntegral(lua_State *L);
int luaClearBlock(lua_State *L);
int luaClearContourPoint(lua_State *L);
int luaExitPost(lua_State *L);
int luaGetCircuitProperties(lua_State *L);
int luaGetElement(lua_State *L);
int luaGetMaterialFromLib(lua_State *L);
int luaGetMeshNode(lua_State *L);
int luaGetPointVals(lua_State *L);
int luaGroupSelectBlock(lua_State *L);
int luaLineIntegral(lua_State *L);
int luaLoadSolution(lua_State *L);
int luaMirrorCopy(lua_State *L);
int luaModifyBoundaryProp(lua_State *L);
int luaModifyCircuitProp(lua_State *L);
int luaModifyMaterialProp(lua_State *L);
int luaModifyPointProp(lua_State *L);
int luaMoveRotate(lua_State *L);
int luaMoveTranslate(lua_State *L);
int luaNewDocument(lua_State *L);
int luaNumElements(lua_State *L);
int luaNumNodes(lua_State *L);
int luaProbDef(lua_State *L);
int luaPurgeMesh(lua_State *L);
int luaReloadNOP(lua_State *L);
int luaScaleMove(lua_State *L);
int luaSelectConductor(lua_State *L);
int luaSelectGroup(lua_State *L);
int luaSelectOutputBlocklabel(lua_State *L);
int luaSelectWithinCircle(lua_State *L);
int luaSelectWithinRectangle(lua_State *L);
int luaSetArcsegmentProp(lua_State *L);
int luaSetBlocklabelProp(lua_State *L);
int luaSetFocus(lua_State *L);
int luaSetNodeProp(lua_State *L);
int luaSetSegmentProp(lua_State *L);
}

} /* namespace FemmLua*/

#endif /* LUAELECTROSTATICSCOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4:

