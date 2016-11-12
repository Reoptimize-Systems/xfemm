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

#ifndef LUAINSTANCE_H
#define LUAINSTANCE_H

#include "FemmState.h"

#include <lua.h>
#include <string>
#include <memory>

struct lua_Debug;

namespace femm
{

/**
 * LuaInstance holds a Lua interface including its state.
 * A LuaInstance takes care of initialization (and destruction) of the Lua state.
 *
 * The following Lua functions are always provided:
 *  - Complex(int[, int])
 *  - setcompatibilitymode(int)
 *  - femmVersion()
 */
class LuaInstance
{
public:
    /**
     * @brief The LuaStackMode enum
     * Determines whether the Lua stack is returned to its old state after a Lua call.
     * SafeStack enforces a clean, unaltered stack after execution.
     * UnsafeStack allows Lua commands to leave items on the stack for the next command.
     */
    enum LuaStackMode { SafeStack, UnsafeStack };

    /**
     * @brief The StackInfoMode enum
     * Carries information whether the LuaStackInfo method should only examine the
     * current stack frame, or the full stack frame.
     */
    enum StackInfoMode { CurrentFrameInfo, FullStackInfo };
    /**
     * @brief LuaInstance constructor
     * Create a Lua instance and initialize it, adding the Lua standard libraries and the CComplex data type.
     * @param stackSize the Lua stack size
     */
    LuaInstance( int stackSize=4096);

    ~LuaInstance();

    /**
     * @brief femmState is a handle on open documents
     * @return the FemmState
     */
    std::shared_ptr<FemmState> femmState() const;

    /**
     * @brief Call lua_dobuffer on the given string.
     * @param luaString a Lua source string or a precompiled chunk
     * @param chunkName an optional name for the chunk (for error messages)
     * @param mode enable/disable stack safety
     * @return  the return value of lua_dobuffer
     */
    int doBuffer( const std::string &luaString, const std::string &chunkName=std::string(), LuaStackMode mode=SafeStack );
    /**
     * @brief Call lua_dofile on a given file name.
     * @param filename the file name of a Lua source file or Lua precompiled chunk
     * @param mode enable/disable stack safety
     * @return the return value of lua_dofile
     */
    int doFile(const std::string &filename, LuaStackMode mode=SafeStack );
    /**
     * @brief Call lua_dostring on the given string.
     * @param luaString a Lua source string
     * @param mode enable/disable stack safety
     * @return  the return value of lua_dostring
     */
    int doString( const std::string &luaString, LuaStackMode mode=SafeStack );

    /**
     * @brief getLuaState
     * @return a pointer to the Lua instance state.
     */
    lua_State *getLuaState() const;

    /**
     * @brief Register a Lua function as with lua_register.
     * @param name
     * @param fun
     */
    void addFunction(const char *name, lua_CFunction fun);

    /**
     * @brief compatibilityMode
     * @return true, if the Lua instance has been set into compatibilty mode. False otherwise.
     */
    bool compatibilityMode() const;

    /**
     * @brief enableTracing
     * Enable or disable lua function call tracing.
     * @param enable
     */
    void enableTracing(bool enable);

private:
    lua_State *lua;
    std::shared_ptr<FemmState> fs;
    bool compatMode;

    static LuaInstance* instance( lua_State *L);

    static int luaComplex(lua_State *L);
    static int luaFemmVersion(lua_State *L);
    static int luaSetCompatibilityMode(lua_State *L);
    static int luaGetCompatibilityMode(lua_State *L);
    static int luaTrace(lua_State *L);
    static void luaStackInfo(lua_State *L, int startLevel, StackInfoMode info );
    static void luaStackHook(lua_State *L, lua_Debug *ar);
};

} /* namespace FemmLua*/

#endif /* LUAINSTANCE_H */
// vi:expandtab:tabstop=4 shiftwidth=4:
