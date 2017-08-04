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

    Date Modified: 2017-07-28
    By:  Emoke Szelitzky
         Tibor Szelitzky
         Richard Crozier
         Johannes Zarl-Zierl
    Contact:
         szelitzkye@gmail.com
         sztibi82@gmail.com
         richard.crozier@yahoo.co.uk
         johannes@zarl-zierl.at

    Contributions by Johannes Zarl-Zierl were funded by
    Linz Center of Mechatronics GmbH (LCM)
*/

#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "femmcomplex.h"
#include "spars.h"
#include "esolver.h"


int main(int argc, char** argv)
{
    ESolver solverInstance;
    char PathName[512];

    if (argc < 2)
    {
        // request the file name from the user
        printf("Enter fee file name without extension:\n");

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

    solverInstance.PathName = PathName;

    if (solverInstance.LoadProblemFile () != true)
    {
        solverInstance.WarnMessage("problem loading .fee file");
        return 1;
    }

    if (!solverInstance.runSolver(true))
        return 2;

    return 0;
}
