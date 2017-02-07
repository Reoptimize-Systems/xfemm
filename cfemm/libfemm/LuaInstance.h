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

#include "FemmStateBase.h"

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
    enum class LuaStackMode { Safe, Unsafe };

    /**
     * @brief The StackInfoMode enum
     * Carries information whether the LuaStackInfo method should only examine the
     * current stack frame, or the full stack frame.
     */
    enum class StackInfoMode { CurrentFrameInfo, FullStackInfo };
    /**
     * @brief LuaInstance constructor
     * Create a Lua instance and initialize it, adding the Lua standard libraries and the CComplex data type.
     * @param stackSize the Lua stack size
     */
    LuaInstance( int stackSize=4096);
    /**
     * @brief LuaInstance
     * Create a Lua instance and initialize it, adding the Lua standard libraries and the CComplex data type.
     * @param state the state object for use by additional Lua command modules (e.g. LuaMagneticsCommands)
     * @param stackSize the Lua stack size
     */
    LuaInstance( std::shared_ptr<FemmStateBase> state, int stackSize=4096);

    ~LuaInstance();

    /**
     * @brief femmState is a handle on open documents and data that is not directly related to lua
     * @return the FemmState
     */
    std::shared_ptr<FemmStateBase> femmState() const;

    /**
     * @brief Call lua_dobuffer on the given string.
     * @param luaString a Lua source string or a precompiled chunk
     * @param chunkName an optional name for the chunk (for error messages)
     * @param mode enable/disable stack safety
     * @return  the return value of lua_dobuffer
     */
    int doBuffer( const std::string &luaString, const std::string &chunkName=std::string(), LuaStackMode mode=LuaStackMode::Safe );
    /**
     * @brief Call lua_dofile on a given file name.
     * @param filename the file name of a Lua source file or Lua precompiled chunk
     * @param mode enable/disable stack safety
     * @return the return value of lua_dofile
     */
    int doFile(const std::string &filename, LuaStackMode mode=LuaStackMode::Safe );
    /**
     * @brief Call lua_dostring on the given string.
     * @param luaString a Lua source string
     * @param mode enable/disable stack safety
     * @return  the return value of lua_dostring
     */
    int doString( const std::string &luaString, LuaStackMode mode=LuaStackMode::Safe );

    /**
     * @brief Get a global lua variable.
     * @param varName the name of the global variable
     * @param ok if not \c nullptr, this variable is set to \c true if \p varName could be retrieved, \c false otherwise.
     * @return The variable value, or 0 on error
     */
    CComplex getGlobal( const std::string &varName, bool *ok=nullptr );

    /**
     * @brief Set a global lua variable.
     * @param varName the name of the global variable
     * @param val the value to be stored
     */
    void setGlobal(const std::string &varName, CComplex val );
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

    /**
    * @brief Extract the LuaInstance object from the LuaState
    * @param L
    * @return the LuaInstance, or nullptr if state is not valid.
    */
    static LuaInstance* instance( lua_State *L);

    /**
     * @brief luaNOP does nothing.
     * This function can be used as "implementation" for intentionally unimplemented lua functions.
     * @return 0
     */
    static int luaNOP(lua_State *);

    /**
     * @brief The base directory for accessory files (like init.lua, matlib.lua).
     * Usually, this is the same as the binary directory of the femm executables.
     * The directory name contains a trailing '/'.
     * @return baseDir
     */
    std::string getBaseDir() const;
    /**
     * @brief setBaseDir
     * If the base dir does not end in a '/', it is added.
     * @param value
     */
    void setBaseDir(const std::string &value);

private:
    lua_State *lua;
    std::shared_ptr<FemmStateBase> fs;
    bool compatMode;

    std::string baseDir;

    /**
     * @brief initialize lua
     */
    void initializeLua(int stackSize);

    static int luaComplex(lua_State *L);
    static int luaFemmVersion(lua_State *L);
    static int luaSetCompatibilityMode(lua_State *L);
    static int luaGetCompatibilityMode(lua_State *L);
    static int luaTrace(lua_State *L);
    static void luaStackInfo(lua_State *L, int startLevel, StackInfoMode info );
    static void luaStackHook(lua_State *L, lua_Debug *ar);
};

} /* namespace */

#endif /* LUAINSTANCE_H */
// vi:expandtab:tabstop=4 shiftwidth=4:
