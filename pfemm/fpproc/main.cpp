#include <iostream>
#include "fpproc.h"

using namespace std;

int main()
{
    string mystr;
    FPProc testFPProc;

    cout << "FPProc Loaded!" << endl;

    getline (cin, mystr);

    testFPProc.OpenDocument(mystr);

    return 0;
}
