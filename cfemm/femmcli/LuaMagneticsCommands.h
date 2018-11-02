/* Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
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

#ifndef LUAMAGNETICSCOMMANDS_H
#define LUAMAGNETICSCOMMANDS_H

struct lua_State;

namespace femm {
class LuaInstance;
}

namespace femmcli
{

/**
 * LuaMagneticsCommands provides the lua commands related to magnetics.
 * The Lua magnetics command set is described in sections 3.3 and 3.4 of the FEMM manual.
 *
 */
namespace LuaMagneticsCommands
{
/**
 * @brief Register the common command set with the given LuaInstance
 * @param li a LuaInstance
 */
void registerCommands(femm::LuaInstance &li );

int luaAddArc(lua_State *L);
int luaAddBHPoint(lua_State *L);
int luaAddBoundaryProperty(lua_State *L);
int luaAddCircuitProperty(lua_State *L);
int luaAddContourPoint(lua_State *L);
int luaAddMatProperty(lua_State *L);
int luaAddPointProperty(lua_State *L);
int luaAnalyze(lua_State *L);
int luaBendContourLine(lua_State *L);
int luaBlockIntegral(lua_State *L);
int luaGapIntegral(lua_State *L);
int luaClearBHPoints(lua_State *L);
int luaClearBlock(lua_State *L);
int luaClearContourPoint(lua_State *L);
int luaGetCircuitProperties(lua_State *L);
int luaGetElement(lua_State *L);
int luaGetMeshNode(lua_State *L);
int luaGetPointValues(lua_State *L);
int luaBGradient(lua_State *L);
int luaGroupSelectBlock(lua_State *L);
int luaLineIntegral(lua_State *L);
int luaModifyBoundaryProperty(lua_State *L);
int luaModifyCircuitProperty(lua_State *L);
int luaModifyMaterialProperty(lua_State *L);
int luaModifyPointProperty(lua_State *L);
int luaNewDocument(lua_State *L);
int luaProblemDefinition(lua_State *L);
int luaSelectOutputBlocklabel(lua_State *L);
int luaAddContourPointFromNode(lua_State *L);
int luaSetArcsegmentProperty(lua_State *L);
int luaSetBlocklabelProperty(lua_State *L);
int luaSetNodeProperty(lua_State *L);
int luaSetPrevious(lua_State *L);
int luaSetSmoothing(lua_State *L);
int luaSetSegmentProperty(lua_State *L);
int luaGetGapB(lua_State *L);
int luaGetGapA(lua_State *L);
int luaGetGapHarmonics(lua_State *L);
}

} /* namespace FemmLua*/

#endif /* LUAMAGNETICSCOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4:

