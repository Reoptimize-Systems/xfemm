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

// fmesher.cpp : implementation of FMesher Class
//

#include "fparse.h"
#include "fmesher.h"
#include "IntPoint.h"
#include "triangle.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

//extern void *pFemmeDoc;
//extern lua_State *lua;
//extern CLuaConsoleDlg *LuaConsole;
//extern bool bLinehook;

//extern void *triangulate;

using namespace std;
using namespace femm;
using namespace fmesher;

namespace {
// set up some default behaviors
constexpr double DEFAULT_MINANGLE=30.;
}

FMesher::FMesher()
{
    // initialise the warning message function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;

    TriMessage = NULL;

    Verbose = true;

    // initialize the problem data structures
    // and default behaviour etc.
    Initialize(femm::FileType::Unknown);

}


FMesher::FMesher(std::shared_ptr<FemmProblem> p)
    : d_EditMode(EditMode::Invalid)
    , problem(p)
    , Verbose(true)
    , WarnMessage(&PrintWarningMsg)
    , TriMessage(nullptr)
{
}

bool FMesher::Initialize(femm::FileType t)
{

    // fire up lua
    //initalise_lua();

    // clear out all current lines, nodes, and block labels
    undonodelist.clear ();
    undolinelist.clear ();
    undoarclist.clear ();
    undolabellist.clear ();
    meshnode.clear ();
    meshline.clear ();
    greymeshline.clear ();
    probdescstrings.clear ();

    problem = std::make_shared<femm::FemmProblem>(t);
    // set problem attributes to generic ones;
    problem->MinAngle = DEFAULT_MINANGLE;

    return true;
}


void FMesher::UnselectAll()
{
    unsigned int i;

    for(i=0; i < problem->nodelist.size(); i++) problem->nodelist[i]->IsSelected=0;
    for(i=0; i < problem->linelist.size(); i++) problem->linelist[i]->IsSelected=0;
    for(i=0; i < problem->labellist.size(); i++) problem->labellist[i]->IsSelected=0;
    for(i=0; i < problem->arclist.size(); i++) problem->arclist[i]->IsSelected=0;
}


void FMesher::GetCircle(CArcSegment &arc, CComplex &c, double &R)
{
    CComplex a0,a1,t;
    double d,tta;

    // construct the coordinates of the two points on the circle
    a0.Set(problem->nodelist[arc.n0]->x,problem->nodelist[arc.n0]->y);
    a1.Set(problem->nodelist[arc.n1]->x,problem->nodelist[arc.n1]->y);

    // calculate distance between arc endpoints
    d = abs(a1 - a0);

    // figure out what the radius of the circle is...

    // get unit vector pointing from a0 to a1
    t = (a1 - a0) / d;

    // convert swept angle from degrees to radians
    tta = arc.ArcLength * PI / 180.;

    // the radius is half the chord length divided by sin of
    // half the swept angle (finding the side length of a
    // triangle formed by the two points and the centre)
    R = d / (2.*sin(tta/2.));

    // center of the arc segment's circle
    c = a0 + (d/2. + I * sqrt(R*R - d*d / 4.)) * t;

}


int FMesher::GetLineArcIntersection(CSegment &seg, CArcSegment &arc, CComplex *p)
{
    CComplex p0,p1,a0,a1,t,v,c;
    double d,l,R,z,tta;
    int i=0;

    p0.Set(problem->nodelist[seg.n0]->x,problem->nodelist[seg.n0]->y);
    p1.Set(problem->nodelist[seg.n1]->x,problem->nodelist[seg.n1]->y);
    a0.Set(problem->nodelist[arc.n0]->x,problem->nodelist[arc.n0]->y);
    a1.Set(problem->nodelist[arc.n1]->x,problem->nodelist[arc.n1]->y);
    d=abs(a1-a0);			// distance between arc endpoints

    // figure out what the radius of the circle is...
    t = (a1-a0) / d;
    tta = arc.ArcLength * PI / 180.;
    R = d / ( 2. * std::sin(tta / 2.));
    c = a0 + (d/2. + I*sqrt(R*R-d*d/4.))*t; // center of the arc segment's circle...

    // figure out the distance between line and circle's center;
    d=abs(p1-p0);
    t=(p1-p0)/d;
    v=(c-p0)/t;
    if (fabs(Im(v))>R) return 0;
    l=sqrt( R*R - Im(v)*Im(v));	// Im(v) is distance between line and center...

    if ((l/R) < 1.e-05) 		// case where line is very close to a tangent;
    {
        p[i]=p0 + Re(v)*t;		// make it be a tangent.
        R=Re((p[i]-p0)/t);
        z=arg((p[i]-c)/(a0-c));
        if ((R>0) && (R<d) && (z>0.) && (z<tta)) i++;
        return i;
    }

    p[i]=p0 + (Re(v)+l)*t;		// first possible intersection;
    R=Re((p[i]-p0)/t);
    z=arg((p[i]-c)/(a0-c));
    if ((R>0) && (R<d) && (z>0.) && (z<tta)) i++;

    p[i]=p0 + (Re(v)-l)*t;		// second possible intersection
    R=Re((p[i]-p0)/t);
    z=arg((p[i]-c)/(a0-c));
    if ((R>0) && (R<d) && (z>0.) && (z<tta)) i++;

    // returns the number of valid intersections found;
    // intersections are returned in the array p[];
    return i;

}

int FMesher::GetArcArcIntersection(CArcSegment &arc0, CArcSegment &arc1, CComplex *p)
{
    CComplex a0,a1,t,c0,c1;
    double d,l,R0,R1,z0,z1,c,tta0,tta1;
    int i=0;

    a0.Set(problem->nodelist[arc0.n0]->x,problem->nodelist[arc0.n0]->y);
    a1.Set(problem->nodelist[arc1.n0]->x,problem->nodelist[arc1.n0]->y);

    GetCircle(arc1,c1,R1);
    GetCircle(arc0,c0,R0);

    d=abs(c1-c0);			// distance between centers

    if ((d>R0+R1) || (d<1.e-08)) return 0;
    // directly eliminate case where there can't
    // be any crossings....

    l=sqrt((R0+R1-d)*(d+R0-R1)*(d-R0+R1)*(d+R0+R1))/(2.*d);
    c=1.+(R0/d)*(R0/d)-(R1/d)*(R1/d);
    t=(c1-c0)/d;
    tta0=arc0.ArcLength*PI/180;
    tta1=arc1.ArcLength*PI/180;

    p[i]=c0 + (c*d/2.+ I*l)*t;		// first possible intersection;
    z0=arg((p[i]-c0)/(a0-c0));
    z1=arg((p[i]-c1)/(a1-c1));
    if ((z0>0.) && (z0<tta0) && (z1>0.) && (z1<tta1)) i++;

    if(fabs(d-R0+R1)/(R0+R1)< 1.e-05)
    {
        p[i]=c0+ c*d*t/2.;
        return i;
    }

    p[i]=c0 + (c*d/2.-I*l)*t;		// second possible intersection
    z0=arg((p[i]-c0)/(a0-c0));
    z1=arg((p[i]-c1)/(a1-c1));
    if ((z0>0.) && (z0<tta0) && (z1>0.) && (z1<tta1)) i++;

    // returns the number of valid intersections found;
    // intersections are returned in the array p[];
    return i;
}


int FMesher::ClosestNode(double x, double y)
{
    unsigned int i,j;
    double d0,d1;

    if(problem->nodelist.size()==0)
    {
        return -1;
    }

    j=0;
    d0=problem->nodelist[0]->GetDistance(x,y);
    for(i=0; i<problem->nodelist.size(); i++)
    {
        d1=problem->nodelist[i]->GetDistance(x,y);
        if(d1<d0)
        {
            d0=d1;
            j=i;
        }
    }

    return j;
}

int FMesher::ClosestBlockLabel(double x, double y)
{
    unsigned int i,j;
    double d0,d1;

    if(problem->labellist.size()==0)
    {
        return -1;
    }

    j=0;
    d0=problem->labellist[0]->GetDistance(x,y);
    for(i=0; i<problem->labellist.size(); i++)
    {
        d1=problem->labellist[i]->GetDistance(x,y);
        if(d1<d0)
        {
            d0=d1;
            j=i;
        }
    }

    return j;
}

double FMesher::ShortestDistanceFromArc(CComplex p, CArcSegment &arc)
{
    double R,d,l,z;
    CComplex a0,a1,c,t;

    a0.Set(problem->nodelist[arc.n0]->x,problem->nodelist[arc.n0]->y);
    a1.Set(problem->nodelist[arc.n1]->x,problem->nodelist[arc.n1]->y);
    GetCircle(arc,c,R);
    d=abs(p-c);

    if(d==0) return R;

    t=(p-c)/d;
    l=abs(p-c-R*t);
    z=arg(t/(a0-c))*180/PI;
    if ((z>0) && (z<arc.ArcLength)) return l;

    z=abs(p-a0);
    l=abs(p-a1);
    if(z<l) return z;
    return l;
}


double FMesher::ShortestDistance(double p, double q, int segm)
{
    double t,x[3],y[3];

    x[0]=problem->nodelist[problem->linelist[segm]->n0]->x;
    y[0]=problem->nodelist[problem->linelist[segm]->n0]->y;
    x[1]=problem->nodelist[problem->linelist[segm]->n1]->x;
    y[1]=problem->nodelist[problem->linelist[segm]->n1]->y;

    t=((p-x[0])*(x[1]-x[0]) + (q-y[0])*(y[1]-y[0]))/
      ((x[1]-x[0])*(x[1]-x[0]) + (y[1]-y[0])*(y[1]-y[0]));

    if (t>1.) t=1.;
    if (t<0.) t=0.;

    x[2]=x[0]+t*(x[1]-x[0]);
    y[2]=y[0]+t*(y[1]-y[0]);

    return sqrt((p-x[2])*(p-x[2]) + (q-y[2])*(q-y[2]));
}

void FMesher::EnforcePSLG(double tol)
{
    std::vector< std::unique_ptr<CNode>> newnodelist;
    std::vector< std::unique_ptr<CSegment>> newlinelist;
    std::vector< std::unique_ptr<CArcSegment>> newarclist;
    std::vector< std::unique_ptr<CBlockLabel>> newlabellist;

    // save existing objects into new*list:
    newnodelist.swap(problem->nodelist);
    newlinelist.swap(problem->linelist);
    newarclist.swap(problem->arclist);
    newlabellist.swap(problem->labellist);

    // find out what tolerance is so that there are not nodes right on
    // top of each other;
    double d = tol;
    if (tol==0)
    {
        if (newnodelist.size()==1)
            d = 1.e-08;
        else
        {
            CComplex p0 = newnodelist[0]->CC();
            CComplex p1 = p0;
            for (int i=1; i<(int)newnodelist.size(); i++)
            {
                if(newnodelist[i]->x<p0.re) p0.re = newnodelist[i]->x;
                if(newnodelist[i]->x>p1.re) p1.re = newnodelist[i]->x;
                if(newnodelist[i]->y<p0.im) p0.im = newnodelist[i]->y;
                if(newnodelist[i]->y>p1.im) p1.im = newnodelist[i]->y;
            }
            d = abs(p1-p0)*CLOSE_ENOUGH;
        }
    }

    // put in all of the nodes;
    for (auto &node: newnodelist)
    {
        AddNode(std::move(node), d);
    }

    // put in all of the lines;
    for (const auto &line: newlinelist)
    {
        // Note(ZaJ): the original code uses a variant of AddSegment that uses coordinates instead of node indices.
        // since I don't see any advantage to that, I just use the regular AddSegment method:
        //CComplex p0 (newnodelist[line->n0]->x, newnodelist[line->n0]->y);
        //CComplex p1 (newnodelist[line->n1]->x, newnodelist[line->n1]->y);
        //AddSegment(p0,p1,newlinelist[i]);

        // using the raw pointer is ok here, because AddSegment creates a copy anyways
        AddSegment(line->n0, line->n1, line.get(), d);
    }

    // put in all of the arcs;
    for (const auto &arc: newarclist)
    {
        // Note(ZaJ): the original code uses a variant of AddArcSegment that uses coordinates instead of the node indices
        // since I don't see any advantage to that, I just use the regular AddArcSegment method:
        //CComplex p0 (newnodelist[arc->n0]->x, newnodelist[arc->n0]->y);
        //CComplex p1 (newnodelist[arc->n1]->x, newnodelist[arc->n1]->y);
        //AddArcSegment(p0,p1,newarclist[i]);

        // using the raw pointer is ok here, because AddArcSegment creates a copy anyways
        AddArcSegment(*arc.get(), d);
    }

    // put in all of the block labels;
    for (auto &label: newlabellist)
    {
        AddBlockLabel(std::move(label), d);
    }

    UnselectAll();
    return;
}

int FMesher::ClosestSegment(double x, double y)
{
    double d0,d1;
    unsigned int i,j;

    if(problem->linelist.size()==0)
    {
        return -1;
    }

    j=0;
    d0=ShortestDistance(x,y,0);
    for(i=0; i<problem->linelist.size(); i++)
    {
        d1=ShortestDistance(x,y,i);
        if(d1<d0)
        {
            d0=d1;
            j=i;
        }
    }

    return j;
}

int FMesher::ClosestArcSegment(double x, double y)
{
    double d0,d1;
    unsigned int i,j;

    if(problem->arclist.size()==0)
    {
        return -1;
    }

    j=0;
    d0=ShortestDistanceFromArc(CComplex(x,y),*(problem->arclist[0]));
    for(i=0; i<problem->arclist.size(); i++)
    {
        d1=ShortestDistanceFromArc(CComplex(x,y),*(problem->arclist[i]));
        if(d1<d0)
        {
            d0=d1;
            j=i;
        }
    }

    return j;
}

bool FMesher::GetIntersection(int n0, int n1, int segm, double *xi, double *yi)
// prospective line specified by n0,n1;
// segment specified by segm;
// coordinates of the intersection returned in xi,yi
{
    CComplex p0,p1,q0,q1;
    double ee,x,z;

    // Check to see if the two lines have a common endpoint
    // If they do, there can be no other intersection...
    if (n0==problem->linelist[segm]->n0) return false;
    if (n0==problem->linelist[segm]->n1) return false;
    if (n1==problem->linelist[segm]->n0) return false;
    if (n1==problem->linelist[segm]->n1) return false;

    // Get a definition of "real small" based on the lengths
    // of the lines of interest;
    p0=problem->nodelist[problem->linelist[segm]->n0]->CC();
    p1=problem->nodelist[problem->linelist[segm]->n1]->CC();
    q0=problem->nodelist[n0]->CC();
    q1=problem->nodelist[n1]->CC();
    ee = (std::min)(abs(p1-p0),abs(q1-q0))*1.0e-8;

    // Rotate and scale the prospective line
    q0=(q0-p0)/(p1-p0);
    q1=(q1-p0)/(p1-p0);

    // Check for cases where there is obviously no intersection
    if ((Re(q0)<=0.) && (Re(q1)<=0.)) return false;
    if ((Re(q0)>=1.) && (Re(q1)>=1.)) return false;
    if ((Im(q0)<=0.) && (Im(q1)<=0.)) return false;
    if ((Im(q0)>=0.) && (Im(q1)>=0.)) return false;

    // compute intersection
    z=Im(q0)/Im(q0-q1);

    // check to see if the line segments intersect at a point sufficiently
    // far from the segment endpoints....
    x=Re((1.0 - z)*q0 + z*q1);
    if((x < ee) || (x > (1.0 - ee))) return false;

    // return resulting intersection point
    p0 = (1.0 - z)*problem->nodelist[n0]->CC() + z*problem->nodelist[n1]->CC();
    *xi=Re(p0);
    *yi=Im(p0);

    return true;
}


femm::FileType FMesher::GetFileType (string PathName)
{
    // find the position of the last '.' in the string
    size_t dotpos = PathName.rfind ('.');

    if (dotpos == string::npos)
    {
        // no '.' found
        return femm::FileType::Unknown;
    }

    // compare different file extensions and return the appropriate string
    if ( PathName.compare (dotpos, string::npos, ".fem") == 0 )
    {
        return femm::FileType::MagneticsFile;
    }
    else if ( PathName.compare (dotpos, string::npos, ".feh") == 0 )
    {
        return femm::FileType::HeatFlowFile;
    }
    else if ( PathName.compare(dotpos, string::npos, ".fee") == 0)
    {
        return femm::FileType::ElectrostaticsFile;
    }
    else
    {
        return femm::FileType::Unknown;
    }

}

void FMesher::addFileStr (char *s)
{
    probdescstrings.push_back (string (s));
}

bool FMesher::SaveFEMFile(string PathName)
{
    FILE *fp;
    unsigned int i, j;
    int k,t;
    string s;

    // check to see if we are ready to write a datafile;
    if ((fp = fopen(PathName.c_str(),"wt"))==NULL)
    {
        WarnMessage("Couldn't write to specified file.\nPerhaps the file is write-protected?");
        return false;
    }

    // echo the start of the input file
    for (i = 0; i < probdescstrings.size (); i++)
    {
        fprintf(fp,"%s", probdescstrings[i].c_str () );
    }

    // write out node list
    fprintf(fp,"[NumPoints] = %i\n", (int) problem->nodelist.size());
    for(i=0; i<problem->nodelist.size(); i++)
    {
        for(j=0,t=0; j<problem->nodeproplist.size(); j++)
            if(problem->nodeproplist[j]->PointName==problem->nodelist[i]->BoundaryMarkerName) t=j+1;
        fprintf(fp,"%.17g\t%.17g\t%i\t%i",problem->nodelist[i]->x,problem->nodelist[i]->y,t,
                problem->nodelist[i]->InGroup);

        if (problem->filetype == femm::FileType::HeatFlowFile
                || problem->filetype == femm::FileType::ElectrostaticsFile )
        {
            // find and write number of conductor property group
            for (j=0,t=0; j<problem->circproplist.size (); j++)
                if (problem->circproplist[j]->CircName==problem->nodelist[i]->InConductorName) t=j+1;

            fprintf(fp,"\t%i",t);
        }

        fprintf(fp,"\n");
    }

    // write out segment list
    fprintf(fp,"[NumSegments] = %i\n", (int) problem->linelist.size());
    for(i=0; i<problem->linelist.size(); i++)
    {
        for(j=0,t=0; j<problem->lineproplist.size(); j++)
            if(problem->lineproplist[j]->BdryName==problem->linelist[i]->BoundaryMarkerName) t=j+1;

        fprintf(fp,"%i\t%i\t",problem->linelist[i]->n0,problem->linelist[i]->n1);

        if(problem->linelist[i]->MaxSideLength<0)
        {
            fprintf(fp,"-1\t");
        }
        else
        {
            fprintf(fp,"%.17g\t",problem->linelist[i]->MaxSideLength);
        }

        fprintf(fp,"%i\t%i\t%i",t,problem->linelist[i]->Hidden,problem->linelist[i]->InGroup);

        if (problem->filetype == femm::FileType::HeatFlowFile
                || problem->filetype == femm::FileType::ElectrostaticsFile )
        {
            // find and write number of conductor property group
            for(j=0,t=0;j<problem->circproplist.size ();j++)
            {
                if(problem->circproplist[j]->CircName==problem->linelist[i]->InConductorName) t = j + 1;
            }
            fprintf(fp,"\t%i",t);
        }

        fprintf(fp,"\n");
    }

    // write out arc segment list
    fprintf(fp,"[NumArcSegments] = %i\n", (int) problem->arclist.size());
    for(i=0; i<problem->arclist.size(); i++)
    {
        for(j=0,t=0; j<problem->lineproplist.size(); j++)
            if(problem->lineproplist[j]->BdryName==problem->arclist[i]->BoundaryMarkerName) t=j+1;
        fprintf(fp,"%i\t%i\t%.17g\t%.17g\t%i\t%i\t%i",problem->arclist[i]->n0,problem->arclist[i]->n1,
                problem->arclist[i]->ArcLength,problem->arclist[i]->MaxSideLength,t,
                problem->arclist[i]->Hidden,problem->arclist[i]->InGroup);

        if (problem->filetype == femm::FileType::HeatFlowFile
                || problem->filetype == femm::FileType::ElectrostaticsFile )
        {
            // find and write number of conductor property group
            for(j=0,t=0;j<problem->circproplist.size ();j++)
                if(problem->circproplist[j]->CircName==problem->arclist[i]->InConductorName) t=j+1;
            fprintf(fp,"\t%i",t);
        }
        fprintf(fp,"\n");
    }

    // write out list of holes;
    for(i=0,j=0; i<problem->labellist.size(); i++)
    {
        if(problem->labellist[i]->BlockTypeName=="<No Mesh>")
        {
            j++;
        }
    }

    fprintf(fp,"[NumHoles] = %i\n",j);
    for(i=0,k=0; i<problem->labellist.size(); i++)
    {
        if(problem->labellist[i]->BlockTypeName=="<No Mesh>")
        {
            fprintf(fp,"%.17g\t%.17g\t%i\n",problem->labellist[i]->x,problem->labellist[i]->y,
                    problem->labellist[i]->InGroup);
            k++;
        }
    }

    fclose(fp);

    return true;
}

