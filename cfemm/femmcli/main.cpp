/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#include <iostream>
#include <string>
#include "LuaInstance.h"
#include "LuaBaseCommands.h"
//#include "LuaMagneticsCommands.h"

using namespace femm;

int main(int argc, char ** argv)
{
    std::string exe { argv[0] };
    std::string exeDir { exe.substr(0,exe.find_last_of("/\\")+1) };
    exe = exe.substr(exeDir.length());

    std::string luascript;
    std::string luainit { exeDir + "init.lua" };

    for(int i=1; i<argc; i++)
    {
        std::string arg { argv[i] };
        if (arg.substr(0,13) == "--lua-script=")
        {
            luascript = arg.substr(13);
            continue;
        }
        if (arg.substr(0,11) == "--lua-init=")
        {
            luainit = arg.substr(11);
            std::cerr << "Using custom init.lua: " << luainit << std::endl;
            continue;
        }
        if (arg == "-h" || arg == "--help")
        {
            std::cout << "Usage: " << exe << " --lua-script=<file.lua>\n";
            return 0;
        }
        std::cerr << "Unknown argument: " << arg << std::endl;
    }
    if (luascript.empty())
    {
        std::cerr << "No lua script given!\n";
        return 1;
    }

    // initialize interpreter
    LuaInstance li;
    LuaBaseCommands::registerCommands(li);
    //LuaMagneticsCommands::registerCommands(li);
    // canned initialization
    if (!luainit.empty())
    {
        FILE *fp;
        if ( NULL != (fp=fopen(luainit.c_str(),"rt")))
        {
            fclose(fp);
            if (int lua_error_code=li.doFile(luainit)!=0)
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


    int err = li.doFile(luascript);
    if (err)
    {
        switch(err)
        {
            case LUA_ERRRUN:
                std::cout << "Error running chunk\n";
                break;
            case LUA_ERRSYNTAX:
                std::cout << "Syntax error\n";
                break;
            case LUA_ERRMEM:
                std::cout << "Out of memory\n";
                break;
            case LUA_ERRERR:
                std::cout << "Error in error handler\n";
                break;
            case LUA_ERRFILE:
                std::cout << "Error reading file " << luascript << std::endl;
                break;
            default:
                // this should really not happen
                std::cout << "Unknown error!\n";
        }
        return err;
    }
    else
    {
        std::cerr << "No errors\n";
    }
    return 0;

}
// vi:expandtab:tabstop=4 shiftwidth=4:
