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
#include "CElement.h"

#include "stringTools.h"

#include <istream>
#include <sstream>

using femm::trim;

femmsolver::CElement::CElement()
    : p{0,0,0}
    , e{0,0,0}
    , blk(0)
    , lbl(0)
    , ctr(0)
    , rsqr(0)
    , n{0,0,0}
{
}

femmsolver::CElement::~CElement()
{
}

femmsolver::CMElement::CMElement()
    : CElement()
    , mu1(0)
    , mu2(0)
    , v12(0)
    , B1(0)
    , B2(0)
    , b1{0,0,0}
    , b2{0,0,0}
    , magdir(0)
{
}

femmsolver::CMElement::~CMElement()
{
}

femmsolver::CMElement femmsolver::CMElement::fromStream(std::istream &input, std::ostream &)
{
    std::string line;
    // read whole line to prevent reading from the next line if a line is malformed/too short
    std::getline(input, line);
    trim(line);
    std::istringstream inputStream(line);

    CMElement e;
    // scan in data
    inputStream >> e.p[0];
    inputStream >> e.p[1];
    inputStream >> e.p[2];
    inputStream >> e.lbl;

    return e;
}

femmsolver::CHSElement::CHSElement()
    : CElement()
    , D(0)
    , d{0,0,0}
{
}

femmsolver::CHSElement femmsolver::CHSElement::fromStream(std::istream &input, std::ostream &)
{
    std::string line;
    // read whole line to prevent reading from the next line if a line is malformed/too short
    std::getline(input, line);
    trim(line);
    std::istringstream inputStream(line);

    CHSElement e;
    // scan in data
    inputStream >> e.p[0];
    inputStream >> e.p[1];
    inputStream >> e.p[2];
    inputStream >> e.lbl;

    return e;
}
