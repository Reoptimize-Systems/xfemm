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


// vectors: overloading operators example
//#include <stdlib.h>
#include "intpoint.h"

//using namespace std;

myPoint::myPoint ()
{
  x = 0;
  y = 0;
}

myPoint::myPoint (long a, long b)
{
  x = a;
  y = b;
}

myPoint myPoint::operator+ (myPoint point) {
  myPoint temp;
  temp.x = x + point.x;
  temp.y = y + point.y;
  return (temp);
}

myPoint myPoint::operator- (myPoint point) {
  myPoint temp;
  temp.x = x - point.x;
  temp.y = y - point.y;
  return (temp);
}

bool myPoint::operator==( myPoint point)
{
	if ((x == point.x) & (y == point.y))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool myPoint::operator!=( myPoint point)
{
	if ((x == point.x) & (y == point.y))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void myPoint::operator+=( myPoint point)        // +ve translation
{
    x += point.x;
    y += point.y;
}

void myPoint::operator-=( myPoint point)        // +ve translation
{
    x -= point.x;
    y -= point.y;
}

void myPoint::Offset(long xOffset, long yOffset)
{
    x = x + xOffset;

    y = y + yOffset;
}

void myPoint::Offset( myPoint point )
{
    x = x + point.x;
    y = y + point.y;
}


//void Point::Offset(SIZE size )
//{
//
//}
