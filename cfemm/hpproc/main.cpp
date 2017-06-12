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
#include <cstdio>
#include "hpproc.h"
#include "femmcomplex.h"

using namespace std;


int main()
{
    string mystr;
    HPProc testHPProc;
    CComplex out;
    CHPointVals u;
    double x, y;

    cout << "HPProc Loaded!" << endl;


    int test = testHPProc.OpenDocument("./test/Temp0.anh");

    if (test==true)
    {
        for(int i=0; i<(int)(testHPProc.blocklist.size()); i++)
        {
            if (testHPProc.blocklist[i].IsSelected == true)
            {
                testHPProc.blocklist[i].IsSelected = false;
            }
        }

       // for(int j=0; j<(int)(testHPProc.blocklist.size()); j++)
        {
          //  if (testHPProc.blocklist[j].InGroup==3)
            {
                testHPProc.blocklist[0].ToggleSelect();
            }

         //   testHPProc.bHasMask = false;
        }


        out = testHPProc.BlockIntegral(0);
        printf ("Block Temperature Integral for block 0 %f\n", out.Re());

        out = testHPProc.BlockIntegral(1);
        printf ("Block Cross-section Area Integral for block 0 %f\n", out.Re());

        out = testHPProc.BlockIntegral(2);
        printf ("Block Volume Integral for block 0 %f\n", out.Re());

        out = testHPProc.BlockIntegral(3);
        printf ("Block Average F Integral for block 0 Fx: %f, Fy: %f\n", out.Re(), out.Im());

        out = testHPProc.BlockIntegral(4);
        printf ("Block Average G Integral for block 0 Gx: %f, Gy: %f\n", out.Re(), out.Im());

        testHPProc.Smooth = false;

        printf ("Field Smoothing OFF\n");

        // get point values
        x = 0.01;
        y = 0.01;
        testHPProc.GetPointValues(x, y, u);
        printf ("Point vals at x = %f, y = %f\nT: %f\tFx: %f\tFy: %f\tKx: %f\tKy: %f\tGx: %f\tGy: %f\n",
                x, y, u.T, u.F.Re(), u.F.Im(), u.K.Re(), u.K.Im(), u.G.Re(), u.G.Im());

        // get point values
        x = 0.005;
        y = 0.005;
        testHPProc.GetPointValues(x, y, u);
        printf ("Point vals at x = %f, y = %f\nT: %f\tFx: %f\tFy: %f\tKx: %f\tKy: %f\tGx: %f\tGy: %f\n",
                x, y, u.T, u.F.Re(), u.F.Im(), u.K.Re(), u.K.Im(), u.G.Re(), u.G.Im());

        testHPProc.Smooth = true;

        printf ("Field Smoothing ON\n");

        // get point values
        x = 0.01;
        y = 0.01;
        testHPProc.GetPointValues(x, y, u);
        printf ("Point vals at x = %f, y = %f\nT: %f\tFx: %f\tFy: %f\tKx: %f\tKy: %f\tGx: %f\tGy: %f\n",
                x, y, u.T, u.F.Re(), u.F.Im(), u.K.Re(), u.K.Im(), u.G.Re(), u.G.Im());

        // get point values
        x = 0.005;
        y = 0.005;
        testHPProc.GetPointValues(x, y, u);
        printf ("Point vals at x = %f, y = %f\nT: %f\tFx: %f\tFy: %f\tKx: %f\tKy: %f\tGx: %f\tGy: %f\n",
                x, y, u.T, u.F.Re(), u.F.Im(), u.K.Re(), u.K.Im(), u.G.Re(), u.G.Im());

    }

    return 0;
}
