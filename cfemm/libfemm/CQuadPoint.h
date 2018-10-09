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

   Contact:
	    richard.crozier@yahoo.co.uk
*/
#ifndef FEMM_CQUADPOINT_H
#define FEMM_CQUADPOINT_H

namespace femm {

class CQuadPoint
{
	public:
		int n0,n1,n2,n3;
		double w0,w1,w2,w3;
};

}
#endif
