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

// fmesher.cpp : implementation of FMesher Class
//

#include <iostream>
#include <cstdio>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include "fparse.h"
#include "fmesher.h"
#include "intpoint.h"
#include "triangle.h"

//extern void *pFemmeDoc;
//extern lua_State *lua;
//extern CLuaConsoleDlg *LuaConsole;
//extern bool bLinehook;

//extern void *triangulate;

using namespace std;
using namespace femm;

FMesher::FMesher()
{
    // initialise the warning message function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;

    TriMessage = NULL;

    Verbose = true;

    filetype = F_TYPE_UNKNOWN;


    // initialize the problem data structures
    // and default behaviour etc.
    Initialize();

}

FMesher::FMesher(string PathName)
{
    // initialise the warning message function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;

    TriMessage = NULL;

    Verbose = true;

    filetype = F_TYPE_UNKNOWN;


    // initialize the problem data structures
    // and default behaviour etc.
    Initialize();

    LoadFEMFile(PathName);
}

bool FMesher::Initialize()
{
    DoForceMaxMeshArea = false;
    DoSmartMesh = false;

    // set up some default behaviors
    d_minangle=30.;

    // fire up lua
    //initalise_lua();

    // clear out all current lines, nodes, and block labels
    nodelist.clear ();
    linelist.clear ();
    arclist.clear ();
    blocklist.clear ();
    undonodelist.clear ();
    undolinelist.clear ();
    undoarclist.clear ();
    undoblocklist.clear ();
    nodeproplist.clear ();
    lineproplist.clear ();
//    blockproplist.clear ();
    circproplist.clear ();
    meshnode.clear ();
    meshline.clear ();
    greymeshline.clear ();
    probdescstrings.clear ();

    // set problem attributes to generic ones;
    MinAngle = d_minangle;

    return true;
}


void FMesher::UnselectAll()
{
    unsigned int i;

    for(i=0; i < nodelist.size(); i++) nodelist[i].IsSelected=0;
    for(i=0; i < linelist.size(); i++) linelist[i].IsSelected=0;
    for(i=0; i < blocklist.size(); i++) blocklist[i].IsSelected=0;
    for(i=0; i < arclist.size(); i++) arclist[i].IsSelected=0;
}


