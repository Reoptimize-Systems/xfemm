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

   Additional changes:
   Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
   Contributions by Johannes Zarl-Zierl were funded by Linz Center of
   Mechatronics GmbH (LCM)
*/


// implementation of various incarnations of calls
// to triangle from the FMesher class
#include "fmesher.h"
#include "fparse.h"
#include "IntPoint.h"
#include "femmconstants.h"
#include "CCommonPoint.h"
//extern "C" {
#include "triangle.h"
#ifndef XFEMM_BUILTIN_TRIANGLE
#include "triangle_api.h"
#endif
//}


#include <cassert>
#include <cmath>
#include <cstdio>
#include <malloc.h>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef REAL
#define REAL double
#endif

#ifndef LineFraction
#define LineFraction 500.0
#endif

// Default mesh size is the diagonal of the geometry's
// bounding box divided by BoundingBoxFraction
#ifndef BoundingBoxFraction
#define BoundingBoxFraction 100.0
#endif


using namespace std;
using namespace femm;
using namespace fmesher;

namespace {

enum class PointMarkerInfo {
    None ///< Use zero / Don't store information in marker list
    , FromProblem ///< Generate marker info using the problem descripton
};
enum class SegmentMarkerInfo {
    FromCnt ///< Generate marker infor from cnt field of segments.
    , FromProblem ///< Generate marker info using the problem descripton
};

/**
 * @brief The TriangulateHelper class encapsulates the interface to triangle,
 * so that the rest of the code doesn't have to deal with changes in its api.
 *
 * All memory that is allocated by its member functions is freed in the destructor.
 * Don't call initialization functions more than once.
 */
class TriangulateHelper {
    using nodelist_t = std::vector<std::unique_ptr<CNode> >;
    using linelist_t = std::vector<std::unique_ptr<CSegment> >;
public:
    TriangulateHelper();
    ~TriangulateHelper();

    /**
     * @brief Build a point list and point marker list as input for triangle.
     * The point marker list is later used to make the connection of meshed nodes back to original nodes.
     * @param nodelst
     * @param problem
     * @return \c true on success, \c false on (allocation) error
     */
    bool initPointsWithMarkers(const nodelist_t &nodelst, const FemmProblem &problem, PointMarkerInfo info);
    /**
     * @brief Build a segment list and segment marker list as input for triangle.
     * The segment marker list is later used to make the connection of meshed arcs/lines back to original arcs/lines.
     * @param linelst
     * @param problem
     * @return \c true on success, \c false on (allocation) error
     */
    bool initSegmentsWithMarkers(const linelist_t &linelst, const FemmProblem &problem, SegmentMarkerInfo info);

    /**
     * @brief Build a list of holes and regions as input for triangle.
     * This translates the CBlockLabel info for triangle.
     * @param problem
     * @param forceMaxMeshArea if \c true, this enforces an upper bound (defaultMeshSize) for the size of regional attributes
     * @param defaultMeshSize size of regional attributes that are not valid (i.e. <=0 or over the upper bound (if enforced))
     * @return \c true on success, \c false on (allocation) error
     */
    bool initHolesAndRegions(const FemmProblem &problem, bool forceMaxMeshArea, double defaultMeshSize);

    /**
     * @brief triangulate
     * The values of minAngle and suppressExteriourSteinerPoints are applied.
     * @param verbose
     * @return
     */
    int triangulate(bool verbose);

    bool writeTriangulationFiles(std::string Pathname) const;

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);

    // pointer to function to use for triangle to issue warning messages
    int (*TriMessage)(const char * format, ...);

    void setMinAngle(double value);
    /**
     * @brief Suppress insertion of Steiner points on the mesh boundary.
     */
    void suppressExteriorSteinerPoints();
    /**
     * @brief Purge vertices that are not part of the final triangulation from the triangulation output.
     * Should be safe to add all the time, but I didn't test with periodic bc triangulation.
     * Therefore only used with nonperiodic triangulation.
     */
    void suppressUnusedVertices();

private:
#ifdef XFEMM_BUILTIN_TRIANGLE
    struct triangulateio in;
    struct triangulateio out;
#else
    triangleio in;
    context *ctx;
#endif
    double m_minAngle;
    bool m_suppressExteriorSteinerPoints; ///< Default: false
    bool m_suppressUnusedVertices; ///< Default: false
};

/**
 * @brief Initialize a triangulateio to all zero.
 * @param io
 */
#ifdef XFEMM_BUILTIN_TRIANGLE
void initialize(struct triangulateio &io)
#else
void initialize(triangleio &io)
#endif
{
    io.pointlist = nullptr;
    io.pointattributelist = nullptr;
    io.pointmarkerlist = nullptr;
    io.numberofpoints = 0;
    io.numberofpointattributes = 0;

    io.trianglelist = nullptr;
    io.triangleattributelist = nullptr;
    io.trianglearealist = nullptr;
    io.neighborlist = nullptr;
    io.numberoftriangles = 0;
    io.numberofcorners = 0;
    io.numberoftriangleattributes = 0;

    io.segmentlist = nullptr;
    io.segmentmarkerlist = nullptr;
    io.numberofsegments = 0;

    io.holelist = nullptr;
    io.numberofholes = 0;

    io.regionlist = nullptr;
    io.numberofregions = 0;

    io.edgelist = nullptr;
    io.edgemarkerlist = nullptr;
#ifdef XFEMM_BUILTIN_TRIANGLE
    io.normlist = nullptr; // only used by voronoi diagram
#endif
    io.numberofedges = 0;
}

}

double FMesher::averageLineLength() const
{
    double z=0;
    const double numLines = problem->linelist.size();
    for (const auto &line : problem->linelist)
    {
        z += problem->lengthOfLine(*line) / numLines;
    }
    return z;
}

double fmesher::defaultMeshSizeHeuristics(const std::vector<std::unique_ptr<CNode> > &nodelst, bool doSmartMesh)
{
    if (nodelst.empty())
        return -1;

    // compute minimum and maximum x/y values
    CComplex min=nodelst[0]->CC();
    CComplex max=min;
    for(const auto &node: nodelst)
    {
        if (node->x < min.re) min.re = node->x;
        if (node->y < min.im) min.im = node->y;
        if (node->x > max.re) max.re = node->x;
        if (node->y > max.im) max.im = node->y;
    }

    if (doSmartMesh)
    {
        double absdist = abs(max-min)/BoundingBoxFraction;
        return absdist * absdist;
    } else {
        return abs(max-min);
    }
}

