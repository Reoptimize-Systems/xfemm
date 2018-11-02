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
#ifndef FEMM_CPOSTPROCMELEMENT_H
#define FEMM_CPOSTPROCMELEMENT_H

#include "CElement.h"
#include "femmcomplex.h"

//#include <iostream>
//#include <string>

namespace femmpostproc {

class CPostProcMElement : public femmsolver::CMElement
{
public:
    CPostProcMElement();
    virtual ~CPostProcMElement();

//    /**
//     * @brief fromStream constructs a CMElement from an input stream (usually an input file stream)
//     * @param input
//     * @param err output stream for error messages
//     * @return a CMElement
//     */
//    static CMElement fromStream( std::istream &input, std::ostream &err = std::cerr );


    double B1p;  /// < previous value of B1
    double B2p;  /// < previous value of B2
};

}
#endif