void FMesher::GetCircle(CArcSegment &arc, CComplex &c, double &R)
{
    CComplex a0,a1,t;
    double d,tta;

    // construct the coordinates of the two points on the circle
    a0.Set(nodelist[arc.n0].x,nodelist[arc.n0].y);
    a1.Set(nodelist[arc.n1].x,nodelist[arc.n1].y);

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

    p0.Set(nodelist[seg.n0].x,nodelist[seg.n0].y);
    p1.Set(nodelist[seg.n1].x,nodelist[seg.n1].y);
    a0.Set(nodelist[arc.n0].x,nodelist[arc.n0].y);
    a1.Set(nodelist[arc.n1].x,nodelist[arc.n1].y);
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

    a0.Set(nodelist[arc0.n0].x,nodelist[arc0.n0].y);
    a1.Set(nodelist[arc1.n0].x,nodelist[arc1.n0].y);

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

    if(nodelist.size()==0)
    {
        return -1;
    }

    j=0;
    d0=nodelist[0].GetDistance(x,y);
    for(i=0; i<nodelist.size(); i++)
    {
        d1=nodelist[i].GetDistance(x,y);
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

    if(blocklist.size()==0)
    {
        return -1;
    }

    j=0;
    d0=blocklist[0].GetDistance(x,y);
    for(i=0; i<blocklist.size(); i++)
    {
        d1=blocklist[i].GetDistance(x,y);
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

    a0.Set(nodelist[arc.n0].x,nodelist[arc.n0].y);
    a1.Set(nodelist[arc.n1].x,nodelist[arc.n1].y);
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

    x[0]=nodelist[linelist[segm].n0].x;
    y[0]=nodelist[linelist[segm].n0].y;
    x[1]=nodelist[linelist[segm].n1].x;
    y[1]=nodelist[linelist[segm].n1].y;

    t=((p-x[0])*(x[1]-x[0]) + (q-y[0])*(y[1]-y[0]))/
      ((x[1]-x[0])*(x[1]-x[0]) + (y[1]-y[0])*(y[1]-y[0]));

    if (t>1.) t=1.;
    if (t<0.) t=0.;

    x[2]=x[0]+t*(x[1]-x[0]);
    y[2]=y[0]+t*(y[1]-y[0]);

    return sqrt((p-x[2])*(p-x[2]) + (q-y[2])*(q-y[2]));
}

int FMesher::ClosestSegment(double x, double y)
{
    double d0,d1;
    unsigned int i,j;

    if(linelist.size()==0)
    {
        return -1;
    }

    j=0;
    d0=ShortestDistance(x,y,0);
    for(i=0; i<linelist.size(); i++)
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

    if(arclist.size()==0)
    {
        return -1;
    }

    j=0;
    d0=ShortestDistanceFromArc(CComplex(x,y),arclist[0]);
    for(i=0; i<arclist.size(); i++)
    {
        d1=ShortestDistanceFromArc(CComplex(x,y),arclist[i]);
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
    if (n0==linelist[segm].n0) return false;
    if (n0==linelist[segm].n1) return false;
    if (n1==linelist[segm].n0) return false;
    if (n1==linelist[segm].n1) return false;

    // Get a definition of "real small" based on the lengths
    // of the lines of interest;
    p0=nodelist[linelist[segm].n0].CC();
    p1=nodelist[linelist[segm].n1].CC();
    q0=nodelist[n0].CC();
    q1=nodelist[n1].CC();
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
    p0 = (1.0 - z)*nodelist[n0].CC() + z*nodelist[n1].CC();
    *xi=Re(p0);
    *yi=Im(p0);

    return true;
}


int FMesher::GetFileType (string PathName)
{
    // find the position of the last '.' in the string
    size_t dotpos = PathName.rfind ('.');

    if (dotpos == string::npos)
    {
        // no '.' found
        return F_TYPE_UNKNOWN;
    }

    // compare different file extensions and return the appropriate string
    if ( PathName.compare (dotpos, string::npos, ".fem") == 0 )
    {
        return F_TYPE_MAGNETICS;
    }
    else if ( PathName.compare (dotpos, string::npos, ".feh") == 0 )
    {
        return F_TYPE_HEATFLOW;
    }
    else
    {
        return F_TYPE_UNKNOWN;
    }

}

int FMesher::LoadFEMFile (string PathName, int ftype)
{
    filetype = ftype;
    return LoadFEMFile(PathName);
}

int FMesher::LoadFEMFile (string PathName)
{
    switch (filetype)
    {
        case F_TYPE_UNKNOWN:
            return F_FILE_UNKNOWN_TYPE;
        case F_TYPE_MAGNETICS:
            break;
        case F_TYPE_HEATFLOW:
            break;
        default:
            return F_FILE_UNKNOWN_TYPE;
    }

    // make sure old data is cleared out...
    Initialize();

    FILE *fp;
    int i,k,t;
    int vers=0;
    char s[1024],q[1024];
    char *v;
    CPointProp	  PProp;
    CBoundaryProp BProp;
    CCircuit	  CProp;
    CNode		node;
    CSegment	segm;
    CArcSegment asegm;
    CBlockLabel blk;

    if ((fp=fopen(PathName.c_str(),"rt"))==NULL)
    {
        WarnMessage("Couldn't read from specified .fem file");
        return F_FILE_NOT_OPENED;
    }

    // parse the file
    while (fgets(s,1024,fp)!=NULL)
    {
        if (sscanf(s,"%s",q)==EOF) q[0] = '\0';
        //	int _strnicmp( const char *string1, const char *string2, size_t count );

        // Deal with flag for file format version
        if( _strnicmp(q,"[format]",8)==0 )
        {
            addFileStr (s);
            v = StripKey(s);
            double dblvers;
            sscanf(v,"%lf",&dblvers);
            vers = (int) (10.*dblvers + 0.5);
            if(vers>40)
            {
                WarnMessage("This file is from a newer version of FEMM\n"
                            "This file may contain attributes not\n"
                            "supported by this version of FEMM");
            }
            q[0] = '\0';
            continue;
        }

        // Minimum Angle Constraint for finite element mesh
        if( _strnicmp(q,"[minangle]",10)==0)
        {
            addFileStr (s);
            v = StripKey(s);
            sscanf(v,"%lf",&MinAngle);
            q[0] = '\0';
            continue;
        }

        // Option to force use of default max mesh, overriding
        // user choice
        if( _strnicmp(q,"[forcemaxmesh]",14)==0)
        {
            int temp = 0;
            v = StripKey(s);
            sscanf(v,"%i",&temp);
            q[0] = '\0';
            // 0 == do not override user mesh choice
            // not 0 == do override user mesh choice
            if (temp == 0)
            {
                DoForceMaxMeshArea = false;
            }
            else
            {
                DoForceMaxMeshArea = true;
            }
            addFileStr (s);
        }

        // Option to use smart meshing
        if( _strnicmp(q,"[dosmartmesh]",13)==0)
        {
            int temp = 0;
            v = StripKey(s);
            sscanf(v,"%i",&temp);
            q[0] = '\0';
            // 0 == do not use smart mesh
            // not 0 == use smart mesh
            if (temp == 0)
            {
                DoSmartMesh = false;
            }
            else
            {
                DoSmartMesh = true;
            }
            addFileStr (s);
        }

        // Point Properties
        if( _strnicmp(q,"<beginpoint>",11)==0)
        {
            string ppropstring (s);

            PProp.PointName="New Point Property";

            while (_strnicmp(q,"<endpoint>",9) != 0)
            {
                q[0] = '\0';
                fgets(s,1024,fp);

                // if we've reached the end of file return an error as
                // it must be malformed
                if (sscanf(s,"%s",q)==EOF)
                {
                    return F_FILE_MALFORMED;
                }

                // append the line to the property string
                ppropstring += s;

                if( _strnicmp(q,"<pointname>",11)==0)
                {
                    v = StripKey(s);
                    k=strlen(v);
                    for(i=0; i<k; i++)
                        if(v[i]=='\"')
                        {
                            v=v+i+1;
                            i=k;
                        }
                    k=strlen(v);
                    if(k>0) for(i=k-1; i>=0; i--)
                        {
                            if(v[i]=='\"')
                            {
                                v[i]=0;
                                i=-1;
                            }
                        }
                    PProp.PointName = v;
                }

                if( _strnicmp(q,"<bdrytype>",10)==0)
                {
                    v = StripKey(s);
                    sscanf(v,"%i",&BProp.BdryFormat);
                }

            }

            // add the point property string to the list of strings
            probdescstrings.push_back (ppropstring);

            q[0] = '\0';

            continue;
        }

        // Boundary Properties;
        if( _strnicmp(q,"<beginbdry>",11)==0)
        {
            // string to hold a copy of the boundary string
            string boundstring (s);

            BProp.BdryName="New Boundary";
            BProp.BdryFormat=0;

            while (_strnicmp(q,"<endbdry>",9) != 0)
            {
                q[0] = '\0';

                fgets(s,1024,fp);

                // if we've reached the end of file return an error as
                // it must be malformed
                if (sscanf(s,"%s",q)==EOF)
                {
                    return F_FILE_MALFORMED;
                }

                // append the line to the boundary string
                boundstring += s;

                if( _strnicmp(q,"<bdryname>",10)==0)
                {
                    v = StripKey(s);
                    k = strlen(v);
                    for(i=0; i<k; i++)
                    {
                        if(v[i]=='\"')
                        {
                            v=v+i+1;
                            i=k;
                        }
                    }

                    k = strlen(v);

                    if(k>0) for(i=k-1; i>=0; i--)
                    {
                        if(v[i]=='\"')
                        {
                            v[i]=0;
                            i=-1;
                        }
                    }

                    BProp.BdryName = v;
                }

                if( _strnicmp(q,"<bdrytype>",10)==0)
                {
                    v = StripKey(s);
                    sscanf(v,"%i",&BProp.BdryFormat);
                }

            }

            lineproplist.push_back (BProp);

            q[0] = '\0';

            // add the boundary string to the list of strings
            probdescstrings.push_back (boundstring);

            continue;
        }

        // Block Properties;
        if( _strnicmp(q,"<beginblock>",12) == 0)
        {
            string mpropstring (s);

            while ( _strnicmp(q,"<endblock>",10) != 0)
            {
                q[0] = '\0';
                fgets(s,1024,fp);

                // if we've reached the end of file return an error as
                // it must be malformed
                if (sscanf(s,"%s",q)==EOF)
                {
                    return F_FILE_MALFORMED;
                }

                // append the line to the boundary string
                mpropstring += s;
            }

            q[0] = '\0';

            // add the material property string to the list of strings
            probdescstrings.push_back (mpropstring);
            continue;
        }


        // Conductor/Circuit Properties
        if ( ( _strnicmp(q,"<begincircuit>",14)==0)
             || ( _strnicmp(q,"<beginconductor>",16)==0) )
        {
            CProp.CircName = "No Name";
            CProp.CircType = 0;

            // add the string to the list of strings to be echoed later
            addFileStr (s);
            q[0] = '\0';
            continue;
        }

        if ( ( _strnicmp(q,"<circuitname>",13)==0)
             || ( _strnicmp(q,"<conductorname>",15)==0) )
        {
            // add the string to the list of strings to be echoed later
            addFileStr (s);
            v = StripKey(s);
            k=strlen(v);
            for(i=0; i<k; i++)
                if(v[i]=='\"')
                {
                    v=v+i+1;
                    i=k;
                }
            k=strlen(v);
            if(k>0) for(i=k-1; i>=0; i--)
                {
                    if(v[i]=='\"')
                    {
                        v[i]=0;
                        i=-1;
                    }
                }
            CProp.CircName=v;
            q[0] = '\0';
            continue;
        }

        if ( ( _strnicmp(q,"<circuittype>",13)==0)
             || ( _strnicmp(q,"<conductortype>",15)==0) )
        {
            // add the string to the list of strings to be echoed later
            addFileStr (s);
            v = StripKey(s);
            sscanf(v,"%i",&CProp.CircType);
            q[0] = '\0';
            continue;
        }

        if ( ( _strnicmp(q,"<endcircuit>",12)==0)
             || ( _strnicmp(q,"<endconductor>",14)==0) )
        {
            // add the string to the list of strings to be echoed later
            addFileStr (s);
            circproplist.push_back(CProp);
            q[0] = '\0';
            continue;
        }

        // Points list;
        if(_strnicmp(q,"[numpoints]",11)==0)
        {
            // note we don't save the points to be echoed later, we just
            // write them out again from the node list
            v = StripKey(s);
            sscanf(v,"%i",&k);
            for(i=0; i<k; i++)
            {
                fgets(s,1024,fp);

                v=ParseDbl(s,&node.x);
                v=ParseDbl(v,&node.y);
                v=ParseInt(v,&t);

                switch (filetype)
                {
                    case F_TYPE_MAGNETICS:


                        v=ParseInt(v,&node.InGroup);

                        if (t==0)
                        {
                            node.BoundaryMarker =" ";
                        }
                        else if (t<= (int)nodeproplist.size())
                        {
                            node.BoundaryMarker = nodeproplist[t-1].PointName;
                        }

                        break;

                    case F_TYPE_HEATFLOW:


                        if (t==0)
                        {
                            node.BoundaryMarker =" ";
                        }
                        else if (t<= (int)nodeproplist.size())
                        {
                            node.BoundaryMarker = nodeproplist[t-1].PointName;
                        }

                        v = ParseInt(v,&node.InGroup);
                        v = ParseInt(v,&t);

                        if(t==0)
                        {
                            node.InConductor = "<None>";
                        }
        				else if(t <= (int)circproplist.size())
		        		{
				            node.InConductor = circproplist[t-1].CircName;
                        }

                        break;
                }

                nodelist.push_back(node);
            }
            q[0] = '\0';
            continue;
        }

        // read in segment list
        if(_strnicmp(q,"[numsegments]",13)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&k);
            for(i=0; i<k; i++)
            {
                fgets(s,1024,fp);

                // some defaults
                segm.MaxSideLength = 0;
                t = 0;
                segm.Hidden = 0;
                segm.InGroup = 0;
                segm.InConductor="<None>";

                // scan in data
                v = ParseInt(s,&segm.n0);
                v = ParseInt(v,&segm.n1);
                v = ParseDbl(v,&segm.MaxSideLength);
                v = ParseInt(v,&t);

                if (t == 0)
                {
                   segm.BoundaryMarker="";
                }
                else if (t<=(int) lineproplist.size())
                {
                    segm.BoundaryMarker = lineproplist[t-1].BdryName;
                }

                int Hidden = 0;
                v = ParseInt(v,&Hidden);

                segm.Hidden = Hidden;

                v = ParseInt(v,&segm.InGroup);

                switch (filetype)
                {
                    case F_TYPE_MAGNETICS:

                        // nothing to do

                        break;

                    case F_TYPE_HEATFLOW:

                        // get the conductor number
                        v = ParseInt(v,&t);

                        if(t==0)
                        {
                            segm.InConductor = "<None>";
                        }
                        else if(t<=(int) circproplist.size())
                        {
                            segm.InConductor = circproplist[t-1].CircName;
                        }

                        break;

                }
                linelist.push_back(segm);
            }
            q[0] = '\0';
            continue;
        }

        // read in arc segment list
        if(_strnicmp(q,"[numarcsegments]",13)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&k);
            for(i=0; i<k; i++)
            {
                fgets(s,1024,fp);

                // some defaults
                asegm.Hidden = 0;
                asegm.MaxSideLength = -1.0;
                asegm.InGroup = 0;
                t = 0;

                // scan in data
                v = ParseInt(s,&asegm.n0);
                v = ParseInt(v,&asegm.n1);
                v = ParseDbl(v,&asegm.ArcLength);
                v = ParseDbl(v,&asegm.MaxSideLength);
                v = ParseInt(v,&t);

                if(t==0)
                {
                    asegm.BoundaryMarker = "";
                }
                else if (t <= (int)lineproplist.size())
                {
                    asegm.BoundaryMarker = lineproplist[t-1].BdryName;
                }
                t = 0;

                int Hidden = 0;
                v = ParseInt(v,&Hidden);
                asegm.Hidden = Hidden;

                v = ParseInt(v,&asegm.InGroup);

                switch (filetype)
                {
                    case F_TYPE_MAGNETICS:

                        // nothing to do

                        break;

                    case F_TYPE_HEATFLOW:

                        // get conductor number
                        if (v != NULL)
                        {
                            v = ParseInt(v,&t);
                        }
                        else
                        {
                            t = 0;
                        }

                        if (t == 0)
                        {
                            segm.InConductor="<None>";
                        }
                        else if(t<=(int) circproplist.size ())
                        {
                            asegm.InConductor = circproplist[t-1].CircName;
                        }

                        break;

                }


                arclist.push_back(asegm);
            }
            q[0] = '\0';
            continue;
        }

        // read in list of holes;
        if(_strnicmp(q,"[numholes]",13) == 0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&k);
            if (k > 0)
            {
                blk.BlockType = "<No Mesh>";
                blk.MaxArea = 0;
                blk.InGroup = 0;
                for(i=0; i<k; i++)
                {
                    fgets(s,1024,fp);
                    v = ParseDbl(s,&blk.x);
                    v = ParseDbl(v,&blk.y);
                    v = ParseInt(v,&blk.InGroup);

                    blocklist.push_back(blk);
                }
            }
            q[0] = '\0';
            continue;
        }

        // read in regional attributes
        if (_strnicmp(q,"[numblocklabels]",13) == 0)
        {
            addFileStr (s);
            v = StripKey (s);
            sscanf (v,"%i",&k);

            for (i = 0; i < k; i++)
            {
                fgets (s,1024,fp);
                addFileStr (s);

                //some defaults
                t = 0;
                blk.BlockType = "";
                blk.MaxArea = 0.;
                blk.InCircuit = "<None>";
                blk.InGroup = 0;

                // scan in data
                v = ParseDbl (s,&blk.x);
                v = ParseDbl (v,&blk.y);

                // block type, not used in meshing
                v = ParseInt (v,&t);

                // max area constraint for triangles in this region
                v = ParseDbl (v,&blk.MaxArea);

                if (blk.MaxArea < 0)
                {
                    blk.MaxArea = 0;
                }
                else
                {
                    blk.MaxArea = PI * blk.MaxArea * blk.MaxArea / 4.0;
                }

                int ignoreint = 0;
                double ignoredbl = 0.0;

                switch (filetype)
                {
                    case F_TYPE_MAGNETICS:
                        // an int and a double come before the InGroup value
                        v = ParseInt (v,&ignoreint);

                        v = ParseDbl (v,&ignoredbl );

                        v = ParseInt (v,&blk.InGroup);

                        break;

                    case F_TYPE_HEATFLOW:
                        // The InGroup value is next
                        v = ParseInt (v,&blk.InGroup);

                        break;

                    default:
                        return F_FILE_UNKNOWN_TYPE;
                }

                blocklist.push_back(blk);
            }
            q[0] = '\0';
            continue;
        }

        // add the string to the list of strings to be echoed later
        // if it was not parsed by any of the preceeding statements
        addFileStr (s);

    }

    fclose(fp);

    return F_FILE_OK;
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
    fprintf(fp,"[NumPoints] = %i\n", (int) nodelist.size());
    for(i=0; i<nodelist.size(); i++)
    {
        for(j=0,t=0; j<nodeproplist.size(); j++)
            if(nodeproplist[j].PointName==nodelist[i].BoundaryMarker) t=j+1;
        fprintf(fp,"%.17g\t%.17g\t%i\t%i",nodelist[i].x,nodelist[i].y,t,
                nodelist[i].InGroup);

        if (filetype == F_TYPE_HEATFLOW)
        {
            for (j=0,t=0; j<circproplist.size (); j++)
                if (circproplist[j].CircName==nodelist[i].InConductor) t=j+1;

            fprintf(fp,"\t%i",t);
        }

        fprintf(fp,"\n");
    }

    // write out segment list
    fprintf(fp,"[NumSegments] = %i\n", (int) linelist.size());
    for(i=0; i<linelist.size(); i++)
    {
        for(j=0,t=0; j<lineproplist.size(); j++)
            if(lineproplist[j].BdryName==linelist[i].BoundaryMarker) t=j+1;

        fprintf(fp,"%i\t%i\t",linelist[i].n0,linelist[i].n1);

        if(linelist[i].MaxSideLength<0)
        {
            fprintf(fp,"-1\t");
        }
        else
        {
            fprintf(fp,"%.17g\t",linelist[i].MaxSideLength);
        }

        fprintf(fp,"%i\t%i\t%i",t,linelist[i].Hidden,linelist[i].InGroup);

        if (filetype == F_TYPE_HEATFLOW)
        {
            for(j=0,t=0;j<circproplist.size ();j++)
            {
                if(circproplist[j].CircName==linelist[i].InConductor) t = j + 1;
            }
            fprintf(fp,"\t%i",t);
        }

        fprintf(fp,"\n");
    }

    // write out arc segment list
    fprintf(fp,"[NumArcSegments] = %i\n", (int) arclist.size());
    for(i=0; i<arclist.size(); i++)
    {
        for(j=0,t=0; j<lineproplist.size(); j++)
            if(lineproplist[j].BdryName==arclist[i].BoundaryMarker) t=j+1;
        fprintf(fp,"%i\t%i\t%.17g\t%.17g\t%i\t%i\t%i",arclist[i].n0,arclist[i].n1,
                arclist[i].ArcLength,arclist[i].MaxSideLength,t,
                arclist[i].Hidden,arclist[i].InGroup);

        if (filetype == F_TYPE_HEATFLOW)
        {
            for(j=0,t=0;j<circproplist.size ();j++)
				if(circproplist[j].CircName==arclist[i].InConductor) t=j+1;
            fprintf(fp,"\t%i",t);
        }
        fprintf(fp,"\n");
    }

    // write out list of holes;
    for(i=0,j=0; i<blocklist.size(); i++)
    {
        if(blocklist[i].BlockType=="<No Mesh>")
        {
            j++;
        }
    }

    fprintf(fp,"[NumHoles] = %i\n",j);
    for(i=0,k=0; i<blocklist.size(); i++)
    {
        if(blocklist[i].BlockType=="<No Mesh>")
        {
            fprintf(fp,"%.17g\t%.17g\t%i\n",blocklist[i].x,blocklist[i].y,
                    blocklist[i].InGroup);
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
        meshnode[i] = node;
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

    myPoint segm;
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
                    meshline[nl++] = segm;
                }
                else
                {
                    greymeshline.push_back(segm);
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
    undoblocklist.clear();

    for(i=0; i<nodelist.size(); i++) undonodelist.push_back(nodelist[i]);
    for(i=0; i<linelist.size(); i++) undolinelist.push_back(linelist[i]);
    for(i=0; i<arclist.size(); i++) undoarclist.push_back(arclist[i]);
    for(i=0; i<blocklist.size(); i++) undoblocklist.push_back(blocklist[i]);


}

void FMesher::Undo()
{
    unsigned int i;

    std::vector < CNode >       tempnodelist;
    std::vector < CSegment >    templinelist;
    std::vector < CArcSegment > temparclist;
    std::vector < CBlockLabel > tempblocklist;

    tempnodelist.clear();
    templinelist.clear();
    temparclist.clear();
    tempblocklist.clear();

    for(i=0; i<nodelist.size(); i++) tempnodelist.push_back(nodelist[i]);
    for(i=0; i<linelist.size(); i++) templinelist.push_back(linelist[i]);
    for(i=0; i<arclist.size(); i++) temparclist.push_back(arclist[i]);
    for(i=0; i<blocklist.size(); i++) tempblocklist.push_back(blocklist[i]);

    nodelist.clear();
    linelist.clear();
    arclist.clear();
    blocklist.clear();

    for(i=0; i<undonodelist.size(); i++) nodelist.push_back(undonodelist[i]);
    for(i=0; i<undolinelist.size(); i++) linelist.push_back(undolinelist[i]);
    for(i=0; i<undoarclist.size(); i++) arclist.push_back(undoarclist[i]);
    for(i=0; i<undoblocklist.size(); i++) blocklist.push_back(undoblocklist[i]);

    undonodelist.clear();
    undolinelist.clear();
    undoarclist.clear();
    undoblocklist.clear();

    for(i=0; i<tempnodelist.size(); i++) undonodelist.push_back(tempnodelist[i]);
    for(i=0; i<templinelist.size(); i++) undolinelist.push_back(templinelist[i]);
    for(i=0; i<temparclist.size(); i++) undoarclist.push_back(temparclist[i]);
    for(i=0; i<tempblocklist.size(); i++) undoblocklist.push_back(tempblocklist[i]);
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



//bool FMesher::CanCreateRadius(int n)
//{
//	// check to see if a selected point, specified by n, can be
//	// converted to a radius.  To be able to be converted to a radius,
//	// the point must be an element of either 2 lines, 2 arcs, or
//	// 1 line and 1 arc.
//
//	int j,k;
//
//	for(k=0,j=0;k<linelist.size();k++)
//		if ((linelist[k].n0==n) || (linelist[k].n1==n)) j++;
//	for(k=0;k<arclist.size();k++)
//		if ((arclist[k].n0==n) || (arclist[k].n1==n)) j++;
//
//	if (j==2) return true;
//
//	return false;
//}
//
//bool FMesher::CreateRadius(int n, double r)
//{
//	// replace the node indexed by n with a radius of r
//
//	if(r<=0) return false;
//
//	int nseg=0;
//	int narc=0;
//	int k,arc[2],seg[2];
//
//	for(k=0;k<linelist.size();k++)
//		if ((linelist[k].n0==n) || (linelist[k].n1==n)) seg[nseg++]=k;
//	for(k=0;k<arclist.size();k++)
//		if ((arclist[k].n0==n) || (arclist[k].n1==n)) arc[narc++]=k;
//	if ((narc+nseg)!=2) return false;
//
//	// there are three valid cases:
//	switch (nseg-narc)
//	{
//		case 0:  // One arc and one line
//		{
//			CComplex c,u,p0,p1,q,p[4],v[8],i1[8],i2[8];
//			double rc,b,R[4],phi;
//			CArcSegment ar;
//			int j,m;
//
//			// inherit the boundary condition from the arc so that
//			// we can apply it to the newly created arc later;
//			ar.InGroup       =arclist[arc[0]].InGroup;
//			ar.BoundaryMarker=arclist[arc[0]].BoundaryMarker;
//
//			// get the center and radius of the circle associated with the arc;
//			GetCircle(arclist[arc[0]],c,rc);
//
//			// get the locations of the endpoints of the segment;
//			p0=nodelist[n].CC();
//			if(linelist[seg[0]].n0==n)
//				p1=nodelist[linelist[seg[0]].n1].CC();
//			else
//				p1=nodelist[linelist[seg[0]].n0].CC();
//
//			u=(p1-p0)/abs(p1-p0);  // unit vector along the line
//			q=p0 + u*Re((c-p0)/u); // closest point on line to center of circle
//			u=(q-c)/abs(q-c); // unit vector from center to closest point on line;
//
//			p[0]=q+r*u; R[0]=rc+r;
//			p[1]=q-r*u; R[1]=rc+r;
//			p[2]=q+r*u; R[2]=rc-r;
//			p[3]=q-r*u; R[3]=rc-r;
//
//			for(j=0,k=0;k<4;k++)
//			{
//				b=R[k]*R[k]-abs(p[k]-c)*abs(p[k]-c);
//				if (b>=0){
//					b=sqrt(b);
//					v[j++]=p[k]+I*b*(p[k]-c)/abs(p[k]-c);
//					v[j++]=p[k]-I*b*(p[k]-c)/abs(p[k]-c);
//				}
//			}
//
//			// locations of the centerpoints that could be for the radius that
//			// we are looking for are stored in v.  We now need to paw through
//			// them to find the one solution that we are after.
//			u=(p1-p0)/abs(p1-p0);  // unit vector along the line
//			for(m=0,k=0;k<j;k++)
//			{
//				i1[m]=p0 +u*Re((v[k]-p0)/u); // intersection with the line
//				i2[m]=c + rc*(v[k]-c)/abs(v[k]-c); // intersection with the arc;
//				v[m]=v[k];
//
//				// add this one to the list of possibly valid solutions if
//				// both of the intersection points actually lie on the arc
//				if ( ShortestDistanceFromArc(i2[m],arclist[arc[0]])<(r/10000.) &&
//					 ShortestDistance(Re(i1[m]),Im(i1[m]),seg[0])<(r/10000.)
//					 && abs(i1[m]-i2[m])>(r/10000.))
//				{
//					m++;
//					if (m==2) break;
//				}
//
//			}
//
//			if (m==0) return false;
//
//			// But there are also special cases where there could be two answers.
//			// We then pick the solution that has the center point closest to the point to be removed.
//			if(m>1)
//			{
//				if (abs(v[0]-p0)<abs(v[1]-p0)) j=0;
//				else j=1;
//			}
//			else j=0;	// The index of the winning case is in j....
//
//			UpdateUndo();
//			AddNode(Re(i1[j]),Im(i1[j]),r/10000.);
//			AddNode(Re(i2[j]),Im(i2[j]),r/10000.);
//			UnselectAll();
//
//			// delete the node that is to be replace by a radius;
//			n=ClosestNode(Re(p0),Im(p0));
//			nodelist[n].IsSelected=1;
//			DeleteSelectedNodes();
//
//			// compute the angle spanned by the new arc;
//			phi=arg((i2[j]-v[j])/(i1[j]-v[j]));
//			if (phi<0)
//			{
//				c=i2[j]; i2[j]=i1[j]; i1[j]=c;
//				phi=fabs(phi);
//			}
//
//			// add in the new radius;
//			ar.n0=ClosestNode(Re(i1[j]),Im(i1[j]));
//			ar.n1=ClosestNode(Re(i2[j]),Im(i2[j]));
//			ar.ArcLength=phi/DEG;
//			AddArcSegment(ar);
//
//			return true;
//		}
//		case 2:  // Two lines
//		{
//			CComplex p0,p1,p2;
//			double phi,len;
//			CArcSegment ar;
//
//			if (linelist[seg[0]].n0==n) p1=nodelist[linelist[seg[0]].n1].CC();
//			else p1=nodelist[linelist[seg[0]].n0].CC();
//
//			if (linelist[seg[1]].n0==n) p2=nodelist[linelist[seg[1]].n1].CC();
//			else p2=nodelist[linelist[seg[1]].n0].CC();
//
//			p0=nodelist[n].CC();
//
//			// get the angle between the lines
//			phi=arg((p2-p0)/(p1-p0));
//
//			// check to see if this case is degenerate
//			if (fabs(phi)>(179.*DEG)) return false;
//
//			// check to see if the points are in the wrong order
//			// and fix it if they are.
//			if (phi<0){
//				p0=p1; p1=p2; p2=p0; p0=nodelist[n].CC();
//				k=seg[0]; seg[0]=seg[1]; seg[1]=k;
//				phi=fabs(phi);
//			}
//
//			len = r/tan(phi/2.); // distance from p0 to the tangency point;
//
//			// catch the case where the desired radius is too big to fit;
//			if ((abs(p1-p0)<len) || (abs(p2-p0)<len)) return false;
//
//			// compute the locations of the tangency points;
//			p1=len*(p1-p0)/abs(p1-p0)+p0;
//			p2=len*(p2-p0)/abs(p2-p0)+p0;
//
//			// inherit the boundary condition from one of the segments
//			// so that we can apply it to the newly created arc later;
//			ar.BoundaryMarker=linelist[seg[0]].BoundaryMarker;
//			ar.InGroup       =linelist[seg[0]].InGroup;
//
//			// add new nodes at ends of radius
//			UpdateUndo();
//			AddNode(Re(p1),Im(p1),len/10000.);
//			AddNode(Re(p2),Im(p2),len/10000.);
//			UnselectAll();
//
//			// delete the node that is to be replace by a radius;
//			n=ClosestNode(Re(p0),Im(p0));
//			nodelist[n].IsSelected=1;
//			DeleteSelectedNodes();
//
//			// add in the new radius;
//			ar.n0=ClosestNode(Re(p2),Im(p2));
//			ar.n1=ClosestNode(Re(p1),Im(p1));
//			ar.ArcLength=180.-phi/DEG;
//			AddArcSegment(ar);
//
//			return true;
//		}
//		case -2: // Two arcs
//		{
//			int j;
//			CComplex c0,c1,c2,p[8],i1[8],i2[8];
//			double a[8],b[8],c,d[8],x[8],r0,r1,r2,phi;
//			CArcSegment ar;
//
//			r0=r;
//			GetCircle(arclist[arc[0]],c1,r1);
//			GetCircle(arclist[arc[1]],c2,r2);
//			c=abs(c2-c1);
//
//			// solve for all of the different possible cases;
//			a[0]=r1+r0; b[0]=r2+r0;
//			a[1]=r1+r0; b[1]=r2+r0;
//			a[2]=r1-r0; b[2]=r2-r0;
//			a[3]=r1-r0; b[3]=r2-r0;
//			a[4]=r1-r0; b[4]=r2+r0;
//			a[5]=r1-r0; b[5]=r2+r0;
//			a[6]=r1+r0; b[6]=r2-r0;
//			a[7]=r1+r0; b[7]=r2-r0;
//			for(k=0;k<8;k++){
//				x[k]=(b[k]*b[k]+c*c-a[k]*a[k])/(2.*c*c);
//				d[k]=sqrt(b[k]*b[k]-x[k]*x[k]*c*c);
//			}
//			for(k=0;k<8;k+=2)
//			{
//				// solve for the center point of the radius for each solution.
//				p[k]  =((1-x[k])*c+I*d[k])*(c2-c1)/abs(c2-c1) + c1;
//				p[k+1]=((1-x[k])*c-I*d[k])*(c2-c1)/abs(c2-c1) + c1;
//			}
//
//			c0=nodelist[n].CC();
//
//			for(j=0,k=0;k<8;k++)
//			{
//				i1[j]=c1 + r1*(p[k]-c1)/abs(p[k]-c1); // compute possible intersection points
//				i2[j]=c2 + r2*(p[k]-c2)/abs(p[k]-c2); // with the arcs;
//				p[j] =p[k];
//
//				// add this one to the list of possibly valid solutions if
//				// both of the intersection points actually lie on the arc
//				if ( ShortestDistanceFromArc(i1[j],arclist[arc[0]])<(r0/10000.) &&
//					 ShortestDistanceFromArc(i2[j],arclist[arc[1]])<(r0/10000.) &&
//					 abs(i1[j]-i2[j])>(r0/10000.))
//				{
//					j++;
//					if (j==2) break;
//				}
//
//			}
//
//			// There could be no valid solutions...
//			if (j==0) return false;
//
//			// But there are also special cases where there could be two answers.
//			// We then pick the solution that has the center point closest to the point to be removed.
//			if(j>1)
//			{
//				if (abs(p[0]-c0)<abs(p[1]-c0)) j=0;
//				else j=1;
//			}
//			else j=0;	// The index of the winning case is in j....
//
//			// inherit the boundary condition from one of the segments
//			// so that we can apply it to the newly created arc later;
//			ar.BoundaryMarker=arclist[arc[0]].BoundaryMarker;
//			ar.InGroup=arclist[arc[0]].InGroup;
//
//			// add new nodes at ends of radius
//			UpdateUndo();
//			AddNode(Re(i1[j]),Im(i1[j]),c/10000.);
//			AddNode(Re(i2[j]),Im(i2[j]),c/10000.);
//			UnselectAll();
//
//			// delete the node that is to be replace by a radius;
//			n=ClosestNode(Re(c0),Im(c0));
//			nodelist[n].IsSelected=1;
//			DeleteSelectedNodes();
//
//			// compute the angle spanned by the new arc;
//			phi=arg((i2[j]-p[j])/(i1[j]-p[j]));
//			if (phi<0)
//			{
//				c0=i2[j]; i2[j]=i1[j]; i1[j]=c0;
//				phi=fabs(phi);
//			}
//
//			// add in the new radius;
//			ar.n0=ClosestNode(Re(i1[j]),Im(i1[j]));
//			ar.n1=ClosestNode(Re(i2[j]),Im(i2[j]));
//			ar.ArcLength=phi/DEG;
//			AddArcSegment(ar);
//
//			return true;
//		}
//	}
//
//	return false;
//}

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

//bool FMesher::DeleteSelectedBlockLabels()
//{
//	int i=0;
//	bool flag=false;
//
//	if (blocklist.size() > 0) do{
//		if(blocklist[i].IsSelected != 0){
//			blocklist.RemoveAt(i,1);
//			flag=true;
//		}
//		else i++;
//	} while (i<blocklist.size());
//
//	blocklist.FreeExtra();
//	return flag;
//}

//bool FMesher::DeleteSelectedSegments()
//{
//	int i=0;
//	bool flag=false;
//
//	if (linelist.size() > 0)	do{
//		if(linelist[i].IsSelected != 0){
//			linelist.RemoveAt(i,1);
//			flag=true;
//		}
//		else i++;
//	} while (i<linelist.size());
//
//	linelist.FreeExtra();
//
//	return flag;
//}

//bool FMesher::DeleteSelectedArcSegments()
//{
//	int i=0;
//	bool flag=false;
//
//	if (arclist.size() > 0)	do{
//		if(arclist[i].IsSelected!=0){
//			arclist.RemoveAt(i,1);
//			flag=true;
//		}
//		else i++;
//	} while (i<arclist.size());
//
//	arclist.FreeExtra();
//
//	return flag;
//}

//bool FMesher::DeleteSelectedNodes()
//{
//	int i=0;
//	int j;
//	bool flag=false;
//
//	if (nodelist.size() > 0) do{
//		if(nodelist[i].IsSelected!=0){
//			flag=true;
//			// first remove all lines that contain the point;
//			for(j=0;j<linelist.size();j++)
//				if((linelist[j].n0==i) || (linelist[j].n1==i))
//					linelist[j].ToggleSelect();
//			DeleteSelectedSegments();
//
//			// remove all arcs that contain the point;
//			for(j=0;j<arclist.size();j++)
//				if((arclist[j].n0==i) || (arclist[j].n1==i))
//					arclist[j].ToggleSelect();
//			DeleteSelectedArcSegments();
//
//			// remove node from the nodelist...
//			nodelist.RemoveAt(i,1);
//
//			// update lines to point to the new node numbering
//			for(j=0;j<linelist.size();j++){
//				if (linelist[j].n0>i) linelist[j].n0--;
//				if (linelist[j].n1>i) linelist[j].n1--;
//			}
//
//			// update arcs to point to the new node numbering
//			for(j=0;j<arclist.size();j++){
//				if (arclist[j].n0>i) arclist[j].n0--;
//				if (arclist[j].n1>i) arclist[j].n1--;
//			}
//		}
//		else i++;
//	} while (i<nodelist.size());
//
//	nodelist.FreeExtra();
//
//	return flag;
//}


//bool FMesher::AddArcSegment(CArcSegment &asegm, double tol)
//{
//	int i,j,k;
//	CSegment segm;
//	CArcSegment newarc;
//	CComplex c,p[2];
//	std::vector < CComplex > newnodes;
//	double R,d,dmin,t;
//
//	newnodes.clear();
//
//	// don't add if line is degenerate
//	if (asegm.n0==asegm.n1) return false;
//
//	// don't add if the arc is already in the list;
//	for(i=0;i<arclist.size();i++){
//		if ((arclist[i].n0==asegm.n0) && (arclist[i].n1==asegm.n1) &&
//			(fabs(arclist[i].ArcLength-asegm.ArcLength)<1.e-02)) return false;
//		// arcs are ``the same'' if start and end points are the same, and if
//		// the arc lengths are relatively close (but a lot farther than
//		// machine precision...
//	}
//
//	// add proposed arc to the linelist
//	asegm.IsSelected=0;
//
//	// check to see if there are intersections
//	for(i=0;i<linelist.size();i++)
//	{
//		j=GetLineArcIntersection(linelist[i],asegm,p);
//		if(j>0) for(k=0;k<j;k++) newnodes.push_back(p[k]);
//	}
//	for(i=0;i<arclist.size();i++)
//	{
//		j=GetArcArcIntersection(asegm,arclist[i],p);
//		if(j>0) for(k=0;k<j;k++) newnodes.push_back(p[k]);
//	}
//
//	// add nodes at intersections
//	if(tol==0)
//	{
//		if (nodelist.size()<2) t=1.e-08;
//		else{
//			CComplex p0,p1;
//			p0=nodelist[0].CC();
//			p1=p0;
//			for(i=1;i<nodelist.size();i++)
//			{
//				if(nodelist[i].x<p0.re) p0.re=nodelist[i].x;
//				if(nodelist[i].x>p1.re) p1.re=nodelist[i].x;
//				if(nodelist[i].y<p0.im) p0.im=nodelist[i].y;
//				if(nodelist[i].y>p1.im) p1.im=nodelist[i].y;
//			}
//			t=abs(p1-p0)*CLOSE_ENOUGH;
//		}
//	}
//	else t=tol;
//
//	for(i=0;i<newnodes.size();i++)
//		AddNode(newnodes[i].re,newnodes[i].im,t);
//
//	// add proposed arc segment;
//	arclist.push_back(asegm);
//
//	// check to see if proposed arc passes through other points;
//    // if so, delete arc and create arcs that link intermediate points;
//    // does this by recursive use of AddArcSegment;
//
//    UnselectAll();
//	GetCircle(asegm,c,R);
//    if (tol==0) dmin=fabs(R*PI*asegm.ArcLength/180.)*1.e-05;
//	else dmin=tol;
//    k=arclist.size()-1;
//
//    for(i=0;i<nodelist.size();i++)
//    {
//        if( (i!=asegm.n0) && (i!=asegm.n1) )
//        {
//			d=ShortestDistanceFromArc(CComplex(nodelist[i].x,nodelist[i].y),arclist[k]);
//
//		//	MsgBox("d=%g dmin=%g",d,dmin);
//			// what is the purpose of this test?
//		//	if (abs(nodelist[i].CC()-nodelist[asegm.n0].CC())<2.*dmin) d=2.*dmin;
//		//	if (abs(nodelist[i].CC()-nodelist[asegm.n1].CC())<2.*dmin) d=2.*dmin;
//
//
//            if (d<dmin){
//
//				CComplex a0,a1,a2;
//				a0.Set(nodelist[asegm.n0].x,nodelist[asegm.n0].y);
//				a1.Set(nodelist[asegm.n1].x,nodelist[asegm.n1].y);
//				a2.Set(nodelist[i].x,nodelist[i].y);
//                arclist[k].ToggleSelect();
//                DeleteSelectedArcSegments();
//
//				newarc=asegm;
//				newarc.n1=i;
//				newarc.ArcLength=arg((a2-c)/(a0-c))*180./PI;
//                AddArcSegment(newarc,dmin);
//
//				newarc=asegm;
//				newarc.n0=i;
//				newarc.ArcLength=arg((a1-c)/(a2-c))*180./PI;
//                AddArcSegment(newarc,dmin);
//
//                i=nodelist.size();
//            }
//        }
//    }
//
//	return true;
//}


//bool FMesher::AddBlockLabel(double x, double y, double d)
//{
//	int i;
//	bool AddFlag=true;
//
//	// test to see if ``too close'' to existing node...
//	for (i=0;i<blocklist.size();i++)
//		if(blocklist[i].GetDistance(x,y)<d) AddFlag=false;
//
//	// can't put a block label on top of an existing node...
//	for (i=0;i<nodelist.size();i++)
//		if(nodelist[i].GetDistance(x,y)<d) return false;
//
//	// can't put a block label on a line, either...
//	for (i=0;i<linelist.size();i++)
//		if(ShortestDistance(x,y,i)<d) return false ;
//
//	// if all is OK, add point in to the node list...
//	if(AddFlag==true){
//		CBlockLabel pt;
//		pt.x=x; pt.y=y;
//		blocklist.push_back(pt);
//	}
//
//	return true;
//}


//bool FMesher::AddNode(double x, double y, double d)
//{
//	int i,k;
//	CNode pt;
//	CSegment segm;
//	CArcSegment asegm;
//	CComplex c,a0,a1,a2;
//	double R;
//
//	// test to see if ``too close'' to existing node...
//	for (i=0;i<nodelist.size();i++)
//		if(nodelist[i].GetDistance(x,y)<d) return false;
//
//	// can't put a node on top of a block label; do same sort of test.
//	for (i=0;i<blocklist.size();i++)
//		if(blocklist[i].GetDistance(x,y)<d) return false;
//
//	// if all is OK, add point in to the node list...
//	pt.x=x; pt.y=y;
//	nodelist.push_back(pt);
//
//	// test to see if node is on an existing line; if so,
//	// break into two lines;
//	k=linelist.size();
//	for(i=0;i<k;i++)
//	{
//		if (fabs(ShortestDistance(x,y,i))<d)
//		{
//			segm=linelist[i];
//			linelist[i].n1=nodelist.size()-1;
//			segm.n0=nodelist.size()-1;
//			linelist.push_back(segm);
//		}
//	}
//
//	// test to see if node is on an existing arc; if so,
//	// break into two arcs;
//	k=arclist.size();
//	for(i=0;i<k;i++)
//	{
//		if (ShortestDistanceFromArc(CComplex(x,y),arclist[i])<d)
//		{
//			a0.Set(nodelist[arclist[i].n0].x,nodelist[arclist[i].n0].y);
//			a1.Set(nodelist[arclist[i].n1].x,nodelist[arclist[i].n1].y);
//			a2.Set(x,y);
//			GetCircle(arclist[i],c,R);
//			asegm=arclist[i];
//			arclist[i].n1=nodelist.size()-1;
//			arclist[i].ArcLength=arg((a2-c)/(a0-c))*180./PI;
//			asegm.n0=nodelist.size()-1;
//			asegm.ArcLength=arg((a1-c)/(a2-c))*180./PI;
//			arclist.push_back(asegm);
//		}
//	}
//	return true;
//}

//bool FMesher::AddSegment(int n0, int n1, double tol)
//{
//	return AddSegment(n0,n1,NULL,tol);
//}

//bool FMesher::AddSegment(int n0, int n1, CSegment *parsegm, double tol)
//{
//	int i,j,k;
//	double xi,yi,t;
//	CComplex p[2];
//	CSegment segm;
//	std::vector < CComplex > newnodes;
//
//	newnodes.clear();
//
//	// don't add if line is degenerate
//	if (n0==n1) return false;
//
//	// don't add if the line is already in the list;
//	for(i=0;i<linelist.size();i++){
//		if ((linelist[i].n0==n0) && (linelist[i].n1==n1)) return false;
//		if ((linelist[i].n0==n1) && (linelist[i].n1==n0)) return false;
//	}
//
//	// add proposed line to the linelist
//	 segm.BoundaryMarker="<None>";
//	if (parsegm!=NULL) segm=*parsegm;
//	segm.IsSelected=0;
//	segm.n0=n0; segm.n1=n1;
//
//	// check to see if there are intersections with segments
//	for(i=0;i<linelist.size();i++)
//		if(GetIntersection(n0,n1,i,&xi,&yi)==true)  newnodes.push_back(CComplex(xi,yi));
//
//	// check to see if there are intersections with arcs
//	for(i=0;i<arclist.size();i++){
//		j=GetLineArcIntersection(segm,arclist[i],p);
//		if (j>0) for(k=0;k<j;k++) newnodes.push_back(p[k]);
//	}
//
//	// add nodes at intersections
//	if (tol==0)
//	{
//		if (nodelist.size()<2) t=1.e-08;
//		else{
//			CComplex p0,p1;
//			p0=nodelist[0].CC();
//			p1=p0;
//			for(i=1;i<nodelist.size();i++)
//			{
//				if(nodelist[i].x<p0.re) p0.re=nodelist[i].x;
//				if(nodelist[i].x>p1.re) p1.re=nodelist[i].x;
//				if(nodelist[i].y<p0.im) p0.im=nodelist[i].y;
//				if(nodelist[i].y>p1.im) p1.im=nodelist[i].y;
//			}
//			t=abs(p1-p0)*CLOSE_ENOUGH;
//		}
//	}
//	else t=tol;
//
//	for(i=0;i<newnodes.size();i++)
//		AddNode(newnodes[i].re,newnodes[i].im,t);
//
//	// Add proposed line segment
//	linelist.push_back(segm);
//
//	// check to see if proposed line passes through other points;
//    // if so, delete line and create lines that link intermediate points;
//    // does this by recursive use of AddSegment;
//	double d,dmin;
//    UnselectAll();
//    if (tol==0) dmin=abs(nodelist[n1].CC()-nodelist[n0].CC())*1.e-05;
//	else dmin=tol;
//
//    k=linelist.size()-1;
//    for(i=0;i<nodelist.size();i++)
//    {
//        if( (i!=n0) && (i!=n1) )
//        {
//            d=ShortestDistance(nodelist[i].x,nodelist[i].y,k);
//			if (abs(nodelist[i].CC()-nodelist[n0].CC())<dmin) d=2.*dmin;
//			if (abs(nodelist[i].CC()-nodelist[n1].CC())<dmin) d=2.*dmin;
//            if (d<dmin){
//                linelist[k].ToggleSelect();
//                DeleteSelectedSegments();
//				if(parsegm==NULL)
//				{
//					AddSegment(n0,i,dmin);
//					AddSegment(i,n1,dmin);
//				}
//				else{
//					AddSegment(n0,i,&segm,dmin);
//					AddSegment(i,n1,&segm,dmin);
//				}
//                i=nodelist.size();
//            }
//        }
//    }
//
//	return true;
//}

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
//			else if(blocklist[i].BlockType!=blocklist[k].BlockType)
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
//		if(blocklist[k].BlockType=="<No Mesh>") zDlg.cursel=1;
//		else for(i=0,zDlg.cursel=0;i<blockproplist.size();i++)
//			if (blockproplist[i].BlockName==blocklist[k].BlockType)
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
//				if (zDlg.cursel==0) blocklist[i].BlockType="<None>";
//				else if(zDlg.cursel==1) blocklist[i].BlockType="<No Mesh>";
//				else blocklist[i].BlockType=blockproplist[zDlg.cursel-2].BlockName;
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
//			else if(nodelist[i].BoundaryMarker!=nodelist[k].BoundaryMarker)
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
//			if (nodeproplist[i].PointName==nodelist[k].BoundaryMarker)
//				zDlg.cursel=i+1;
//	}
//	else zDlg.cursel=0;
//
//	if (zDlg.DoModal()==IDOK){
//		for(i=0;i<nodelist.size();i++)
//		{
//			if(nodelist[i].IsSelected!=0){
//				if (zDlg.cursel==0) nodelist[i].BoundaryMarker="<None>";
//				else nodelist[i].BoundaryMarker=nodeproplist[zDlg.cursel-1].PointName;
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
//			else if(linelist[i].BoundaryMarker!=linelist[k].BoundaryMarker)
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
//			if (lineproplist[i].BdryName==linelist[k].BoundaryMarker)
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
//				if (zDlg.cursel==0) linelist[i].BoundaryMarker="<None>";
//				else linelist[i].BoundaryMarker=lineproplist[zDlg.cursel-1].BdryName;
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
//			else if(arclist[i].BoundaryMarker!=arclist[k].BoundaryMarker)
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
//			if (lineproplist[i].BdryName==arclist[k].BoundaryMarker)
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
//				if (zDlg.cursel==0) arclist[i].BoundaryMarker="<None>";
//				else arclist[i].BoundaryMarker=lineproplist[zDlg.cursel-1].BdryName;
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
