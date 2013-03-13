#include <iostream>
#include "fpproc.h"
#include "complex.h"

using namespace std;

int main()
{
    string mystr;
    FPProc testFPProc;
    CComplex out;

    cout << "FPProc Loaded!" << endl;

    //getline (cin, mystr);

    //testFPProc.OpenDocument(mystr);

    if (testFPProc.OpenDocument("/home/s0237326/FEAerror.ans")==TRUE);
    {

        for(int i=0; i<testFPProc.blocklist.size(); i++)
        {
            if (testFPProc.blocklist[i].IsSelected == TRUE)
            {
                testFPProc.blocklist[i].IsSelected = FALSE;
            }
        }

        for(int j=0; j<testFPProc.blocklist.size(); j++)
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
