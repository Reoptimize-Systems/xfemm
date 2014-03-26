#include <iostream>
#include "fpproc.h"
#include "femmcomplex.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

using namespace std;

int main()
{
    string mystr;
    FPProc testFPProc;
    CComplex out;

    cout << "FPProc Loaded!" << endl;

    //getline (cin, mystr);

    //testFPProc.OpenDocument(mystr);
    int test = testFPProc.OpenDocument("./test/Temp.ans");

    if (test==TRUE)
    {
        for(int i=0; i<(int)(testFPProc.blocklist.size()); i++)
        {
            if (testFPProc.blocklist[i].IsSelected == TRUE)
            {
                testFPProc.blocklist[i].IsSelected = FALSE;
            }
        }

        for(int j=0; j<(int)(testFPProc.blocklist.size()); j++)
        {
            if (testFPProc.blocklist[j].InGroup==3)
            {
                testFPProc.blocklist[j].ToggleSelect();
            }

            testFPProc.bHasMask = FALSE;
        }

        testFPProc.MakeMask();

        out = testFPProc.BlockIntegral(18);
    }

    return 0;
}
