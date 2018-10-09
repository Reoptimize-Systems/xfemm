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
#ifndef FEMM_CAIRGAPELEMENT_H
#define FEMM_CAIRGAPELEMENT_H

#include "femmcomplex.h"

#include <iostream>
#include <string>

namespace femmsolver {

/**
 * @brief The CAirGapElement class holds solution elements for the air gap elements.
  *
 * \internal
 * ### FEMM reference source:
 * - \femm42{femm/Problem.h,CAirGapElement}
 * - \femm42{femm/NOSEBL.H,CAirGapElement}
 * - \femm42{fkn/mesh.h,CAirGapElement}
 */
 */
class CAirGapElement
{

public:

    CAirGapElement();
    virtual ~CAirGapElement();

//    /**
//     * @brief fromStream constructs a CAirGapElement from an input stream (usually an input file stream)
//     * @param input
//     * @param err output stream for error messages
//     * @return a CAirGapElement
//     */
//    static CAirGapElement fromStream( std::istream &input, std::ostream &err = std::cerr );

    std::string BdryName;
    int BdryFormat; ///<  0 = Periodic, 1 = Antiperiodic

    int totalArcElements; ///< total elements in the initial meshing
    double totalArcLength; ///< sum of the angles of all constituent arcs
    double ri,ro; ///< inner and outer radii of the air gap element
    double InnerAngle;///< Angle in degrees through which the inner ring (rotor) is turned
    double OuterAngle;///< Angle in degrees through which the inner ring (stator) is turned
    double InnerShift;///< fraction of an element that inner mesh is shifted relative to annular mesh
    double OuterShift;///< fraction of an element that outer mesh is shifted relative to annular mesh
    CComplex agc; ///< centre of the air gap element
    int *node; ///< node numbers that are part of the air gap element

    CQuadPoint *qp; ///< mapping of mesh nodes onto an annular ring

    int nn;
    CComplex aco;
    CComplex *brc, *brs; ///< harmonic components of air gap centerline flux density
    CComplex *btc, *bts;
    CComplex *br, *bt;
    double *brcPrev, *brsPrev; ///< harmonic components of air gap centerline flux density from prev solution
    double *btcPrev, *btsPrev;
    double *brPrev,*btPrev;
    int *nh; ///< list of number associated with each harmonic in harmonic problem


private:
};

}
#endif
