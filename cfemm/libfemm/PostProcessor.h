#ifndef FEMM_POSTPROCESSOR_J
#define FEMM_POSTPROCESSOR_J

#include "lua.h"
#include "luadebug.h"
#include "femmcomplex.h"
#include "fparse.h"
#include "FemmProblem.h"

#include <vector>


class PostProcessor
{
public:

    PostProcessor();
    virtual ~PostProcessor();

    std::shared_ptr<femm::FemmProblem> problem;
    // General problem attributes
    double *LengthConv;
    bool    Smooth;
    bool    bMultiplyDefinedLabels;

    double  PlotBounds[9][2];
    double  d_PlotBounds[9][2];

    // Some default behaviors
    int  d_LineIntegralPoints;
    bool bHasMask;

    // List of elements connected to each node;
    int *NumList;
    int **ConList;

    // list of points in a user-defined contour;
    std::vector< CComplex > contour;

    // member functions
    int InTriangle(double x, double y);
    bool InTriangleTest(double x, double y, int i);
    int ClosestNode(double x, double y);
    CComplex Ctr(int i);
    double ElmArea(int i);
    double ElmArea(femmsolver::CElement *elm);

    int ClosestArcSegment(double x, double y);
    void GetCircle(femm::CArcSegment &asegm,CComplex &c, double &R);
    double ShortestDistanceFromArc(CComplex p, femm::CArcSegment &arc);
    double ShortestDistanceFromSegment(double p, double q, int segm);
    void BendContour(double angle, double anglestep);
    CComplex HenrotteVector(int k);
    void FindBoundaryEdges();

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);
    //	void MsgBox(const char* message);
};
#endif
