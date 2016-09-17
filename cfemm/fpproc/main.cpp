#include <iostream>
#include "fpproc.h"
#include "femmcomplex.h"

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

    if (test==true)
    {
        for(int i=0; i<(int)(testFPProc.blocklist.size()); i++)
        {
            if (testFPProc.blocklist[i].IsSelected == true)
            {
                testFPProc.blocklist[i].IsSelected = false;
            }
        }

        for(int j=0; j<(int)(testFPProc.blocklist.size()); j++)
        {
            if (testFPProc.blocklist[j].InGroup==3)
            {
                testFPProc.blocklist[j].ToggleSelect();
            }

            testFPProc.bHasMask = false;
        }

        testFPProc.MakeMask();

        out = testFPProc.BlockIntegral(18);
    }

    return 0;
}
