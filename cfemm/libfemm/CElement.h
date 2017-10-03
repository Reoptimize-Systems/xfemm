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

#include "femmcomplex.h"

#include <iostream>
#include <string>

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
    int blk;  ///< block property applied to the element, i.e. an index into the CMaterialProp list (blockproplist)
    int lbl;  ///< block label associated with the element, i.e. an index into the CBlockLabel list (labellist)
    CComplex ctr;
    double rsqr;
    int n[3]; ///< 3 ints to store elem's neighbouring elements
};

class CMElement : public CElement
{
public:
    CMElement();
    virtual ~CMElement();

    /**
     * @brief fromStream constructs a CMElement from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CMElement
     */
    static CMElement fromStream( std::istream &input, std::ostream &err = std::cerr );

    CComplex mu1,mu2;
    CComplex B1,B2;
    CComplex b1[3],b2[3];
    double magdir;
};

class CHElement : public CElement
{
public:
    CHElement();
    /**
     * @brief fromStream constructs a CHElement from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CHElement
     */
    static CHElement fromStream( std::istream &input, std::ostream &err = std::cerr );

    CComplex D;		// elemental flux density
    CComplex d[3];  // smoothed flux density at corners

private:
};

/**
 * @brief The CSElement class
 * This seems to be the same as the CHElement class.
 *
 * \internal
 * ### FEMM reference source:
 * - \femm42{femm/bv_problem.h,CElement}
 */
class CSElement : public CElement
{
public:
    CSElement();
    /**
     * @brief fromStream constructs a CHElement from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @return a CHElement
     * \internal
     * ### FEMM reference source:
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::OnOpenDocument()}
     * \endinternal
     */
    static CSElement fromStream( std::istream &input, std::ostream &err = std::cerr );

    CComplex D;    // elemental flux density
    CComplex d[3];  // smoothed flux density at corners

    //CComplex E(); unused?
};

}
#endif
