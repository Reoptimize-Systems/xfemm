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


// implementation of various incarnations of calls
// to triangle from the FMesher class

#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <malloc.h>
#include "fmesher.h"
#include "fparse.h"
#include "intpoint.h"
#include "femmconstants.h"
//extern "C" {
#include "triangle.h"
//}


#ifndef REAL
#define REAL double
#endif

#ifndef LineFraction
#define LineFraction 500.0
#endif

// preprocessor tolerance
#ifndef CLOSE_ENOUGH
#define CLOSE_ENOUGH 1.e-06
#endif

// Default mesh size is the diagonal of the geometry's
// bounding box divided by BoundingBoxFraction
#ifndef BoundingBoxFraction
#define BoundingBoxFraction 100.0
#endif

#ifndef DEFAULT_MINIMUM_ANGLE
#define DEFAULT_MINIMUM_ANGLE 30.0
#endif

using namespace std;
using namespace femm;


double FMesher::LineLength(int i)
{
    return abs(nodelist[linelist[i].n0].CC()-
           nodelist[linelist[i].n1].CC());
}


bool FMesher::HasPeriodicBC()
{
    bool flag=false;
    unsigned int i,j;
    int k;

    for(i=0;i<lineproplist.size();i++)
    {
        if ((lineproplist[i].BdryFormat==4) ||
            (lineproplist[i].BdryFormat==5))
            flag=true;
    }
    // if flag is false, there can't be any lines
    // with periodic BC's, because no periodic boundary
    // conditions have been defined.
    if (flag==false) return false;

    //now, if there are some periodic boundary conditions,
    //we have to check to see if any have actually been
    //applied to the model
    flag=false; // reset flag

    // first, test the segments
    for(i=0;i<linelist.size();i++)
    {
        for(j=0,k=-1;j<lineproplist.size();j++)
        {
            if(lineproplist[j].BdryName==
               linelist[i].BoundaryMarker)
            {
                k=j;
                break;
            }
        }
        if(k>=0){
            if ((lineproplist[k].BdryFormat==4) ||
                (lineproplist[k].BdryFormat==5))
            {
                flag=true;
                break;
            }
        }
    }

    if (flag==true) return true;

    // If we've gotten this far, we still need to check the
    // arc segments.
    for(i=0;i<arclist.size();i++)
    {
        for(j=0,k=-1;j<lineproplist.size();j++)
        {
            if(lineproplist[j].BdryName==
               arclist[i].BoundaryMarker)
            {
                k=j;
                break;
            }
        }
        if(k>=0){
            if ((lineproplist[k].BdryFormat==4) ||
                (lineproplist[k].BdryFormat==5))
            {
                flag=true;
                break;
            }
        }
    }

    // Finally, we're done. The value of flag now reflects
    // the judgement on whether or not we have periodic
    // and/or antiperiodic boundaries.
    return flag;
}


