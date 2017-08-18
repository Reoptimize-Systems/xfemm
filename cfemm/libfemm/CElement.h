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

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/
#ifndef FEMM_CELEMENT_H
#define FEMM_CELEMENT_H

#include <string>
#include "femmcomplex.h"

namespace femmsolver {

/**
 * @brief The CElement class holds solution elements.
 * This class has been merged from the CElement classes in libfemm/feasolver, esolver, and fpproc.
 */
class CElement
{
public:
    CElement();
    virtual ~CElement();

    int p[3]; ///< nodes at the corners of the element
    int e[3]; ///< boundary condition applied to each edge of the element
    int blk;  ///< block property applied to the element
    int lbl;  ///< block label associated with the element
    CComplex ctr;
    double rsqr;
    int n[3]; ///< 3 ints to store elem's neighbouring elements
};

class CMElement : public CElement
{
public:
    CMElement();
    virtual ~CMElement();
    CComplex mu1,mu2;
    CComplex B1,B2;
    CComplex b1[3],b2[3];
    double magdir;
};

class CHElement : public CElement
{
public:
    CHElement();

    CComplex D;		// elemental flux density
    CComplex d[3];  // smoothed flux density at corners

private:
};

}
#endif
