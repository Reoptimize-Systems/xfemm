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

// IntPoint, replacement for the MFC CPoint class
#ifndef INTPOINT_H
#define INTPOINT_H

namespace femm {

/**
 * @brief The IntPoint class is a drop-in replacement for CPoint (as far as the needs of FEMM are concerned).
 */
class IntPoint {

  public:

    IntPoint ();

    IntPoint (long a, long b);

    long x, y;


    IntPoint operator + (IntPoint point);
    IntPoint operator - (IntPoint point);
    void operator += ( IntPoint point);
    void operator -= ( IntPoint point);

    bool operator == (IntPoint point);
    bool operator != (IntPoint point);

    void Offset(long x, long y);

    void Offset( IntPoint point );

};

} //namespace
#endif
