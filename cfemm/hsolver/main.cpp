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

   Date Modified: 2014 - 03 - 21
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
   Contact:
        szelitzkye@gmail.com
        sztibi82@gmail.com
        richard.crozier@yahoo.co.uk
*/

#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "femmcomplex.h"
#include "spars.h"
#include "hsolver.h"

//using namespace std;


int main(int argc, char** argv)
{
    HSolver theHSolver;
    char PathName[512];

    if (argc < 2)
    {
        // request the file name from the user
        printf("Enter feh file name without extension:\n");

        fgets(PathName, 512, stdin);
        char *pos;
        if ((pos=strchr(PathName, '\n')) != NULL)
            *pos = '\0';

    }
    else if(argc > 2)
    {
        printf("Too many arguments");
    }
    else
    {
        strcpy(PathName, argv[1]);
    }

    theHSolver.PathName = PathName;

    if (theHSolver.LoadProblemFile () != true)
    {
        theHSolver.WarnMessage("problem loading .feh file");
        return 1;
    }

    if ( !theHSolver.runSolver(true))
        return 2;

    return 0;
}
