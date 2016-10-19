#include "CCommonPoint.h"

#include <algorithm> //std::swap, pre C++11
#include <utility>   //std::swap, since C++11

// CCommonPoint construction
femm::CCommonPoint::CCommonPoint()
	: x(0),y(0),t(0)
{
}

void femm::CCommonPoint::sortXY()
{
    if(x>y)
    {
        std::swap(x,y);
    }
}
