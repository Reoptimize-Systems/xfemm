
//#undef min
//#undef max
//#define NOMINMAX
/*
   This code is a modified version of an algorithm
   forming part of the software program Finite
   Element Method Magnetics (FEMM), authored by
   David Meeker. The original software code is
   subject to the Aladdin Free Public Licence
   version 8, November 18, 1999. For more information
   on FEMM see www.femm.info. This modified version
   is not endorsed in any way by the original
   authors of FEMM.

   This software has been modified to use the C++
   standard template libraries and remove all Microsoft (TM)
   MFC dependent code to allow easier reuse across
   multiple operating system platforms.

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "femmcomplex.h"
//#include "spars.h"
//#include "mmesh.h"
#include "feasolver.h"
#include "fsolver.h"

//using namespace std;

//#include "lua.h"

int main(int argc, char** argv)
{
    FSolver theFSolver;
    char PathName[512];
//    int i;

    if (argc < 2)
    {
        // request the file name from the user
        printf("Enter fem file name without extension:\n");

        //char tempFilePath[512];

        //scanf("%s", tempFilePath);

        fgets(PathName, 512, stdin);
        char *pos;
        if ((pos=strchr(PathName, '\n')) != NULL)
            *pos = '\0';

        //PathName = tempFilePath;

    }
    else if(argc > 2)
    {
        printf("Too many arguments");
    }
    else
    {
        strcpy(PathName, argv[1]);
    }

    theFSolver.PathName = PathName;

    if (theFSolver.LoadProblemFile () != true)
    {
        theFSolver.WarnMessage("problem loading .fem file\n");
        return 1;
    }

    if ( !theFSolver.runSolver(true))
        return 2;

    return 0;
}
