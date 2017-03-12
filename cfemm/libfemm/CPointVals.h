#ifndef FEMM_CPOINTVALS_H
#define FEMM_CPOINTVALS_H

#include <string>
#include "femmcomplex.h"

namespace femm {
class CMPointVals
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

        CMPointVals();

    private:
};

// FIXME(ZaJ): before merging with CPointVals, both classes have to be adopted to fit
class CSPointVals
{
public:
    CSPointVals();

    double V;      // vector potential
    CComplex D;    // flux density
    CComplex e;    // permeability
    CComplex E;    // field intensity
    double nrg;    // energy stored in the field
};

}
#endif
