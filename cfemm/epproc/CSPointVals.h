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
#ifndef CSPOINTVALS_H
#define CSPOINTVALS_H

#include "femmcomplex.h"

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

#endif
