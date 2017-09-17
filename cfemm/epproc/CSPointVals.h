/* Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by
 * Linz Center of Mechatronics GmbH (LCM)
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */
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

class CHPointVals
{
public:
    CHPointVals();

    double T;		// temperature
    CComplex F;		// heat flux density
    CComplex K;		// thermal conductivity
    CComplex G;		// temperature gradient
private:
};

}
#endif
