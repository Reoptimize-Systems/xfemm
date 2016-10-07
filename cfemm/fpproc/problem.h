#ifndef PROBLEM_H
#define PROBLEM_H

#include <string>
#include "femmcomplex.h"

namespace FPProcdata{

using std::string;


class CElement
{
    public:

        int p[3];
        int blk,lbl;
        CComplex B1,B2;
        CComplex b1[3],b2[3];
        double magdir;
        CComplex ctr;
        double rsqr;
        int n[3];  // 3 ints to store elem's neighbouring elements

    private:
};

class CPointVals
{
    public:

        CComplex A;            // vector potential
        CComplex B1,B2;        // flux density
        CComplex mu1,mu2;    // permeability
        CComplex H1,H2;        // field intensity
        CComplex Je,Js;        // eddy current and source current densities
        CComplex Hc;        // Magnetization for regions with a PM.
        double c;            // conductivity
        double E;            // energy stored in the magnetic field
        double Ph;            // power dissipated by hysteresis
        double Pe;            // power dissipated by eddy currents
        double ff;            // winding fill factor

        CPointVals();

    private:
};

}
#endif

using namespace FPProcdata;
