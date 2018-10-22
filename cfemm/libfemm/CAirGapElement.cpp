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
#include "CAirGapElement.h"

#include "stringTools.h"

#include <istream>
#include <sstream>

using femm::trim;


femmsolver::CAirGapElement::~CAirGapElement()
{
	if (node!=NULL) free(node);
}


femmsolver::CAirGapElement::CAirGapElement()
    : BdryName("")
    , BdryFormat(0)
    , totalArcElements(0)
    , totalArcLength(0)
    , ri(0)
    , ro(0)
    , InnerAngle(0)
    , OuterAngle(0)
    , agc(0)
    , node(NULL)
{
}


//femmsolver::CAirGapElement femmsolver::CMElement::fromStream(std::istream &input, std::ostream &)
//{
//    std::string line;
//    // read whole line to prevent reading from the next line if a line is malformed/too short
//    std::getline(input, line);
//    trim(line);
//    std::istringstream inputStream(line);
//
//    CMElement e;
//    // scan in data
//    inputStream >> e.p[0];
//    inputStream >> e.p[1];
//    inputStream >> e.p[2];
//    inputStream >> e.lbl;
//
//    return e;
//}

