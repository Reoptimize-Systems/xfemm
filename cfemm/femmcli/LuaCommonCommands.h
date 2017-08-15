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

#ifndef LUACOMMONCOMMANDS_H
#define LUACOMMONCOMMANDS_H

class lua_State;

namespace femm {
class LuaInstance;
}

namespace femmcli
{

/**
 * LuaCommonCommands provides lua commands which are shared between different modules.
 * These commands are registered by the individual module's registerCommands().
 *
 * \see LuaMagneticsCommands::registerCommands()
 * \see LuaElectrostaticsCommands::registerCommands()
 *
 */
namespace LuaCommonCommands
{
int luaAddArc(lua_State *L);
int luaAddBlocklabel(lua_State *L);
int luaAddLine(lua_State *L);
int luaAddNode(lua_State *L);
int luaAttachDefault(lua_State *L);
int luaAttachOuterSpace(lua_State *L);
int luaClearSelected(lua_State *L);
int luaCopyRotate(lua_State *L);
int luaCopyTranslate(lua_State *L);
int luaExitPre(lua_State *L);
int luaSaveDocument(lua_State *L);
int luaSelectArcsegment(lua_State *L);
int luaSelectBlocklabel(lua_State *L);
int luaSelectNode(lua_State *L);
int luaSelectSegment(lua_State *L);
}

} /* namespace femmcli*/

#endif /* LUACOMMONCOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4:

