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

// guard mesh class definitions
#ifndef MMESH_H
#define MMESH_H

#include "mesh.h"
#include "femmcomplex.h"
#include "CMaterialProp.h"

namespace fsolver {

class CMMaterialProp : public femm::CMaterialProp
{
    // Properties
public:

    double Theta_m;			// orientation of magnetization, degrees

    // Methods
public:

    CMMaterialProp();
    virtual ~CMMaterialProp();
    CMMaterialProp( const CMMaterialProp & );
    CComplex GetH(double B); // ill-matched override
    CComplex Get_dvB2(double B);
    CComplex Get_v(double B);
    void GetBHProps(double B, CComplex &v, CComplex &dv);
    void GetBHProps(double B, double &v, double &dv);
    virtual CComplex LaminatedBH(double omega, int i) override;

    /**
     * @brief fromStream constructs a CMaterialProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CMaterialProp
     */
    static CMMaterialProp fromStream( std::istream &input, std::ostream &err = std::cerr );
    virtual void toStream( std::ostream &out ) const override;
private:
};

}


#endif
