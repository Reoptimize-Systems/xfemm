/* Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of 
 * Mechatronics GmbH (LCM)
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#include "CliTools.h"
#include "FemmState.h"
#include "femmversion.h"
#include "fmesher.h" // for triangle version
#include "locationTools.h"
#include "LuaBaseCommands.h"
#include "LuaInstance.h"
#include "LuaElectrostaticsCommands.h"
#include "LuaHeatflowCommands.h"
#include "LuaMagneticsCommands.h"
#include "stringTools.h"

#include <cassert>
#include <memory>
#include <iostream>
#include <string>

#define DEBUG_FEMMCLI
#ifdef DEBUG_FEMMCLI
#define debug std::cerr
#else
#define debug while(false) std::cerr
#endif

using namespace std;
using namespace femm;
using namespace femmcli;

/**
 * @brief quiet if true, reduce commandline-output
 */
bool quiet = false;

/**
 * \brief Execute a Lua File
 * \param inputFile the lua file
 * \param luaInit a lua file containing initialization code
 * \param luaTrace enable function tracing for lua
 * \param luaBaseDir base directory for lua
 * \return the result of lua_dostring()
 */
int execLuaFile( const std::string &inputFile, const std::string &luaInit, bool luaTrace, const std::string &luaBaseDir, bool luaPedanticMode)
{
    // initialize interpreter
    shared_ptr<FemmState> state = make_shared<FemmState>();
    LuaInstance li(static_pointer_cast<FemmStateBase>(state));
    LuaBaseCommands::registerCommands(li);
    LuaMagneticsCommands::registerCommands(li);
    LuaElectrostaticsCommands::registerCommands(li);
    LuaHeatflowCommands::registerCommands(li);
    li.enableTracing(luaTrace);
    li.setPedanticMode(luaPedanticMode);
    li.setBaseDir(luaBaseDir);
    // canned initialization
    if (!luaInit.empty())
    {
        FILE *fp;
        if ( NULL != (fp=fopen(luaInit.c_str(),"rt")))
        {
            fclose(fp);
            if (int lua_error_code=li.doFile(luaInit)!=0)
            {
                if (lua_error_code==LUA_ERRRUN)
                    std::cerr << "Run Error in init.lua\n";
                if (lua_error_code==LUA_ERRMEM)
                    std::cerr << "Lua memory Error in init.lua\n";
                if (lua_error_code==LUA_ERRERR)
                    std::cerr << "User error error in init.lua\n";
                if (lua_error_code==LUA_ERRFILE)
                    std::cerr << "File Error in init.lua\n";
            }
        }
    }

    int err = li.doFile(inputFile);
    switch(err)
    {
        case 0:
            if (!quiet)
            {
                std::cerr << "No errors\n";
            }
            break;
        case LUA_ERRRUN:
            std::cerr << "Error running chunk\n";
            break;
        case LUA_ERRSYNTAX:
            std::cerr << "Syntax error\n";
            break;
        case LUA_ERRMEM:
            std::cerr << "Out of memory\n";
            break;
        case LUA_ERRERR:
            std::cerr << "Error in error handler\n";
            break;
        case LUA_ERRFILE:
            std::cerr << "Error reading file " << inputFile << std::endl;
            break;
        default:
            // this should really not happen
            std::cerr << "Unknown error!\n";
    }
    return err;
}

