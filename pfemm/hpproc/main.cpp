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

#include <iostream>
#include "hpproc.h"
#include "complex.h"

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
    HPProc testHPProc;
    CComplex out;

    cout << "HPProc Loaded!" << endl;


    int test = testHPProc.OpenDocument("htutor0.anh");

    if (test==TRUE)
    {
        for(int i=0; i<(int)(testHPProc.blocklist.size()); i++)
        {
            if (testHPProc.blocklist[i].IsSelected == TRUE)
            {
                testHPProc.blocklist[i].IsSelected = FALSE;
            }
        }

        for(int j=0; j<(int)(testHPProc.blocklist.size()); j++)
        {
          //  if (testHPProc.blocklist[j].InGroup==3)
            {
                testHPProc.blocklist[0].ToggleSelect();
            }

            testHPProc.bHasMask = FALSE;
        }



        out = testHPProc.BlockIntegral(2);
    }

    return 0;
}
