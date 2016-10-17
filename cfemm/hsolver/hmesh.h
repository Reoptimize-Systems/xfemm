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

// guard hmesh class definitions
#ifndef HMESH_H
#define HMESH_H

#include "mesh.h"
#include "femmcomplex.h"
#include "CBoundaryProp.h"
#include "CCircuit.h"

#include <iostream>


/////////////////////////////////////////////////////////////////////////////
// Classes that hold property data:  CMaterialProp, CHBoundaryProp, CPointProp
class CHMaterialProp
{
    // Properties
public:

   	double Kx,Ky;		// thermal conductivity for linear (possibly anisotropic) regions
	double Kt;			// volumetric heat capacity
	double qv;			// volume heat generation

	// properties for nonlinear conductivity
	int npts;			// number of points in the nonlinear conductivity curve
	CComplex Kn[128];   // here, I'm being _very_ lazy by defining a fixed-length buffer for the
	                    // thermal conductivity data points.

    // Methods
public:

    CHMaterialProp();
    ~CHMaterialProp();
    CHMaterialProp( const CHMaterialProp & );
    CComplex GetK(double t);

    /**
     * @brief fromStream constructs a CHMaterialProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CHMaterialProp
     */
    static CHMaterialProp fromStream( std::istream &input, std::ostream &err = std::cerr );
private:
};

class CHPointProp
{
public:
    /**
     * @brief fromStream constructs a CHPointProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CHPointProp
     */
    static CHPointProp fromStream( std::istream &input, std::ostream &err = std::cerr );

    double V,qp;

private:
};

class CHConductor : public femm::CCircuit
{
public:

    /**
     * @brief V thermal conductivity(?)
     *
     * Unit: ?
     * \verbatim
     * <tc>
     * \endverbatim
     */
    double V;
    /**
     * @brief q
     *
     * Unit: ?
     * \verbatim
     * <qc>
     * \endverbatim
     */
    double q;

    /**
     * @brief fromStream constructs a CHConductor from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CHConductor
     */
    static CHConductor fromStream( std::istream &input, std::ostream &err = std::cerr );
private:
};

#endif


