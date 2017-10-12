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

#include "hpproc.h"
#include "femmcomplex.h"

#include <cstdio>
#include <iostream>

using namespace std;


int main(int argc, char **argv)
{
    string doc;
    if (argc<=1)
        doc = "./test/Temp0.anh";
    else if (argc >= 2)
        doc = argv[1];

    FILE *of = stdout;
    if (argc == 3)
    {
        of = fopen(argv[2], "w");
        if (!of)
        {
            printf("Could not open file %s for writing!\n",argv[2]);
            return 1;
        }
    }


    HPProc testHPProc;
    CComplex out;
    CHPointVals u;
    double x, y;

    cout << "HPProc Loaded!" << endl;

    int test = testHPProc.OpenDocument(doc);

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
        fprintf (of, "Block Temperature Integral for block 0 %f\n", out.Re());

        out = testHPProc.BlockIntegral(1);
        fprintf (of, "Block Cross-section Area Integral for block 0 %f\n", out.Re());

        out = testHPProc.BlockIntegral(2);
        fprintf (of, "Block Volume Integral for block 0 %f\n", out.Re());

        out = testHPProc.BlockIntegral(3);
        fprintf (of, "Block Average F Integral for block 0 Fx: %f, Fy: %f\n", out.Re(), out.Im());

        out = testHPProc.BlockIntegral(4);
        fprintf (of, "Block Average G Integral for block 0 Gx: %f, Gy: %f\n", out.Re(), out.Im());

        testHPProc.Smooth = false;

        fprintf (of, "Field Smoothing OFF\n");

        // get point values
        x = 0.01;
        y = 0.01;
        testHPProc.GetPointValues(x, y, u);
        fprintf (of, "Point vals at x = %f, y = %f\nT: %f\tFx: %f\tFy: %f\tKx: %f\tKy: %f\tGx: %f\tGy: %f\n",
                x, y, u.T, u.F.Re(), u.F.Im(), u.K.Re(), u.K.Im(), u.G.Re(), u.G.Im());

        // get point values
        x = 0.005;
        y = 0.005;
        testHPProc.GetPointValues(x, y, u);
        fprintf (of, "Point vals at x = %f, y = %f\nT: %f\tFx: %f\tFy: %f\tKx: %f\tKy: %f\tGx: %f\tGy: %f\n",
                x, y, u.T, u.F.Re(), u.F.Im(), u.K.Re(), u.K.Im(), u.G.Re(), u.G.Im());

        testHPProc.Smooth = true;

        fprintf (of, "Field Smoothing ON\n");

        // get point values
        x = 0.01;
        y = 0.01;
        testHPProc.GetPointValues(x, y, u);
        fprintf (of, "Point vals at x = %f, y = %f\nT: %f\tFx: %f\tFy: %f\tKx: %f\tKy: %f\tGx: %f\tGy: %f\n",
                x, y, u.T, u.F.Re(), u.F.Im(), u.K.Re(), u.K.Im(), u.G.Re(), u.G.Im());

        // get point values
        x = 0.005;
        y = 0.005;
        testHPProc.GetPointValues(x, y, u);
        fprintf (of, "Point vals at x = %f, y = %f\nT: %f\tFx: %f\tFy: %f\tKx: %f\tKy: %f\tGx: %f\tGy: %f\n",
                x, y, u.T, u.F.Re(), u.F.Im(), u.K.Re(), u.K.Im(), u.G.Re(), u.G.Im());

        return 0;
    }

    return 1;
}
