#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "fpproc.h"
#include "femmcomplex.h"

#ifndef NDEBUG
//setbuf(stdout, NULL);
#endif // NDEBUG

using namespace std;

int main(int argc, char** argv)
{
    string mystr;
    FPProc testFPProc;
    CComplex out;

    char PathName[512];
    char outstr[1024];
//    int i;

    if (argc < 2)
    {
        // request the file name from the user
        printf("Enter ans file name:\n");

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

    //getline (cin, mystr);

    //testFPProc.OpenDocument(mystr);
    int test = testFPProc.OpenDocument(PathName);

    if (test==true)
    {
        // Deselect all blocks in problem
        for(int i=0; i<(int)(testFPProc.blocklist.size()); i++)
        {
            if (testFPProc.blocklist[i].IsSelected == true)
            {
                testFPProc.blocklist[i].IsSelected = false;
            }
        }

        // select all blocks with group number 3
        for(int j=0; j<(int)(testFPProc.blocklist.size()); j++)
        {
            if (testFPProc.blocklist[j].InGroup==3)
            {
                testFPProc.blocklist[j].ToggleSelect();
            }

            testFPProc.bHasMask = false;
        }

        // perform integral
        testFPProc.MakeMask();

        out = testFPProc.BlockIntegral(18);
    }

    return 0;
}