bool FMesher::LoadMesh(string PathName)
{
    int i,j,k,q,nl;
    string pathname,rootname,infile;
    FILE *fp;
    char s[1024];

    // clear out the old mesh...
    meshnode.clear();
    meshline.clear();
    greymeshline.clear();

    pathname = PathName;
    if (pathname.length()==0)
    {
        WarnMessage("No mesh to display");
        return false;
    }

    rootname = pathname.substr(0,pathname.find_last_of('.'));

    //read meshnodes;
    infile = rootname + ".node";
    if((fp=fopen(infile.c_str(),"rt"))==NULL)
    {
        WarnMessage("No mesh to display");
        return false;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    meshnode.resize(k);
    CNode node;
    for(i=0; i<k; i++)
    {
        fgets(s,1024,fp);
        sscanf(s,"%i\t%lf\t%lf",&j,&node.x,&node.y);
        meshnode[i] = std::make_unique<CNode>(node);
    }
    fclose(fp);

    //read meshlines;
    infile = rootname + ".edge";
    if((fp=fopen(infile.c_str(),"rt"))==NULL)
    {
        WarnMessage("No mesh to display");
        return false;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    meshline.resize(k);
    fclose(fp);

    infile = rootname + ".ele";
    if((fp=fopen(infile.c_str(),"rt"))==NULL)
    {
        WarnMessage("No mesh to display");
        return false;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);

    IntPoint segm;
    int n[3],p;
    for(i=0,nl=0; i<k; i++)
    {
        fgets(s,1024,fp);
        sscanf(s,"%i	%i	%i	%i	%i",&q,&n[0],&n[1],&n[2],&j);
        for(q=0; q<3; q++)
        {
            p=q+1;
            if(p==3) p=0;
            if (n[p]>n[q])
            {
                segm.x = n[p];
                segm.y = n[q];

                if (j != 0)
                {
                    meshline[nl++] = std::make_unique<femm::IntPoint>(segm);
                }
                else
                {
                    greymeshline.push_back(std::make_unique<femm::IntPoint>(segm));
                }
            }
        }
    }
    meshline.resize(nl);
    fclose(fp);

    // clear out temporary files
    infile = rootname + ".ele";
    remove(infile.c_str());
    infile = rootname + ".node";
    remove(infile.c_str());
    infile = rootname + ".edge";
    remove(infile.c_str());
    infile = rootname + ".pbc";
    remove(infile.c_str());
    infile = rootname + ".poly";
    remove(infile.c_str());

    return true;
}

void FMesher::UpdateUndo()
{

    unsigned int i;

    undonodelist.clear();
    undolinelist.clear();
    undoarclist.clear();
    undolabellist.clear();

    // copy each entry
    for(i=0; i<problem->nodelist.size(); i++)
        undonodelist.push_back(
                std::make_unique<CNode>(*problem->nodelist[i]));
    for(i=0; i<problem->linelist.size(); i++)
        undolinelist.push_back(
                    std::make_unique<CSegment>(*problem->linelist[i]));
    for(i=0; i<problem->arclist.size(); i++)
        undoarclist.push_back(
                    std::make_unique<CArcSegment>(*problem->arclist[i]));
    for (const auto &label: problem->labellist)
        undolabellist.push_back(label->clone());


}

void FMesher::Undo()
{
    unsigned int i;

    for(i=0; i<undolinelist.size(); i++) problem->linelist[i].swap(undolinelist[i]);
    for(i=0; i<undoarclist.size(); i++) problem->arclist[i].swap(undoarclist[i]);
    for(i=0; i<undolabellist.size(); i++) problem->labellist[i].swap(undolabellist[i]);
}

//bool FMesher::ScanPreferences()
//{
//	FILE *fp;
//	CStdString fname;
//
//	fname = BinDir + "femme.cfg";
//
//	fp=fopen(fname,"rt");
//	if (fp!=NULL)
//	{
//		bool flag=false;
//		char s[1024];
//		char q[1024];
//		char *v;
//
//		// parse the file
//		while (fgets(s,1024,fp)!=NULL)
//		{
//			sscanf(s,"%s",q);
//
//			if( _strnicmp(q,"<Precision>",11)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_prec);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<MinAngle>",10)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_minangle);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<Frequency>",11)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_freq);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<Depth>",7)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_depth);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<Coordinates>",13)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_coord);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<LengthUnits>",13)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_length);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<ProblemType>",13)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_type);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<ACSolver>",8)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_solver);
//			  q[0] = '\0';
//			}
//		}
//		fclose(fp);
//			return true;
//	}
//
//	return false;
//}


//--------------------------------------------------------------


//void CFemmeView::lnuMakeMesh()
//{
//	OnMakeMesh();
//}



bool FMesher::CanCreateRadius(int n)
{
    // check to see if a selected point, specified by n, can be
    // converted to a radius.  To be able to be converted to a radius,
    // the point must be an element of either 2 lines, 2 arcs, or
    // 1 line and 1 arc.

    int j=0;
    for (const auto &line: problem->linelist)
    {
        if (n==line->n0 || n==line->n1)
            j++;
    }
    for (const auto &arc: problem->arclist)
    {
        if (n==arc->n0 || n==arc->n1)
            j++;
    }

    return (j==2);
}

bool FMesher::CreateRadius(int n, double r)
{
    // replace the node indexed by n with a radius of r

    if(r<=0)
        return false;

    int nseg=0;
    int narc=0;
    int arc[2],seg[2];

    for(int k=0; k<(int)problem->linelist.size(); k++)
        if ((problem->linelist[k]->n0==n) || (problem->linelist[k]->n1==n))
            seg[nseg++]=k;
    for(int k=0; k<(int)problem->arclist.size(); k++)
        if ((problem->arclist[k]->n0==n) || (problem->arclist[k]->n1==n))
            arc[narc++]=k;

    if ((narc+nseg)!=2)
        return false;

    // there are three valid cases:
    switch (nseg-narc)
    {
    case 0:  // One arc and one line
    {
        CComplex c,u,p0,p1,q,p[4],v[8],i1[8],i2[8];
        double rc,b,R[4],phi;
        CArcSegment ar;

        // inherit the boundary condition from the arc so that
        // we can apply it to the newly created arc later;
        ar.InGroup       =problem->arclist[arc[0]]->InGroup;
        ar.BoundaryMarkerName=problem->arclist[arc[0]]->BoundaryMarkerName;

        // get the center and radius of the circle associated with the arc;
        GetCircle(*problem->arclist[arc[0]],c,rc);

        // get the locations of the endpoints of the segment;
        p0=problem->nodelist[n]->CC();
        if(problem->linelist[seg[0]]->n0==n)
            p1=problem->nodelist[problem->linelist[seg[0]]->n1]->CC();
        else
            p1=problem->nodelist[problem->linelist[seg[0]]->n0]->CC();

        u=(p1-p0)/abs(p1-p0);  // unit vector along the line
        q=p0 + u*Re((c-p0)/u); // closest point on line to center of circle
        u=(q-c)/abs(q-c); // unit vector from center to closest point on line;

        p[0]=q+r*u; R[0]=rc+r;
        p[1]=q-r*u; R[1]=rc+r;
        p[2]=q+r*u; R[2]=rc-r;
        p[3]=q-r*u; R[3]=rc-r;

        int j=0;
        for(int k=0;k<4;k++)
        {
            b=R[k]*R[k]-abs(p[k]-c)*abs(p[k]-c);
            if (b>=0){
                b=sqrt(b);
                v[j++]=p[k]+I*b*(p[k]-c)/abs(p[k]-c);
                v[j++]=p[k]-I*b*(p[k]-c)/abs(p[k]-c);
            }
        }

        // locations of the centerpoints that could be for the radius that
        // we are looking for are stored in v.  We now need to paw through
        // them to find the one solution that we are after.
        u=(p1-p0)/abs(p1-p0);  // unit vector along the line
        int m=0;
        for(int k=0;k<j;k++)
        {
            i1[m]=p0 +u*Re((v[k]-p0)/u); // intersection with the line
            i2[m]=c + rc*(v[k]-c)/abs(v[k]-c); // intersection with the arc;
            v[m]=v[k];

            // add this one to the list of possibly valid solutions if
            // both of the intersection points actually lie on the arc
            if ( ShortestDistanceFromArc(i2[m],*problem->arclist[arc[0]])<(r/10000.) &&
                 ShortestDistance(Re(i1[m]),Im(i1[m]),seg[0])<(r/10000.)
                 && abs(i1[m]-i2[m])>(r/10000.))
            {
                m++;
                if (m==2) break;
            }

        }

        if (m==0) return false;

        // But there are also special cases where there could be two answers.
        // We then pick the solution that has the center point closest to the point to be removed.
        if(m>1)
        {
            if (abs(v[0]-p0)<abs(v[1]-p0)) j=0;
            else j=1;
        }
        else j=0;	// The index of the winning case is in j....

        UpdateUndo();
        AddNode(Re(i1[j]),Im(i1[j]),r/10000.);
        AddNode(Re(i2[j]),Im(i2[j]),r/10000.);
        UnselectAll();

        // delete the node that is to be replace by a radius;
        n=ClosestNode(Re(p0),Im(p0));
        problem->nodelist[n]->IsSelected=true;
        DeleteSelectedNodes();

        // compute the angle spanned by the new arc;
        phi=arg((i2[j]-v[j])/(i1[j]-v[j]));
        if (phi<0)
        {
            c=i2[j]; i2[j]=i1[j]; i1[j]=c;
            phi=fabs(phi);
        }

        // add in the new radius;
        ar.n0=ClosestNode(Re(i1[j]),Im(i1[j]));
        ar.n1=ClosestNode(Re(i2[j]),Im(i2[j]));
        ar.ArcLength=phi/DEG;
        AddArcSegment(ar);

        return true;
    }
    case 2:  // Two lines
    {
        CComplex p0,p1,p2;
        double phi,len;
        CArcSegment ar;

        if (problem->linelist[seg[0]]->n0==n) p1=problem->nodelist[problem->linelist[seg[0]]->n1]->CC();
        else p1=problem->nodelist[problem->linelist[seg[0]]->n0]->CC();

        if (problem->linelist[seg[1]]->n0==n) p2=problem->nodelist[problem->linelist[seg[1]]->n1]->CC();
        else p2=problem->nodelist[problem->linelist[seg[1]]->n0]->CC();

        p0=problem->nodelist[n]->CC();

        // get the angle between the lines
        phi=arg((p2-p0)/(p1-p0));

        // check to see if this case is degenerate
        if (fabs(phi)>(179.*DEG)) return false;

        // check to see if the points are in the wrong order
        // and fix it if they are.
        if (phi<0){
            p0=p1; p1=p2; p2=p0; p0=problem->nodelist[n]->CC();
            swap(seg[0],seg[1]);
            phi=fabs(phi);
        }

        len = r/tan(phi/2.); // distance from p0 to the tangency point;

        // catch the case where the desired radius is too big to fit;
        if ((abs(p1-p0)<len) || (abs(p2-p0)<len)) return false;

        // compute the locations of the tangency points;
        p1=len*(p1-p0)/abs(p1-p0)+p0;
        p2=len*(p2-p0)/abs(p2-p0)+p0;

        // inherit the boundary condition from one of the segments
        // so that we can apply it to the newly created arc later;
        ar.BoundaryMarkerName=problem->linelist[seg[0]]->BoundaryMarkerName;
        ar.InGroup       =problem->linelist[seg[0]]->InGroup;

        // add new nodes at ends of radius
        UpdateUndo();
        AddNode(Re(p1),Im(p1),len/10000.);
        AddNode(Re(p2),Im(p2),len/10000.);
        UnselectAll();

        // delete the node that is to be replace by a radius;
        n=ClosestNode(Re(p0),Im(p0));
        problem->nodelist[n]->IsSelected=true;
        DeleteSelectedNodes();

        // add in the new radius;
        ar.n0=ClosestNode(Re(p2),Im(p2));
        ar.n1=ClosestNode(Re(p1),Im(p1));
        ar.ArcLength=180.-phi/DEG;
        AddArcSegment(ar);

        return true;
    }
    case -2: // Two arcs
    {
        CComplex c0,c1,c2,p[8],i1[8],i2[8];
        double a[8],b[8],c,d[8],x[8],r0,r1,r2,phi;
        CArcSegment ar;

        r0=r;
        GetCircle(*problem->arclist[arc[0]],c1,r1);
        GetCircle(*problem->arclist[arc[1]],c2,r2);
        c=abs(c2-c1);

        // solve for all of the different possible cases;
        a[0]=r1+r0; b[0]=r2+r0;
        a[1]=r1+r0; b[1]=r2+r0;
        a[2]=r1-r0; b[2]=r2-r0;
        a[3]=r1-r0; b[3]=r2-r0;
        a[4]=r1-r0; b[4]=r2+r0;
        a[5]=r1-r0; b[5]=r2+r0;
        a[6]=r1+r0; b[6]=r2-r0;
        a[7]=r1+r0; b[7]=r2-r0;
        for(int k=0;k<8;k++){
            x[k]=(b[k]*b[k]+c*c-a[k]*a[k])/(2.*c*c);
            d[k]=sqrt(b[k]*b[k]-x[k]*x[k]*c*c);
        }
        for(int k=0;k<8;k+=2)
        {
            // solve for the center point of the radius for each solution.
            p[k]  =((1-x[k])*c+I*d[k])*(c2-c1)/abs(c2-c1) + c1;
            p[k+1]=((1-x[k])*c-I*d[k])*(c2-c1)/abs(c2-c1) + c1;
        }

        c0=problem->nodelist[n]->CC();

        int j=0;
        for(int k=0;k<8;k++)
        {
            i1[j]=c1 + r1*(p[k]-c1)/abs(p[k]-c1); // compute possible intersection points
            i2[j]=c2 + r2*(p[k]-c2)/abs(p[k]-c2); // with the arcs;
            p[j] =p[k];

            // add this one to the list of possibly valid solutions if
            // both of the intersection points actually lie on the arc
            if ( ShortestDistanceFromArc(i1[j],*problem->arclist[arc[0]])<(r0/10000.) &&
                 ShortestDistanceFromArc(i2[j],*problem->arclist[arc[1]])<(r0/10000.) &&
                 abs(i1[j]-i2[j])>(r0/10000.))
            {
                j++;
                if (j==2) break;
            }

        }

        // There could be no valid solutions...
        if (j==0) return false;

        // But there are also special cases where there could be two answers.
        // We then pick the solution that has the center point closest to the point to be removed.
        if(j>1)
        {
            if (abs(p[0]-c0)<abs(p[1]-c0)) j=0;
            else j=1;
        }
        else j=0;	// The index of the winning case is in j....

        // inherit the boundary condition from one of the segments
        // so that we can apply it to the newly created arc later;
        ar.BoundaryMarkerName=problem->arclist[arc[0]]->BoundaryMarkerName;
        ar.InGroup=problem->arclist[arc[0]]->InGroup;

        // add new nodes at ends of radius
        UpdateUndo();
        AddNode(Re(i1[j]),Im(i1[j]),c/10000.);
        AddNode(Re(i2[j]),Im(i2[j]),c/10000.);
        UnselectAll();

        // delete the node that is to be replace by a radius;
        n=ClosestNode(Re(c0),Im(c0));
        problem->nodelist[n]->IsSelected=true;
        DeleteSelectedNodes();

        // compute the angle spanned by the new arc;
        phi=arg((i2[j]-p[j])/(i1[j]-p[j]));
        if (phi<0)
        {
            c0=i2[j]; i2[j]=i1[j]; i1[j]=c0;
            phi=fabs(phi);
        }

        // add in the new radius;
        ar.n0=ClosestNode(Re(i1[j]),Im(i1[j]));
        ar.n1=ClosestNode(Re(i2[j]),Im(i2[j]));
        ar.ArcLength=phi/DEG;
        AddArcSegment(ar);

        return true;
    }
    }

    return false;
}

//void CFemmeView::lnu_purge_mesh()
//{
//	OnPurgemesh();
//}
//
//void CFemmeView::lnu_show_mesh()
//{
//	OnShowMesh();
//}
//
//void CFemmeView::lnu_analyze(int bShow)
//{
//	if (bShow) bLinehook=HiddenLua;
//	else bLinehook=NormalLua;
//	OnMenuAnalyze();
//}
//
//void CFemmeView::lua_zoomnatural()
//{
//	OnZoomNatural();
//}
//
//void CFemmeView::lua_zoomout()
//{
//	OnZoomOut();
//}
//
//void CFemmeView::lua_zoomin()
//{
//	OnZoomIn();
//}

//bool FMesher::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
//{
//	// TODO: Add your specialized code here and/or call the base class
//	if (bLinehook!=false) return true;
//	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
//}

//void FMesher::OnEditExterior()
//{
//	// TODO: Add your command handler code here
//	CExteriorProps dlg;
//
//	dlg.m_Ro=extRo;
//	dlg.m_Ri=extRi;
//	dlg.m_Zo=extZo;
//	if(dlg.DoModal()==IDOK)
//	{
//		extRo=dlg.m_Ro;
//		extRi=dlg.m_Ri;
//		extZo=dlg.m_Zo;
//	}
//}

bool FMesher::DeleteSelectedBlockLabels()
{
    auto &labellist = problem->labellist;
    size_t oldsize = labellist.size();

    if (!labellist.empty())
    {
        // remove selected elements
        labellist.erase(
                    std::remove_if(labellist.begin(),labellist.end(),
                                   [](const auto& label){ return label->IsSelected;} ),
                    labellist.end()
                    );
    }
    labellist.shrink_to_fit();

    return labellist.size() != oldsize;
}

bool FMesher::DeleteSelectedSegments()
{
    auto &linelist = problem->linelist;
    size_t oldsize = linelist.size();

    if (!linelist.empty())
    {
        // remove selected elements
        linelist.erase(
                    std::remove_if(linelist.begin(),linelist.end(),
                                   [](const auto& segm){ return segm->IsSelected;} ),
                    linelist.end()
                    );
    }
    linelist.shrink_to_fit();

    return linelist.size() != oldsize;
}

bool FMesher::DeleteSelectedArcSegments()
{
    auto &arclist = problem->arclist;
    size_t oldsize = arclist.size();

    if (!arclist.empty())
    {
        // remove selected elements
        arclist.erase(
                    std::remove_if(arclist.begin(),arclist.end(),
                                   [](const auto& arc){ return arc->IsSelected;} ),
                    arclist.end()
                    );
    }
    arclist.shrink_to_fit();

    return arclist.size() != oldsize;
}

bool FMesher::DeleteSelectedNodes()
{
    auto &nodelist = problem->nodelist;
    bool changed = false;

    if (nodelist.size() > 0)
    {
        int i=0;
        do
        {
            if(nodelist[i]->IsSelected!=0)
            {
                changed=true;
                auto &linelist = problem->linelist;
                auto &arclist = problem->arclist;
                // first remove all lines that contain the point;
                for (int j=0; j<(int)linelist.size(); j++)
                    if((linelist[j]->n0==i) || (linelist[j]->n1==i))
                        linelist[j]->ToggleSelect();
                DeleteSelectedSegments();

                // remove all arcs that contain the point;
                for (int j=0; j<(int)arclist.size(); j++)
                    if((arclist[j]->n0==i) || (arclist[j]->n1==i))
                        arclist[j]->ToggleSelect();
                DeleteSelectedArcSegments();

                // remove node from the nodelist...
                nodelist.erase(nodelist.begin()+i);

                // update lines to point to the new node numbering
                for (int j=0; j<(int)linelist.size(); j++)
                {
                    if (linelist[j]->n0>i) linelist[j]->n0--;
                    if (linelist[j]->n1>i) linelist[j]->n1--;
                }

                // update arcs to point to the new node numbering
                for (int j=0; j<(int)arclist.size(); j++)
                {
                    if (arclist[j]->n0>i) arclist[j]->n0--;
                    if (arclist[j]->n1>i) arclist[j]->n1--;
                }
            } else
                i++;
        } while (i<(int)nodelist.size());
    }

    nodelist.shrink_to_fit();
    return changed;
}


bool FMesher::AddArcSegment(CArcSegment &asegm, double tol)
{
    // don't add if line is degenerate
    if (asegm.n0==asegm.n1)
        return false;

    // don't add if the arc is already in the list;
    for(int i=0; i<(int)problem->arclist.size(); i++){
        if ((problem->arclist[i]->n0==asegm.n0) && (problem->arclist[i]->n1==asegm.n1) &&
                (fabs(problem->arclist[i]->ArcLength-asegm.ArcLength)<1.e-02)) return false;
        // arcs are ``the same'' if start and end points are the same, and if
        // the arc lengths are relatively close (but a lot farther than
        // machine precision...
    }

    // add proposed arc to the linelist
    asegm.IsSelected = 0;

    CComplex p[2];
    std::vector < CComplex > newnodes;
    // check to see if there are intersections
    for(int i=0; i<(int)problem->linelist.size(); i++)
    {
        int j = GetLineArcIntersection(*problem->linelist[i],asegm,p);
        if (j>0)
            for(int k=0; k<j; k++)
                newnodes.push_back(p[k]);
    }
    for (int i=0; i<(int)problem->arclist.size(); i++)
    {
        int j = GetArcArcIntersection(asegm,*problem->arclist[i],p);
        if (j>0)
            for(int k=0; k<j; k++)
                newnodes.push_back(p[k]);
    }

    // add nodes at intersections
    double t;
    if (tol==0)
    {
        if (problem->nodelist.size()<2) t=1.e-08;
        else{
            CComplex p0,p1;
            p0 = problem->nodelist[0]->CC();
            p1 = p0;
            for (int i=1; i<(int)problem->nodelist.size(); i++)
            {
                if(problem->nodelist[i]->x<p0.re) p0.re = problem->nodelist[i]->x;
                if(problem->nodelist[i]->x>p1.re) p1.re = problem->nodelist[i]->x;
                if(problem->nodelist[i]->y<p0.im) p0.im = problem->nodelist[i]->y;
                if(problem->nodelist[i]->y>p1.im) p1.im = problem->nodelist[i]->y;
            }
            t = abs(p1-p0)*CLOSE_ENOUGH;
        }
    }
    else t = tol;

    for (int i=0; i<(int)newnodes.size(); i++)
        AddNode(newnodes[i].re,newnodes[i].im,t);

    // add proposed arc segment;
    problem->arclist.push_back(std::make_unique<CArcSegment>(asegm));

    // check to see if proposed arc passes through other points;
    // if so, delete arc and create arcs that link intermediate points;
    // does this by recursive use of AddArcSegment;

    UnselectAll();
    CComplex c;
    double R;
    GetCircle(asegm,c,R);

    double dmin = tol;
    if (tol==0)
        dmin = fabs(R*PI*asegm.ArcLength/180.)*1.e-05;

    int k = (int)problem->arclist.size()-1;
    for(int i=0; i<(int)problem->nodelist.size(); i++)
    {
        if( (i!=asegm.n0) && (i!=asegm.n1) )
        {
            double d=ShortestDistanceFromArc(CComplex(problem->nodelist[i]->x,problem->nodelist[i]->y),*problem->arclist[k]);

            //	MsgBox("d=%g dmin=%g",d,dmin);
            // what is the purpose of this test?
            //	if (abs(problem->nodelist[i]->CC()-problem->nodelist[asegm.n0]->CC())<2.*dmin) d=2.*dmin;
            //	if (abs(problem->nodelist[i]->CC()-problem->nodelist[asegm.n1]->CC())<2.*dmin) d=2.*dmin;


            if (d<dmin){

                CComplex a0,a1,a2;
                a0.Set(problem->nodelist[asegm.n0]->x,problem->nodelist[asegm.n0]->y);
                a1.Set(problem->nodelist[asegm.n1]->x,problem->nodelist[asegm.n1]->y);
                a2.Set(problem->nodelist[i]->x,problem->nodelist[i]->y);
                problem->arclist[k]->ToggleSelect();
                DeleteSelectedArcSegments();

                CArcSegment newarc = asegm;
                newarc.n1 = i;
                newarc.ArcLength = arg((a2-c)/(a0-c))*180./PI;
                AddArcSegment(newarc,dmin);

                newarc = asegm;
                newarc.n0 = i;
                newarc.ArcLength = arg((a1-c)/(a2-c))*180./PI;
                AddArcSegment(newarc,dmin);

                i = problem->nodelist.size();
            }
        }
    }

    return true;
}

void FMesher::MirrorCopy(double x0, double y0, double x1, double y1, EditMode selector)
{
    assert(selector != EditMode::Invalid);
    CComplex x=x0 + I*y0;
    CComplex p=(x1-x0) + I*(y1-y0);
    if(abs(p)==0)
        return;
    p/=abs(p);

    if (selector==EditMode::EditNodes || selector == EditMode::EditGroup)
    {
        for (const auto &node: problem->nodelist)
        {
            if (node->IsSelected)
            {
                CComplex y (node->x,node->y);
                y = (y-x) / p;
                y = p*y.Conj()+x;
                // create copy
                std::unique_ptr<CNode> newnode = std::make_unique<CNode>(*node);
                // overwrite coordinates in copy
                newnode->x = y.re;
                newnode->y = y.im;
                newnode->IsSelected = false;
                problem->nodelist.push_back(std::move(newnode));
            }
        }
    }
    if (selector == EditMode::EditLines || selector == EditMode::EditGroup)
    {
        for (const auto &line: problem->linelist)
        {
            if (line->IsSelected)
            {
                // copy endpoints
                std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[line->n0]);
                CComplex y0 (n0->x,n0->y);
                y0 = (y0-x) / p;
                y0 = p*y0.Conj()+x;
                n0->x = y0.re;
                n0->y = y0.im;
                n0->IsSelected = false;

                std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[line->n1]);
                CComplex y1 (n1->x,n1->y);
                y1 = (y1-x) / p;
                y1 = p*y1.Conj()+x;
                n1->x = y1.re;
                n1->y = y1.im;
                n1->IsSelected = false;

                // copy line (with identical endpoints)
                std::unique_ptr<CSegment> newline = std::make_unique<CSegment>(*line);
                newline->IsSelected = false;
                // set endpoints
                newline->n0 = (int)problem->nodelist.size();
                problem->nodelist.push_back(std::move(n0));
                newline->n1 = (int)problem->nodelist.size();
                problem->nodelist.push_back(std::move(n1));
                problem->linelist.push_back(std::move(newline));
            }
        }
    }

    if (selector == EditMode::EditLabels || selector == EditMode::EditGroup)
    {
        for (const auto &label: problem->labellist)
        {
            if (label->IsSelected)
            {
                std::unique_ptr<CBlockLabel> newlabel = label->clone();
                CComplex y (label->x,label->y);
                y = (y-x) / p;
                y = p*y.Conj()+x;
                newlabel->x = y.re;
                newlabel->y = y.im;
                newlabel->IsSelected = false;
                // set specific problem parameters:
                if (CMBlockLabel *ptr=dynamic_cast<CMBlockLabel*>(newlabel.get()))
                    ptr->MagDir = (180./PI)*arg(p*conj(exp(I*ptr->MagDir*PI/180.)/p));

                problem->labellist.push_back(std::move(newlabel));
            }
        }
    }
    if (selector == EditMode::EditArcs || selector == EditMode::EditGroup)
    {
        for (const auto &arc: problem->arclist)
        {
            if (arc->IsSelected)
            {
                // copy endpoints
                std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[arc->n0]);
                CComplex y0 (n0->x,n0->y);
                y0 = (y0-x) / p;
                y0 = p*y0.Conj()+x;
                n0->x = y0.re;
                n0->y = y0.im;
                n0->IsSelected = false;

                std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[arc->n1]);
                CComplex y1 (n1->x,n1->y);
                y1 = (y1-x) / p;
                y1 = p*y1.Conj()+x;
                n1->x = y1.re;
                n1->y = y1.im;
                n1->IsSelected = false;

                // copy arc (with identical endpoints)
                std::unique_ptr<CArcSegment> newarc = std::make_unique<CArcSegment>(*arc);
                newarc->IsSelected = false;
                // set endpoints
                newarc->n0 = (int)problem->nodelist.size();
                problem->nodelist.push_back(std::move(n0));
                newarc->n1 = (int)problem->nodelist.size();
                problem->nodelist.push_back(std::move(n1));
                problem->arclist.push_back(std::move(newarc));
            }
        }
    }
    EnforcePSLG();
}

void FMesher::RotateCopy(CComplex c, double dt, int ncopies, femm::EditMode selector)
{
    assert(selector != EditMode::Invalid);
    for(int nc=0; nc<ncopies; nc++)
    {
        // accumulated angle
        double t = ((double) (nc+1))*dt;

        CComplex z = exp(I*t*PI/180);

        if (selector==EditMode::EditNodes || selector == EditMode::EditGroup)
        {
            for (const auto &node: problem->nodelist)
            {
                if (node->IsSelected)
                {
                    CComplex x (node->x, node->y);
                    x=(x-c)*z+c;
                    // create copy
                    std::unique_ptr<CNode> newnode = std::make_unique<CNode>(*node);
                    // overwrite coordinates in copy
                    newnode->x = x.re;
                    newnode->y = x.im;
                    newnode->IsSelected = false;
                    problem->nodelist.push_back(std::move(newnode));
                }
            }
        }

        if (selector == EditMode::EditLines || selector == EditMode::EditGroup)
        {
            for (const auto &line: problem->linelist)
            {
                if (line->IsSelected)
                {
                    // copy endpoints
                    std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[line->n0]);
                    CComplex x0 (n0->x,n0->y);
                    x0 = (x0-c)*z+c;
                    n0->x = x0.re;
                    n0->y = x0.im;
                    n0->IsSelected = false;

                    std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[line->n1]);
                    CComplex x1 (n1->x,n1->y);
                    x1 = (x1-c)*z+c;
                    n1->x = x1.re;
                    n1->y = x1.im;
                    n1->IsSelected = false;

                    // copy line (with identical endpoints)
                    std::unique_ptr<CSegment> newline = std::make_unique<CSegment>(*line);
                    newline->IsSelected = false;
                    // set endpoints
                    newline->n0 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n0));
                    newline->n1 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n1));
                    problem->linelist.push_back(std::move(newline));
                }
            }
        }

        if (selector == EditMode::EditArcs || selector == EditMode::EditGroup)
        {
            for (const auto &arc: problem->arclist)
            {
                if (arc->IsSelected)
                {
                    // copy endpoints
                    std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[arc->n0]);
                    CComplex x0 (n0->x,n0->y);
                    x0 = (x0-c)*z+c;
                    n0->x = x0.re;
                    n0->y = x0.im;
                    n0->IsSelected = false;

                    std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[arc->n1]);
                    CComplex x1 (n1->x,n1->y);
                    x1 = (x1-c)*z+c;
                    n1->x = x1.re;
                    n1->y = x1.im;
                    n1->IsSelected = false;

                    // copy arc (with identical endpoints)
                    std::unique_ptr<CArcSegment> newarc = std::make_unique<CArcSegment>(*arc);
                    newarc->IsSelected = false;
                    // set endpoints
                    newarc->n0 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n0));
                    newarc->n1 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n1));
                    problem->arclist.push_back(std::move(newarc));
                }
            }
        }

        if (selector == EditMode::EditLabels || selector == EditMode::EditGroup)
        {
            for (const auto &label: problem->labellist)
            {
                if (label->IsSelected)
                {
                    std::unique_ptr<CBlockLabel> newlabel = label->clone();
                    CComplex x(label->x,label->y);
                    x = (x-c)*z+c;
                    newlabel->x = x.re;
                    newlabel->y = x.im;
                    newlabel->IsSelected = false;

                    for (const auto &bprop: problem->blockproplist)
                    {
                        CMMaterialProp *prop = dynamic_cast<CMMaterialProp*>(bprop.get());
                        if (prop
                                && prop->BlockName == newlabel->BlockTypeName
                                && prop->H_c != 0)
                        {
                            if (CMBlockLabel *ptr=dynamic_cast<CMBlockLabel*>(newlabel.get()))
                                ptr->MagDir += t;
                        }
                    }

                    problem->labellist.push_back(std::move(newlabel));
                }
            }
        }
    }

    EnforcePSLG();
}

void FMesher::RotateMove(CComplex c, double t, femm::EditMode selector)
{
    assert(selector != EditMode::Invalid);
    bool processNodes = (selector == EditMode::EditNodes);

    const CComplex z = exp(I*t*PI/180);

    if(selector==EditMode::EditLines || selector==EditMode::EditGroup)
    {
        for (const auto &line: problem->linelist)
        {
            if (line->IsSelected)
            {
                problem->nodelist[line->n0]->IsSelected = true;
                problem->nodelist[line->n1]->IsSelected = true;
            }
        }
        processNodes = true;
    }

    if(selector==EditMode::EditArcs || selector==EditMode::EditGroup)
    {
        for (const auto &arc: problem->arclist)
        {
            if (arc->IsSelected)
            {
                problem->nodelist[arc->n0]->IsSelected = true;
                problem->nodelist[arc->n1]->IsSelected = true;
            }
        }
        processNodes = true;
    }

    if(selector==EditMode::EditLabels || selector==EditMode::EditGroup)
    {
        for (auto &label: problem->labellist)
        {
            if (label->IsSelected)
            {
                CComplex x (label->x, label->y);
                x = (x-c)*z+c;
                label->x = x.re;
                label->y = x.im;

                for (const auto &bprop : problem->blockproplist)
                {
                    CMMaterialProp *prop = dynamic_cast<CMMaterialProp*>(bprop.get());
                    if (prop
                            && prop->BlockName == label->BlockTypeName
                            && prop->H_c != 0)
                    {
                        if (CMBlockLabel *ptr=dynamic_cast<CMBlockLabel*>(label.get()))
                            ptr->MagDir += t;
                    }
                }
            }
        }
    }

    if(processNodes)
    {
        for (auto &node : problem->nodelist)
        {
            if (node->IsSelected)
            {
                CComplex x(node->x,node->y);
                x = (x-c)*z+c;
                node->x = x.re;
                node->y = x.im;
            }
        }
    }
    EnforcePSLG();
}

