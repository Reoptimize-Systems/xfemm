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

#ifndef LUABASECOMMANDS_H
#define LUABASECOMMANDS_H

struct lua_State;

namespace femm {
class LuaInstance;
}

namespace femmcli
{

/**
 * LuaBaseCommands provides the common lua commands usually provided by the FEMM UI.
 * The commmon Lua command set is described in section 3.2 of the FEMM manual.
 *
 * Since most commands in this set are UI-related, this namespace can only provide a dummy interface.
 *
 * Note: mathlink commands (mlput, mlopen, mlclose) are not provided.
 */
namespace LuaBaseCommands
{
/**
 * @brief Register the common command set with the given LuaInstance
 * This calls both registerBaseCommands() and registerNOPCommands().
 * @param li a LuaInstance
 */
void registerCommands(femm::LuaInstance &li );

int luaError(lua_State *L);
int luaExit(lua_State *L);
int luaMessageBox(lua_State *L);
int luaNewDocument(lua_State *L);
int luaOpenDocument(lua_State *L);
int luaPromptBox(lua_State *L);
int luaSetWorkingDirectory(lua_State *L);
}

} /* namespace FemmLua*/

#endif /* LUABASECOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4:
