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

   Date Modified: 2017 - 01 - 14
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
        Johannes Zarl-Zierl
   Contact:
        szelitzkye@gmail.com
        sztibi82@yahoo.com
        richard.crozier@yahoo.co.uk
        johannes@zarl-zierl.at

 Contributions by Johannes Zarl-Zierl were funded by Linz Center of 
 Mechatronics GmbH (LCM)
*/
#ifndef FEMM_CCOMMONPOINT_H
#define FEMM_CCOMMONPOINT_H


namespace femm {

/**
 * @brief The CCommonPoint class
 */
class CCommonPoint
{
    public:
        CCommonPoint();

        int x;
        int y;
        int t;

        /**
         * @brief sortXY
         * Sort the fields x and y.
         * After this operation, <tt>x <= y</tt> holds true.
         */
        void sortXY();
    private:

};

}
#endif