void fmesher::discretizeInputSegments(const FemmProblem &problem, std::vector<std::unique_ptr<CNode> > &nodelst, std::vector<std::unique_ptr<CSegment> > &linelst, bool doSmartMesh, double dL, SegmentFilter filter)
{
    for(int i=0; i<(int)problem.linelist.size(); i++)
    {
        const CSegment &line = *problem.linelist[i];

        if (filter == SegmentFilter::OnlyUnselected && line.IsSelected )
            continue;

        const CNode &n0 = *problem.nodelist[line.n0];
        const CNode &n1 = *problem.nodelist[line.n1];
        const CComplex a0 = n0.CC();
        const CComplex a1 = n1.CC();
        // create working copy:
        CSegment segm = line;
        // use the cnt flag to carry a notation
        // of which line or arc in the input geometry a
        // particular segment is associated with
        // (this info is only used in the periodic BC triangulation, and is ignored in the nonperiodic one)
        segm.cnt = i;

        double lineLength = problem.lengthOfLine(line);

        int numParts;
        if (line.MaxSideLength == -1) {
            numParts = 1;
        }
        else{
            numParts = (unsigned int) std::ceil(lineLength/line.MaxSideLength);
        }

        if (numParts == 1) // default condition where discretization on line is not specified
        {
            if (lineLength < (3. * dL) || doSmartMesh == false)
            {
                // line is too short to add extra points
                linelst.push_back(segm.clone());
            }
            else{
                // add extra points at a distance of dL from the ends of the line.
                // this forces Triangle to finely mesh near corners
                int l = (int) nodelst.size();

                // first part
                CComplex a2 = a0 + dL * (a1-a0) / abs(a1-a0);
                CNode node1 (a2.re, a2.im);
                nodelst.push_back(node1.clone());
                segm.n0 = line.n0;
                segm.n1 = l;
                linelst.push_back(segm.clone());

                // middle part
                a2 = a1 + dL * (a0-a1) / abs(a1-a0);
                CNode node2 (a2.re, a2.im);
                nodelst.push_back(node2.clone());
                segm.n0 = l - 1;
                segm.n1 = l;
                linelst.push_back(segm.clone());

                // end part
                segm.n0 = l - 1;
                segm.n1 = line.n1;
                linelst.push_back(segm.clone());
            }
        }
        else{
            for(int j=0; j<numParts; j++)
            {
                CComplex a2 = a0 + (a1-a0)*((double) (j+1)) / ((double) numParts);
                CNode node (a2.re, a2.im);
                if(j == 0){
                    // first part -> n0 == line.n0
                    int l=nodelst.size();
                    nodelst.push_back(node.clone());
                    segm.n0=line.n0;
                    segm.n1=l;
                    linelst.push_back(segm.clone());
                }
                else if(j == (numParts-1))
                {
                    // last part -> n1 == line.n1 ; endpoint already exists
                    int l=nodelst.size()-1;
                    segm.n0=l;
                    segm.n1=line.n1;
                    linelst.push_back(segm.clone());
                }
                else{
                    int l=nodelst.size();
                    nodelst.push_back(node.clone());
                    segm.n0=l-1;
                    segm.n1=l;
                    linelst.push_back(segm.clone());
                }
            }
        }
    }
}

void fmesher::discretizeInputArcSegments(const FemmProblem &problem, std::vector<std::unique_ptr<CNode> > &nodelst, std::vector<std::unique_ptr<CSegment> > &linelst, SegmentFilter filter)
{
    for(int i=0;i<(int)problem.arclist.size();i++)
    {
        const CArcSegment &arc = *problem.arclist[i];

        if (filter == SegmentFilter::OnlyUnselected && arc.IsSelected )
            continue;

        // smart meshing does not apply to arc segments
        assert(arc.MaxSideLength != -1);

        // create working copy:
        CSegment segm = arc;
        // use the cnt flag to carry a notation
        // of which line or arc in the input geometry a
        // particular segment is associated with
        // (this info is only used in the periodic BC triangulation, and is ignored in the nonperiodic one)
        segm.cnt=i+problem.linelist.size();

        segm.BoundaryMarkerName=arc.BoundaryMarkerName;
        if (problem.filetype != FileType::MagneticsFile)
            segm.InConductorName=arc.InConductorName; // not relevant/compatible to magnetics problems

        int numParts=(int) ceil(arc.ArcLength/arc.MaxSideLength);

        CComplex center;
        double R=0;
        problem.getCircle(arc,center,R);

        CComplex a1=exp(I*arc.ArcLength*PI/(((double) numParts)*180.));
        CComplex a2=problem.nodelist[arc.n0]->CC();

        if(numParts==1){
            linelst.push_back(segm.clone());
        }
        else for(int j=0;j<numParts;j++)
        {
            // move point along arc
            a2=(a2-center)*a1+center;
            CNode node(a2.re,a2.im);
            int l = (int)nodelst.size();
            if(j==0){
                // first part -> n0 == arc.n0
                nodelst.push_back(node.clone());
                segm.n0=arc.n0;
                segm.n1=l;
            }
            else if(j==(numParts-1))
            {
                // last part -> n1 == arc.n1 ; endpoint already exists
                segm.n0=l-1;
                segm.n1=arc.n1;
            }
            else{
                nodelst.push_back(node.clone());
                segm.n0=l-1;
                segm.n1=l;
            }
            linelst.push_back(segm.clone());
        }
    }
}

/**
 * @brief FMesher::HasPeriodicBC
 * @return \c true, if there are periodic or antiperiodic boundary conditions, \c false otherwise.
 *
 * \internal
 *  * \femm42{femm/writepoly.cpp}
 *  * \femm42{femm/bd_writepoly.cpp}
 *  * \femm42{femm/hd_writepoly.cpp}
 */
bool FMesher::HasPeriodicBC()
{
    bool hasPeriodicBC = false;
    for (const auto &bdryProp: problem->lineproplist)
    {
        if (bdryProp->isPeriodic())
            hasPeriodicBC = true;
    }
    // if flag is false, there can't be any lines
    // with periodic BC's, because no periodic boundary
    // conditions have been defined.
    if (hasPeriodicBC==false) return false;

    //now, if there are some periodic boundary conditions,
    //we have to check to see if any have actually been
    //applied to the model

    // first, test the segments
    for(int i=0;i<(int)problem->linelist.size();i++)
    {
        int k=-1;
        for(int j=0;j<(int)problem->lineproplist.size();j++)
        {
            if(problem->lineproplist[j]->BdryName==
               problem->linelist[i]->BoundaryMarkerName)
            {
                k=j;
                break;
            }
        }
        if(k>=0){
            if (problem->lineproplist[k]->isPeriodic())
            {
                return true;
            }
        }
    }

    // If we've gotten this far, we still need to check the
    // arc segments.
    for(int i=0;i<(int)problem->arclist.size();i++)
    {
        int k=-1;
        for(int j=0;j<(int)problem->lineproplist.size();j++)
        {
            if(problem->lineproplist[j]->BdryName==
               problem->arclist[i]->BoundaryMarkerName)
            {
                k=j;
                break;
            }
        }
        if(k>=0){
            if (problem->lineproplist[k]->isPeriodic())
            {
                return true;
            }
        }
    }

    // Finally, we're done.
    // we could not find periodic and/or antiperiodic boundaries.
    return false;
}


bool TriangulateHelper::writeTriangulationFiles(string PathName) const
{
    FILE *fp;
    std::string msg;

    // write the .edge file
    string plyname = PathName.substr(0, PathName.find_last_of('.')) + ".edge";

    // check to see if we are ready to write an edge datafile;

    if ((fp = fopen(plyname.c_str(),"wt"))==NULL){
        msg = "Couldn't write to specified .edge file\n";
        WarnMessage(msg.c_str());
        return false;
    }
#ifdef XFEMM_BUILTIN_TRIANGLE

    if (out.numberofedges > 0)
    {
        // write number of edges, number of boundary markers, 0 or 1
        fprintf(fp, "%i\t%i\n", out.numberofedges, 1);

        // write the edges in the format
        // <edge #> <endpoint> <endpoint> [boundary marker]
        // Endpoints are indices into the corresponding .edge file.
        for(int i=0; i < 2 * (out.numberofedges) - 1; i = i + 2)
        {
            fprintf(fp, "%i\t%i\t%i\t%i\n", i/2, out.edgelist[i], out.edgelist[i+1], out.edgemarkerlist[i/2]);
        }

        fclose(fp);
    } else {
        WarnMessage("No edges to write!\n");
    }
#else
    int status = triangle_write_edges(ctx, fp);
    fclose(fp);
    if (status != TRI_OK)
    {
        msg = "Failed to write to specified .edge file\n";
        WarnMessage(msg.c_str());
        return false;
    }
#endif

    // write the .ele file
    plyname = PathName.substr(0, PathName.find_last_of('.')) + ".ele";


    // check to see if we are ready to write a .ele datafile containing
    // thr triangle elements

    if ((fp = fopen(plyname.c_str(),"wt"))==NULL){
        WarnMessage("Couldn't write to specified .ele file");
        return false;
    }

#ifdef XFEMM_BUILTIN_TRIANGLE
    if (out.numberoftriangles > 0)
    {
        // write number of triangle elements, number of corners per triangle and
        // the number of attributes per triangle
        fprintf(fp, "%i\t%i\t%i\n", out.numberoftriangles, out.numberofcorners, out.numberoftriangleattributes);

        // write the triangle info to the file with the format
        // <triangle #> <node> <node> <node> ... [attributes]
        // Endpoints are indices into the corresponding .node file.
        for(int i=0, nexttriattrib=0; i < (out.numberofcorners) * (out.numberoftriangles) - (out.numberofcorners - 1); i = i + (out.numberofcorners))
        {
            // print the triangle number
            fprintf(fp, "%i\t", i / (out.numberofcorners));

            // print the corner nodes
            for (int j = 0; j < (out.numberofcorners); j++)
            {
                fprintf(fp, "%i\t", out.trianglelist[i+j]);
            }

            // print the triangle attributes, if there are any
            if (out.numberoftriangleattributes > 0)
            {
                for(int j = 0; j < (out.numberoftriangleattributes); j++)
                {
                    fprintf(fp, "%.17g\t", out.triangleattributelist[nexttriattrib+j]);
                }

                // set the position of the next set of triangle attributes
                nexttriattrib = nexttriattrib + (out.numberoftriangleattributes);
            }

            // go to the next line
            fprintf(fp, "\n");
        }

        fclose(fp);

    }
#else
    status = triangle_write_elements(ctx, fp);
    fclose(fp);
    if (status != TRI_OK)
    {
        msg = "Failed to write to specified .ele file\n";
        WarnMessage(msg.c_str());
        return false;
    }
#endif

    // write the .node file
    plyname = PathName.substr(0, PathName.find_last_of('.')) + ".node";

    // check to see if we are ready to write a .node datafile containing
    // the nodes

    if ((fp = fopen(plyname.c_str(),"wt"))==NULL){
        WarnMessage("Couldn't write to specified .node file");
        return false;
    }

#ifdef XFEMM_BUILTIN_TRIANGLE
    if (out.numberofpoints > 0)
    {
        // <# of vertices> <dimension (must be 2)> <# of attributes> <# of boundary markers (0 or 1)>
        fprintf(fp, "%i\t%i\t%i\t%i\n", out.numberofpoints, 2, 0, 1);
        //fprintf(fp, "%i\t%i\t%i\n", out.numberofpoints, 2, out.numberofpoints, 1);

        // <vertex #> <x> <y> [attributes] [boundary marker]
        for(int i = 0; i < (2 * out.numberofpoints) - 1; i = i + 2)
        {
            fprintf(fp, "%i\t%.17g\t%.17g\t%i\n", i/2, out.pointlist[i], out.pointlist[i+1], out.pointmarkerlist[i/2]);
        }

        fclose(fp);
    }
#else
    status = triangle_write_nodes(ctx, fp);
    fclose(fp);
    if (status != TRI_OK)
    {
        msg = "Failed to write to specified .node file\n";
        WarnMessage(msg.c_str());
        return false;
    }
#endif

    return true;

}

/**
 * @brief FMesher::DoNonPeriodicBCTriangulation
 * What we do in the normal case is DoNonPeriodicBCTriangulation
 * @param PathName
 * @return
 *
 * Original function name:
 *  * \femm42{femm/writepoly.cpp,CFemmeDoc::OnWritePoly()}
 *  * \femm42{femm/bd_writepoly.cpp,CbeladrawDoc::OnWritePoly()}
 *  * \femm42{femm/hd_writepoly.cpp,ChdrawDoc::OnWritePoly()}
 */
int FMesher::DoNonPeriodicBCTriangulation(string PathName)
{
    // // if incremental permeability solution, we crib mesh from the previous problem.
    // // we can just bail out in that case.
    // if (!problem->PrevSoln.empty() && problem->Frequency>0)
    //     return true;

    FILE *fp;
    double dL;
    //CStdString s;
    string plyname;
    std::vector < std::unique_ptr<CNode> >       nodelst;
    std::vector < std::unique_ptr<CSegment> >    linelst;

    nodelst.clear();
    linelst.clear();
    // calculate length used to kludge fine meshing near input node points
    dL = averageLineLength() / LineFraction;

    // copy node list as it is;
    for (const auto &node : problem->nodelist)
        nodelst.push_back(node->clone());

    // discretize input segments
    discretizeInputSegments(*problem, nodelst, linelst, DoSmartMesh, dL);

    // discretize input arc segments
    discretizeInputArcSegments(*problem, nodelst, linelst);

    // create correct output filename;
    string pn = PathName;

    // figure out a good default mesh size for block labels where
    // mesh size isn't explicitly specified
    double DefaultMeshSize = defaultMeshSizeHeuristics(nodelst, DoSmartMesh);

//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i]->BlockTypeName=="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g\n",k,blocklist[i]->x,blocklist[i]->y);
//            k++;
//        }
//
//    // write out regional attributes
//    fprintf(fp,"%i\n",blocklist.size()-j);
//
//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i]->BlockTypeName!="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g    ",k,blocklist[i]->x,blocklist[i]->y);
//            fprintf(fp,"%i    ",k+1);
//            if (blocklist[i]->MaxArea>0)
//                fprintf(fp,"%.17g\n",blocklist[i]->MaxArea);
//            else fprintf(fp,"-1\n");
//            k++;
//        }
//    fclose(fp);

    // write out a trivial pbc file
    plyname = pn.substr(0,pn.find_last_of('.')) + ".pbc";
    if ((fp=fopen(plyname.c_str(),"wt"))==NULL){
        WarnMessage("Couldn't write to specified .pbc file");
        return -1;
    }
    fprintf(fp,"0\n");
    fclose(fp);

    // **********         call triangle       ***********

    {
        TriangulateHelper triHelper;
        triHelper.WarnMessage = WarnMessage;
        triHelper.TriMessage = this->TriMessage;
        if (!triHelper.initPointsWithMarkers(nodelst,*problem, PointMarkerInfo::FromProblem))
            return -1;
        if (!triHelper.initSegmentsWithMarkers(linelst,*problem,SegmentMarkerInfo::FromProblem))
            return -1;
        if (!triHelper.initHolesAndRegions(*problem, problem->DoForceMaxMeshArea, DefaultMeshSize))
            return -1;
        triHelper.setMinAngle(problem->MinAngle);
        triHelper.suppressUnusedVertices();
        int tristatus = triHelper.triangulate(Verbose);
        if (tristatus != 0)
            return tristatus;

        triHelper.writeTriangulationFiles(PathName);
    }

    return 0;
}


/**
 * \brief Call triangle twice to order segments on the boundary properly
 * for periodic or antiperiodic boundary conditions
 *
 * Original function name:
 *  * \femm42{femm/writepoly.cpp,CFemmeDoc::FunnyOnWritePoly()}
 *  * \femm42{femm/bd_writepoly.cpp,CbeladrawDoc::FunnyOnWritePoly()}
 *  * \femm42{femm/hd_writepoly.cpp,ChdrawDoc::FunnyOnWritePoly()}
 */
int FMesher::DoPeriodicBCTriangulation(string PathName)
{
    // // if incremental permeability solution, we crib mesh from the previous problem.
    // // we can just bail out in that case.
    // if (!problem->PrevSoln.empty() && problem->Frequency>0)
    //     return true;
    FILE *fp;
    int i, j, k, n;
    int l,t,n0,n1,n2;
    double z,dL;
    CComplex a0,a1,a2;
    CComplex b0,b1,b2;
    char instring[1024];
    //string s;
    string plyname;
    std::vector < std::unique_ptr<CNode> >             nodelst;
    std::vector < std::unique_ptr<CSegment> >          linelst;
    std::vector < std::unique_ptr<CPeriodicBoundary> > pbclst;
    std::vector < std::unique_ptr<CCommonPoint> >      ptlst;
    CSegment segm;
    CPeriodicBoundary pbc;
    CCommonPoint pt;

    nodelst.clear();
    linelst.clear();
    pbclst.clear();
    ptlst.clear();

    problem->updateUndo();

    // calculate length used to kludge fine meshing near input node points
    dL = averageLineLength() / LineFraction;

    // copy node list as it is;
    for (const auto &node : problem->nodelist)
        nodelst.push_back(node->clone());

    // discretize input segments
    discretizeInputSegments(*problem, nodelst, linelst, DoSmartMesh, dL);

    // discretize input arc segments
    discretizeInputArcSegments(*problem, nodelst, linelst);


    // create correct output filename;
    string pn = PathName;
//    CStdString plyname=pn.Left(pn.ReverseFind('.')) + ".poly";
//
//    // check to see if we are ready to write a datafile;
//
//    if ((fp=fopen(plyname,"wt"))==NULL){
//        WarnMessage("Couldn't write to specified .poly file");
//        Undo();  UnselectAll();
//        return false;
//    }
//
//    // write out node list
//    fprintf(fp,"%i    2    0    1\n",nodelst.size());
//    for(i=0;i<nodelst.size();i++)
//    {
//        fprintf(fp,"%i    %.17g    %.17g    %i\n",
//                 i,nodelst[i]->x,nodelst[i]->y,0);
//    }
//
//    // write out segment list
//    fprintf(fp,"%i    1\n",linelst.size());
//    for(i=0;i<linelst.size();i++)
//    {
//        t=-(linelst[i]->cnt+2);
//        fprintf(fp,"%i    %i    %i    %i\n",i,linelst[i]->n0,linelst[i]->n1,t);
//    }

    // figure out a good default mesh size for block labels where
    // mesh size isn't explicitly specified
    double DefaultMeshSize = defaultMeshSizeHeuristics(nodelst, DoSmartMesh);

//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i]->BlockTypeName=="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g\n",k,blocklist[i]->x,blocklist[i]->y);
//            k++;
//        }

//    // write out regional attributes
//    fprintf(fp,"%i\n",blocklist.size()-j);
//
//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i]->BlockTypeName!="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g    ",k,blocklist[i]->x,blocklist[i]->y);
//            fprintf(fp,"%i    ",k+1);
//            if (blocklist[i]->MaxArea>0)
//                fprintf(fp,"%.17g\n",blocklist[i]->MaxArea);
//            else fprintf(fp,"-1\n");
//            k++;
//        }
//
//    fclose(fp);


    // **********         call triangle       ***********

    {
        TriangulateHelper triHelper;
        triHelper.WarnMessage = WarnMessage;
        triHelper.TriMessage = this->TriMessage;

        if (!triHelper.initPointsWithMarkers(nodelst,*problem, PointMarkerInfo::None))
            return -1;
        if (!triHelper.initSegmentsWithMarkers(linelst,*problem,SegmentMarkerInfo::FromCnt))
            return -1;
        if (!triHelper.initHolesAndRegions(*problem, true, DefaultMeshSize))
            return -1;

        triHelper.setMinAngle(problem->MinAngle);
        int tristatus = triHelper.triangulate(Verbose);
        if (tristatus != 0)
            return tristatus;

        triHelper.writeTriangulationFiles(PathName);
    }

    // So far, so good.  Now, read back in the .edge file
    // to make sure the points in the segments and arc
    // segments are ordered in a consistent way so that
    // the (anti)periodic boundary conditions can be applied.


    // read meshlines;
    plyname = pn.substr(0,pn.find_last_of('.')) + ".edge";
    if((fp=fopen(plyname.c_str(),"rt"))==NULL){
        WarnMessage("Call to triangle was unsuccessful\n");
        problem->undo();  problem->unselectAll();
        return -1;
    }
    fgets(instring,1024,fp);
    sscanf(instring,"%i",&k);
    // use cnt again to keep a
    // tally of how many subsegments each
    // entity is sliced into.
    for(i=0; i < (int)problem->arclist.size(); i++) problem->arclist[i]->cnt=0;

    ptlst.resize(problem->linelist.size()+problem->arclist.size());

    for(i=0;i<(int)ptlst.size();i++)
    {
        ptlst[i] = std::make_unique<CCommonPoint>();
    }

    for(i=0;i<k;i++)
    {
        // get the next edge from the file
        fgets(instring,1024,fp);
        // get the edge number, start and end points (n0 and n1) and the
        // segment/arc marker j
        sscanf(instring,"%i    %i    %i    %i",&l,&n0,&n1,&j);
        // if j != 0, this edge is part of a segment/arc
        if(j!=0)
        {
            // convert back to the `right' numbering
            j=-(j+2);

            // store a reference line that we can use to
            // determine whether or not this is a
            // boundary segment w/out re-running triangle.
            if (ptlst[j]->t==0)
            {
                ptlst[j]->t=1;
                if(n0<n1){
                    ptlst[j]->x=n0;
                    ptlst[j]->y=n1;
                }
                else{
                    ptlst[j]->x=n1;
                    ptlst[j]->y=n0;
                }
            }

            if(j<(int)problem->linelist.size())
            {
                // deal with segments

                // increment cnt for the segment which the edge we are
                // examining is a part of to get a tally of how many edges
                // are a part of the segment/boundary
                problem->linelist[j]->cnt++;
                // check if the end n0 of the segment is the same node as the
                // end n1 of the edge, or if the end n1 of the segment is the
                // same node and end n0 of the edge. If so, flip the direction
                // of the segment
                if((problem->linelist[j]->n0 == n1) || (problem->linelist[j]->n1 == n0))
                {
                    // flip the end points of the segment
                    t = problem->linelist[j]->n0;
                    problem->linelist[j]->n0 = problem->linelist[j]->n1;
                    problem->linelist[j]->n1 = t;
                }
            }
            else{
                // deal with arc segments;
                // Can't just flip the point order with
                // impunity in the arc segments, so we flip
                // a marker which denotes which side the
                // normal is on.

                j=j-problem->linelist.size();
                problem->arclist[j]->cnt++;
                if((problem->arclist[j]->n0==n1) || (problem->arclist[j]->n1==n0))
                    problem->arclist[j]->NormalDirection=false;
                if((problem->arclist[j]->n0==n0) || (problem->arclist[j]->n1==n1))
                    problem->arclist[j]->NormalDirection=true;
            }
        }
    }
    fclose(fp);

    // figure out which segments / arcsegments are on the
    // boundary and force an appropriate mesh density on
    // these based on how many divisions are in the first
    // trial meshing of the domain.

    // paw through the element list to find out how many
    // elements each reference segment appears in.  If a
    // segment is on the boundary, it ought to appear in just
    // one element.  Otherwise, it appears in two.
    plyname = pn.substr(0,pn.find_last_of('.')) + ".ele";
    if((fp=fopen(plyname.c_str(),"rt"))==NULL){
        WarnMessage("Call to triangle was unsuccessful");
        problem->undo();  problem->unselectAll();
        return -1;
    }
    fgets(instring,1024,fp);
    sscanf(instring,"%i",&k);

    for(i=0;i<k;i++)
    {
        fgets(instring,1024,fp);
        sscanf(instring,"%i    %i    %i    %i",&j,&n0,&n1,&n2);

        // Sort out the three nodes...
        if (n0>n1) { n=n0; n0=n1; n1=n; }
        if (n1>n2) { n=n1; n1=n2; n2=n; }
        if (n0>n1) { n=n0; n0=n1; n1=n; }

        // now, check to see if any of the test segments
        // are sides of this node...
        for(j=0;j<(int)ptlst.size();j++)
        {
            if ((n0==ptlst[j]->x) && (n1==ptlst[j]->y)) ptlst[j]->t--;
            if ((n0==ptlst[j]->x) && (n2==ptlst[j]->y)) ptlst[j]->t--;
            if ((n1==ptlst[j]->x) && (n2==ptlst[j]->y)) ptlst[j]->t--;
        }
    }
    fclose(fp);

    // impose "new" mesh constraints on bdry arcs and segments....
    for(i=0; i < (int)problem->linelist.size(); i++)
    {
        if (ptlst[i]->t == 0)
        {
            // simply make the max side length equal to the
            // length of the boundary divided by the number
            // of elements that were created in the first
            // attempt at meshing
            problem->linelist[i]->MaxSideLength = problem->lengthOfLine(i) / ((double) problem->linelist[i]->cnt);
        }
    }

    for(i=0; i < (int)problem->arclist.size(); i++)
    {
        if (ptlst[i+problem->linelist.size()]->t == 0)
        {
            // alter maxsidelength, but do it in such
            // a way that it carries only 4 significant
            // digits.  There's no use in carrying double
            // precision here, because it looks crappy
            // when you open up the arc segment to see
            // its properties.
            char kludge[32];

            problem->arclist[i]->MaxSideLength = problem->arclist[i]->ArcLength/((double) problem->arclist[i]->cnt);

            sprintf(kludge,"%.1e",problem->arclist[i]->MaxSideLength);

            sscanf(kludge,"%lf",&problem->arclist[i]->MaxSideLength);
        }
    }

    ptlst.clear();

        // want to impose explicit discretization only on
        // the boundary arcs and segments.  After the meshing
        // is done, spacing on boundary segments should be
        // restored to the value that was there before meshing
        // was called, but the arc segments should keep the
        // "new" MaxSideLength--this is used in other places
        // and must always be consistent with the the mesh.


    // Now, do a shitload of checking to make sure that
    // the PBCs haven't been defined by the user
    // in a messed up way.

    // First, search through defined bc's for periodic ones;
    for(i=0;i<(int)problem->lineproplist.size();i++)
    {
        if (problem->lineproplist[i]->isPeriodic())
        {
            pbc.BdryName=problem->lineproplist[i]->BdryName;
            pbc.antiPeriodic = problem->lineproplist[i]->isPeriodic(CBoundaryProp::PeriodicityType::AntiPeriodic);
            pbclst.push_back(std::make_unique<CPeriodicBoundary>(pbc));
        }
    }

    for(i=0;i<(int)problem->linelist.size();i++)
    {
        for(j=0;j<(int)pbclst.size();j++)
        {
            if (pbclst[j]->BdryName==problem->linelist[i]->BoundaryMarkerName)
            {
                // A pbc or apbc can only be applied to 2 segs
                // at a time.  If it is applied to multiple segs
                // at the same time, flag it and kick it out.
                if (pbclst[j]->nseg==2)
                {
                    WarnMessage("An (anti)periodic BC is assigned to more than two segments");
                    problem->undo();  problem->unselectAll();
                    return -1;
                }
                pbclst[j]->seg[pbclst[j]->nseg]=i;
                pbclst[j]->nseg++;
            }
        }
    }

    for(i=0;i<(int)problem->arclist.size();i++)
    {
        for(j=0;j<(int)pbclst.size();j++)
        {
            if (pbclst[j]->BdryName==problem->arclist[i]->BoundaryMarkerName)
            {
                // A pbc or apbc can only be applied to 2 arcs
                // at a time.  If it is applied to multiple arcs
                // at the same time, flag it and kick it out.
                if (pbclst[j]->narc==2)
                {
                    WarnMessage("An (anti)periodic BC is assigned to more than two arcs");
                    problem->undo();  problem->unselectAll();
                    return -1;
                }
                pbclst[j]->seg[pbclst[j]->narc]=i;
                pbclst[j]->narc++;
            }
        }
    }

    j=0;
    while(j<(int)pbclst.size())
    {
        // check for a bc that is a mix of arcs and segments.
        // this is an error, and it should get flagged.
        if ((pbclst[j]->nseg>0) && (pbclst[j]->narc>0))
        {
            WarnMessage("Can't mix arcs and segments for (anti)periodic BCs");
            problem->undo();  problem->unselectAll();
            return -1;
        }


        // remove any periodic BC's that aren't actually in play
        if((pbclst[j]->nseg<2) && (pbclst[j]->narc<2)) pbclst.erase(pbclst.begin()+j);
        else j++;
    }

    for(j=0;j<(int)pbclst.size();j++)
    {
        // check to see if adjoining entries are applied
        // to objects of compatible size/shape, and
        // reconcile meshing on the objects.

        // for segments:
        if(pbclst[j]->nseg>0){

            // make sure that lines are pretty much the same length
            if(fabs(problem->lengthOfLine(pbclst[j]->seg[0])
                   -problem->lengthOfLine(pbclst[j]->seg[1]))>1.e-06)
            {
                WarnMessage("(anti)periodic BCs applied to dissimilar segments");
                problem->undo();  problem->unselectAll();
                return -1;
            }

            // make sure that both lines have the same spacing
            double len1,len2,len;
            len1=problem->linelist[pbclst[j]->seg[0]]->MaxSideLength;
            len2=problem->linelist[pbclst[j]->seg[1]]->MaxSideLength;

            if(len1<=0) len1=len2;
            if(len2<=0) len2=len1;
            len=(std::min)(len1,len2);

            problem->linelist[pbclst[j]->seg[0]]->MaxSideLength=len;
            problem->linelist[pbclst[j]->seg[1]]->MaxSideLength=len;
        }

        // for arc segments:
        if(pbclst[j]->narc>0){

            // make sure that arcs are pretty much the
            // same arc length
            if(fabs(problem->arclist[pbclst[j]->seg[0]]->ArcLength
                   -problem->arclist[pbclst[j]->seg[1]]->ArcLength)>1.e-06)
            {
                WarnMessage("(anti)periodic BCs applied to dissimilar arc segments");
                problem->undo();  problem->unselectAll();
                return -1;
            }

            // make sure that both lines have the same spacing
            double len1,len2,len;
            len1=problem->arclist[pbclst[j]->seg[0]]->MaxSideLength;
            len2=problem->arclist[pbclst[j]->seg[1]]->MaxSideLength;

            len=(std::min)(len1,len2);

            problem->arclist[pbclst[j]->seg[0]]->MaxSideLength=len;
            problem->arclist[pbclst[j]->seg[1]]->MaxSideLength=len;
        }
    }

    // write out new poly and write out adjacent
    // boundary nodes in a separate .pbc file.

    // kludge things a bit and use IsSelected to denote
    // whether or not a line or arc has already been processed.
    problem->unselectAll();
    nodelst.clear();
    linelst.clear();

    // first, add in existing nodes
    for(const auto &node: problem->nodelist)
        nodelst.push_back(node->clone());

    for(n=0; n<(int)pbclst.size(); n++)
    {
        if (pbclst[n]->nseg != 0) // if this pbc is a line segment...
        {
            int s0,s1;
            CNode node0,node1;

            s0=pbclst[n]->seg[0];
            s1=pbclst[n]->seg[1];
            problem->linelist[s0]->IsSelected=1;
            problem->linelist[s1]->IsSelected=1;

            // make it so that first point on first line
            // maps to first point on second line...
            t = problem->linelist[s1]->n1;
            problem->linelist[s1]->n1 = problem->linelist[s1]->n0;
            problem->linelist[s1]->n0 = t;

            // store number of sub-segments in k
            if (problem->linelist[s0]->MaxSideLength == -1)
            {
                k = 1;
            }
            else{
                a0 = problem->nodelist[problem->linelist[s0]->n0]->CC();
                a1 = problem->nodelist[problem->linelist[s0]->n1]->CC();
                b0 = problem->nodelist[problem->linelist[s1]->n0]->CC();
                b1 = problem->nodelist[problem->linelist[s1]->n1]->CC();
                z = abs(a1-a0);
                k = (int) std::ceil(z/problem->linelist[s0]->MaxSideLength);
            }

            // add segment end points to the list;
            pt.x = problem->linelist[s0]->n0;
            pt.y = problem->linelist[s1]->n0;
            pt.t = pbclst[n]->antiPeriodic;
            ptlst.push_back(std::make_unique<CCommonPoint>(pt));
            pt.x = problem->linelist[s0]->n1;
            pt.y = problem->linelist[s1]->n1;
            pt.t = pbclst[n]->antiPeriodic;
            ptlst.push_back(std::make_unique<CCommonPoint>(pt));

            if (k == 1){
                // catch the case in which the line
                // doesn't get subdivided.
                linelst.push_back(problem->linelist[s0]->clone());
                linelst.push_back(problem->linelist[s1]->clone());
            }
            else{
                segm = *problem->linelist[s0];
                for(j=0; j<k; j++)
                {
                    a2=a0+(a1-a0)*((double) (j+1))/((double) k);
                    b2 = b0+(b1-b0)*((double) (j+1))/((double) k);
                    node0.x = a2.re; node0.y = a2.im;
                    node1.x = b2.re; node1.y = b2.im;
                    if(j==0){
                        l = nodelst.size();
                        nodelst.push_back(node0.clone());
                        segm.n0 = problem->linelist[s0]->n0;
                        segm.n1 = l;
                        linelst.push_back(segm.clone());
                        pt.x = l;

                        l = nodelst.size();
                        nodelst.push_back(node1.clone());
                        segm.n0 = problem->linelist[s1]->n0;
                        segm.n1 = l;
                        linelst.push_back(segm.clone());
                        pt.y = l;

                        pt.t = pbclst[n]->antiPeriodic;
                        ptlst.push_back(std::make_unique<CCommonPoint>(pt));
                    }
                    else if(j==(k-1))
                    {
                        // last subdivision--no ptlst
                        // entry associated with this one.
                        l = nodelst.size()-2;
                        segm.n0 = l;
                        segm.n1 = problem->linelist[s0]->n1;
                        linelst.push_back(segm.clone());

                        l = nodelst.size()-1;
                        segm.n0 = l;
                        segm.n1 = problem->linelist[s1]->n1;
                        linelst.push_back(segm.clone());
                    }
                    else{
                        l = nodelst.size();

                        nodelst.push_back(node0.clone());
                        nodelst.push_back(node1.clone());

                        segm.n0 = l-2;
                        segm.n1 = l;
                        linelst.push_back(segm.clone());

                        segm.n0 = l-1;
                        segm.n1 = l+1;
                        linelst.push_back(segm.clone());

                        pt.x = l;
                        pt.y = l+1;
                        pt.t = pbclst[n]->antiPeriodic;
                        ptlst.push_back(std::make_unique<CCommonPoint>(pt));
                    }
                }
            }
        }
        else{  // if this pbc is an arc segment...

            int s0,s1;
            int p0[2],p1[2];
            CNode node0,node1;
            CComplex bgn0,bgn1,c0,c1,d0,d1;
            double r0,r1;

            s0 = pbclst[n]->seg[0];
            s1 = pbclst[n]->seg[1];
            problem->arclist[s0]->IsSelected = 1;
            problem->arclist[s1]->IsSelected = 1;

            k = (int) ceil(problem->arclist[s0]->ArcLength/problem->arclist[s0]->MaxSideLength);
            segm.BoundaryMarkerName = problem->arclist[s0]->BoundaryMarkerName;
            if (problem->filetype != FileType::MagneticsFile)
                segm.InConductorName=problem->arclist[i]->InConductorName; // not relevant for magnetics
            problem->getCircle(*problem->arclist[s0],c0,r0);
            problem->getCircle(*problem->arclist[s1],c1,r1);

            if (problem->arclist[s0]->NormalDirection ==0){
                bgn0 = problem->nodelist[problem->arclist[s0]->n0]->CC();
                d0 = exp(I*problem->arclist[s0]->ArcLength*PI/(((double) k)*180.));
                p0[0] = problem->arclist[s0]->n0;
                p0[1] = problem->arclist[s0]->n1;
            }
            else{
                bgn0 = problem->nodelist[problem->arclist[s0]->n1]->CC();
                d0 = exp(-I*problem->arclist[s0]->ArcLength*PI/(((double) k)*180.));
                p0[0] = problem->arclist[s0]->n1;
                p0[1] = problem->arclist[s0]->n0;
            }

            if (problem->arclist[s1]->NormalDirection!=0){
                bgn1 = problem->nodelist[problem->arclist[s1]->n0]->CC();
                d1 = exp(I*problem->arclist[s1]->ArcLength*PI/(((double) k)*180.));
                p1[0] = problem->arclist[s1]->n0;
                p1[1] = problem->arclist[s1]->n1;
            }
            else{
                bgn1 = problem->nodelist[problem->arclist[s1]->n1]->CC();
                d1 = exp(-I*problem->arclist[s1]->ArcLength*PI/(((double) k)*180.));
                p1[0] = problem->arclist[s1]->n1;
                p1[1] = problem->arclist[s1]->n0;
            }

            // add arc segment end points to the list;
            pt.x=p0[0]; pt.y=p1[0]; pt.t=pbclst[n]->antiPeriodic;
            ptlst.push_back(std::make_unique<CCommonPoint>(pt));
            pt.x=p0[1]; pt.y=p1[1]; pt.t=pbclst[n]->antiPeriodic;
            ptlst.push_back(std::make_unique<CCommonPoint>(pt));

            if (k==1){

                // catch the case in which the line
                // doesn't get subdivided.
                segm.n0=p0[0]; segm.n1=p0[1];
                linelst.push_back(segm.clone());
                segm.n0=p1[0]; segm.n1=p1[1];
                linelst.push_back(segm.clone());
            }
            else{
                for(j=0;j<k;j++)
                {
                    bgn0=(bgn0-c0)*d0+c0;
                    node0.x=bgn0.re; node0.y=bgn0.im;

                    bgn1=(bgn1-c1)*d1+c1;
                    node1.x=bgn1.re; node1.y=bgn1.im;

                    if(j==0){
                        l=nodelst.size();
                        nodelst.push_back(node0.clone());
                        segm.n0=p0[0];
                        segm.n1=l;
                        linelst.push_back(segm.clone());
                        pt.x=l;

                        l=nodelst.size();
                        nodelst.push_back(node1.clone());
                        segm.n0=p1[0];
                        segm.n1=l;
                        linelst.push_back(segm.clone());
                        pt.y=l;

                        pt.t=pbclst[n]->antiPeriodic;
                        ptlst.push_back(std::make_unique<CCommonPoint>(pt));
                    }
                    else if(j==(k-1))
                    {
                        // last subdivision--no ptlst
                        // entry associated with this one.
                        l=nodelst.size()-2;
                        segm.n0=l;
                        segm.n1=p0[1];
                        linelst.push_back(segm.clone());

                        l=nodelst.size()-1;
                        segm.n0=l;
                        segm.n1=p1[1];
                        linelst.push_back(segm.clone());
                    }
                    else{
                        l=nodelst.size();

                        nodelst.push_back(node0.clone());
                        nodelst.push_back(node1.clone());

                        segm.n0=l-2;
                        segm.n1=l;
                        linelst.push_back(segm.clone());

                        segm.n0=l-1;
                        segm.n1=l+1;
                        linelst.push_back(segm.clone());

                        pt.x=l;
                        pt.y=l+1;
                        pt.t=pbclst[n]->antiPeriodic;
                        ptlst.push_back(std::make_unique<CCommonPoint>(pt));
                    }
                }

            }
        }
    }

    // Then, do the rest of the lines and arcs in the
    // "normal" way and write .poly file.

    // discretize input segments
    discretizeInputSegments(*problem, nodelst, linelst, DoSmartMesh, dL, SegmentFilter::OnlyUnselected);

    // discretize input arc segments
    discretizeInputArcSegments(*problem, nodelst, linelst, SegmentFilter::OnlyUnselected);

    // create correct output filename;
    pn = PathName;

//    plyname=pn.Left(pn.ReverseFind('.')) + ".poly";
//
//    // check to see if we are ready to write a datafile;
//
//    if ((fp=fopen(plyname,"wt"))==NULL){
//        WarnMessage("Couldn't write to specified .poly file");
//        Undo();  UnselectAll();
//        return false;
//    }
//
//    // write out node list
//    fprintf(fp,"%i    2    0    1\n",nodelst.size());
//    for(i=0;i<nodelst.size();i++)
//    {
//        for(j=0,t=0;j<nodeproplist.size();j++)
//                if(nodeproplist[j]->PointName==nodelst[i]->BoundaryMarkerName) t=j+2;
//        fprintf(fp,"%i    %.17g    %.17g    %i\n",i,nodelst[i]->x,nodelst[i]->y,t);
//    }
//
//    // write out segment list
//    fprintf(fp,"%i    1\n",linelst.size());
//    for(i=0;i<linelst.size();i++)
//    {
//        for(j=0,t=0;j<lineproplist.size();j++)
//                if(lineproplist[j]->BdryName==linelst[i]->BoundaryMarkerName) t=-(j+2);
//        fprintf(fp,"%i    %i    %i    %i\n",i,linelst[i]->n0,linelst[i]->n1,t);
//    }

//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i]->BlockTypeName=="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g\n",k,blocklist[i]->x,blocklist[i]->y);
//            k++;
//        }
//
//    // write out regional attributes
//    fprintf(fp,"%i\n",blocklist.size()-j);
//
//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i]->BlockTypeName!="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g    ",k,blocklist[i]->x,blocklist[i]->y);
//            fprintf(fp,"%i    ",k+1);
//            if (blocklist[i]->MaxArea>0)
//                fprintf(fp,"%.17g\n",blocklist[i]->MaxArea);
//            else fprintf(fp,"-1\n");
//            k++;
//        }
//    fclose(fp);

    // Make sure to prune out any duplications in the ptlst
    for(k=0;k<(int)ptlst.size();k++) ptlst[k]->sortXY();
    k=0;
    while((k+1) < (int)ptlst.size())
    {
        j=k+1;
        while(j < (int)ptlst.size())
        {
            if((ptlst[k]->x==ptlst[j]->x) && (ptlst[k]->y==ptlst[j]->y))
                ptlst.erase(ptlst.begin()+j);
            else j++;
        }
        k++;
    }

    // used to have a check to eliminate the case where a point
    // and its companion are the same point--actually, this shouldn't
    // be a problem just to let the algorithm deal with this
    // as usual.

    // One last error check--each point must have only one companion point.
    // however, it would be possible to screw up in the definition of the BCs
    // so that this isn't the case.  Look through the points to try and catch
    // this one.
/*
    // let's let this check go away for a minute...

    for(k=0,n=false;(k+1)<ptlst.size();k++)
    {
        for(j=k+1;j<ptlst.size();j++)
        {
            if(ptlst[k]->x==ptlst[j]->x) n=true;
            if(ptlst[k]->y==ptlst[j]->y) n=true;
            if(ptlst[k]->x==ptlst[j]->y) n=true;
            if(ptlst[k]->y==ptlst[j]->x) n=true;
        }
    }
    if (n==true){
        WarnMessage("Nonphysical (anti)periodic boundary assignments");
        Undo();  UnselectAll();
        return false;
    }
*/
    // write out a pbc file containing a list of linked nodes
    plyname = pn.substr(0,pn.find_last_of('.')) + ".pbc";
    if ((fp=fopen(plyname.c_str(),"wt"))==NULL){
        WarnMessage("Couldn't write to specified .pbc file");
        problem->undo();  problem->unselectAll();
        return -1;
    }
    fprintf(fp,"%i\n", (int) ptlst.size());
    for(k=0;k<(int)ptlst.size();k++)
        fprintf(fp,"%i    %i    %i    %i\n",k,ptlst[k]->x,ptlst[k]->y,ptlst[k]->t);
    fclose(fp);

    // call triangle with -Y flag.
    {
        TriangulateHelper triHelper;
        triHelper.WarnMessage = WarnMessage;
        triHelper.TriMessage = this->TriMessage;

        if (!triHelper.initPointsWithMarkers(nodelst,*problem, PointMarkerInfo::FromProblem))
            return -1;
        if (!triHelper.initSegmentsWithMarkers(linelst,*problem,SegmentMarkerInfo::FromProblem))
            return -1;
        if (!triHelper.initHolesAndRegions(*problem, true, DefaultMeshSize))
            return -1;

        triHelper.setMinAngle(problem->MinAngle);
        triHelper.suppressExteriorSteinerPoints();
        int tristatus = triHelper.triangulate(Verbose);
        if (tristatus != 0)
            return tristatus;

        triHelper.writeTriangulationFiles(PathName);
    }

    problem->unselectAll();

    // Now restore boundary segment discretizations that have
    // been mucked up in the process...
    problem->undoLines();

    // and save the latest version of the document to make sure
    // any changes to arc discretization get propagated into
    // the solution description....
    //SaveFEMFile(pn);
    problem->saveFEMFile(pn);

    return 0;
}

TriangulateHelper::TriangulateHelper()
    : WarnMessage(&PrintWarningMsg)
    , TriMessage(nullptr)
    , m_minAngle(0.)
    , m_suppressExteriorSteinerPoints(false)
{
    initialize(in);
#ifdef XFEMM_BUILTIN_TRIANGLE
    initialize(out); // triangle-api writes to input struct
#else
    ctx = triangle_context_create();
#endif
}

TriangulateHelper::~TriangulateHelper()
{
    if (in.pointlist) { free(in.pointlist); }
    if (in.pointattributelist) { free(in.pointattributelist); }
    if (in.pointmarkerlist) { free(in.pointmarkerlist); }
    if (in.regionlist) { free(in.regionlist); }
    if (in.segmentlist) { free(in.segmentlist); }
    if (in.segmentmarkerlist) { free(in.segmentmarkerlist); }
    if (in.holelist) { free(in.holelist); }

#ifdef XFEMM_BUILTIN_TRIANGLE
    if (out.pointlist) { free(out.pointlist); }
    if (out.pointattributelist) { free(out.pointattributelist); }
    if (out.pointmarkerlist) { free(out.pointmarkerlist); }
    if (out.trianglelist) { free(out.trianglelist); }
    if (out.triangleattributelist) { free(out.triangleattributelist); }
    if (out.trianglearealist) { free(out.trianglearealist); }
    if (out.neighborlist) { free(out.neighborlist); }
    if (out.segmentlist) { free(out.segmentlist); }
    if (out.segmentmarkerlist) { free(out.segmentmarkerlist); }
    if (out.edgelist) { free(out.edgelist); }
    if (out.edgemarkerlist) { free(out.edgemarkerlist); }
#else
    triangle_context_destroy(ctx);
#endif
}

bool TriangulateHelper::initPointsWithMarkers(const TriangulateHelper::nodelist_t &nodelst, const FemmProblem &problem, PointMarkerInfo info)
{
    // calling this method on an already initialized object would leak memory
    if (in.numberofpoints!=0)
    {
        WarnMessage("initPointsWithMarkers called twice!\n");
        return false;
    }

    in.numberofpoints = nodelst.size();

    in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
    if (!in.pointlist) {
        WarnMessage("Point list for triangulation is null!\n");
        return false;
    }

    for(int i=0; i < in.numberofpoints; i++)
    {
        in.pointlist[2*i] = nodelst[i]->x;
        in.pointlist[2*i+1] = nodelst[i]->y;
    }

    // Initialise the pointmarkerlist
    in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));
    if (!in.pointmarkerlist) {
        WarnMessage("Point marker list for triangulation is null!\n");
        return false;
    }

    // write out node marker list
    for(int i=0; i<in.numberofpoints; i++)
    {
        int t=0;
        if (info==PointMarkerInfo::FromProblem)
        {
            for(int j=0; j<(int)problem.nodeproplist.size(); j++)
                if(problem.nodeproplist[j]->PointName==nodelst[i]->BoundaryMarkerName)
                    t = j + 2;

            if (problem.filetype != femm::FileType::MagneticsFile)
            {
                // include conductor number;
                for(int j = 0; j < (int)problem.circproplist.size(); j++)
                {
                    // add the conductor number using a mask
                    if(problem.circproplist[j]->CircName == nodelst[i]->InConductorName)
                        t += ((j+1) * 0x10000);
                }
            }
        }

        in.pointmarkerlist[i] = t;
    }
    return true;
}

bool TriangulateHelper::initSegmentsWithMarkers(const TriangulateHelper::linelist_t &linelst, const FemmProblem &problem, SegmentMarkerInfo info)
{
    // calling this method on an already initialized object would leak memory
    if (in.numberofsegments!=0)
    {
        WarnMessage("initSegmentsWithMarkers called twice!\n");
        return false;
    }

    in.numberofsegments = linelst.size();

    // Initialise the segmentlist
    in.segmentlist = (int *) malloc(2 * in.numberofsegments * sizeof(int));
    if (!in.segmentlist) {
        WarnMessage("Segment list for triangulation is null!\n");
        return false;
    }
    // Initialise the segmentmarkerlist
    in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));
    if (!in.segmentmarkerlist) {
        WarnMessage("Segment marker list for triangulation is null!\n");
        return false;
    }

    // build the segmentlist
    for(int i=0; i<in.numberofsegments; i++)
    {
            in.segmentlist[2*i] = linelst[i]->n0;
            in.segmentlist[2*i+1] = linelst[i]->n1;
    }

    // now build the segment marker list
    // construct the segment list
    for(int i=0; i<in.numberofsegments; i++)
    {
        int t=0;
        if (info==SegmentMarkerInfo::FromProblem)
        {
            for(int j=0; j <(int)problem.lineproplist.size(); j++)
            {
                if (problem.lineproplist[j]->BdryName == linelst[i]->BoundaryMarkerName)
                {
                    t = -(j+2);
                }
            }

            if (problem.filetype != femm::FileType::MagneticsFile)
            {
                // include conductor number;
                for (int j=0; j <(int)problem.circproplist.size(); j++)
                {
                    if (problem.circproplist[j]->CircName == linelst[i]->InConductorName)
                    {
                        t -= ((j+1) * 0x10000);
                    }
                }
            }
        } else {
            t = -(linelst[i]->cnt+2);
        }
        in.segmentmarkerlist[i] = t;
    }
    return true;
}

bool TriangulateHelper::initHolesAndRegions(const FemmProblem &problem, bool forceMaxMeshArea, double defaultMeshSize)
{
    // calling this method on an already initialized object would leak memory
    if (in.numberofholes!=0)
    {
        WarnMessage("initHolesAndRegions called twice!\n");
        return false;
    }

    in.numberofholes = problem.countHoles();
    if(in.numberofholes > 0)
    {
        in.holelist = (REAL *) malloc(in.numberofholes * 2 * sizeof(REAL));
        if (!in.holelist) {
            WarnMessage("Hole list for triangulation is null!\n");
            return false;
        }

        // Construct the holes array
        int k=0;
        for(const auto &label: problem.labellist)
        {
            // we search through the block list looking for blocks that have
            // the tag <no mesh>
            if(!label->hasBlockType())
            {
                //fprintf(fp,"%i    %.17g    %.17g\n", k, blocklist[i]->x, blocklist[i]->y);
                in.holelist[k++] = label->x;
                in.holelist[k++] = label->y;
            }
        }
    }

    in.numberofregions = problem.labellist.size() - in.numberofholes;
    in.regionlist = (REAL *) malloc(in.numberofregions * 4 * sizeof(REAL));
    if (!in.regionlist) {
        WarnMessage("Region list for triangulation is null!\n");
        return false;
    }

    int j=0;
    int k=0;
    for(const auto & label: problem.labellist)
    {
        if(label->hasBlockType())
        {
            in.regionlist[j] = label->x;
            in.regionlist[j+1] = label->y;
            in.regionlist[j+2] = k + 1; // Regional attribute (for whole mesh).

            // Note(ZaJ): this is the code that was used in the periodic bc triangulation:
            //  if (label->MaxArea>0 && (label->MaxArea<defaultMeshSize))
            //      in.regionlist[j+3] = label->MaxArea;  // Area constraint
            //  else
            //      in.regionlist[j+3] = defaultMeshSize;
            // ... which is equivalent to the code below (if forceMaxMeshArea is true).
            // ... the code below is a copy of the nonperiodic case (if forceMaxMeshArea is set to problem->DoForceMaxMeshArea)

            // Area constraint
            if (label->MaxArea <= 0)
            {
                // if no mesh size has been specified use the default
                in.regionlist[j+3] = defaultMeshSize;
            }
            else if ((label->MaxArea > defaultMeshSize) && (forceMaxMeshArea))
            {
                // if the user has specied that FEMM should choose an
                // upper mesh size limit, regardles of their choice,
                // and their choice is less than that limit, change it
                // to that limit
                in.regionlist[j+3] = defaultMeshSize;
            }
            else
            {
                // Use the user's choice of mesh size
                in.regionlist[j+3] = label->MaxArea;
            }

            j += 4;
            k++;
        }
    }
    return true;
}

int TriangulateHelper::triangulate(bool verbose)
{
    // An explaination of the input parameters used for Triangle
    //
    // -p Triangulates a Planar Straight Line Graph, i.e. list of segments.
    // -P Suppresses the output .poly file.
    // -q Quality mesh generation with no angles smaller than specified in the following number
    // -e Outputs a list of edges of the triangulation.
    // -A Assigns a regional attribute to each triangle that identifies what segment-bounded region it belongs to.
    // -a Imposes a maximum triangle area constraint.
    // -z Numbers all items starting from zero (rather than one)
    // -I Suppresses mesh iteration numbers
    // -j prevents duplicated input vertices, or vertices `eaten' by holes,
    //    from appearing in the output .node file.  Thus, if two input vertices
    //    have exactly the same coordinates, only the first appears in the
    //    output.
    // -Y Suppresses the creation of Steiner points on the exterior boundary.
    //
    // See http://www.cs.cmu.edu/~quake/triangle.switch.html for more info
    std::string triArgs = "-pPq" + to_string(m_minAngle) + "eAaz" + (verbose?"":"Q") + "I";
    if (m_suppressUnusedVertices)
        triArgs += "j";
    if (m_suppressExteriorSteinerPoints)
        triArgs += "Y";

    // this is a mess, but building the string with std::string is more flexible than sprintf
    // (and the triangulate api is ancient)
    char cmdline[512];
    sprintf(cmdline, "%s",triArgs.c_str());

#ifdef XFEMM_BUILTIN_TRIANGLE
    int tristatus = ::triangulate(cmdline, &in, &out, (struct triangulateio *) nullptr, this->TriMessage);
    if (tristatus!=0)
    {
        std::string msg = "Call to triangulate failed with status code: " + to_string(tristatus) +"\n";
        WarnMessage(msg.c_str());
        return tristatus;
    }
#else
    // parse options
    int tristatus = triangle_context_options(ctx, cmdline);
    if (tristatus != TRI_OK)
    {
        WarnMessage("Invalid option string for triangle!\n");
        return tristatus;
    }
    // Triangulate the polygon.
    tristatus = triangle_mesh_create(ctx, &in);
    if (tristatus != TRI_OK)
    {
        std::string msg = "Call to triangulate failed with status code: " + to_string(tristatus) +"\n";
        WarnMessage(msg.c_str());
        return tristatus;
    }
#endif
    return 0;
}

void TriangulateHelper::setMinAngle(double value)
{
    m_minAngle = value;
}

void TriangulateHelper::suppressExteriorSteinerPoints()
{
    m_suppressExteriorSteinerPoints = true;
}

void TriangulateHelper::suppressUnusedVertices()
{
    m_suppressUnusedVertices = true;
}