int main(int argc, char ** argv)
{
    std::string exe { argv[0] };
    exe = exe.substr(0,exe.find_last_of("/\\")+1);

    std::string baseDir;
    std::string inputFile;
    std::string luaInit = location::locateFile(location::LocationType::SystemData, "xfemm", "release/init.lua");
    bool luaTrace = false;
    bool luaPedanticMode = false;

    for(int i=1; i<argc; i++)
    {
        std::string arg;
        std::string value;
        femmutils::splitArg(argv[i],arg,value);
        if (arg == "--lua-script")
        {
            // allow both "--arg=value" and "--arg value"
            if (value.empty())
            {
                i++;
                if (i<argc)
                    inputFile = argv[i];
            } else {
                inputFile = value;
            }
            continue;
        }
        if (arg == "--lua-init")
        {
            if (value.empty())
            {
                i++;
                if (i<argc)
                    luaInit = argv[i];
            } else {
                luaInit = value;
            }
            if (!quiet)
                std::cerr << "Using custom init.lua: " << luaInit << std::endl;
            continue;
        }
        if (arg == "--lua-trace-functions" )
        {
            luaTrace = true;
            continue;
        }
        if (arg == "--lua-base-dir")
        {
            if (value.empty())
            {
                i++;
                if (i<argc)
                    baseDir = argv[i];
            } else {
                baseDir = value;
            }
            if (!quiet)
                std::cerr << "Using custom base directory " << baseDir << std::endl;
            continue;
        }
        if (arg == "--version" )
        {
            std::cout << "femmcli version " << FEMM_VERSION_STRING << "\n"
                      << "using triangle " << fmesher::triangleVersionString() << "\n";
#ifndef NDEBUG
            std::cout << "assertions enabled\n";
#endif
            return 0;
        }
        if (arg == "-q" || arg == "--quiet")
        {
            quiet = true;
            continue;
        }
        if (arg == "--lua-pedantic-mode" )
        {
            luaPedanticMode = true;
            continue;
        }
        // unhandled argument -> print usage and exit
        int exitval = 0;
        if (arg != "-h" && arg != "--help")
        {
            std::cerr << "Unknown argument: " << arg << std::endl;
            exitval = 1;
        }
        std::cout << "Command-line interpreter for FEMM-specific lua files.\n";
        std::cout << "\n";
        std::cout << "Usage: " << exe << " [-q|--quiet] [--lua-trace-functions] [--lua-pedantic-mode] [--lua-init=<init.lua>] [--lua-base-dir=<dir>] --lua-script=<file.lua>\n";
        std::cout << "       " << exe << " [-h|--help] [--version]\n";
        std::cout << "\n";
        std::cout << "Command line arguments:\n";
        std::cout << " --lua-base-dir=<dir>     Set base directory for matlib.dat.\n";
        std::cout << "                          [default: " << baseDir << "]\n";
        std::cout << " --lua-init=<init.lua>    Initialize the lua state with a custom lua script.\n";
        std::cout << "                          [default: " << luaInit <<"]\n";
        std::cout << " --lua-pedantic-mode      Additional checks for lua scripts.\n";
        std::cout << " --lua-script=<file.lua>  Execute the lua file.\n";
        std::cout << " --lua-trace-functions    Show what lua functions are being executed.\n";
        std::cout << "\n";
        std::cout << "Additional options:\n";
        std::cout << " -h, --help               Show this help message and exit.\n";
        std::cout << " -q, --quiet              Be somewhat less verbose.\n";
        std::cout << "     --version            Show version information and exit.\n";
        std::cout << "\n";
        std::cout << "Base directories for search:\n";
        for (const auto &dir: location::baseDirectories(location::LocationType::SystemData))
            std::cout << " " << dir << "\n";
        std::cout << "\n";
        std::cout << "Examples:\n";
        std::cout << "To execute a lua file, you can omit the '=' and write two arguments instead:\n";
        std::cout << " \"femmcli --lua-script=file.lua\"\n";
        std::cout << "is the same as:\n";
        std::cout << " \"femmcli --lua-script file.lua\"\n";
        std::cout << "\n";
        return exitval;
    }
    if (inputFile.empty())
    {
        std::cerr << "No file name given! Try \"femmcli --help\"...\n";
        return 1;
    }

    return execLuaFile(inputFile, luaInit, luaTrace, baseDir, luaPedanticMode);
}
// vi:expandtab:tabstop=4 shiftwidth=4:
