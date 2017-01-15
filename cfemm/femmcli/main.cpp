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
#include "FemmState.h"
#include "LuaBaseCommands.h"
#include "LuaInstance.h"
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

enum OperationMode { NoOperation, LuaMode, SolverMode };

/**
 * \brief Execute a Lua File
 * \param inputFile the lua file
 * \param luaInit a lua file containing initialization code
 * \param luaTrace enable function tracing for lua
 * \return the result of lua_dostring()
 */
int execLuaFile( const std::string &inputFile, const std::string &luaInit, bool luaTrace)
{
    // initialize interpreter
    shared_ptr<FemmState> state = make_shared<FemmState>();
    LuaInstance li(static_pointer_cast<FemmStateBase>(state));
    LuaBaseCommands::registerCommands(li);
    LuaMagneticsCommands::registerCommands(li);
    li.enableTracing(luaTrace);
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

/**
 * \brief solveFile
 * Determines the file type and runs the appropriate solver on the file.
 * \param inputFile
 * \return
 */
int solveFile( const std::string &inputFile)
{
    if (inputFile.size() < 3)
    {
        std::cerr << "Input file name too short.\n";
        return 1;
    }
    auto dotIdx = inputFile.find_last_of('.');
    std::string ending = inputFile.substr(dotIdx+1);
    to_lower(ending);
    if (ending == "fem")
    {
        FSolver theFSolver;
        theFSolver.PathName = inputFile.substr(0,dotIdx);
        debug << inputFile << " is a magnetics file; setting PathName to " << theFSolver.PathName << "\n";

        if (theFSolver.LoadProblemFile () != true)
        {
            theFSolver.WarnMessage("problem loading .fem file\n");
            return 1;
        }

        // load mesh
        int err = theFSolver.LoadMesh();
        if (err != 0)
        {
            theFSolver.WarnMessage("problem loading mesh:\n");

            switch (err)
            {
                case ( BADEDGEFILE ):
                    theFSolver.WarnMessage("Could not open .edge file.\n");
                    break;

                case ( BADELEMENTFILE ):
                    theFSolver.WarnMessage("Could not open .ele file.\n");
                    break;

                case( BADFEMFILE ):
                    theFSolver.WarnMessage("Could not open .fem file.\n");
                    break;

                case( BADNODEFILE ):
                    theFSolver.WarnMessage("Could not open .node file.\n");
                    break;

                case( BADPBCFILE ):
                    theFSolver.WarnMessage("Could not open .pbc file.\n");
                    break;

                case( MISSINGMATPROPS ):
                    theFSolver.WarnMessage("Material properties have not been defined for all regions.\n");
                    break;

                default:
                    theFSolver.WarnMessage("AN unknown error occured.\n");
                    break;
            }

            return 2;
        }

        // renumber using Cuthill-McKee
        std::cout << "renumbering nodes\n";
        if (theFSolver.Cuthill() != true)
        {
            theFSolver.WarnMessage("problem renumbering node points");
            return 3;
        }

        std::cout << "solving...\n";

        std::cout.precision(2);
        std::cout.width(3);
        std::cout << "Problem Statistics:\n"
                  << theFSolver.NumNodes << " nodes\n"
                  << theFSolver.NumEls << " elements\n"
                  << "Precision: " << theFSolver.Precision << "\n";


        //    double mr = (8.*((double) Doc.NumNodes)*((double) Doc.BandWidth)) / 1.e06;

        if(theFSolver.Frequency == 0)
        {

            CBigLinProb L;

            L.Precision = theFSolver.Precision;

            // initialize the problem, allocating the space required to solve it.
            if (L.Create(theFSolver.NumNodes, theFSolver.BandWidth) == false)
            {
                theFSolver.WarnMessage("couldn't allocate enough space for matrices");
                return 4;
            }

            // Create element matrices and solve the problem;
            if (theFSolver.ProblemType == false)
            {
                if (theFSolver.Static2D(L) == false)
                {
                    theFSolver.WarnMessage("Couldn't solve the problem");
                    return 5;
                }
                std::cout << "Static 2-D problem solved\n";
            }
            else
            {

                if (theFSolver.StaticAxisymmetric(L) == false)
                {
                    theFSolver.WarnMessage("Couldn't solve the problem");
                    return 5;
                }
                std::cout << "Static axisymmetric problem solved\n";
            }

            if (theFSolver.WriteStatic2D(L) == false)
            {
                theFSolver.WarnMessage("couldn't write results to disk");
                return 6;
            }
            std::cout << "results written to disk\n";
        }
        else
        {
            CBigComplexLinProb L;

            L.Precision = theFSolver.Precision;

            // initialize the problem, allocating the space required to solve it.

            if (L.Create(theFSolver.NumNodes+theFSolver.NumCircProps, theFSolver.BandWidth, theFSolver.NumNodes) == false)
            {
                theFSolver.WarnMessage("couldn't allocate enough space for matrices");
                return 4;
            }

            // Create element matrices and solve the problem;
            if (theFSolver.ProblemType == false)
            {
                if (theFSolver.Harmonic2D(L) == false)
                {
                    theFSolver.WarnMessage("Couldn't solve the problem");
                    return 5;
                }
                std::cout << "Harmonic 2-D problem solved\n";
            }
            else
            {
                if (theFSolver.HarmonicAxisymmetric(L) == false)
                {
                    theFSolver.WarnMessage("Couldn't solve the problem");
                    return 5;
                }
                std::cout << "Harmonic axisymmetric problem solved\n";
            }


            if (theFSolver.WriteHarmonic2D(L)==false)
            {
                theFSolver.WarnMessage("couldn't write results to disk");
                return 6;
            }
            std::cout << "results written to disk.\n";
        }
        return 0;
    }

    // fall-through:
    std::cerr << "Could not determine file type!\n";
    return 1;
}

/**
 * @brief splitArg splits a c string into two parts and appends it to two std::strings.
 * @param cstr the source c string (e.g. "--arg=value")
 * @param arg an existing string where the first part of cstr (e.g. "--arg") is appended.
 * @param value an existing string where the second part of cstr (e.g. "value") is appended.
 */
void splitArg(const char cstr[], std::string &arg, std::string &value)
{
    bool isValue=false;
    for(const char *c=cstr; *c!=0 ; c++)
    {
        if (*c=='=')
            isValue = true;
        else if (isValue)
            value += *c;
        else
            arg += *c;
    }
}

int main(int argc, char ** argv)
{
    std::string exe { argv[0] };
    std::string exeDir { exe.substr(0,exe.find_last_of("/\\")+1) };
    exe = exe.substr(exeDir.length());

    OperationMode opMode { NoOperation };
    std::string inputFile;
    std::string luaInit { exeDir + "init.lua" };
    bool luaTrace = false;

    for(int i=1; i<argc; i++)
    {
        std::string arg;
        std::string value;
        splitArg(argv[i],arg,value);
        bool setInputFile=false;
        if (arg == "--lua-script")
        {
            opMode = LuaMode;
            setInputFile=true;
        }
        if (arg == "--solve-file")
        {
            opMode = SolverMode;
            setInputFile=true;
        }
        if (setInputFile)
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
            std::cerr << "Using custom init.lua: " << luaInit << std::endl;
            continue;
        }
        if (arg == "--lua-enable-tracing" )
        {
            luaTrace = true;
            continue;
        }
        if (arg == "-q" || arg == "--quiet")
        {
            quiet = true;
            continue;
        }
        // unhandled argument -> print usage and exit
        if (arg != "-h" && arg != "--help")
            std::cerr << "Unknown argument: " << arg << std::endl;
        std::cout << "Usage: " << exe << " [-q|--quiet] [--lua-enable-tracing] --lua-script=<file.lua>\n";
        std::cout << "       " << exe << " --solve-file=<file.feX>\n";
        return 1;
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
            exitCode = execLuaFile(inputFile, luaInit, luaTrace);
            break;
        case SolverMode:
            exitCode = solveFile(inputFile);
            break;
    }

    return exitCode;

}
// vi:expandtab:tabstop=4 shiftwidth=4:
