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

#include <vector>
#include <string>
#include "femmcomplex.h"

namespace femm
{

/////////////////////////////////////////////////////////////////////////////
// CNode -- class that holds information about each control point.

class CNode
{
public:
    CNode();

    double x,y;
    int xs,ys;
    int IsSelected;
    std::string BoundaryMarker;
    std::string InConductor;
    int InGroup;

    double GetDistance(double xo, double yo);
    CComplex CC();
    void ToggleSelect();

private:

};

/////////////////////////////////////////////////////////////////////////////
// CSegment -- class that holds information about lines joining control pts

class CSegment
{
public:
    CSegment();

    int n0,n1;
    int IsSelected;
    bool Hidden;
    double MaxSideLength;
    std::string BoundaryMarker;
    std::string InConductor;
    int InGroup;

    void ToggleSelect();

private:

};

/////////////////////////////////////////////////////////////////////////////
// CArcSegment -- class that holds information about arcs joining control pts

class CArcSegment
{
public:
    CArcSegment();

    int n0,n1;
    bool NormalDirection;
    int IsSelected;
    bool Hidden;
    double MaxSideLength,ArcLength;
    std::string BoundaryMarker;
    std::string InConductor;
    int InGroup;

    void ToggleSelect();

private:

};
/////////////////////////////////////////////////////////////////////////////
// CBlockLabel -- class that holds block label information

class CBlockLabel
{
public:
    CBlockLabel();

    // common properties
    double x,y;
    double MaxArea;
    int IsSelected;
    std::string BlockType;
    int InGroup;

    // problem specific properties
    std::string InCircuit;

    void ToggleSelect();
    double GetDistance(double xo, double yo);

private:

};

/////////////////////////////////////////////////////////////////////////////
// CBoundaryProp -- class that holds information about boundary conditions

class CBoundaryProp
{
public:

    CBoundaryProp();

    std::string BdryName;
    int BdryFormat;            // type of boundary condition we are applying
    // 0 = constant value
    // 1 = Small skin depth eddy current BC
    // 2 = Mixed BC
    // 3 = SDI
    // 4 = Periodic
    // 5 = Antiperiodic

private:
};

/////////////////////////////////////////////////////////////////////////////
// CPointProp -- class that holds information about point properties

class CPointProp
{
public:

    CPointProp();

    std::string PointName;

private:
};

/////////////////////////////////////////////////////////////////////////////
// CCircuit -- class that holds information about boundary conditions

class CCircuit
{
public:

    CCircuit();

    std::string CircName;
    int        CircType;

private:

};

/////////////////////////////////////////////////////////////////////////////
// CPeriodicBoundary -- class that holds information on periodic boundaries

class CPeriodicBoundary
{
public:

    CPeriodicBoundary();

    std::string BdryName;
    int BdryFormat;            // 0 = Periodic
    // 1 = Antiperiodic
    int nseg;                // number of segs with this bc
    int narc;                // number of arcs with this bc
    int seg[2];                // (arc)segments to which is applied

private:
};

class CCommonPoint
{
public:

    CCommonPoint();
    void Order();

    int x,y,t;

private:
};

} // namespace femm

