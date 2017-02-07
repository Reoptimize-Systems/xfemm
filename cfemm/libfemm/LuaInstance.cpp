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

#include "femmcomplex.h"
#include "femmversion.h"
#include "FemmStateBase.h"

#include <lua.h>
#include <lualib.h>
#include <luadebug.h>

#include <cassert>
#include <string>
#include <iostream>

#ifdef DEBUG_FEMMLUA
#define debug std::cerr
#else
#define debug while(false) std::cerr
#endif

femm::LuaInstance::LuaInstance(int stackSize)
    : fs ()
    , compatMode(false)
{
    initializeLua(stackSize);
}

femm::LuaInstance::LuaInstance(std::shared_ptr<FemmStateBase> state, int stackSize)
    : fs(state)
    , compatMode(false)
{
    initializeLua(stackSize);
}

femm::LuaInstance::~LuaInstance()
{
    lua_close(lua);
}

std::shared_ptr<femm::FemmStateBase> femm::LuaInstance::femmState() const
{
    return fs;
}

int femm::LuaInstance::doBuffer(const std::string &luaString, const std::string &chunkName, LuaStackMode mode)
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

int femm::LuaInstance::doFile(const std::string &filename, femm::LuaInstance::LuaStackMode mode)
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

int femm::LuaInstance::doString(const std::string &luaString, femm::LuaInstance::LuaStackMode mode)
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

CComplex femm::LuaInstance::getGlobal(const std::__cxx11::string &varName, bool *ok)
{
    lua_getglobal(lua, varName.c_str()); //+1

    CComplex value = lua_tonumber(lua,-1);
    if (ok!=nullptr)
        *ok = (lua_isnumber(lua,-1));
    lua_pop(lua, 1); //-1

    return value;
}

void femm::LuaInstance::setGlobal(const std::string &varName, CComplex val)
{
    lua_pushnumber(lua, val); //+1
    lua_setglobal(lua, varName.c_str()); //-1
}

bool femm::LuaInstance::compatibilityMode() const
{
    return compatMode;
}

void femm::LuaInstance::enableTracing(bool enable)
{
    if (enable)
    {
        lua_setcallhook(lua, luaStackHook);
    } else {
        lua_setcallhook(lua, nullptr);
    }
}

femm::LuaInstance *femm::LuaInstance::instance(lua_State *L)
{
    lua_getregistry(L); //+1
    lua_rawgeti(L,-1,1); //+1
    void *obj = lua_touserdata(L, -1); //-1,+1
    lua_pop(L,2); //-2
    return static_cast<LuaInstance*>(obj);
}

int femm::LuaInstance::luaNOP(lua_State *)
{
    return 0;
}

std::string femm::LuaInstance::getBaseDir() const
{
    return baseDir;
}

void femm::LuaInstance::setBaseDir(const std::string &value)
{
    baseDir = value;

    if (baseDir[baseDir.length()-1] != '/' )
        baseDir.push_back('/');
}

void femm::LuaInstance::initializeLua(int stackSize)
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
    addFunction("trace",luaTrace);
}

lua_State *femm::LuaInstance::getLuaState() const
{
    return lua;
}

void femm::LuaInstance::addFunction(const char *name, lua_CFunction fun)
{
    lua_register(lua, name, fun);
    //debug << "Registered function " << name << std::endl;
}

/**
 * @brief Create a Complex number from its 1 or 2 numeric parameters.
 * @param L the Lua state
 * @return 1
 * \ingroup LuaCommon
 * \femm42{femm/femm.cpp,lua_Complex()}
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
 * \remark This method does not exist in standard FEMM.
 * @param L
 * @return 1
 * \ingroup LuaCommon
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
 * \ingroup LuaCommon
 * \femm42{femm/femm.cpp,lua_compatibilitymode()}
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
 * \remark This method does not exist in standard FEMM.
 * @param L
 * @return 1
 * \ingroup LuaCommon
 */
int femm::LuaInstance::luaGetCompatibilityMode(lua_State *L)
{
    LuaInstance *me = instance(L);
    //assert(me)
    CComplex compatMode = (me->compatibilityMode()) ? 1.0 : 0.0;
    lua_pushnumber(L, compatMode);
    return 1;
}

/**
 * @brief Prints info on the stack frame.
 * \verbatim
 * trace()
 * trace(int bt) ->.. bt=1 for full backtrace\endverbatim
 * \remark This method does not exist in standard FEMM.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 */
int femm::LuaInstance::luaTrace(lua_State *L)
{
    StackInfoMode mode = CurrentFrameInfo;
    if (lua_gettop(L)!=0)
    {
        if (1 == lua_tonumber(L,1).Re())
            mode = FullStackInfo;
    }
    // startLevel=1 to omit trace function itself
    luaStackInfo(L, 1, mode);
    return 0;
}

/**
 * @brief Prints information about the call stack
 * @param L the lua state
 * @param level start level of stack trace (use 0 for current stack frame, 1 for the one above that, ...)
 * @param info what information to print
 */
void femm::LuaInstance::luaStackInfo(lua_State *L, int level, femm::LuaInstance::StackInfoMode info)
{
    do {
        lua_Debug ar; // activation record
        // collect frame info for current frame
        if (!lua_getstack(L,level,&ar))
            break;
        // force event to a sane value:
        ar.event = "";
        // print info
        luaStackHook(L, &ar);
        level++;
    } while (info == FullStackInfo);
}

/**
 * @brief Prints info on the given activation record (aka stack frame).
 * @param L
 * @param ar
 */
void femm::LuaInstance::luaStackHook(lua_State *L, lua_Debug *ar)
{
    assert(ar);
    // fill ar fields from collected information
    lua_getinfo(L, "lnS", ar);
    // line
    if (ar->currentline != -1)
    {
        std::cout << "L";
        std::cout.width(9);
        std::cout << std::left << ar->currentline << " ";
    }
    // call/return
    if (ar->event && ar->event[0] != '\0' )
    {
        std::cout.width(10);
        std::cout << std::right << ar->event << " ";
    }
    if (ar->namewhat[0] != '\0')
        std::cout << ar->namewhat << " ";
    std::cout << ar->what;
    if (ar->name)
        std::cout << " " << ar->name << "()";
    std::cout << " [" << ar->short_src;
    if (ar->linedefined != -1)
        std::cout << ":" << ar->linedefined;
    std::cout << "]\n";
}

// vi:expandtab:tabstop=4 shiftwidth=4:
