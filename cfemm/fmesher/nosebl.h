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

   Date Modified: 2017
   By: Richard Crozier
       Johannes Zarl-Zierl
   Contact:
        richard.crozier@yahoo.co.uk
        johannes.zarl-zierl@jku.at

   Contributions by Johannes Zarl-Zierl were funded by
   Linz Center of Mechatronics GmbH (LCM)
*/

#include <memory>
#include <vector>
#include <string>
#include "femmcomplex.h"

namespace fmesher
{


/**
 * \brief CPeriodicBoundary class holds information on periodic boundaries
 */
class CPeriodicBoundary
{
public:

    CPeriodicBoundary();

    /**
     * @brief clone returns a copy of the CPeriodicBoundary that is memory managed using a unique_ptr.
     * This is a convenience function to make code more readable.
     * @return a unique_ptr holding a copy of this object.
     */
    std::unique_ptr<CPeriodicBoundary> clone() const;

    std::string BdryName;
    int BdryFormat;
    bool antiPeriodic;  ///< \brief \c true for antiperiodic boundary conditions, \c false for periodic ones.
    int nseg;                // number of segs with this bc
    int narc;                // number of arcs with this bc
    int seg[2];                // (arc)segments to which is applied

private:
};


} // namespace femm