bool FMesher::WriteTriangulationFiles(const struct triangulateio &out, string PathName)
{
    FILE *fp;
    int i, j, nexttriattrib;
    std::string msg;

    // write the .edge file
    string plyname = PathName.substr(0, PathName.find_last_of('.')) + ".edge";

    if (out.numberofedges > 0)
    {
        // check to see if we are ready to write an edge datafile;

        if ((fp = fopen(plyname.c_str(),"wt"))==NULL){
            msg = "Couldn't write to specified .edge file";
            WarnMessage(msg.c_str());
            return false;
        }

        // write number of edges, number of boundary markers, 0 or 1
        fprintf(fp, "%i\t%i\n", out.numberofedges, 1);

        // write the edges in the format
        // <edge #> <endpoint> <endpoint> [boundary marker]
        // Endpoints are indices into the corresponding .edge file.
        for(i=0; i < 2 * (out.numberofedges) - 1; i = i + 2)
        {
            fprintf(fp, "%i\t%i\t%i\t%i\n", i/2, out.edgelist[i], out.edgelist[i+1], out.edgemarkerlist[i/2]);
        }

        fclose(fp);

    }

    // write the .ele file
    plyname = PathName.substr(0, PathName.find_last_of('.')) + ".ele";

    if (out.numberoftriangles > 0)
    {

        // check to see if we are ready to write a .ele datafile containing
        // thr triangle elements

        if ((fp = fopen(plyname.c_str(),"wt"))==NULL){
            WarnMessage("Couldn't write to specified .ele file");
            return false;
        }

        // write number of triangle elements, number of corners per triangle and
        // the number of attributes per triangle
        fprintf(fp, "%i\t%i\t%i\n", out.numberoftriangles, out.numberofcorners, out.numberoftriangleattributes);

        // write the triangle info to the file with the format
        // <triangle #> <node> <node> <node> ... [attributes]
        // Endpoints are indices into the corresponding .node file.
        for(i=0, nexttriattrib=0; i < (out.numberofcorners) * (out.numberoftriangles) - (out.numberofcorners - 1); i = i + (out.numberofcorners))
        {
            // print the triangle number
            fprintf(fp, "%i\t", i / (out.numberofcorners));

            // print the corner nodes
            for (j = 0; j < (out.numberofcorners); j++)
            {
                fprintf(fp, "%i\t", out.trianglelist[i+j]);
            }

            // print the triangle attributes, if there are any
            if (out.numberoftriangleattributes > 0)
            {
                for(j = 0; j < (out.numberoftriangleattributes); j++)
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

    // write the .node file
    plyname = PathName.substr(0, PathName.find_last_of('.')) + ".node";

    if (out.numberofpoints > 0)
    {

        // check to see if we are ready to write a .node datafile containing
        // the nodes

        if ((fp = fopen(plyname.c_str(),"wt"))==NULL){
            WarnMessage("Couldn't write to specified .node file");
            return false;
        }

        // <# of vertices> <dimension (must be 2)> <# of attributes> <# of boundary markers (0 or 1)>
        fprintf(fp, "%i\t%i\t%i\t%i\n", out.numberofpoints, 2, 0, 1);
        //fprintf(fp, "%i\t%i\t%i\n", out.numberofpoints, 2, out.numberofpoints, 1);

        // <vertex #> <x> <y> [attributes] [boundary marker]
        for(i = 0; i < (2 * out.numberofpoints) - 1; i = i + 2)
        {
            fprintf(fp, "%i\t%.17g\t%.17g\t%i\n", i/2, out.pointlist[i], out.pointlist[i+1], out.pointmarkerlist[i/2]);
        }

        fclose(fp);

    }

    return true;

}

// What we do in the normal case is DoNonPeriodicBCTriangulation
int FMesher::DoNonPeriodicBCTriangulation(string PathName)
{
    FILE *fp;
    unsigned int i,j,k;
    int l,t,NRegionalAttribs,Nholes,tristatus;
    double z,R,dL;
    CComplex a0,a1,a2,c;
    //CStdString s;
    string plyname;
    std::vector < CNode >       nodelst;
    std::vector < CSegment >    linelst;
    std::vector < CArcSegment > arclst;
    std::vector < CBlockLabel > blocklst;
    CNode node;
    CSegment segm;
    // structures to hold the iinput and output of triangulaye call
    char CommandLine[512];
    struct triangulateio in, out;

    nodelst.clear();
    linelst.clear();
    // calculate length used to kludge fine meshing near input node points
    for (i=0,z=0;i < linelist.size();i++)
    {
        a0.Set(nodelist[linelist[i].n0].x,nodelist[linelist[i].n0].y);
        a1.Set(nodelist[linelist[i].n1].x,nodelist[linelist[i].n1].y);
        z += (abs(a1-a0)/((double) linelist.size()));
    }
    dL=z/LineFraction;

    // copy node list as it is;
    for(i=0;i<nodelist.size();i++) nodelst.push_back(nodelist[i]);

    // discretize input segments
    for(i=0;i<linelist.size();i++)
    {
        a0.Set(nodelist[linelist[i].n0].x,nodelist[linelist[i].n0].y);
        a1.Set(nodelist[linelist[i].n1].x,nodelist[linelist[i].n1].y);
        if (linelist[i].MaxSideLength==-1) k=1;
        else{
            z=abs(a1-a0);
            k=(int) std::ceil(z/linelist[i].MaxSideLength);
        }

        if (k==1) // default condition where discretization on line is not specified
        {
            if (abs(a1-a0) < (3.*dL) || DoSmartMesh == false)
            {
                linelst.push_back(linelist[i]); // line is too short to add extra points
            }
            else
            {
                // add extra points at a distance of dL from the ends of the line.
                // this forces Triangle to finely mesh near corners
                segm=linelist[i];
                for(j=0;j<3;j++)
                {
                    if(j==0)
                    {
                        a2=a0+dL*(a1-a0)/abs(a1-a0);
                        node.x=a2.re; node.y=a2.im;
                        l=(int) nodelst.size();
                        nodelst.push_back(node);
                        segm.n0=linelist[i].n0;
                        segm.n1=l;
                        linelst.push_back(segm);
                    }

                    if(j==1)
                    {
                        a2=a1+dL*(a0-a1)/abs(a1-a0);
                        node.x=a2.re; node.y=a2.im;
                        l=(int) nodelst.size();
                        nodelst.push_back(node);
                        segm.n0=l-1;
                        segm.n1=l;
                        linelst.push_back(segm);
                    }

                    if(j==2)
                    {
                        l=(int) nodelst.size()-1;
                        segm.n0=l;
                        segm.n1=linelist[i].n1;
                        linelst.push_back(segm);
                    }

                }
            }
        }
        else{
            segm=linelist[i];
            for(j=0;j<k;j++)
            {
                a2=a0+(a1-a0)*((double) (j+1))/((double) k);
                node.x=a2.re; node.y=a2.im;
                if(j==0){
                    l=nodelst.size();
                    nodelst.push_back(node);
                    segm.n0=linelist[i].n0;
                    segm.n1=l;
                    linelst.push_back(segm);
                }
                else if(j==(k-1))
                {
                    l=nodelst.size()-1;
                    segm.n0=l;
                    segm.n1=linelist[i].n1;
                    linelst.push_back(segm);
                }
                else{
                    l=nodelst.size();
                    nodelst.push_back(node);
                    segm.n0=l-1;
                    segm.n1=l;
                    linelst.push_back(segm);
                }
            }
        }
    }

    // discretize input arc segments
    for(i=0;i<arclist.size();i++)
    {
        a2.Set(nodelist[arclist[i].n0].x,nodelist[arclist[i].n0].y);
        k = (unsigned int) std::ceil(arclist[i].ArcLength/arclist[i].MaxSideLength);
        segm.BoundaryMarker=arclist[i].BoundaryMarker;
        GetCircle(arclist[i],c,R);
        a1=exp(I*arclist[i].ArcLength*PI/(((double) k)*180.));

        if(k==1){
            segm.n0=arclist[i].n0;
            segm.n1=arclist[i].n1;
            linelst.push_back(segm);
        }
        else for(j=0;j<k;j++)
        {
            a2=(a2-c)*a1+c;
            node.x=a2.re; node.y=a2.im;
            if(j==0){
                l=nodelst.size();
                nodelst.push_back(node);
                segm.n0=arclist[i].n0;
                segm.n1=l;
                linelst.push_back(segm);
            }
            else if(j==(k-1))
            {
                l=nodelst.size()-1;
                segm.n0=l;
                segm.n1=arclist[i].n1;
                linelst.push_back(segm);
            }
            else{
                l=nodelst.size();
                nodelst.push_back(node);
                segm.n0=l-1;
                segm.n1=l;
                linelst.push_back(segm);
            }
        }
    }


    // create correct output filename;
    string pn = PathName;

    // write out list of holes;
    for(i=0,j=0;i<blocklist.size();i++)
    {
        if(blocklist[i].BlockType=="<No Mesh>")
        {
            j++;
        }
    }

    // store the number of holes
    Nholes = j;

    NRegionalAttribs = blocklist.size() - j;

    // figure out a good default mesh size for block labels where
    // mesh size isn't explicitly specified
    CComplex xx,yy;
    double absdist;
    double DefaultMeshSize;
    if (nodelst.size()>1)
    {
        xx=nodelst[0].CC(); yy=xx;
        for(k=0;k<nodelst.size();k++)
        {
            if (nodelst[k].x<Re(xx)) xx.re=nodelst[k].x;
            if (nodelst[k].y<Im(xx)) xx.im=nodelst[k].y;
            if (nodelst[k].x>Re(yy)) yy.re=nodelst[k].x;
            if (nodelst[k].y>Im(yy)) yy.im=nodelst[k].y;
        }
        absdist = (double)(abs(yy-xx)/BoundingBoxFraction);
        DefaultMeshSize = std::pow(absdist,2);

        if (DoSmartMesh == false)
        {
            DefaultMeshSize = abs(yy-xx);
        }
    }
    else DefaultMeshSize=-1;

//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i].BlockType=="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g\n",k,blocklist[i].x,blocklist[i].y);
//            k++;
//        }
//
//    // write out regional attributes
//    fprintf(fp,"%i\n",blocklist.size()-j);
//
//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i].BlockType!="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g    ",k,blocklist[i].x,blocklist[i].y);
//            fprintf(fp,"%i    ",k+1);
//            if (blocklist[i].MaxArea>0)
//                fprintf(fp,"%.17g\n",blocklist[i].MaxArea);
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

    in.numberofpoints = nodelst.size();

    in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
    if (in.pointlist == NULL) {
        return -1;
    }

    for(i=0; i < (unsigned int)(2 * in.numberofpoints - 1); i = i + 2)
    {
        in.pointlist[i] = nodelst[i/2].x;
        in.pointlist[i+1] = nodelst[i/2].y;
    }

    in.numberofpointattributes = 0;

    in.pointattributelist = (REAL *) NULL;

    // Initialise the pointmarkerlist
    in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));
    if (in.pointmarkerlist == NULL) {
        return false;
    }

    t = 0;
    // write out node marker list
    for(i=0;i<nodelst.size();i++)
    {
        for(j=0,t=0;j<nodeproplist.size ();j++)
                if(nodeproplist[j].PointName==nodelst[i].BoundaryMarker) t = j + 2;

        if (filetype == F_TYPE_HEATFLOW)
        {
            // include conductor number;
            for(j = 0; j < circproplist.size (); j++)
            {
                // add the conductor numer using a mask
                if(circproplist[j].CircName == nodelst[i].InConductor) t += ((j+1) * 0x10000);
            }
        }

        in.pointmarkerlist[i] = t;
    }

    in.numberofsegments = linelst.size();

    // Initialise the segmentlist
    in.segmentlist = (int *) malloc(2 * in.numberofsegments * sizeof(int));
    if (in.segmentlist == NULL) {
        return -1;
    }
    // Initialise the segmentmarkerlist
    in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));
    if (in.segmentmarkerlist == NULL) {
        return -1;
    }

    // build the segmentlist
    for(i=0; i < (unsigned int)(2*in.numberofsegments - 1); i = i + 2)
    {
            in.segmentlist[i] = linelst[i/2].n0;

            in.segmentlist[i+1] = linelst[i/2].n1;
    }

    // now build the segment marker list
    t = 0;

    // construct the segment list
    for(i=0;i<linelst.size();i++)
    {
        for(j=0,t=0; j < lineproplist.size (); j++)
        {
                if (lineproplist[j].BdryName == linelst[i].BoundaryMarker)
                {
                    t = -(j+2);
                }
        }

        if (filetype == F_TYPE_HEATFLOW)
        {
            // include conductor number;
            for (j=0; j < circproplist.size (); j++)
            {
                if (circproplist[j].CircName == linelst[i].InConductor)
                {
                    t -= ((j+1) * 0x10000);
                }
            }
        }

        in.segmentmarkerlist[i] = t;
    }

    in.numberofholes = Nholes;
    in.holelist = (REAL *) malloc(in.numberofholes * 2 * sizeof(REAL));
    if (in.holelist == NULL) {
        return -1;
    }

    // Construct the holes array
    for(i=0, k=0; i < blocklist.size(); i++)
    {
        // we search through the block list looking for blocks that have
        // the tag <no mesh>
        if(blocklist[i].BlockType == "<No Mesh>")
        {
            in.holelist[k] = blocklist[i].x;
            in.holelist[k+1] = blocklist[i].y;
            k = k + 2;
        }
    }

    in.numberofregions = NRegionalAttribs;
    in.regionlist = (REAL *) malloc(in.numberofregions * 4 * sizeof(REAL));
    if (in.regionlist == NULL) {
        return -1;
    }

    for(i = 0, j = 0, k = 0; i < blocklist.size(); i++)
    {
        if(blocklist[i].BlockType != "<No Mesh>")
        {
            in.regionlist[j] = blocklist[i].x;
            in.regionlist[j+1] = blocklist[i].y;
            in.regionlist[j+2] = k + 1; // Regional attribute (for whole mesh).

//            if (blocklist[i].MaxArea>0 && (blocklist[i].MaxArea<DefaultMeshSize))
//            {
//                in.regionlist[j+3] = blocklist[i].MaxArea;  // Area constraint
//            }
//            else
//            {
//                in.regionlist[j+3] = DefaultMeshSize;
//            }

            // Area constraint
            if (blocklist[i].MaxArea <= 0)
            {
                // if no mesh size has been specified use the default
                in.regionlist[j+3] = DefaultMeshSize;
            }
            else if ((blocklist[i].MaxArea > DefaultMeshSize) && (DoForceMaxMeshArea))
            {
                // if the user has specied that FEMM should choose an
                // upper mesh size limit, regardles of their choice,
                // and their choice is less than that limit, change it
                // to that limit
                in.regionlist[j+3] = DefaultMeshSize;
            }
            else
            {
                // Use the user's choice of mesh size
                in.regionlist[j+3] = blocklist[i].MaxArea;
            }

            j = j + 4;
            k++;
        }
    }

    // Finally, we have no triangle area constraints so initialize to null
    in.trianglearealist = (REAL *) NULL;

    /* Make necessary initializations so that Triangle can return a */
    /*   triangulation in `out'  */

    out.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
    /* Not needed if -N switch used or number of point attributes is zero: */
    out.pointattributelist = (REAL *) NULL;
    out.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */
    out.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
    /* Not needed if -E switch used or number of triangle attributes is zero: */
    out.triangleattributelist = (REAL *) NULL;
    /* No triangle area constraints */
    out.trianglearealist = (REAL *) NULL;
    out.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */
    /* Needed only if segments are output (-p or -c) and -P not used: */
    out.segmentlist = (int *) NULL;
    /* Needed only if segments are output (-p or -c) and -P and -B not used: */
    out.segmentmarkerlist = (int *) NULL;
    out.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
    out.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

    string rootname = pn.substr(0,pn.find_last_of('.'));

    if (Verbose)
    {
        sprintf(CommandLine, "-pPq%feAazI", MinAngle);
    }
    else
    {
        sprintf(CommandLine, "-pPq%feAazQI", MinAngle);
    }

    tristatus = triangulate(CommandLine, &in, &out, (struct triangulateio *) NULL, this->TriMessage);

    // copy the exit status status of the triangle library from the global variable, eueghh.
    //trilibrary_exit_code;
    if (tristatus != 0)
    {
        // free allocated memory
        if (in.pointlist != NULL) { free(in.pointlist); }
        if (in.pointattributelist != NULL) { free(in.pointattributelist); }
        if (in.pointmarkerlist != NULL) { free(in.pointmarkerlist); }
        if (in.regionlist != NULL) { free(in.regionlist); }
        if (in.segmentlist != NULL) { free(in.segmentlist); }
        if (in.segmentmarkerlist != NULL) { free(in.segmentmarkerlist); }
        if (in.holelist != NULL) { free(in.holelist); }

        if (out.pointlist != NULL) { free(out.pointlist); }
        if (out.pointattributelist != NULL) { free(out.pointattributelist); }
        if (out.pointmarkerlist != NULL) { free(out.pointmarkerlist); }
        if (out.trianglelist != NULL) { free(out.trianglelist); }
        if (out.triangleattributelist != NULL) { free(out.triangleattributelist); }
        if (out.trianglearealist != NULL) { free(out.trianglearealist); }
        if (out.neighborlist != NULL) { free(out.neighborlist); }
        if (out.segmentlist != NULL) { free(out.segmentlist); }
        if (out.segmentmarkerlist != NULL) { free(out.segmentmarkerlist); }
        if (out.edgelist != NULL) { free(out.edgelist); }
        if (out.edgemarkerlist != NULL) { free(out.edgemarkerlist); }

        return tristatus;
    }

    WriteTriangulationFiles(out, PathName);

    // free allocated memory
    if (in.pointlist != NULL) { free(in.pointlist); }
    if (in.pointattributelist != NULL) { free(in.pointattributelist); }
    if (in.pointmarkerlist != NULL) { free(in.pointmarkerlist); }
    if (in.regionlist != NULL) { free(in.regionlist); }
    if (in.segmentlist != NULL) { free(in.segmentlist); }
    if (in.segmentmarkerlist != NULL) { free(in.segmentmarkerlist); }
    if (in.holelist != NULL) { free(in.holelist); }

    if (out.pointlist != NULL) { free(out.pointlist); }
    if (out.pointattributelist != NULL) { free(out.pointattributelist); }
    if (out.pointmarkerlist != NULL) { free(out.pointmarkerlist); }
    if (out.trianglelist != NULL) { free(out.trianglelist); }
    if (out.triangleattributelist != NULL) { free(out.triangleattributelist); }
    if (out.trianglearealist != NULL) { free(out.trianglearealist); }
    if (out.neighborlist != NULL) { free(out.neighborlist); }
    if (out.segmentlist != NULL) { free(out.segmentlist); }
    if (out.segmentmarkerlist != NULL) { free(out.segmentmarkerlist); }
    if (out.edgelist != NULL) { free(out.edgelist); }
    if (out.edgemarkerlist != NULL) { free(out.edgemarkerlist); }

    return 0;
}


// Call triangle twice to order segments on the boundary properly
// for periodic or antiperiodic boundary conditions
int FMesher::DoPeriodicBCTriangulation(string PathName)
{
    FILE *fp;
    unsigned int i, j, k, n;
    int l,t,n0,n1,n2,NRegionalAttribs,Nholes,tristatus;
    double z,R,dL;
    CComplex a0,a1,a2,c;
    CComplex b0,b1,b2;
    char instring[1024];
    //string s;
    string plyname;
    std::vector < CNode >             nodelst;
    std::vector < CSegment >          linelst;
    std::vector < CArcSegment >       arclst;
    std::vector < CBlockLabel >       blocklst;
    std::vector < CPeriodicBoundary > pbclst;
    std::vector < CCommonPoint >      ptlst;
    CNode node;
    CSegment segm;
    CPeriodicBoundary pbc;
    CCommonPoint pt;
    char CommandLine[512];
    struct triangulateio in, out;

    nodelst.clear();
    linelst.clear();
    pbclst.clear();
    ptlst.clear();

    UpdateUndo();

    // calculate length used to kludge fine meshing near input node points
    for (i=0, z=0; i<linelist.size(); i++)
    {
        a0.Set(nodelist[linelist[i].n0].x,nodelist[linelist[i].n0].y);
        a1.Set(nodelist[linelist[i].n1].x,nodelist[linelist[i].n1].y);
        z += (abs(a1-a0) / ((double) linelist.size()));
    }
    dL = z / LineFraction;

    // copy node list as it is;
    for(i=0; i<nodelist.size(); i++)
    {
        nodelst.push_back(nodelist[i]);
    }

    // discretize input segments
    for(i=0; i<linelist.size(); i++)
    {
        // abuse the IsSelected flag to carry a notation
        // of which line or arc in the input geometry a
        // particular segment is associated with
        segm = linelist[i];
        segm.IsSelected = i;
        a0.Set(nodelist[linelist[i].n0].x, nodelist[linelist[i].n0].y);
        a1.Set(nodelist[linelist[i].n1].x, nodelist[linelist[i].n1].y);

        if (linelist[i].MaxSideLength == -1) {
            k = 1;
        }
        else{
            z = abs(a1-a0);
            k = (unsigned int) std::ceil(z/linelist[i].MaxSideLength);
        }

        if (k == 1) // default condition where discretization on line is not specified
        {
            if (abs(a1-a0) < (3. * dL) || DoSmartMesh == false)
            {
                // line is too short to add extra points
                linelst.push_back(segm);
            }
            else{
                // add extra points at a distance of dL from the ends of the line.
                // this forces Triangle to finely mesh near corners
                for(j=0; j<3; j++)
                {
                    if(j==0)
                    {
                        a2 = a0 + dL * (a1-a0) / abs(a1-a0);
                        node.x = a2.re;
                        node.y = a2.im;
                        l = (int) nodelst.size();
                        nodelst.push_back(node);
                        segm.n0 = linelist[i].n0;
                        segm.n1 = l;
                        linelst.push_back(segm);
                    }

                    if(j == 1)
                    {
                        a2 = a1 + dL * (a0-a1) / abs(a1-a0);
                        node.x = a2.re;
                        node.y = a2.im;
                        l = (int) nodelst.size();
                        nodelst.push_back(node);
                        segm.n0 = l - 1;
                        segm.n1 = l;
                        linelst.push_back(segm);
                    }

                    if(j == 2)
                    {
                        l = (int) nodelst.size() - 1;
                        segm.n0 = l;
                        segm.n1 = linelist[i].n1;
                        linelst.push_back(segm);
                    }

                }
            }
        }
        else{
            for(j=0; j<k; j++)
            {
                a2 = a0 + (a1-a0)*((double) (j+1)) / ((double) k);
                node.x = a2.re;
                node.y = a2.im;
                if(j == 0){
                    l=nodelst.size();
                    nodelst.push_back(node);
                    segm.n0=linelist[i].n0;
                    segm.n1=l;
                    linelst.push_back(segm);
                }
                else if(j == (k-1))
                {
                    l=nodelst.size()-1;
                    segm.n0=l;
                    segm.n1=linelist[i].n1;
                    linelst.push_back(segm);
                }
                else{
                    l=nodelst.size();
                    nodelst.push_back(node);
                    segm.n0=l-1;
                    segm.n1=l;
                    linelst.push_back(segm);
                }
            }
        }
    }

    // discretize input arc segments
    for(i=0;i<arclist.size();i++)
    {
        segm.IsSelected=i+linelist.size();
        a2.Set(nodelist[arclist[i].n0].x,nodelist[arclist[i].n0].y);
        k=(int) ceil(arclist[i].ArcLength/arclist[i].MaxSideLength);
        segm.BoundaryMarker=arclist[i].BoundaryMarker;
        GetCircle(arclist[i],c,R);
        a1=exp(I*arclist[i].ArcLength*PI/(((double) k)*180.));

        if(k==1){
            segm.n0=arclist[i].n0;
            segm.n1=arclist[i].n1;
            linelst.push_back(segm);
        }
        else for(j=0;j<k;j++)
        {
            a2=(a2-c)*a1+c;
            node.x=a2.re; node.y=a2.im;
            if(j==0){
                l=nodelst.size();
                nodelst.push_back(node);
                segm.n0=arclist[i].n0;
                segm.n1=l;
                linelst.push_back(segm);
            }
            else if(j==(k-1))
            {
                l=nodelst.size()-1;
                segm.n0=l;
                segm.n1=arclist[i].n1;
                linelst.push_back(segm);
            }
            else{
                l=nodelst.size();
                nodelst.push_back(node);
                segm.n0=l-1;
                segm.n1=l;
                linelst.push_back(segm);
            }
        }
    }


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
//                 i,nodelst[i].x,nodelst[i].y,0);
//    }
//
//    // write out segment list
//    fprintf(fp,"%i    1\n",linelst.size());
//    for(i=0;i<linelst.size();i++)
//    {
//        t=-(linelst[i].IsSelected+2);
//        fprintf(fp,"%i    %i    %i    %i\n",i,linelst[i].n0,linelst[i].n1,t);
//    }

    // write out list of holes;
    for(i=0,j=0;i<blocklist.size();i++)
    {
        if(blocklist[i].BlockType=="<No Mesh>")
        {
            j++;
        }
    }

//    fprintf(fp,"%i\n",j);

    Nholes = j;
    NRegionalAttribs = blocklist.size() - Nholes;

    // figure out a good default mesh size for block labels where
    // mesh size isn't explicitly specified
    CComplex xx,yy;
    double temp;
    double DefaultMeshSize;
    if (nodelst.size()>1)
    {
        xx=nodelst[0].CC(); yy=xx;
        for(k=0;k<nodelst.size();k++)
        {
            if (nodelst[k].x<Re(xx)) xx.re=nodelst[k].x;
            if (nodelst[k].y<Im(xx)) xx.im=nodelst[k].y;
            if (nodelst[k].x>Re(yy)) yy.re=nodelst[k].x;
            if (nodelst[k].y>Im(yy)) yy.im=nodelst[k].y;
        }
        temp = (double)(abs(yy-xx)/BoundingBoxFraction);
        DefaultMeshSize=std::pow(temp,2);

        if (DoSmartMesh == false)
        {
            DefaultMeshSize = abs(yy-xx);
        }
    }
    else DefaultMeshSize=-1;

//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i].BlockType=="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g\n",k,blocklist[i].x,blocklist[i].y);
//            k++;
//        }

//    // write out regional attributes
//    fprintf(fp,"%i\n",blocklist.size()-j);
//
//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i].BlockType!="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g    ",k,blocklist[i].x,blocklist[i].y);
//            fprintf(fp,"%i    ",k+1);
//            if (blocklist[i].MaxArea>0)
//                fprintf(fp,"%.17g\n",blocklist[i].MaxArea);
//            else fprintf(fp,"-1\n");
//            k++;
//        }
//
//    fclose(fp);


    // **********         call triangle       ***********

    in.numberofpoints = nodelst.size();

    in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
    if (in.pointlist == NULL) {
        return -1;
    }

    for(i=0; i < (unsigned int)(2 * in.numberofpoints-1); i = i + 2)
    {
        in.pointlist[i] = nodelst[i/2].x;
        in.pointlist[i+1] = nodelst[i/2].y;
    }

    in.numberofpointattributes = 0;

    in.pointattributelist = (REAL *) NULL;

    // Initialise the pointmarkerlist
    in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));
    if (in.pointmarkerlist == NULL) {
        return -1;
    }

    // write out node marker list
    for(i=0; i < nodelst.size(); i++)
    {
        in.pointmarkerlist[i] = 0;
    }

    in.numberofsegments = linelst.size();

    // Initialise the segmentlist
    in.segmentlist = (int *) malloc(2 * in.numberofsegments * sizeof(int));
    if (in.segmentlist == NULL) {
        return -1;
    }
    // Initialise the segmentmarkerlist
    in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));
    if (in.segmentmarkerlist == NULL) {
        return -1;
    }

    // build the segmentlist
    for(i=0; i < (unsigned int)(2*in.numberofsegments - 1); i = i + 2)
    {
            in.segmentlist[i] = linelst[i/2].n0;

            in.segmentlist[i+1] = linelst[i/2].n1;

            //PRINTF("i: %i, segmentlist[i]: %i, segmentlist[i+1]: %i\n", i, in.segmentlist[i], in.segmentlist[i+1]);
    }

    // now build the segment marker list
    t = 0;

    // construct the segment marker list
    for(i=0; i < linelst.size(); i++)
    {
        t = -(linelst[i].IsSelected+2);

        in.segmentmarkerlist[i] = t;
    }

    in.numberofholes = Nholes;
    if(Nholes > 0)
    {
        in.holelist = (REAL *) malloc(in.numberofholes * 2 * sizeof(REAL));
        if (in.holelist == NULL) {
            return -1;
        }

        // Construct the holes array
        for(i=0, k=0; i < blocklist.size(); i++)
        {
            // we search through the block list looking for blocks that have
            // the tag <no mesh>
            if(blocklist[i].BlockType == "<No Mesh>")
            {
                //fprintf(fp,"%i    %.17g    %.17g\n", k, blocklist[i].x, blocklist[i].y);
                in.holelist[k] = blocklist[i].x;
                in.holelist[k+1] = blocklist[i].y;
                k = k + 2;
            }
        }
    }
    else
    {
        in.holelist = (REAL *) NULL;
    }

    in.numberofregions = NRegionalAttribs;
    in.regionlist = (REAL *) malloc(in.numberofregions * 4 * sizeof(REAL));
    if (in.regionlist == NULL) {
        return -1;
    }

    for(i = 0, j = 0, k = 0; i < blocklist.size(); i++)
    {
        if(blocklist[i].BlockType != "<No Mesh>")
        {

            in.regionlist[j] = blocklist[i].x;
            in.regionlist[j+1] = blocklist[i].y;
            in.regionlist[j+2] = k + 1; // Regional attribute (for whole mesh).

            if (blocklist[i].MaxArea > 0 && (blocklist[i].MaxArea<DefaultMeshSize))
            {
                in.regionlist[j+3] = blocklist[i].MaxArea;  // Area constraint
            }
            else
            {
                in.regionlist[j+3] = DefaultMeshSize;
            }

            j = j + 4;
            k++;
        }
    }

    // Finally, we have no triangle area constraints so initialize to null
    in.trianglearealist = (REAL *) NULL;

    /* Make necessary initializations so that Triangle can return a */
    /*   triangulation in `out'  */

    out.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
    /* Not needed if -N switch used or number of point attributes is zero: */
    out.pointattributelist = (REAL *) NULL;
    out.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */
    out.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
    /* Not needed if -E switch used or number of triangle attributes is zero: */
    out.triangleattributelist = (REAL *) NULL;
    /* No triangle area constraints */
    out.trianglearealist = (REAL *) NULL;
    out.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */
    /* Needed only if segments are output (-p or -c) and -P not used: */
    out.segmentlist = (int *) NULL;
    /* Needed only if segments are output (-p or -c) and -P and -B not used: */
    out.segmentmarkerlist = (int *) NULL;
    out.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
    out.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

    string rootname = pn.substr(0,pn.find_last_of('.'));

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
    //
    // See http://www.cs.cmu.edu/~quake/triangle.switch.html for more info
    if (Verbose)
    {
        sprintf(CommandLine, "-pPq%feAazI", MinAngle);
    }
    else
    {
        // -Q silences output
        sprintf(CommandLine, "-pPq%feAazQI", MinAngle);
    }

    // call triangulate (Triangle as library) to perform the meshing
    tristatus = triangulate(CommandLine, &in, &out, (struct triangulateio *) NULL, this->TriMessage);

    if (tristatus != 0)
    {
        // free allocated memory
        if (in.pointlist != NULL) { free(in.pointlist); }
        if (in.pointattributelist != NULL) { free(in.pointattributelist); }
        if (in.pointmarkerlist != NULL) { free(in.pointmarkerlist); }
        if (in.regionlist != NULL) { free(in.regionlist); }
        if (in.segmentlist != NULL) { free(in.segmentlist); }
        if (in.segmentmarkerlist != NULL) { free(in.segmentmarkerlist); }
        if (in.holelist != NULL) { free(in.holelist); }

        if (out.pointlist != NULL) { free(out.pointlist); }
        if (out.pointattributelist != NULL) { free(out.pointattributelist); }
        if (out.pointmarkerlist != NULL) { free(out.pointmarkerlist); }
        if (out.trianglelist != NULL) { free(out.trianglelist); }
        if (out.triangleattributelist != NULL) { free(out.triangleattributelist); }
        if (out.trianglearealist != NULL) { free(out.trianglearealist); }
        if (out.neighborlist != NULL) { free(out.neighborlist); }
        if (out.segmentlist != NULL) { free(out.segmentlist); }
        if (out.segmentmarkerlist != NULL) { free(out.segmentmarkerlist); }
        if (out.edgelist != NULL) { free(out.edgelist); }
        if (out.edgemarkerlist != NULL) { free(out.edgemarkerlist); }

        return tristatus;
    }

    WriteTriangulationFiles(out, PathName);

    // free allocated memory
    if (in.pointlist != NULL) { free(in.pointlist); }
    if (in.pointattributelist != NULL) { free(in.pointattributelist); }
    if (in.pointmarkerlist != NULL) { free(in.pointmarkerlist); }
    if (in.regionlist != NULL) { free(in.regionlist); }
    if (in.segmentlist != NULL) { free(in.segmentlist); }
    if (in.segmentmarkerlist != NULL) { free(in.segmentmarkerlist); }
    if (in.holelist != NULL) { free(in.holelist); }

    if (out.pointlist != NULL) { free(out.pointlist); }
    if (out.pointattributelist != NULL) { free(out.pointattributelist); }
    if (out.pointmarkerlist != NULL) { free(out.pointmarkerlist); }
    if (out.trianglelist != NULL) { free(out.trianglelist); }
    if (out.triangleattributelist != NULL) { free(out.triangleattributelist); }
    if (out.trianglearealist != NULL) { free(out.trianglearealist); }
    if (out.neighborlist != NULL) { free(out.neighborlist); }
    if (out.segmentlist != NULL) { free(out.segmentlist); }
    if (out.segmentmarkerlist != NULL) { free(out.segmentmarkerlist); }
    if (out.edgelist != NULL) { free(out.edgelist); }
    if (out.edgemarkerlist != NULL) { free(out.edgemarkerlist); }


    // So far, so good.  Now, read back in the .edge file
    // to make sure the points in the segments and arc
    // segments are ordered in a consistent way so that
    // the (anti)periodic boundary conditions can be applied.


    // read meshlines;
    plyname = pn.substr(0,pn.find_last_of('.')) + ".edge";
    if((fp=fopen(plyname.c_str(),"rt"))==NULL){
        WarnMessage("Call to triangle was unsuccessful");
        Undo();  UnselectAll();
        return -1;
    }
    fgets(instring,1024,fp);
    sscanf(instring,"%i",&k);
    UnselectAll();    // abuse IsSelected again to keep a
                    // tally of how many subsegments each
                    // entity is sliced into.

    ptlst.resize(linelist.size()+arclist.size());

    for(i=0;i<ptlst.size();i++)
    {
        ptlst[i].t = 0;
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
            if (ptlst[j].t==0)
            {
                ptlst[j].t=1;
                if(n0<n1){
                    ptlst[j].x=n0;
                    ptlst[j].y=n1;
                }
                else{
                    ptlst[j].x=n1;
                    ptlst[j].y=n0;
                }
            }

            if(j<linelist.size())
            {
                // deal with segments

                // increment IsSelected for the segment which the edge we are
                // examining is a part of to get a tally of how many edges
                // are a part of the segment/boundary
                linelist[j].IsSelected++;
                // check if the end n0 of the segment is the same node as the
                // end n1 of the edge, or if the end n1 of the segment is the
                // same node and end n0 of the edge. If so, flip the direction
                // of the segment
                if((linelist[j].n0 == n1) || (linelist[j].n1 == n0))
                {
                    // flip the end points of the segment
                    t = linelist[j].n0;
                    linelist[j].n0 = linelist[j].n1;
                    linelist[j].n1 = t;
                }
            }
            else{
                // deal with arc segments;
                // Can't just flip the point order with
                // impunity in the arc segments, so we flip
                // a marker which denotes which side the
                // normal is on.

                j=j-linelist.size();
                arclist[j].IsSelected++;
                if((arclist[j].n0==n1) || (arclist[j].n1==n0))
                    arclist[j].NormalDirection=false;
                if((arclist[j].n0==n0) || (arclist[j].n1==n1))
                    arclist[j].NormalDirection=true;
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
        Undo();  UnselectAll();
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
        for(j=0;j<ptlst.size();j++)
        {
            if ((n0==ptlst[j].x) && (n1==ptlst[j].y)) ptlst[j].t--;
            if ((n0==ptlst[j].x) && (n2==ptlst[j].y)) ptlst[j].t--;
            if ((n1==ptlst[j].x) && (n2==ptlst[j].y)) ptlst[j].t--;
        }
    }
    fclose(fp);

    // impose "new" mesh constraints on bdry arcs and segments....
    for(i=0; i < linelist.size(); i++)
    {
        if (ptlst[i].t == 0)
        {
            // simply make the max side length equal to the
            // length of the boundary divided by the number
            // of elements that were created in the first
            // attempt at meshing
            linelist[i].MaxSideLength = LineLength(i) / ((double) linelist[i].IsSelected);
        }
    }

    for(i=0; i < arclist.size(); i++)
    {
        if (ptlst[i+linelist.size()].t == 0)
        {
            // alter maxsidelength, but do it in such
            // a way that it carries only 4 significant
            // digits.  There's no use in carrying double
            // precision here, because it looks crappy
            // when you open up the arc segment to see
            // its properties.
            char kludge[32];

            arclist[i].MaxSideLength = arclist[i].ArcLength/((double) arclist[i].IsSelected);

            sprintf(kludge,"%.1e",arclist[i].MaxSideLength);

            sscanf(kludge,"%lf",&arclist[i].MaxSideLength);
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
    for(i=0;i<lineproplist.size();i++)
    {
        if ((lineproplist[i].BdryFormat==4) ||
            (lineproplist[i].BdryFormat==5)){
            pbc.BdryName=lineproplist[i].BdryName;
            pbc.BdryFormat=lineproplist[i].BdryFormat-4; // 0 for pbc, 1 for apbc
            pbclst.push_back(pbc);
        }
    }

    for(i=0;i<linelist.size();i++)
    {
        for(j=0;j<pbclst.size();j++)
        {
            if (pbclst[j].BdryName==linelist[i].BoundaryMarker)
            {
                // A pbc or apbc can only be applied to 2 segs
                // at a time.  If it is applied to multiple segs
                // at the same time, flag it and kick it out.
                if (pbclst[j].nseg==2)
                {
                    WarnMessage("An (anti)periodic BC is assigned to more than two segments");
                    Undo();  UnselectAll();
                    return -1;
                }
                pbclst[j].seg[pbclst[j].nseg]=i;
                pbclst[j].nseg++;
            }
        }
    }

    for(i=0;i<arclist.size();i++)
    {
        for(j=0;j<pbclst.size();j++)
        {
            if (pbclst[j].BdryName==arclist[i].BoundaryMarker)
            {
                // A pbc or apbc can only be applied to 2 arcs
                // at a time.  If it is applied to multiple arcs
                // at the same time, flag it and kick it out.
                if (pbclst[j].narc==2)
                {
                    WarnMessage("An (anti)periodic BC is assigned to more than two arcs");
                    Undo();  UnselectAll();
                    return -1;
                }
                pbclst[j].seg[pbclst[j].narc]=i;
                pbclst[j].narc++;
            }
        }
    }

    j=0;
    while(j<pbclst.size())
    {
        // check for a bc that is a mix of arcs and segments.
        // this is an error, and it should get flagged.
        if ((pbclst[j].nseg>0) && (pbclst[j].narc>0))
        {
            WarnMessage("Can't mix arcs and segments for (anti)periodic BCs");
            Undo();  UnselectAll();
            return -1;
        }


        // remove any periodic BC's that aren't actually in play
        if((pbclst[j].nseg<2) && (pbclst[j].narc<2)) pbclst.erase(pbclst.begin()+j);
        else j++;
    }

    for(j=0;j<pbclst.size();j++)
    {
        // check to see if adjoining entries are applied
        // to objects of compatible size/shape, and
        // reconcile meshing on the objects.

        // for segments:
        if(pbclst[j].nseg>0){

            // make sure that lines are pretty much the same length
            if(fabs(LineLength(pbclst[j].seg[0])
                   -LineLength(pbclst[j].seg[1]))>1.e-06)
            {
                WarnMessage("(anti)periodic BCs applied to dissimilar segments");
                Undo();  UnselectAll();
                return -1;
            }

            // make sure that both lines have the same spacing
            double len1,len2,len;
            len1=linelist[pbclst[j].seg[0]].MaxSideLength;
            len2=linelist[pbclst[j].seg[1]].MaxSideLength;

            if(len1<=0) len1=len2;
            if(len2<=0) len2=len1;
            len=(std::min)(len1,len2);

            linelist[pbclst[j].seg[0]].MaxSideLength=len;
            linelist[pbclst[j].seg[1]].MaxSideLength=len;
        }

        // for arc segments:
        if(pbclst[j].narc>0){

            // make sure that arcs are pretty much the
            // same arc length
            if(fabs(arclist[pbclst[j].seg[0]].ArcLength
                   -arclist[pbclst[j].seg[1]].ArcLength)>1.e-06)
            {
                WarnMessage("(anti)periodic BCs applied to dissimilar arc segments");
                Undo();  UnselectAll();
                return -1;
            }

            // make sure that both lines have the same spacing
            double len1,len2,len;
            len1=arclist[pbclst[j].seg[0]].MaxSideLength;
            len2=arclist[pbclst[j].seg[1]].MaxSideLength;

            len=(std::min)(len1,len2);

            arclist[pbclst[j].seg[0]].MaxSideLength=len;
            arclist[pbclst[j].seg[1]].MaxSideLength=len;
        }
    }

    // write out new poly and write out adjacent
    // boundary nodes in a separate .pbc file.

    // kludge things a bit and use IsSelected to denote
    // whether or not a line or arc has already been processed.
    UnselectAll();
    nodelst.clear();
    linelst.clear();

    // first, add in existing nodes
    for(n=0; n < nodelist.size(); n++)
    {
        nodelst.push_back(nodelist[n]);
    }

    for(n=0; n<pbclst.size(); n++)
    {
        if (pbclst[n].nseg != 0) // if this pbc is a line segment...
        {
            int s0,s1;
            CNode node0,node1;

            s0=pbclst[n].seg[0];
            s1=pbclst[n].seg[1];
            linelist[s0].IsSelected=1;
            linelist[s1].IsSelected=1;

            // make it so that first point on first line
            // maps to first point on second line...
            t = linelist[s1].n1;
            linelist[s1].n1 = linelist[s1].n0;
            linelist[s1].n0 = t;

            // store number of sub-segments in k
            if (linelist[s0].MaxSideLength == -1)
            {
                k = 1;
            }
            else{
                a0 = nodelist[linelist[s0].n0].CC();
                a1 = nodelist[linelist[s0].n1].CC();
                b0 = nodelist[linelist[s1].n0].CC();
                b1 = nodelist[linelist[s1].n1].CC();
                z = abs(a1-a0);
                k = (int) std::ceil(z/linelist[s0].MaxSideLength);
            }

            // add segment end points to the list;
            pt.x = linelist[s0].n0;
            pt.y = linelist[s1].n0;
            pt.t = pbclst[n].BdryFormat;
            ptlst.push_back(pt);
            pt.x = linelist[s0].n1;
            pt.y = linelist[s1].n1;
            pt.t = pbclst[n].BdryFormat;
            ptlst.push_back(pt);

            if (k == 1){
                // catch the case in which the line
                // doesn't get subdivided.
                linelst.push_back(linelist[s0]);
                linelst.push_back(linelist[s1]);
            }
            else{
                segm = linelist[s0];
                for(j=0; j<k; j++)
                {
                    a2=a0+(a1-a0)*((double) (j+1))/((double) k);
                    b2 = b0+(b1-b0)*((double) (j+1))/((double) k);
                    node0.x = a2.re; node0.y = a2.im;
                    node1.x = b2.re; node1.y = b2.im;
                    if(j==0){
                        l = nodelst.size();
                        nodelst.push_back(node0);
                        segm.n0 = linelist[s0].n0;
                        segm.n1 = l;
                        linelst.push_back(segm);
                        pt.x = l;

                        l = nodelst.size();
                        nodelst.push_back(node1);
                        segm.n0 = linelist[s1].n0;
                        segm.n1 = l;
                        linelst.push_back(segm);
                        pt.y = l;

                        pt.t = pbclst[n].BdryFormat;
                        ptlst.push_back(pt);
                    }
                    else if(j==(k-1))
                    {
                        // last subdivision--no ptlst
                        // entry associated with this one.
                        l = nodelst.size()-2;
                        segm.n0 = l;
                        segm.n1 = linelist[s0].n1;
                        linelst.push_back(segm);

                        l = nodelst.size()-1;
                        segm.n0 = l;
                        segm.n1 = linelist[s1].n1;
                        linelst.push_back(segm);
                    }
                    else{
                        l = nodelst.size();

                        nodelst.push_back(node0);
                        nodelst.push_back(node1);

                        segm.n0 = l-2;
                        segm.n1 = l;
                        linelst.push_back(segm);

                        segm.n0 = l-1;
                        segm.n1 = l+1;
                        linelst.push_back(segm);

                        pt.x = l;
                        pt.y = l+1;
                        pt.t = pbclst[n].BdryFormat;
                        ptlst.push_back(pt);
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

            s0 = pbclst[n].seg[0];
            s1 = pbclst[n].seg[1];
            arclist[s0].IsSelected = 1;
            arclist[s1].IsSelected = 1;

            k = (int) ceil(arclist[s0].ArcLength/arclist[s0].MaxSideLength);
            segm.BoundaryMarker = arclist[s0].BoundaryMarker;
            GetCircle(arclist[s0],c0,r0);
            GetCircle(arclist[s1],c1,r1);

            if (arclist[s0].NormalDirection ==0){
                bgn0 = nodelist[arclist[s0].n0].CC();
                d0 = exp(I*arclist[s0].ArcLength*PI/(((double) k)*180.));
                p0[0] = arclist[s0].n0;
                p0[1] = arclist[s0].n1;
            }
            else{
                bgn0 = nodelist[arclist[s0].n1].CC();
                d0 = exp(-I*arclist[s0].ArcLength*PI/(((double) k)*180.));
                p0[0] = arclist[s0].n1;
                p0[1] = arclist[s0].n0;
            }

            if (arclist[s1].NormalDirection!=0){
                bgn1 = nodelist[arclist[s1].n0].CC();
                d1 = exp(I*arclist[s1].ArcLength*PI/(((double) k)*180.));
                p1[0] = arclist[s1].n0;
                p1[1] = arclist[s1].n1;
            }
            else{
                bgn1 = nodelist[arclist[s1].n1].CC();
                d1 = exp(-I*arclist[s1].ArcLength*PI/(((double) k)*180.));
                p1[0] = arclist[s1].n1;
                p1[1] = arclist[s1].n0;
            }

            // add arc segment end points to the list;
            pt.x=p0[0]; pt.y=p1[0]; pt.t=pbclst[n].BdryFormat;
            ptlst.push_back(pt);
            pt.x=p0[1]; pt.y=p1[1]; pt.t=pbclst[n].BdryFormat;
            ptlst.push_back(pt);

            if (k==1){

                // catch the case in which the line
                // doesn't get subdivided.
                segm.n0=p0[0]; segm.n1=p0[1];
                linelst.push_back(segm);
                segm.n0=p1[0]; segm.n1=p1[1];
                linelst.push_back(segm);
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
                        nodelst.push_back(node0);
                        segm.n0=p0[0];
                        segm.n1=l;
                        linelst.push_back(segm);
                        pt.x=l;

                        l=nodelst.size();
                        nodelst.push_back(node1);
                        segm.n0=p1[0];
                        segm.n1=l;
                        linelst.push_back(segm);
                        pt.y=l;

                        pt.t=pbclst[n].BdryFormat;
                        ptlst.push_back(pt);
                    }
                    else if(j==(k-1))
                    {
                        // last subdivision--no ptlst
                        // entry associated with this one.
                        l=nodelst.size()-2;
                        segm.n0=l;
                        segm.n1=p0[1];
                        linelst.push_back(segm);

                        l=nodelst.size()-1;
                        segm.n0=l;
                        segm.n1=p1[1];
                        linelst.push_back(segm);
                    }
                    else{
                        l=nodelst.size();

                        nodelst.push_back(node0);
                        nodelst.push_back(node1);

                        segm.n0=l-2;
                        segm.n1=l;
                        linelst.push_back(segm);

                        segm.n0=l-1;
                        segm.n1=l+1;
                        linelst.push_back(segm);

                        pt.x=l;
                        pt.y=l+1;
                        pt.t=pbclst[n].BdryFormat;
                        ptlst.push_back(pt);
                    }
                }

            }
        }
    }

    // Then, do the rest of the lines and arcs in the
    // "normal" way and write .poly file.

    // discretize input segments
    for(i=0;i<linelist.size();i++)
    if(linelist[i].IsSelected==0){

        a0.Set(nodelist[linelist[i].n0].x,nodelist[linelist[i].n0].y);
        a1.Set(nodelist[linelist[i].n1].x,nodelist[linelist[i].n1].y);

        if (linelist[i].MaxSideLength==-1) k=1;
        else{
            z=abs(a1-a0);
            k=(int) ceil(z/linelist[i].MaxSideLength);
        }

        segm=linelist[i];
        if (k==1) // default condition where discretization on line is not specified
        {
            if (abs(a1-a0)<(3.*dL)) linelst.push_back(segm); // line is too short to add extra points
            else{
                // add extra points at a distance of dL from the ends of the line.
                // this forces Triangle to finely mesh near corners
                for(j=0;j<3;j++)
                {
                    if(j==0)
                    {
                        a2=a0+dL*(a1-a0)/abs(a1-a0);
                        node.x=a2.re; node.y=a2.im;
                        l=(int) nodelst.size();
                        nodelst.push_back(node);
                        segm.n0=linelist[i].n0;
                        segm.n1=l;
                        linelst.push_back(segm);
                    }

                    if(j==1)
                    {
                        a2=a1+dL*(a0-a1)/abs(a1-a0);
                        node.x=a2.re; node.y=a2.im;
                        l=(int) nodelst.size();
                        nodelst.push_back(node);
                        segm.n0=l-1;
                        segm.n1=l;
                        linelst.push_back(segm);
                    }

                    if(j==2)
                    {
                        l=(int) nodelst.size()-1;
                        segm.n0=l;
                        segm.n1=linelist[i].n1;
                        linelst.push_back(segm);
                    }

                }
            }
        }
        else{
            for(j=0;j<k;j++)
            {
                a2=a0+(a1-a0)*((double) (j+1))/((double) k);
                node.x=a2.re; node.y=a2.im;
                if(j==0){
                    l=nodelst.size();
                    nodelst.push_back(node);
                    segm.n0=linelist[i].n0;
                    segm.n1=l;
                    linelst.push_back(segm);
                }
                else if(j==(k-1))
                {
                    l=nodelst.size()-1;
                    segm.n0=l;
                    segm.n1=linelist[i].n1;
                    linelst.push_back(segm);
                }
                else{
                    l=nodelst.size();
                    nodelst.push_back(node);
                    segm.n0=l-1;
                    segm.n1=l;
                    linelst.push_back(segm);
                }
            }
        }
    }

    // discretize input arc segments
    for(i=0;i<arclist.size();i++)
    {
        if(arclist[i].IsSelected==0)
        {
            a2.Set(nodelist[arclist[i].n0].x,nodelist[arclist[i].n0].y);
            k=(int) ceil(arclist[i].ArcLength/arclist[i].MaxSideLength);
            segm.BoundaryMarker=arclist[i].BoundaryMarker;
            GetCircle(arclist[i],c,R);
            a1=exp(I*arclist[i].ArcLength*PI/(((double) k)*180.));

            if(k==1){
                segm.n0=arclist[i].n0;
                segm.n1=arclist[i].n1;
                linelst.push_back(segm);
            }
            else for(j=0;j<k;j++)
            {
                a2=(a2-c)*a1+c;
                node.x=a2.re; node.y=a2.im;
                if(j==0){
                    l=nodelst.size();
                    nodelst.push_back(node);
                    segm.n0=arclist[i].n0;
                    segm.n1=l;
                    linelst.push_back(segm);
                }
                else if(j==(k-1))
                {
                    l=nodelst.size()-1;
                    segm.n0=l;
                    segm.n1=arclist[i].n1;
                    linelst.push_back(segm);
                }
                else{
                    l=nodelst.size();
                    nodelst.push_back(node);
                    segm.n0=l-1;
                    segm.n1=l;
                    linelst.push_back(segm);
                }
            }
        }
    }


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
//                if(nodeproplist[j].PointName==nodelst[i].BoundaryMarker) t=j+2;
//        fprintf(fp,"%i    %.17g    %.17g    %i\n",i,nodelst[i].x,nodelst[i].y,t);
//    }
//
//    // write out segment list
//    fprintf(fp,"%i    1\n",linelst.size());
//    for(i=0;i<linelst.size();i++)
//    {
//        for(j=0,t=0;j<lineproplist.size();j++)
//                if(lineproplist[j].BdryName==linelst[i].BoundaryMarker) t=-(j+2);
//        fprintf(fp,"%i    %i    %i    %i\n",i,linelst[i].n0,linelst[i].n1,t);
//    }

    // write out list of holes;
    for(i=0,j=0;i<blocklist.size();i++)
    {
        if(blocklist[i].BlockType=="<No Mesh>")
        {
            j++;
        }
    }

//    fprintf(fp,"%i\n",j);

    Nholes = j;

    NRegionalAttribs = blocklist.size() - Nholes;

//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i].BlockType=="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g\n",k,blocklist[i].x,blocklist[i].y);
//            k++;
//        }
//
//    // write out regional attributes
//    fprintf(fp,"%i\n",blocklist.size()-j);
//
//    for(i=0,k=0;i<blocklist.size();i++)
//        if(blocklist[i].BlockType!="<No Mesh>")
//        {
//            fprintf(fp,"%i    %.17g    %.17g    ",k,blocklist[i].x,blocklist[i].y);
//            fprintf(fp,"%i    ",k+1);
//            if (blocklist[i].MaxArea>0)
//                fprintf(fp,"%.17g\n",blocklist[i].MaxArea);
//            else fprintf(fp,"-1\n");
//            k++;
//        }
//    fclose(fp);

    // Make sure to prune out any duplications in the ptlst
    for(k=0;k<ptlst.size();k++) ptlst[k].Order();
    k=0;
    while((k+1) < ptlst.size())
    {
        j=k+1;
        while(j < ptlst.size())
        {
            if((ptlst[k].x==ptlst[j].x) && (ptlst[k].y==ptlst[j].y))
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
            if(ptlst[k].x==ptlst[j].x) n=true;
            if(ptlst[k].y==ptlst[j].y) n=true;
            if(ptlst[k].x==ptlst[j].y) n=true;
            if(ptlst[k].y==ptlst[j].x) n=true;
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
        Undo();  UnselectAll();
        return -1;
    }
    fprintf(fp,"%i\n", (int) ptlst.size());
    for(k=0;k<ptlst.size();k++)
        fprintf(fp,"%i    %i    %i    %i\n",k,ptlst[k].x,ptlst[k].y,ptlst[k].t);
    fclose(fp);

    // call triangle with -Y flag.

    in.numberofpoints = nodelst.size();

    in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
    if (in.pointlist == NULL) {
        return -1;
    }

    for(i=0; i < (unsigned int)(2 * in.numberofpoints-1); i = i + 2)
    {
        in.pointlist[i] = nodelst[i/2].x;
        in.pointlist[i+1] = nodelst[i/2].y;
    }

    in.numberofpointattributes = 0;

    in.pointattributelist = (REAL *) NULL;

    // Initialise the pointmarkerlist
    in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));
    if (in.pointmarkerlist == NULL) {
        return -1;
    }

    t = 0;
    // write out node marker list
    for(i=0;i<nodelst.size ();i++)
    {
        for(j=0,t=0; j < nodeproplist.size (); j++)
                if(nodeproplist[j].PointName == nodelst[i].BoundaryMarker) t = j + 2;

        if (filetype == F_TYPE_HEATFLOW)
        {
            // include conductor number;
            for(j=0; j < circproplist.size (); j++)
                if(circproplist[j].CircName == nodelst[i].InConductor) t += ((j+1) * 0x10000);
        }

        in.pointmarkerlist[i] = t;
    }

    in.numberofsegments = linelst.size();

    // Initialise the segmentlist
    in.segmentlist = (int *) malloc(2 * in.numberofsegments * sizeof(int));
    if (in.segmentlist == NULL) {
        return -1;
    }

    // Initialise the segmentmarkerlist
    in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));
    if (in.segmentmarkerlist == NULL) {
        return -1;
    }

    // build the segmentlist
    for(i=0; i < (unsigned int)(2*in.numberofsegments - 1); i = i + 2)
    {
            in.segmentlist[i] = linelst[i/2].n0;

            in.segmentlist[i+1] = linelst[i/2].n1;

            //PRINTF("i: %i, segmentlist[i]: %i, segmentlist[i+1]: %i\n", i, in.segmentlist[i], in.segmentlist[i+1]);
    }

    // now build the segment marker list
    t = 0;

    // construct the segment list
    for(i=0; i < linelst.size (); i++)
    {
        for(j=0,t=0; j < lineproplist.size (); j++)
                if(lineproplist[j].BdryName==linelst[i].BoundaryMarker) t = -(j+2);

        if (filetype == F_TYPE_HEATFLOW)
        {
            // include conductor number;
            for(j=0; j < circproplist.size (); j++)
                if(circproplist[j].CircName == linelst[i].InConductor) t -= ((j+1) * 0x10000);
        }

        in.segmentmarkerlist[i] = t;
    }

    in.numberofholes = Nholes;
    in.holelist = (REAL *) malloc(in.numberofholes * 2 * sizeof(REAL));
    if (in.holelist == NULL) {
        return -1;
    }

    // Construct the holes array
    for(i=0, k=0; i < blocklist.size(); i++)
    {
        // we search through the block list looking for blocks that have
        // the tag <no mesh>
        if(blocklist[i].BlockType == "<No Mesh>")
        {
            in.holelist[k] = blocklist[i].x;
            in.holelist[k+1] = blocklist[i].y;
            k = k + 2;
        }
    }


    in.numberofregions = NRegionalAttribs;
    in.regionlist = (REAL *) malloc(in.numberofregions * 4 * sizeof(REAL));
    if (in.regionlist == NULL) {
        WarnMessage("Error: Memory unable to be allocated.\n");
        return -1;
    }

    for(i = 0, j = 0, k = 0; i < blocklist.size(); i++)
    {
        if(blocklist[i].BlockType != "<No Mesh>")
        {

            in.regionlist[j] = blocklist[i].x;
            in.regionlist[j+1] = blocklist[i].y;
            in.regionlist[j+2] = k + 1; // Regional attribute (for whole mesh).

            if (blocklist[i].MaxArea>0 && (blocklist[i].MaxArea<DefaultMeshSize))
            {
                in.regionlist[j+3] = blocklist[i].MaxArea;  // Area constraint
            }
            else
            {
                in.regionlist[j+3] = DefaultMeshSize;
            }

            j = j + 4;
            k++;
        }
    }

    // Finally, we have no triangle area constraints so initialize to null
    in.trianglearealist = (REAL *) NULL;

    /* Make necessary initializations so that Triangle can return a */
    /* triangulation in `out' */

    out.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
    /* Not needed if -N switch used or number of point attributes is zero: */
    out.pointattributelist = (REAL *) NULL;
    out.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */
    out.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
    /* Not needed if -E switch used or number of triangle attributes is zero: */
    out.triangleattributelist = (REAL *) NULL;
    /* No triangle area constraints */
    out.trianglearealist = (REAL *) NULL;
    out.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */
    /* Needed only if segments are output (-p or -c) and -P not used: */
    out.segmentlist = (int *) NULL;
    /* Needed only if segments are output (-p or -c) and -P and -B not used: */
    out.segmentmarkerlist = (int *) NULL;
    out.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
    out.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

    rootname = pn.substr(0,pn.find_last_of('.'));

    if (Verbose)
    {
        sprintf(CommandLine,"-pPq%feAazIY", MinAngle);
    }
    else
    {
        sprintf(CommandLine,"-pPq%feAazQIY", MinAngle);
    }

    tristatus = triangulate(CommandLine, &in, &out, (struct triangulateio *) NULL, this->TriMessage);

    // copy the exit status status of the triangle library from the global variable, eueghh.
    //trilibrary_exit_code;
    if (tristatus != 0)
    {
        if (in.pointlist != NULL) { free(in.pointlist); }
        if (in.pointattributelist != NULL) { free(in.pointattributelist); }
        if (in.pointmarkerlist != NULL) { free(in.pointmarkerlist); }
        if (in.regionlist != NULL) { free(in.regionlist); }
        if (in.segmentlist != NULL) { free(in.segmentlist); }
        if (in.segmentmarkerlist != NULL) { free(in.segmentmarkerlist); }
        if (in.holelist != NULL) { free(in.holelist); }

        if (out.pointlist != NULL) { free(out.pointlist); }
        if (out.pointattributelist != NULL) { free(out.pointattributelist); }
        if (out.pointmarkerlist != NULL) { free(out.pointmarkerlist); }
        if (out.trianglelist != NULL) { free(out.trianglelist); }
        if (out.triangleattributelist != NULL) { free(out.triangleattributelist); }
        if (out.trianglearealist != NULL) { free(out.trianglearealist); }
        if (out.neighborlist != NULL) { free(out.neighborlist); }
        if (out.segmentlist != NULL) { free(out.segmentlist); }
        if (out.segmentmarkerlist != NULL) { free(out.segmentmarkerlist); }
        if (out.edgelist != NULL) { free(out.edgelist); }
        if (out.edgemarkerlist != NULL) { free(out.edgemarkerlist); }

        return tristatus;
    }

    WriteTriangulationFiles(out, PathName);

    // now deallocate memory where necessary
    if (in.pointlist != NULL) { free(in.pointlist); }
    if (in.pointattributelist != NULL) { free(in.pointattributelist); }
    if (in.pointmarkerlist != NULL) { free(in.pointmarkerlist); }
    if (in.regionlist != NULL) { free(in.regionlist); }
    if (in.segmentlist != NULL) { free(in.segmentlist); }
    if (in.segmentmarkerlist != NULL) { free(in.segmentmarkerlist); }
    if (in.holelist != NULL) { free(in.holelist); }

    if (out.pointlist != NULL) { free(out.pointlist); }
    if (out.pointattributelist != NULL) { free(out.pointattributelist); }
    if (out.pointmarkerlist != NULL) { free(out.pointmarkerlist); }
    if (out.trianglelist != NULL) { free(out.trianglelist); }
    if (out.triangleattributelist != NULL) { free(out.triangleattributelist); }
    if (out.trianglearealist != NULL) { free(out.trianglearealist); }
    if (out.neighborlist != NULL) { free(out.neighborlist); }
    if (out.segmentlist != NULL) { free(out.segmentlist); }
    if (out.segmentmarkerlist != NULL) { free(out.segmentmarkerlist); }
    if (out.edgelist != NULL) { free(out.edgelist); }
    if (out.edgemarkerlist != NULL) { free(out.edgemarkerlist); }

    UnselectAll();

    // Now restore boundary segment discretizations that have
    // been mucked up in the process...
    for(i=0;i<linelist.size();i++)
        linelist[i]=undolinelist[i];

    // and save the latest version of the document to make sure
    // any changes to arc discretization get propagated into
    // the solution description....
    SaveFEMFile(pn);

    return 0;
}
