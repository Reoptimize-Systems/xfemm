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
        sztibi82@yahoo.com
        richard.crozier@yahoo.co.uk
*/

#ifndef HSPARS_H
#define HSPARS_H

#include "spars.h"

class CHBigLinProb : public CBigLinProb
{
public:

    // constructor
    CHBigLinProb();
    // destructor
    ~CHBigLinProb();

    // data members
    int *Q;

    // member functions

    int Create(int d, int bw);	// initialize the problem

};

#endif
