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

#ifndef CMPOINTVALS_H
#define CMPOINTVALS_H

#include "femmcomplex.h"

class CMPointVals
{
public:
    CMPointVals();

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
private:
};

#endif
