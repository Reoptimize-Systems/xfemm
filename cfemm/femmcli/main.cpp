/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#include "feasolver.h"
#include "fsolver.h"
#include "LuaBaseCommands.h"
#include "LuaInstance.h"
#include "stringTools.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace femm;

enum OperationMode { NoOperation, LuaMode, SolverMode };

/**
 * @brief execLuaFile
 * Execute a Lua File
 * @param inputFile the lua file
 * @param luaInit a lua file containing initialization code
 * @return the result of lua_dostring()
 */
int execLuaFile( const std::string &inputFile, const std::string &luaInit)
{
    // initialize interpreter
    LuaInstance li;
    LuaBaseCommands::registerCommands(li);
    //LuaMagneticsCommands::registerCommands(li);
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
            std::cerr << "No errors\n";
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

/**
 * @brief solveFile
 * Determines the file type and runs the appropriate solver on the file.
 * @param inputFile
 * @return
 */
int solveFile( const std::string &inputFile)
{
    return 0;
}

int main(int argc, char ** argv)
{
    std::string exe { argv[0] };
    std::string exeDir { exe.substr(0,exe.find_last_of("/\\")+1) };
    exe = exe.substr(exeDir.length());

    OperationMode opMode { NoOperation };
    std::string inputFile;
    std::string luaInit { exeDir + "init.lua" };

    for(int i=1; i<argc; i++)
    {
        const std::string arg { argv[i] };
        if (arg.substr(0,13) == "--lua-script=")
        {
            inputFile = arg.substr(13);
            opMode = LuaMode;
            continue;
        }
        if (arg.substr(0,13) == "--solve-file=")
        {
            inputFile = arg.substr(13);
            opMode = SolverMode;
            continue;
        }
        if (arg.substr(0,11) == "--lua-init=")
        {
            luaInit = arg.substr(11);
            std::cerr << "Using custom init.lua: " << luaInit << std::endl;
            continue;
        }
        if (arg == "-h" || arg == "--help")
        {
            std::cout << "Usage: " << exe << " (--lua-script=<file.lua>|--solve-file=<file.feX>)\n";
            return 0;
        }
        std::cerr << "Unknown argument: " << arg << std::endl;
    }
    if (inputFile.empty())
    {
        std::cerr << "No file name given!\n";
        return 1;
    }

    int exitCode = 0;
    switch (opMode) {
        case NoOperation:
            assert(false); // already covered by inputFile.empty()
            break;
        case LuaMode:
            exitCode = execLuaFile(inputFile, luaInit);
            break;
        case SolverMode:
            exitCode = solveFile(inputFile);
            break;
    }

    return exitCode;

}
// vi:expandtab:tabstop=4 shiftwidth=4:
