#include <iostream>
#include "fpproc.h"

using namespace std;

int main()
{
    string mystr;
    FPProc testFPProc;

    cout << "FPProc Loaded!" << endl;

    //getline (cin, mystr);

    //testFPProc.OpenDocument(mystr);

    testFPProc.OpenDocument("/home/s0237326/src/mfemm/pfemm/fsolver/test/Temp.ans");

    return 0;
}
