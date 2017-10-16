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

#ifndef LUAHEATFLOWCOMMANDS_H
#define LUAHEATFLOWCOMMANDS_H

struct lua_State;

namespace femm {
class LuaInstance;
}

namespace femmcli
{

/**
 * LuaHeatflowCommands provides the lua commands related to Heatflow.
 * The Lua magnetics command set is described in sections 3.7 and 3.8 of the FEMM manual.
 *
 */
namespace LuaHeatflowCommands
{
/**
 * @brief Register the common command set with the given LuaInstance
 * @param li a LuaInstance
 */
void registerCommands(femm::LuaInstance &li );

int luaAddBoundaryProperty(lua_State *L);
int luaAddConductorProperty(lua_State *L);
int luaAddMaterialProperty(lua_State *L);
int luaAddPointProperty(lua_State *L);
int luaAddtkpoint(lua_State *L);
int luaAnalyze(lua_State *L);
int luaBlockIntegral(lua_State *L);
int luaCleartkpoints(lua_State *L);
int luaExitPost(lua_State *L);
int luaGetConductorProperties(lua_State *L);
int luaGetMaterialFromLib(lua_State *L);
int luaGetPointValues(lua_State *L);
int luaLineIntegral(lua_State *L);
int luaModifyBoundaryProperty(lua_State *L);
int luaModifyConductorProperty(lua_State *L);
int luaModifyMaterialProperty(lua_State *L);
int luaModifyPointProperty(lua_State *L);
int luaNewDocument(lua_State *L);
int luaProblemDefinition(lua_State *L);
int luaSelectConductor(lua_State *L);
int luaSetNodeProperty(lua_State *L);
int luaSetSegmentProperty(lua_State *L);
}

} /* namespace FemmLua*/

#endif /* LUAHEATFLOWCOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4:

