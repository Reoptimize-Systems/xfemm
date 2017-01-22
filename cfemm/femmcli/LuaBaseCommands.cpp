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

#include "LuaBaseCommands.h"

#include "FemmProblem.h"
#include "FemmReader.h"
#include "FemmState.h"
#include "LuaInstance.h"
#include "fsolver.h"

#include <lua.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#ifdef WIN32
#include <direct.h> // _chdir
#else
#include <unistd.h> // chdir
#endif

#ifdef DEBUG_FEMMLUA
#define debug std::cerr
#else
#define debug while(false) std::cerr
#endif

using namespace femm;

void femmcli::LuaBaseCommands::registerCommands(LuaInstance &li)
{
    li.addFunction("_ALERT",luaError);
    li.addFunction("messagebox",luaMessageBox);
    li.addFunction("pause",luaPause);
    //li.addFunction("prompt",luaPromptBox);
    li.addFunction("open",luaOpenDocument);
    li.addFunction("quit",LuaInstance::luaNOP);
    li.addFunction("exit",LuaInstance::luaNOP);
    li.addFunction("setcurrentdirectory",luaSetWorkingDirectory);
    li.addFunction("chdir",luaSetWorkingDirectory);

    li.addFunction("create",luaNewDocument);
    li.addFunction("newdocument",luaNewDocument);
    li.addFunction("new_document",luaNewDocument);

    li.addFunction("showconsole",LuaInstance::luaNOP);
    li.addFunction("show_console",LuaInstance::luaNOP);
    li.addFunction("showpointprops",LuaInstance::luaNOP);
    li.addFunction("hidepointprops",LuaInstance::luaNOP);
    li.addFunction("show_point_props",LuaInstance::luaNOP);
    li.addFunction("hide_point_props",LuaInstance::luaNOP);

    //lua_register(lua,"flput",lua_to_filelink);
    //lua_register(lua,"smartmesh",lua_smartmesh);
}

/**
 * @brief Print an error message.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 * \femm42{femm/femm.cpp,lua_endapp()}
 */
int femmcli::LuaBaseCommands::luaError(lua_State *L)
{
    // Somthing went wrong in lua execution
    std::cerr << lua_tostring(L,1) << std::endl;

    return 0;
}

/**
 * @brief Write a message.
 * Usually, this would go to a messagebox, but since this is a text implementation, print it to stdout.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 * \femm42{femm/femm.cpp,lua_messagebox()}
 */
int femmcli::LuaBaseCommands::luaMessageBox(lua_State *L)
{
    std::cout << "* " << lua_tostring(L,1) << std::endl;
    return 0;
}

/**
 * @brief Create new document
 * @param L
 * @return 0
 * \ingroup LuaCommon
 * \femm42{femm/femm.cpp,lua_newdocument()}
 */
int femmcli::LuaBaseCommands::luaNewDocument(lua_State *L)
{
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(LuaInstance::instance(L)->femmState());
    assert(femmState);

    int docType = static_cast<int>(lua_tonumber(L,1).Re());
    switch (docType) {
        case 0: // magnetics
            femmState->femmDocument = std::make_shared<femm::FemmProblem>(femm::MagneticsFile);
            break;
        case 1: // electrostatics
        case 2: // heat flow
        case 3: // current flow
            debug << "NOP: newdocument("<<docType<<")" << std::endl;
            break;
        default:
            // we don't need to handle other docTypes that are used in femm
            // -> the other types are gui-specific
            debug << "document type " << docType << "not supported.\n";
            break;
    }
    return 0;
}

/**
 * @brief Open a document
 * @param L
 * @return 0
 * \ingroup LuaCommon
 * \femm42{femm/femm.cpp,luaOpenDocument()}
 */
int femmcli::LuaBaseCommands::luaOpenDocument(lua_State *L)
{
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(LuaInstance::instance(L)->femmState());
    assert(femmState);
    std::string filename = lua_tostring(L,1);

    switch (fmesher::FMesher::GetFileType(filename)) {
    case femm::MagneticsFile:
    {
        femmState->femmDocument = std::make_shared<femm::FemmProblem>(femm::MagneticsFile);
        std::stringstream err;
        femm::MagneticsReader reader(femmState->femmDocument, err);
        if (reader.parse(filename)!=F_FILE_OK)
        {
            std::string msg = "Could not read file " + filename;
            msg += "\nError: " + err.str() + "\n";
            lua_error(L, msg.c_str());
        }
    }
        break;
    case CurrentFlowFile:
    case ElectrostaticsFile:
    case HeatFlowFile:
    case UnknownFile:
        std::string msg = "File not supported: " + filename;
        lua_error(L, msg.c_str());
        break;
    }
    return 0;
}

/**
 * @brief Dummy-function for compatibility.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 * \femm42{femm/femm.cpp,lua_afxpause()}
 */
int femmcli::LuaBaseCommands::luaPause(lua_State *)
{
    debug << "NOP: luaPause" << std::endl;
    return 0;
}

/**
 * @brief Set/Change working Directory.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 * \femm42{femm/femm.cpp,lua_setcurrentdirectory()}
 */
int femmcli::LuaBaseCommands::luaSetWorkingDirectory(lua_State *L)
{
    if (lua_gettop(L) != 0)
    {
        std::string newDirectory { lua_tostring(L,1) };
#ifdef WIN32
        _chdir(newDirectory.c_str());
#else
        chdir(newDirectory.c_str());
#endif
    }
    return 0;
}

// vi:expandtab:tabstop=4 shiftwidth=4:
