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


#include "IntPoint.h"

//using namespace std;

femm::IntPoint::IntPoint ()
    : x(0), y(0)
{
}

femm::IntPoint::IntPoint (long a, long b)
    : x(a), y(b)
{
}

femm::IntPoint femm::IntPoint::operator+ (femm::IntPoint point) {
  femm::IntPoint temp;
  temp.x = x + point.x;
  temp.y = y + point.y;
  return (temp);
}

femm::IntPoint femm::IntPoint::operator- (femm::IntPoint point) {
  femm::IntPoint temp;
  temp.x = x - point.x;
  temp.y = y - point.y;
  return (temp);
}

bool femm::IntPoint::operator==( femm::IntPoint point)
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

bool femm::IntPoint::operator!=( femm::IntPoint point)
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

void femm::IntPoint::operator+=( femm::IntPoint point)        // +ve translation
{
    x += point.x;
    y += point.y;
}

void femm::IntPoint::operator-=( femm::IntPoint point)        // +ve translation
{
    x -= point.x;
    y -= point.y;
}

void femm::IntPoint::Offset(long xOffset, long yOffset)
{
    x = x + xOffset;

    y = y + yOffset;
}

void femm::IntPoint::Offset( femm::IntPoint point )
{
    x = x + point.x;
    y = y + point.y;
}
