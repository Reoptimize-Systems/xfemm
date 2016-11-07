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

#include "LuaInstance.h"

#include <femmcomplex.h>
#include <femmversion.h>

#include <lua.h>
#include <lualib.h>

#include <string>
#include <iostream>

#ifdef DEBUG_FEMMLUA
#define debug std::cerr
#else
#define debug while(false) std::cerr
#endif

femm::LuaInstance::LuaInstance(int stackSize)
    : compatMode(false)
{
    debug << "Initializing Lua with stacksize = " << stackSize << std::endl;
    lua = lua_open(stackSize);

    // add pointer to this object to Lua_registry[1]
    lua_getregistry(lua); //+1
    lua_pushuserdata(lua,this); //+1
    lua_rawseti(lua,1,1); //-1
    lua_pop(lua,1); //-1

    // register Lua standard libraries:
    lua_baselibopen(lua);
    lua_strlibopen(lua);
    lua_mathlibopen(lua);
    lua_iolibopen(lua);

    // register some common Lua functionality
    addFunction("Complex", luaComplex);
    addFunction("setcompatibilitymode",luaSetCompatibilityMode);
    addFunction("getcompatibilitymode",luaGetCompatibilityMode);
    addFunction("femmVersion",luaFemmVersion);
}

femm::LuaInstance::~LuaInstance()
{
    lua_close(lua);
}

int femm::LuaInstance::doBuffer(const std::__cxx11::string &luaString, const std::__cxx11::string &chunkName, LuaStackMode mode)
{
    int stackTop = lua_gettop(lua);
    int result = lua_dobuffer(lua, luaString.c_str(), luaString.size(), chunkName.c_str());
    if (mode==LuaStackMode::SafeStack)
    {
        // ensure that no values are left on the stack
        lua_settop(lua, stackTop);
    }
    return result;
}

int femm::LuaInstance::doFile(const std::__cxx11::string &filename, femm::LuaInstance::LuaStackMode mode)
{
    int stackTop = lua_gettop(lua);
    int result = lua_dofile(lua, filename.c_str());
    if (mode==LuaStackMode::SafeStack)
    {
        // ensure that no values are left on the stack
        lua_settop(lua, stackTop);
    }
    return result;
}

int femm::LuaInstance::doString(const std::__cxx11::string &luaString, femm::LuaInstance::LuaStackMode mode)
{
    int stackTop = lua_gettop(lua);
    int result = lua_dostring(lua, luaString.c_str());
    if (mode==LuaStackMode::SafeStack)
    {
        // ensure that no values are left on the stack
        lua_settop(lua, stackTop);
    }
    return result;

}

bool femm::LuaInstance::compatibilityMode() const
{
    return compatMode;
}

/**
 * @brief Extract the LuaInstance object from the LuaState
 * @param L
 * @return the LuaInstance, or nullptr if state is not valid.
 */
femm::LuaInstance *femm::LuaInstance::instance(lua_State *L)
{
    lua_getregistry(L); //+1
    lua_rawgeti(L,-1,1); //+1
    void *obj = lua_touserdata(L, -1); //-1
    lua_pop(L,1); //-1
    return static_cast<LuaInstance*>(obj);
}

lua_State *femm::LuaInstance::getLuaState() const
{
    return lua;
}

void femm::LuaInstance::addFunction(const char *name, lua_CFunction fun)
{
    lua_register(lua, name, fun);
    debug << "Registered function " << name << std::endl;
}

/**
 * @brief Create a Complex number from its 1 or 2 numeric parameters.
 * @param L the Lua state
 * @return 1
 * \ingroup common
 * \sa FEMM42/femm.cpp:lua_Complex()
 */
int femm::LuaInstance::luaComplex(lua_State *L)
{
    CComplex y;
    int numArgs=lua_gettop(L);
    // FIXME raise error when arg is not a number
    if (numArgs==2)
        y=lua_tonumber(L,1)+I*lua_tonumber(L,2);
    else if (numArgs==1)
        y=lua_tonumber(L,1);
    else y=0;

    lua_pushnumber(L,y);
    return 1;
}

/**
 * @brief Return the version number of xfemm.
 * \remark{This method does not exist in standard FEMM.}
 * @param L
 * @return 1
 * \ingroup common
 */
int femm::LuaInstance::luaFemmVersion(lua_State *L)
{
    CComplex version = FEMM_VERSION_INT;
    lua_pushnumber(L,version);
    return 1;
}

/**
 * @brief Set compatibility mode.
 * @param L
 * @return 0
 * \ingroup common
 * \sa FEMM42/femm.cpp:lua_compatibilitymode()
 */
int femm::LuaInstance::luaSetCompatibilityMode(lua_State *L)
{
    if (lua_gettop(L)!=0)
    {
        LuaInstance *me = instance(L);
        if (me)
            me->compatMode = (1 == lua_tonumber(L,1).Re());
    }

    return 0;
}

/**
 * @brief Allow querying state of compatibility mode from Lua code.
 * \remark{This method does not exist in standard FEMM.}
 * @param L
 * @return 1
 * \ingroup common
 */
int femm::LuaInstance::luaGetCompatibilityMode(lua_State *L)
{
    LuaInstance *me = instance(L);
    //assert(me)
    CComplex compatMode = (me->compatibilityMode()) ? 1.0 : 0.0;
    lua_pushnumber(L, compatMode);
    return 1;
}

// vi:expandtab:tabstop=4 shiftwidth=4:
