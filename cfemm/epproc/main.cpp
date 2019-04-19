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

   Date Modified: 2019-04-05
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
		  Johannes Zarl-Zierl
   Contact:
        szelitzkye@gmail.com
        sztibi82@gmail.com
        richard.crozier@yahoo.co.uk
		  johannes@zarl-zierl.at
*/

#include "epproc.h"
#include "femmcomplex.h"

#include <cstdio>
#include <iostream>

using namespace std;


int main(int argc, char **argv)
{
    string doc;
    if (argc >= 2)
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


    ElectrostaticsPostProcessor testEPProc;
    CComplex out;
    CSPointVals u;
    double x, y;

    cout << "EPProc Loaded!" << endl;

    int test = testEPProc.OpenDocument(doc);

    if (test==true)
    {
        testEPProc.clearSelection();

        testEPProc.getProblem()->labellist[0]->ToggleSelect();

        out = testEPProc.blockIntegral(0);
        fprintf (of, "Stored Energy Integral for block 0 %f\n", out.Re());

        out = testEPProc.blockIntegral(1);
        fprintf (of, "Block Cross-section Integral for block 0 %f\n", out.Re());

        out = testEPProc.blockIntegral(2);
        fprintf (of, "Block Volume Integral for block 0 %f\n", out.Re());

        out = testEPProc.blockIntegral(3);
        fprintf (of, "Average D Integral for block 0 Fx: %f, Fy: %f\n", out.Re(), out.Im());

        out = testEPProc.blockIntegral(4);
        fprintf (of, "Average E Integral for block 0 Gx: %f, Gy: %f\n", out.Re(), out.Im());

        testEPProc.makeMask();
        out = testEPProc.blockIntegral(5);
        fprintf (of, "Weighted Stress Tensor Force Integral for block 0 Gx: %f, Gy: %f\n", out.Re(), out.Im());

        out = testEPProc.blockIntegral(6);
        fprintf (of, "Weighted Stress Tensor Torque Integral for block 0 Gx: %f, Gy: %f\n", out.Re(), out.Im());

        testEPProc.setSmoothing(false);

        fprintf (of, "Field Smoothing OFF\n");

        // get point values
        x = 0.25;
        y = 0;
        testEPProc.getPointValues(x, y, u);
        fprintf (of, "Point vals at x = %f, y = %f\nDx: %f\tDy: %f\tEx: %f\tEy: %f\tV: %f\tex: %f\tey: %f\t nrg: %f\n",
                x, y, u.D.Re() , u.D.Im(), u.E.Re(), u.E.Im(), u.V, u.e.Re(), u.e.Im(), u.nrg);

        // get point values
        x = 0.1;
        y = 0.8;
        testEPProc.getPointValues(x, y, u);
        fprintf (of, "Point vals at x = %f, y = %f\nDx: %f\tDy: %f\tEx: %f\tEy: %f\tV: %f\tex: %f\tey: %f\t nrg: %f\n",
                x, y, u.D.Re() , u.D.Im(), u.E.Re(), u.E.Im(), u.V, u.e.Re(), u.e.Im(), u.nrg);

        testEPProc.setSmoothing(true);

        fprintf (of, "Field Smoothing ON\n");

        // get point values
        x = 0.25;
        y = 0;
        testEPProc.getPointValues(x, y, u);
        fprintf (of, "Point vals at x = %f, y = %f\nDx: %f\tDy: %f\tEx: %f\tEy: %f\tV: %f\tex: %f\tey: %f\t nrg: %f\n",
                x, y, u.D.Re() , u.D.Im(), u.E.Re(), u.E.Im(), u.V, u.e.Re(), u.e.Im(), u.nrg);

        // get point values
        x = 0.1;
        y = 0.8;
        testEPProc.getPointValues(x, y, u);
        fprintf (of, "Point vals at x = %f, y = %f\nDx: %f\tDy: %f\tEx: %f\tEy: %f\tV: %f\tex: %f\tey: %f\t nrg: %f\n",
                x, y, u.D.Re() , u.D.Im(), u.E.Re(), u.E.Im(), u.V, u.e.Re(), u.e.Im(), u.nrg);

        return 0;
    }

    return 1;
}