void FMesher::ScaleMove(double bx, double by, double sf, EditMode selector)
{
    assert(selector != EditMode::Invalid);
    bool processNodes = (selector == EditMode::EditNodes);

    if (selector==EditMode::EditLines || selector==EditMode::EditGroup)
    {
        for (const auto& line: problem->linelist)
        {
            if (line->IsSelected)
            {
                problem->nodelist[line->n0]->IsSelected = true;
                problem->nodelist[line->n1]->IsSelected = true;
            }
        }
        processNodes = true;
    }

    if (selector==EditMode::EditArcs || selector==EditMode::EditGroup)
    {
        for (const auto &arc: problem->arclist)
        {
            if (arc->IsSelected)
            {
                problem->nodelist[arc->n0]->IsSelected = true;
                problem->nodelist[arc->n1]->IsSelected = true;
            }
        }
        processNodes = true;
    }

    if (selector==EditMode::EditLabels || selector==EditMode::EditGroup)
    {
        for (auto &label: problem->labellist)
        {
            if (label->IsSelected)
            {
                label->x = bx+sf*(label->x - bx);
                label->y = by+sf*(label->y - by);
                label->MaxArea *= (sf*sf);
            }
        }
    }

    if (processNodes)
    {
        for (auto &node : problem->nodelist)
        {
            if (node->IsSelected)
            {
                node->x = bx+sf*(node->x - bx);
                node->y = by+sf*(node->y - by);
            }
        }
    }
    EnforcePSLG();
}

void FMesher::TranslateCopy(double incx, double incy, int ncopies, femm::EditMode selector)
{
    assert(selector != EditMode::Invalid);
    for(int nc=0; nc<ncopies; nc++)
    {
        // accumulated offsets
        double dx=((double)(nc+1))*incx;
        double dy=((double)(nc+1))*incy;

        if (selector==EditMode::EditNodes || selector == EditMode::EditGroup)
        {
            for (const auto &node: problem->nodelist)
            {
                if (node->IsSelected)
                {
                    // create copy
                    std::unique_ptr<CNode> newnode = std::make_unique<CNode>(*node);
                    // overwrite coordinates in copy
                    newnode->x += dx;
                    newnode->y += dy;
                    newnode->IsSelected = false;
                    problem->nodelist.push_back(std::move(newnode));
                }
            }
        }

        if (selector == EditMode::EditLines || selector == EditMode::EditGroup)
        {
            for (const auto &line: problem->linelist)
            {
                if (line->IsSelected)
                {
                    // copy endpoints
                    std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[line->n0]);
                    n0->x += dx;
                    n0->y += dy;
                    n0->IsSelected = false;

                    std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[line->n1]);
                    n1->x += dx;
                    n1->y += dy;
                    n1->IsSelected = false;

                    // copy line (with identical endpoints)
                    std::unique_ptr<CSegment> newline = std::make_unique<CSegment>(*line);
                    newline->IsSelected = false;
                    // set endpoints
                    newline->n0 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n0));
                    newline->n1 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n1));
                    problem->linelist.push_back(std::move(newline));
                }
            }
        }

        if (selector == EditMode::EditLabels || selector == EditMode::EditGroup)
        {
            for (const auto &label: problem->labellist)
            {
                if (label->IsSelected)
                {
                    std::unique_ptr<CBlockLabel> newlabel = label->clone();
                    newlabel->x += dx;
                    newlabel->y += dy;
                    newlabel->IsSelected = false;

                    problem->labellist.push_back(std::move(newlabel));
                }
            }
        }

        if (selector == EditMode::EditArcs || selector == EditMode::EditGroup)
        {
            for (const auto &arc: problem->arclist)
            {
                if (arc->IsSelected)
                {
                    // copy endpoints
                    std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[arc->n0]);
                    n0->x += dx;
                    n0->y += dy;
                    n0->IsSelected = false;

                    std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[arc->n1]);
                    n1->x += dx;
                    n1->y += dy;
                    n1->IsSelected = false;

                    // copy arc (with identical endpoints)
                    std::unique_ptr<CArcSegment> newarc = std::make_unique<CArcSegment>(*arc);
                    newarc->IsSelected = false;
                    // set endpoints
                    newarc->n0 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n0));
                    newarc->n1 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n1));
                    problem->arclist.push_back(std::move(newarc));
                }
            }
        }
    }

    EnforcePSLG();
}

void FMesher::TranslateMove(double dx, double dy, femm::EditMode selector)
{
    assert(selector != EditMode::Invalid);
    bool processNodes = (selector == EditMode::EditNodes);

    if (selector == EditMode::EditLines || selector == EditMode::EditGroup)
    {
        // select end points of selected lines:
        for (auto &line: problem->linelist)
        {
            if (line->IsSelected)
            {
                problem->nodelist[line->n0]->IsSelected = true;
                problem->nodelist[line->n1]->IsSelected = true;
            }
        }
        // make sure to translate endpoints
        processNodes = true;
    }
    if (selector == EditMode::EditArcs || selector == EditMode::EditGroup)
    {
        // select end points of selected arcs:
        for (auto &arc: problem->arclist)
        {
            if (arc->IsSelected)
            {
                problem->nodelist[arc->n0]->IsSelected = true;
                problem->nodelist[arc->n1]->IsSelected = true;
            }
        }
        // make sure to translate endpoints
        processNodes = true;
    }

    if (selector == EditMode::EditLabels || selector == EditMode::EditGroup)
    {
        for (auto &lbl: problem->labellist)
        {
            if (lbl->IsSelected)
            {
                lbl->x += dx;
                lbl->y += dy;
            }
        }
    }
    if (processNodes)
    {
        for (auto &node: problem->nodelist)
        {
            if (node->IsSelected)
            {
                node->x += dx;
                node->y += dy;
            }
        }
    }
    EnforcePSLG();
}


bool FMesher::AddBlockLabel(double x, double y, double d)
{
    std::unique_ptr<CBlockLabel> pt;
    switch (problem->filetype) {
    case FileType::MagneticsFile:
        pt = std::make_unique<CMBlockLabel>();
        break;
    case FileType::HeatFlowFile:
        pt = std::make_unique<CHBlockLabel>();
        break;
    case FileType::ElectrostaticsFile:
        pt = std::make_unique<CSBlockLabel>();
        break;
    default:
        assert(false && "Unhandled file type");
        break;
    }
    pt->x = x;
    pt->y = y;

    return AddBlockLabel(std::move(pt), d);
}

bool FMesher::AddBlockLabel(std::unique_ptr<CBlockLabel> &&label, double d)
{
    double x = label->x;
    double y = label->y;

    // can't put a block label on top of an existing node...
    for (int i=0; i<(int)problem->nodelist.size(); i++)
        if(problem->nodelist[i]->GetDistance(x,y)<d) return false;

    // can't put a block label on a line, either...
    for (int i=0; i<(int)problem->linelist.size(); i++)
        if(ShortestDistance(x,y,i)<d) return false;

    // test to see if ``too close'' to existing node...
    bool exists=false;
    for (int i=0; i<(int)problem->labellist.size(); i++)
        if(problem->labellist[i]->GetDistance(x,y)<d) {
            exists=true;
            break;
        }

    // if all is OK, add point in to the node list...
    if(!exists){
        problem->labellist.push_back(std::move(label));
    }

    return true;
}


bool FMesher::AddNode(double x, double y, double d)
{
    // create an appropriate node and call AddNode on it
    std::unique_ptr<CNode> node = std::make_unique<CNode>(x,y);
    return AddNode(std::move(node), d);
}

bool FMesher::AddNode(std::unique_ptr<CNode> &&node, double d)
{
    CComplex c,a0,a1,a2;
    double R;
    double x = node->x;
    double y = node->y;

    // test to see if ``too close'' to existing node...
    for (int i=0; i<(int)problem->nodelist.size(); i++)
        if(problem->nodelist[i]->GetDistance(x,y)<d) return false;

    // can't put a node on top of a block label; do same sort of test.
    for (int i=0;i<(int)problem->labellist.size();i++)
        if(problem->labellist[i]->GetDistance(x,y)<d) return false;

    // if all is OK, add point in to the node list...
    problem->nodelist.push_back(std::move(node));

    // test to see if node is on an existing line; if so,
    // break into two lines;

    for(int i=0, k=(int)problem->linelist.size(); i<k; i++)
    {
        if (fabs(ShortestDistance(x,y,i))<d)
        {
            std::unique_ptr<CSegment> segm;
            segm = std::make_unique<CSegment>(*problem->linelist[i]);
            problem->linelist[i]->n1=problem->nodelist.size()-1;
            segm->n0=problem->nodelist.size()-1;
            problem->linelist.push_back(std::move(segm));
        }
    }

    // test to see if node is on an existing arc; if so,
    // break into two arcs;
    for(int i=0, k=(int)problem->arclist.size(); i<k; i++)
    {
        if (ShortestDistanceFromArc(CComplex(x,y),*problem->arclist[i])<d)
        {
            a0.Set(problem->nodelist[problem->arclist[i]->n0]->x,problem->nodelist[problem->arclist[i]->n0]->y);
            a1.Set(problem->nodelist[problem->arclist[i]->n1]->x,problem->nodelist[problem->arclist[i]->n1]->y);
            a2.Set(x,y);
            GetCircle(*problem->arclist[i],c,R);

            std::unique_ptr<CArcSegment> asegm;
            asegm = std::make_unique<CArcSegment>(*problem->arclist[i]);
            problem->arclist[i]->n1 = problem->nodelist.size()-1;
            problem->arclist[i]->ArcLength = arg((a2-c)/(a0-c))*180./PI;
            asegm->n0 = problem->nodelist.size()-1;
            asegm->ArcLength = arg((a1-c)/(a2-c))*180./PI;
            problem->arclist.push_back(std::move(asegm));
        }
    }
    return true;
}

bool FMesher::AddSegment(int n0, int n1, double tol)
{
    return AddSegment(n0,n1,nullptr,tol);
}

bool FMesher::AddSegment(int n0, int n1, const CSegment *parsegm, double tol)
{
    double xi,yi,t;
    CComplex p[2];
    CSegment segm;
    std::vector < CComplex > newnodes;

    // don't add if line is degenerate
    if (n0==n1) return false;

    // don't add if the line is already in the list;
    for (int i=0; i<(int)problem->linelist.size(); i++){
        if ((problem->linelist[i]->n0==n0) && (problem->linelist[i]->n1==n1)) return false;
        if ((problem->linelist[i]->n0==n1) && (problem->linelist[i]->n1==n0)) return false;
    }

    // add proposed line to the linelist
    segm.BoundaryMarkerName="<None>";
    if (parsegm!=NULL) segm=*parsegm;
    segm.IsSelected=0;
    segm.n0=n0; segm.n1=n1;

    // check to see if there are intersections with segments
    for (int i=0; i<(int)problem->linelist.size(); i++)
        if(GetIntersection(n0,n1,i,&xi,&yi)) newnodes.push_back(CComplex(xi,yi));

    // check to see if there are intersections with arcs
    for (int i=0; i<(int)problem->arclist.size(); i++){
        int j = GetLineArcIntersection(segm,*problem->arclist[i],p);
        if (j>0)
            for(int k=0;k<j;k++)
                newnodes.push_back(p[k]);
    }

    // add nodes at intersections
    if (tol==0)
    {
        if (problem->nodelist.size()<2)
            t = 1.e-08;
        else{
            CComplex p0,p1;
            p0 = problem->nodelist[0]->CC();
            p1 = p0;
            for (int i=1; i<(int)problem->nodelist.size(); i++)
            {
                if(problem->nodelist[i]->x<p0.re) p0.re=problem->nodelist[i]->x;
                if(problem->nodelist[i]->x>p1.re) p1.re=problem->nodelist[i]->x;
                if(problem->nodelist[i]->y<p0.im) p0.im=problem->nodelist[i]->y;
                if(problem->nodelist[i]->y>p1.im) p1.im=problem->nodelist[i]->y;
            }
            t=abs(p1-p0)*CLOSE_ENOUGH;
        }
    }
    else t=tol;

    for (int i=0; i<(int)newnodes.size(); i++)
        AddNode(newnodes[i].re,newnodes[i].im,t);

    // Add proposed line segment
    problem->linelist.push_back(std::make_unique<CSegment>(segm));

    // check to see if proposed line passes through other points;
    // if so, delete line and create lines that link intermediate points;
    // does this by recursive use of AddSegment;
    double d,dmin;
    UnselectAll();
    if (tol==0)
        dmin = abs(problem->nodelist[n1]->CC()-problem->nodelist[n0]->CC())*1.e-05;
    else dmin = tol;

    for (int i=0, k=problem->linelist.size()-1; i<(int)problem->nodelist.size(); i++)
    {
        if( (i!=n0) && (i!=n1) )
        {
            d=ShortestDistance(problem->nodelist[i]->x,problem->nodelist[i]->y,k);
            if (abs(problem->nodelist[i]->CC()-problem->nodelist[n0]->CC())<dmin) d=2.*dmin;
            if (abs(problem->nodelist[i]->CC()-problem->nodelist[n1]->CC())<dmin) d=2.*dmin;
            if (d<dmin){
                problem->linelist[k]->ToggleSelect();
                DeleteSelectedSegments();
                if(parsegm==NULL)
                {
                    AddSegment(n0,i,dmin);
                    AddSegment(i,n1,dmin);
                }
                else{
                    AddSegment(n0,i,&segm,dmin);
                    AddSegment(i,n1,&segm,dmin);
                }
                i=problem->nodelist.size();
            }
        }
    }

    return true;
}

//void FMesher::OnEditMatprops()
//{
//	// TODO: Add your command handler code here
//	CPtProp pProp;
//
//	pProp.pblockproplist=&blockproplist;
//	pProp.PropType=2;
//	pProp.ProblemType=ProblemType;
//
//	pProp.DoModal();
//}

//void FMesher::OnEditPtprops()
//{
//	// TODO: Add your command handler code here
//	CPtProp pProp;
//
//	pProp.pnodeproplist=&nodeproplist;
//	pProp.PropType=0;
//
//	pProp.DoModal();
//}

//void FMesher::OnEditSegprops()
//{
//	// TODO: Add your command handler code here
//	CPtProp pProp;
//
//	pProp.plineproplist=&lineproplist;
//	pProp.PropType=1;
//	pProp.ProblemType=ProblemType;
//	pProp.DoModal();
//}

//void FMesher::OnEditCircprops()
//{
//	CPtProp pProp;
//
//	pProp.pcircproplist=&circproplist;
//	pProp.ProblemType=ProblemType;
//	pProp.PropType=3;
//
//	pProp.DoModal();
//}

//bool FMesher::OpBlkDlg()
//{
//	int i,j,k,nselected,cselected;
//	double a;
//	COpBlkDlg zDlg;
//
//	zDlg.ProblemType=ProblemType;
//
//	// check to see how many (if any) blocks are selected.
//	for(i=0,k=0,nselected=0,cselected=0;i<blocklist.size();i++)
//	{
//		if (blocklist[i].IsSelected!=0){
//			if(nselected==0){
//				nselected++;
//				zDlg.m_ingroup=blocklist[i].InGroup;
//			}
//			else if(blocklist[i].BlockTypeName!=blocklist[k].BlockTypeName)
//				nselected++;
//			if(cselected==0) cselected++;
//			else if(blocklist[i].InCircuit!=blocklist[k].InCircuit)
//				cselected++;
//			if(blocklist[i].InGroup!=zDlg.m_ingroup) zDlg.m_ingroup=0;
//			k=i;
//			if(blocklist[i].IsExternal) zDlg.m_isexternal=true;
//		}
//	}
//	if (nselected==0) return false;
//
//	// find average block size;
//	for(i=0,j=0,a=0.;i<blocklist.size();i++)
//		if(blocklist[i].IsSelected!=0)
//			if (blocklist[i].MaxArea>a) a=blocklist[i].MaxArea;
//
//	zDlg.m_sidelength=floor(2.e07*sqrt(a/PI)+0.5)/1.e07;
//
//	zDlg.pblockproplist=&blockproplist;
//	zDlg.pcircproplist=&circproplist;
//
//	if (nselected==1){
//		if(blocklist[k].BlockTypeName=="<No Mesh>") zDlg.cursel=1;
//		else for(i=0,zDlg.cursel=0;i<blockproplist.size();i++)
//			if (blockproplist[i].BlockName==blocklist[k].BlockTypeName)
//				zDlg.cursel=i+2;
//	}
//	else zDlg.cursel=0;
//
//	if (cselected==1){
//		for(i=0,zDlg.circsel=0;i<circproplist.size();i++)
//			if (circproplist[i].CircName==blocklist[k].InCircuit)
//				zDlg.circsel=i+1;
//	}
//	else zDlg.circsel=0;
//
//	zDlg.m_magdir=blocklist[k].MagDir;
//	zDlg.m_magdirfctn=blocklist[k].MagDirFctn;
//	zDlg.m_turns=blocklist[k].Turns;
//
//	if (zDlg.DoModal()==IDOK){
//		for(i=0;i<blocklist.size();i++)
//		{
//			if(blocklist[i].IsSelected!=0){
//				blocklist[i].MaxArea=PI*zDlg.m_sidelength*zDlg.m_sidelength/4.;
//				blocklist[i].MagDir=zDlg.m_magdir;
//				blocklist[i].MagDirFctn=zDlg.m_magdirfctn;
//				blocklist[i].Turns=zDlg.m_turns;
//				if (blocklist[i].Turns==0) blocklist[i].Turns++;
//				if (zDlg.cursel==0) blocklist[i].BlockTypeName="<None>";
//				else if(zDlg.cursel==1) blocklist[i].BlockTypeName="<No Mesh>";
//				else blocklist[i].BlockTypeName=blockproplist[zDlg.cursel-2].BlockName;
//				if (zDlg.circsel==0) blocklist[i].InCircuit="<None>";
//				else blocklist[i].InCircuit=circproplist[zDlg.circsel-1].CircName;
//				blocklist[i].InGroup=zDlg.m_ingroup;
//				blocklist[i].IsExternal=zDlg.m_isexternal;
//			}
//		}
//		return true;
//	}
//
//	return false;
//}

//void FMesher::OpNodeDlg()
//{
//	int i,k,nselected;
//	COpNodeDlg zDlg;
//
//	// check to see how many (if any) nodes are selected.
//	for(i=0,k=0,nselected=0;i<nodelist.size();i++)
//	{
//		if (nodelist[i].IsSelected!=0){
//			if(nselected==0){
//				nselected++;
//				zDlg.m_ingroup=nodelist[i].InGroup;
//			}
//			else if(nodelist[i].BoundaryMarkerName!=nodelist[k].BoundaryMarkerName)
//				nselected++;
//			if(nodelist[i].InGroup!=zDlg.m_ingroup) zDlg.m_ingroup=0;
//			k=i;
//		}
//	}
//	if (nselected==0) return;
//
//	zDlg.pnodeproplist=&nodeproplist;
//	if (nselected==1){
//		for(i=0,zDlg.cursel=0;i<nodeproplist.size();i++)
//			if (nodeproplist[i].PointName==nodelist[k].BoundaryMarkerName)
//				zDlg.cursel=i+1;
//	}
//	else zDlg.cursel=0;
//
//	if (zDlg.DoModal()==IDOK){
//		for(i=0;i<nodelist.size();i++)
//		{
//			if(nodelist[i].IsSelected!=0){
//				if (zDlg.cursel==0) nodelist[i].BoundaryMarkerName="<None>";
//				else nodelist[i].BoundaryMarkerName=nodeproplist[zDlg.cursel-1].PointName;
//				nodelist[i].InGroup=zDlg.m_ingroup;
//			}
//		}
//	}
//}

//void FMesher::OpSegDlg()
//{
//	int i,j,k,nselected;
//	COpSegDlg zDlg;
//
//	// check to see how many (if any) nodes are selected.
//	for(i=0,k=0,nselected=0;i<linelist.size();i++)
//	{
//		if (linelist[i].IsSelected!=0){
//			if(nselected==0){
//				nselected++;
//				zDlg.m_ingroup=linelist[i].InGroup;
//			}
//			else if(linelist[i].BoundaryMarkerName!=linelist[k].BoundaryMarkerName)
//				nselected++;
//			if(linelist[i].InGroup!=zDlg.m_ingroup) zDlg.m_ingroup=0;
//			k=i;
//		}
//	}
//	if (nselected==0) return;
//
//	// find properties block size;
//	double ms=0;
//	zDlg.m_hide=false;
//	for(i=0,j=0;i<linelist.size();i++)
//		if(linelist[i].IsSelected!=0){
//			if(linelist[i].MaxSideLength<0) ms=-1;
//			if (ms>=0) ms+=linelist[k].MaxSideLength;
//			j++;
//			if (linelist[i].Hidden==true) zDlg.m_hide=true;
//		}
//	ms/=(double) j;
//
//	zDlg.plineproplist=&lineproplist;
//	if (nselected==1){
//		for(i=0,zDlg.cursel=0;i<lineproplist.size();i++)
//			if (lineproplist[i].BdryName==linelist[k].BoundaryMarkerName)
//				zDlg.cursel=i+1;
//	}
//	else zDlg.cursel=0;
//
//	if (ms<0){
//		zDlg.m_automesh=true;
//		zDlg.m_linemeshsize=0;
//	}
//	else{
//		zDlg.m_automesh=false;
//		zDlg.m_linemeshsize=ms;
//	}
//
//	if (zDlg.DoModal()==IDOK){
//		for(i=0;i<linelist.size();i++)
//		{
//			if(linelist[i].IsSelected!=0){
//
//				if (zDlg.m_automesh==true) linelist[i].MaxSideLength=-1;
//				else{
//					if (zDlg.m_linemeshsize>0)
//						linelist[i].MaxSideLength=zDlg.m_linemeshsize;
//					else zDlg.m_linemeshsize=-1;
//				}
//				if (zDlg.cursel==0) linelist[i].BoundaryMarkerName="<None>";
//				else linelist[i].BoundaryMarkerName=lineproplist[zDlg.cursel-1].BdryName;
//
//				linelist[i].Hidden=zDlg.m_hide;
//				linelist[i].InGroup=zDlg.m_ingroup;
//			}
//		}
//	}
//}

//void FMesher::OpArcSegDlg()
//{
//	int i,j,k,nselected;
//	COpArcSegDlg zDlg;
//
//	// check to see how many (if any) arcs are selected.
//
//
//	for(i=0,k=0,nselected=0;i<arclist.size();i++)
//	{
//		if (arclist[i].IsSelected!=0){
//			if(nselected==0){
//				nselected++;
//				zDlg.m_ingroup=arclist[i].InGroup;
//			}
//			else if(arclist[i].BoundaryMarkerName!=arclist[k].BoundaryMarkerName)
//				nselected++;
//			if(arclist[i].InGroup!=zDlg.m_ingroup) zDlg.m_ingroup=0;
//			k=i;
//		}
//	}
//	if (nselected==0) return;
//
//	// find properties block size;
//	double ms=0;
//	zDlg.m_hide=false;
//	for(i=0,j=0;i<arclist.size();i++)
//		if(arclist[i].IsSelected!=0){
//			ms+=arclist[k].MaxSideLength;
//			j++;
//			if(arclist[i].Hidden==true) zDlg.m_hide=true;
//		}
//
//	ms/=(double) j;
//
//	zDlg.plineproplist=&lineproplist;
//	if (nselected==1){
//		zDlg.m_MaxSeg=ms;
//		for(i=0,zDlg.cursel=0;i<lineproplist.size();i++)
//			if (lineproplist[i].BdryName==arclist[k].BoundaryMarkerName)
//				zDlg.cursel=i+1;
//	}
//	else{
//		zDlg.cursel=0;
//		zDlg.m_MaxSeg=ms;
//	}
//
//	if (zDlg.DoModal()==IDOK){
//		for(i=0;i<arclist.size();i++)
//		{
//			if(arclist[i].IsSelected!=0){
//				if (zDlg.cursel==0) arclist[i].BoundaryMarkerName="<None>";
//				else arclist[i].BoundaryMarkerName=lineproplist[zDlg.cursel-1].BdryName;
//				arclist[i].MaxSideLength=zDlg.m_MaxSeg;
//				arclist[i].Hidden=zDlg.m_hide;
//				arclist[i].InGroup=zDlg.m_ingroup;
//			}
//		}
//	}
//}

//void FMesher::OpGrpDlg()
//{
//	COpGrp dlg;
//	bool bFlag=false;
//	int nsel=0;
//	int i;
//
//	dlg.m_ingroup=0;
//
//	for(i=0;i<arclist.size();i++)
//		if (arclist[i].IsSelected!=0){
//			nsel++;
//			if(dlg.m_ingroup!=arclist[i].InGroup)
//			{
//				if(bFlag==false){
//					dlg.m_ingroup=arclist[i].InGroup;
//					bFlag=true;
//				}
//				else dlg.m_ingroup=0;
//			}
//		}
//
//	for(i=0;i<linelist.size();i++)
//		if (linelist[i].IsSelected!=0)
//		{
//			nsel++;
//			if(dlg.m_ingroup!=linelist[i].InGroup)
//			{
//				if(bFlag==false){
//					dlg.m_ingroup=linelist[i].InGroup;
//					bFlag=true;
//				}
//				else dlg.m_ingroup=0;
//			}
//		}
//
//	for(i=0;i<blocklist.size();i++)
//		if (blocklist[i].IsSelected!=0)
//		{
//			nsel++;
//			if(dlg.m_ingroup!=blocklist[i].InGroup)
//			{
//				if(bFlag==false){
//					dlg.m_ingroup=blocklist[i].InGroup;
//					bFlag=true;
//				}
//				else dlg.m_ingroup=0;
//			}
//		}
//
//	for(i=0;i<nodelist.size();i++)
//		if (nodelist[i].IsSelected!=0)
//		{
//			nsel++;
//			if(dlg.m_ingroup!=nodelist[i].InGroup)
//			{
//				if(bFlag==false){
//					dlg.m_ingroup=nodelist[i].InGroup;
//					bFlag=true;
//				}
//				else dlg.m_ingroup=0;
//			}
//		}
//	if (nsel==0) return;
//
//	if(dlg.DoModal()==IDOK){
//
//		for(i=0;i<nodelist.size();i++)
//			if(nodelist[i].IsSelected!=0)
//				nodelist[i].InGroup=dlg.m_ingroup;
//
//		for(i=0;i<linelist.size();i++)
//			if(linelist[i].IsSelected!=0)
//				linelist[i].InGroup=dlg.m_ingroup;
//
//		for(i=0;i<arclist.size();i++)
//			if(arclist[i].IsSelected!=0)
//				arclist[i].InGroup=dlg.m_ingroup;
//
//		for(i=0;i<blocklist.size();i++)
//			if(blocklist[i].IsSelected!=0)
//				blocklist[i].InGroup=dlg.m_ingroup;
//
//	}
//
//}
