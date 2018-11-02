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

   Date Modified: 2018 - 10 - 31
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

#include "CPostProcMElement.h"

femmpostproc::CPostProcMElement::CPostProcMElement()
    : femmsolver::CMElement()
    , B1p(0.)
    , B2p(0.)
{
}

femmpostproc::CPostProcMElement::~CPostProcMElement()
{
}




