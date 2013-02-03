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

// for compiling as mex we replace printf with mexPrintf
// here we ensure it is printf otherwise
#ifdef MATLAB_MEX_FILE
#include "mex.h"
#endif

//#include "stdafx.h"
//#include "femm.h"
//#include "FemmeDoc.h"
//#include "FemmeView.h"

//#include "probdlg.h"
//#include "PtProp.h"
//#include "OpBlkDlg.h"
//#include "OpNodeDlg.h"
//#include "OpSegDlg.h"
//#include "OpArcSegDlg.h"
//#include "OpGrp.h"
//#include "ArcDlg.h"
//#include "ExteriorProps.h"

//extern void *pFemmeDoc;
//extern lua_State *lua;
//extern CLuaConsoleDlg *LuaConsole;
//extern bool bLinehook;


extern void *triangulate;

using namespace std;

FMesher::FMesher()
{
    // initialize the problem data structures
    // and default behaviour etc.
    Initialize();
}

FMesher::FMesher(string PathName)
{
    // initialize the problem data structures
    // and default behaviour etc.
    Initialize();

    LoadFEMFile(PathName);
}

bool FMesher::Initialize()
{

    // set some default values for starting up rendering
    // things properly
    FirstDraw=false;
    NoDraw=false;

    // set up some default behaviors
    d_prec=1.e-08;
    d_minangle=30.;
    d_freq=0;
    d_depth=1;
    d_coord=0;
    d_length=0;
    d_type=0;
    d_solver=0;

    // Figure out what directory the executables
    // are in, so we can call `triangle' if we need to.
    // BinDir=((CFemmApp *)AfxGetApp())->GetExecutablePath();
    BinDir = "";

    // read default behaviors from disk
    // ScanPreferences();

    // fire up lua
    //initalise_lua();

    // clear out all current lines, nodes, and block labels
    nodelist.clear();
    linelist.clear();
    arclist.clear();
    blocklist.clear();
    undonodelist.clear();
    undolinelist.clear();
    undoarclist.clear();
    undoblocklist.clear();
    nodeproplist.clear();
    lineproplist.clear();
    blockproplist.clear();
    circproplist.clear();
    meshnode.clear();
    meshline.clear();
    greymeshline.clear();

    // set problem attributes to generic ones;
    Frequency=d_freq;
    Precision=d_prec;
    MinAngle=d_minangle;
    Depth=d_depth;
    LengthUnits=d_length;
    ProblemType=d_type;
    ACSolver=d_solver;
    Coords=d_coord;
    ProblemNote="Add comments here.";
    extRo=extRi=extZo=0;

    return true;
}

//FMesher::~FMesher()
//{
//	//pFemmeDoc=NULL;
//}

/////////////////////////////////////////////////////////////////////////////
// FMesher commands

//void FMesher::OnDefineProblem()
//{
//	probdlg pDlg;
//
//	// Send present parameter values to the dialog
//	// pDlg.m_rji=TheDoc->vi[0];
//	pDlg.probtype       = ProblemType;
//	pDlg.m_problem_note = ProblemNote;
//	pDlg.m_frequency    = Frequency;
//	pDlg.m_precision	= Precision;
//	pDlg.m_minangle		= MinAngle;
//	pDlg.m_depth		= Depth;
//	pDlg.lengthunits	= LengthUnits;
//	pDlg.solver			= ACSolver;
//
//	// Display dialog and collect data
//	if(pDlg.DoModal()==IDOK)
//	{
//		Frequency   = pDlg.m_frequency;
//		Precision	= pDlg.m_precision;
//		MinAngle	= pDlg.m_minangle;
//		ProblemNote = pDlg.m_problem_note;
//		ProblemType = pDlg.probtype;
//		LengthUnits = pDlg.lengthunits;
//		Depth		= pDlg.m_depth;
//		ACSolver	= pDlg.solver;
//	}
//}




void FMesher::AfxMessageBox(char* msg)
{
    // AfxMessageBox, replacement for MFC function which pops up
    // a message to the user. Here we instead print the message
    // to stdout

    printf(msg);
}

void FMesher::AfxMessageBox(std::string msg)
{
    // AfxMessageBox, replacement for MFC function which pops up
    // a message to the user. Here we instead print the message
    // to stdout

    printf(msg.c_str());
}


void FMesher::UnselectAll()
{
    unsigned int i;

    for(i=0; i < nodelist.size(); i++) nodelist[i].IsSelected=0;
    for(i=0; i < linelist.size(); i++) linelist[i].IsSelected=0;
    for(i=0; i < blocklist.size(); i++) blocklist[i].IsSelected=0;
    for(i=0; i < arclist.size(); i++) arclist[i].IsSelected=0;
}


void FMesher::GetCircle(CArcSegment &arc,CComplex &c, double &R)
{
    CComplex a0,a1,t;
    double d,tta;

    a0.Set(nodelist[arc.n0].x,nodelist[arc.n0].y);
    a1.Set(nodelist[arc.n1].x,nodelist[arc.n1].y);
    d=abs(a1-a0);			// distance between arc endpoints

    // figure out what the radius of the circle is...
    t=(a1-a0)/d;
    tta=arc.ArcLength*PI/180.;
    R=d/(2.*sin(tta/2.));
    c=a0 + (d/2. + I*sqrt(R*R-d*d/4.))*t; // center of the arc segment's circle...
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

    if(nodelist.size()==0) return -1;

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

    if(blocklist.size()==0) return -1;

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

    if(linelist.size()==0) return -1;

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

    if(arclist.size()==0) return -1;

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


//bool FMesher::OldOnOpenDocument(LPCTSTR lpszPathName)
//{
//	FILE *fp;
//	int i,j,k,t,len;
//	char s[1024];
//	CPointProp	  PProp;
//	CBoundaryProp BProp;
//	CMaterialProp MProp;
//	CNode		node;
//	CSegment	segm;
//	CArcSegment asegm;
//	CBlockLabel blk;
//
//	if ((fp=fopen(lpszPathName,"rt"))==NULL){
//		AfxMessageBox("Couldn't read from specified .fem file");
//		return false;
//	}
//
//	fscanf(fp,"Frequency   =  %lf\n",&Frequency);
//	fscanf(fp,"LengthUnits =  %i\n",&LengthUnits);
//	fscanf(fp,"ProblemType =  %i\n",&ProblemType);
//	fscanf(fp,"Coords      =  %i\n",&Coords);
//	fgets(s,1024,fp);
//		len=strlen(s);
//	len--;
//	s[len]=NULL;
//	for(i=0;i<len;i++){
//		if(s[i]=='{') s[i]=13;
//		if(s[i]=='}') s[i]=10;
//	}
//	ProblemNote=s;
//
//	// Read in materials library...
//
//	fscanf(fp,"%i\n",&k);
//	for(i=0;i<k;i++)
//	{
//
//		fgets(s,1024,fp);
//		len=strlen(s); for(j=0;j<len;j++) if (s[j]<26) s[j]=NULL;
//		PProp.PointName=s;
//		fscanf(fp,"%lf	%lf	%lf	%lf\n",&PProp.Jp.re,&PProp.Jp.im,&PProp.Ap.re,&PProp.Ap.im);
//		nodeproplist.push_back(PProp);
//	}
//
//	fscanf(fp,"%i\n",&k);
//	for(i=0;i<k;i++)
//	{
//		fgets(s,1024,fp);
//		len=strlen(s); for(j=0;j<len;j++) if (s[j]<26) s[j]=NULL;
//		BProp.BdryName=s;
//		fscanf(fp,"%i	%lf	%lf	%lf	%lf	%lf	%lf	%lf	%lf\n",&BProp.BdryFormat,
//			&BProp.A0,&BProp.A1,&BProp.A2,&BProp.phi,&BProp.Mu,
//			&BProp.Sig,&BProp.c0,&BProp.c1);
//		lineproplist.push_back(BProp);
//	}
//
//	fscanf(fp,"%i\n",&k);
//	for(i=0;i<k;i++)
//	{
//		fgets(s,1024,fp);
//		len=strlen(s); for(j=0;j<len;j++) if (s[j]<26) s[j]=NULL;
//		MProp.BlockName=s;
//		fscanf(fp,"%lf	%lf	%lf	%lf	%lf	%lf	%lf	%lf	%lf	%i	%lf\n",
//			&MProp.mu_x,&MProp.mu_y,&MProp.H_c,
//			&MProp.Theta_m,&MProp.Jsrc.re,&MProp.Jsrc.im,&MProp.Cduct,
//			&MProp.Lam_d,&MProp.Theta_hn,&MProp.LamType,&MProp.LamFill);
//		MProp.Theta_hx=MProp.Theta_hn;
//		MProp.Theta_hy=MProp.Theta_hn;
//		fgets(s,1024,fp);
//		sscanf(s,"%i",&MProp.BHpoints);
// //		if (MProp.BHpoints>0)
// //			MProp.BHdata=(CComplex *)calloc(MProp.BHpoints,sizeof(CComplex));
//      CComplex tempComplex;
//		for(j=0;j<MProp.BHpoints;j++){
//			fgets(s,1024,fp);
//			sscanf(s,"%lf	%lf", &tempComplex.re, &tempComplex.im);
//          MProp.BHdata.push_back(tempComplex);
//		}
//		blockproplist.push_back(MProp);
//	}
//	MProp.BHpoints=0;
//
//	// read in node list
//	fgets(s,1024,fp);
//	sscanf(s,"%i",&k);
//	for(i=0;i<k;i++)
//	{
//		fscanf(fp,"%lf	%lf	%i\n",&node.x,&node.y,&t);
//		t=t-2;
//		if(t<0) node.BoundaryMarker="";
//		else node.BoundaryMarker=nodeproplist[t].PointName;
//		nodelist.push_back(node);
//	}
//
//	// read in segment list
//	fgets(s,1024,fp);
//	sscanf(s,"%i",&k);
//	for(i=0;i<k;i++)
//	{
//		fscanf(fp,"%i	%i	%lf %i\n",&segm.n0,&segm.n1,&segm.MaxSideLength,&t);
//		t=-(t+2);
//		if(t<0) segm.BoundaryMarker="";
//		else segm.BoundaryMarker=lineproplist[t].BdryName;
//		linelist.push_back(segm);
//	}
//
//	// read in arc segment list
//	fgets(s,1024,fp);
//	sscanf(s,"%i",&k);
//	for(i=0;i<k;i++)
//	{
//		fscanf(fp,"%i	%i	%lf	%lf %i\n",&asegm.n0,&asegm.n1,
//			&asegm.ArcLength,&asegm.MaxSideLength,&t);
//		t=-(t+2);
//		if(t<0) asegm.BoundaryMarker="";
//		else asegm.BoundaryMarker=lineproplist[t].BdryName;
//		arclist.push_back(asegm);
//	}
//
//	// read in list of holes;
//	fscanf(fp,"%i\n",&k);
//	blk.BlockType="<No Mesh>";
//	blk.MaxArea=0;
//	for(i=0;i<k;i++)
//	{
//		fscanf(fp,"%lf	%lf\n",&blk.x,&blk.y);
//		blocklist.push_back(blk);
//	}
//
//	// read in regional attributes
//	fscanf(fp,"%i\n",&k);
//	for(i=0;i<k;i++)
//	{
//		fscanf(fp,"%lf	%lf	%i	%lf\n",&blk.x,&blk.y,&t,&blk.MaxArea);
//		blk.MagDir=0.;
//		blk.MagDirFctn.Empty();
//		blk.Turns=1;
//		if (blk.MaxArea<0) blk.MaxArea=0;
//		if (t<0) blk.BlockType="<None>";
//		else{
//			blk.BlockType=blockproplist[t].BlockName;
//			blk.MagDir   =blockproplist[t].Theta_m;
//		}
//		blocklist.push_back(blk);
//	}
//	fclose(fp);
//
//	FirstDraw=true;
//
//	return true;
//}

bool FMesher::LoadFEMFile(string PathName)
{

    //if (!CDocument::OnOpenDocument(lpszPathName))
    //	return false;

    // make sure old data is cleared out...
    Initialize();

    FILE *fp;
    int i,j,k,t;
    int vers=0;
    char s[1024],q[1024];
    char *v;
    CPointProp	  PProp;
    CBoundaryProp BProp;
    CMaterialProp MProp;
    CCircuit	  CProp;
    CNode		node;
    CSegment	segm;
    CArcSegment asegm;
    CBlockLabel blk;

    if ((fp=fopen(PathName.c_str(),"rt"))==NULL)
    {
        AfxMessageBox((std::string)"Couldn't read from specified .fem file");
        return false;
    }

    //// Check to see if this is an old-version femm datafile
    //fgets(s,1024,fp);
    //if (strncmp(s,"Frequency",8)==0){
    //	fclose(fp);
    //	return OldOnOpenDocument(lpszPathName);
    //}
    //else rewind(fp); // Go back to beginning of the file;

    // hook to catch old files where depth wasn't defined:
    Depth = -1;

    // parse the file
    while (fgets(s,1024,fp)!=NULL)
    {
        if (sscanf(s,"%s",q)==EOF) q[0] = NULL;
        //	int _strnicmp( const char *string1, const char *string2, size_t count );

        // Deal with flag for file format version
        if( _strnicmp(q,"[format]",8)==0 )
        {
            v = StripKey(s);
            double dblvers;
            sscanf(v,"%lf",&dblvers);
            vers = (int) (10.*dblvers + 0.5);
            if(vers>40)
            {
                AfxMessageBox((std::string)"This file is from a newer version of FEMM\nThis file may contain attributes not\nsupported by this version of FEMM");
            }
            q[0] = NULL;
        }

        // Frequency of the problem
        if( _strnicmp(q,"[frequency]",11)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&Frequency);
            q[0] = NULL;
        }

        // Depth in the into-the-page direction
        if( _strnicmp(q,"[depth]",7)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&Depth);
            q[0] = NULL;
        }

        // Required solver precision
        if( _strnicmp(q,"[precision]",11)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&Precision);
            q[0] = NULL;
        }

        // Solver to use for AC problems
        if( _strnicmp(q,"[acsolver]",8)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&ACSolver);
            q[0] = NULL;
            // 0 == successive approx
            // 1 == newton
        }

        // Minimum Angle Constraint for finite element mesh
        if( _strnicmp(q,"[minangle]",10)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MinAngle);
            q[0] = NULL;
        }

        // Units of length used by the problem
        if( _strnicmp(q,"[lengthunits]",13)==0)
        {
            v = StripKey(s);
            sscanf(v,"%s",q);
            if( _strnicmp(q,"inches",6)==0) LengthUnits = 0;
            else if( _strnicmp(q,"millimeters",11)==0) LengthUnits = 1;
            else if( _strnicmp(q,"centimeters",1)==0) LengthUnits = 2;
            else if( _strnicmp(q,"mils",4)==0) LengthUnits = 4;
            else if( _strnicmp(q,"microns",6)==0) LengthUnits = 5;
            else if( _strnicmp(q,"meters",6)==0) LengthUnits = 3;
            q[0] = NULL;
        }

        // Problem Type (planar or axisymmetric)
        if( _strnicmp(q,"[problemtype]",13)==0)
        {
            v = StripKey(s);
            sscanf(v,"%s",q);
            if( _strnicmp(q,"planar",6)==0) ProblemType=0;
            if( _strnicmp(q,"axisymmetric",3)==0) ProblemType=1;
            q[0] = NULL;
        }

        // Coordinates (cartesian or polar)
        if( _strnicmp(q,"[coordinates]",13)==0)
        {
            v = StripKey(s);
            sscanf(v,"%s",q);
            if ( _strnicmp(q,"cartesian",4)==0) Coords=0;
            if ( _strnicmp(q,"polar",5)==0) Coords=1;
            q[0] = NULL;
        }

        // Comments
        if (_strnicmp(q,"[comment]",9)==0)
        {
            v = StripKey(s);
            // put in carriage returns;
            k = strlen(v);
            for(i=0; i<k; i++)
                if((v[i]=='\\') && (v[i+1]=='n'))
                {
                    v[i] = 13;
                    v[i+1] = 10;
                }

            for(i=0; i<k; i++)
                if(v[i]=='\"')
                {
                    v = v + i + 1;
                    i = k;
                }
            k = strlen(v);
            if(k>0) for(i=k-1; i>=0; i--)
                {
                    if(v[i]=='\"')
                    {
                        v[i] = 0;
                        i = -1;
                    }
                }
            ProblemNote = v;
            q[0] = NULL;
        }

        // properties for axisymmetric external region
        if( _strnicmp(q,"[extzo]",7)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&extZo);
            q[0] = NULL;
        }

        if( _strnicmp(q,"[extro]",7)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&extRo);
            q[0] = NULL;
        }

        if( _strnicmp(q,"[extri]",7)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&extRi);
            q[0] = NULL;
        }

        // Point Properties
        if( _strnicmp(q,"<beginpoint>",11)==0)
        {
            PProp.PointName="New Point Property";
            PProp.Jp=0;
            PProp.Ap=0;
            q[0] = NULL;
        }

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
            PProp.PointName=v;
            q[0] = NULL;
        }

        if( _strnicmp(q,"<A_re>",6)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&PProp.Ap.re);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<A_im>",6)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&PProp.Ap.im);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<I_re>",6)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&PProp.Jp.re);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<I_im>",6)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&PProp.Jp.im);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<endpoint>",9)==0)
        {
            nodeproplist.push_back(PProp);
            q[0] = NULL;
        }

        // Boundary Properties;
        if( _strnicmp(q,"<beginbdry>",11)==0)
        {
            BProp.BdryName="New Boundary";
            BProp.BdryFormat=0;
            BProp.A0=0.;
            BProp.A1=0.;
            BProp.A2=0.;
            BProp.phi=0.;
            BProp.Mu=0.;
            BProp.Sig=0.;
            BProp.c0=0.;
            BProp.c1=0.;
            q[0] = NULL;
        }

        if( _strnicmp(q,"<bdryname>",10)==0)
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
            BProp.BdryName=v;
            q[0] = NULL;
        }

        if( _strnicmp(q,"<bdrytype>",10)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&BProp.BdryFormat);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<mu_ssd>",8)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.Mu);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<sigma_ssd>",11)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.Sig);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<A_0>",5)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.A0);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<A_1>",5)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.A1);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<A_2>",5)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.A2);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<phi>",5)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.phi);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<c0>",4)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.c0.re);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<c1>",4)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.c1.re);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<c0i>",5)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.c0.im);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<c1i>",5)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&BProp.c1.im);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<endbdry>",9)==0)
        {
            lineproplist.push_back(BProp);
            q[0] = NULL;
        }


        // Block Properties;
        if( _strnicmp(q,"<beginblock>",12)==0)
        {
            MProp.BlockName="New Material";
            MProp.mu_x=1.;
            MProp.mu_y=1.;			// permeabilities, relative
            MProp.H_c=0.;			// magnetization, A/m
            MProp.Jsrc=0;			// applied current density, MA/m^2
            MProp.Cduct=0.;		    // conductivity of the material, MS/m
            MProp.Lam_d=0.;			// lamination thickness, mm
            MProp.Theta_hn=0.;		// hysteresis angle, degrees
            MProp.Theta_hx=0.;
            MProp.Theta_hy=0.;
            MProp.LamFill=1.;		// lamination fill factor;
            MProp.LamType=0;		// type of lamination;
            MProp.NStrands=0;
            MProp.WireD=0;
            MProp.BHpoints=0;
            MProp.BHdata.clear();
            q[0] = NULL;
        }

        if( _strnicmp(q,"<blockname>",10)==0)
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
            MProp.BlockName=v;
            q[0] = NULL;
        }

        if( _strnicmp(q,"<mu_x>",6)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.mu_x);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<mu_y>",6)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.mu_y);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<H_c>",5)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.H_c);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<J_re>",6)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.Jsrc.re);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<J_im>",6)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.Jsrc.im);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<sigma>",7)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.Cduct);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<phi_h>",7)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.Theta_hn);
            if(vers==30)
            {
                MProp.Theta_hx = MProp.Theta_hn;
                MProp.Theta_hy = MProp.Theta_hn;
            }
            q[0] = NULL;
        }

        if( _strnicmp(q,"<phi_hx>",8)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.Theta_hx);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<phi_hy>",8)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.Theta_hy);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<d_lam>",7)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.Lam_d);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<LamFill>",8)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.LamFill);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<LamType>",9)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&MProp.LamType);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<NStrands>",10)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&MProp.NStrands);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<WireD>",7)==0)
        {
            v = StripKey(s);
            sscanf(v,"%lf",&MProp.WireD);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<BHPoints>",10)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&MProp.BHpoints);
            if (MProp.BHpoints>0)
            {
                //MProp.BHdata=(CComplex *)calloc(MProp.BHpoints,sizeof(CComplex));
                CComplex tempComplex;

                for(j=0; j<MProp.BHpoints; j++)
                {

                    fgets(s,1024,fp);

                    sscanf(s,"%lf	%lf", &tempComplex.re, &tempComplex.im);

                    MProp.BHdata.push_back(tempComplex);
                }
            }
            q[0] = NULL;
        }

        if( _strnicmp(q,"<endblock>",9)==0)
        {
            blockproplist.push_back(MProp);
            MProp.BHpoints=0;
            q[0] = NULL;
        }

        // Circuit Properties
        if( _strnicmp(q,"<begincircuit>",14)==0)
        {
            CProp.CircName="New Circuit";
            CProp.Amps=0.;
            CProp.CircType=0;
            q[0] = NULL;
        }

        if( _strnicmp(q,"<circuitname>",13)==0)
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
            CProp.CircName=v;
            q[0] = NULL;
        }

        if( _strnicmp(q,"<totalamps_re>",14)==0)
        {
            double inval;
            v = StripKey(s);
            sscanf(v,"%lf",&inval);
            CProp.Amps+=inval;
            q[0] = NULL;
        }

        if( _strnicmp(q,"<totalamps_im>",14)==0)
        {
            double inval;
            v = StripKey(s);
            sscanf(v,"%lf",&inval);
            CProp.Amps+=(I*inval);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<circuittype>",13)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&CProp.CircType);
            q[0] = NULL;
        }

        if( _strnicmp(q,"<endcircuit>",12)==0)
        {
            circproplist.push_back(CProp);
            q[0] = NULL;
        }

        // Points list;
        if(_strnicmp(q,"[numpoints]",11)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&k);
            for(i=0; i<k; i++)
            {
                fgets(s,1024,fp);

                v=ParseDbl(s,&node.x);
                v=ParseDbl(v,&node.y);
                v=ParseInt(v,&t);
                v=ParseInt(v,&node.InGroup);
                if(t==0) node.BoundaryMarker="";
                else if(t<= (int)nodeproplist.size())
                    node.BoundaryMarker=nodeproplist[t-1].PointName;
                nodelist.push_back(node);
            }
            q[0] = NULL;
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

                // scan in data
                v = ParseInt(s,&segm.n0);
                v = ParseInt(v,&segm.n1);
                v = ParseDbl(v,&segm.MaxSideLength);
                v = ParseInt(v,&t);
                int Hidden = 0;
                v = ParseInt(v,&Hidden);
                //v=ParseInt(v,&segm.Hidden);
                segm.Hidden = Hidden;
                v = ParseInt(v,&segm.InGroup);

                if(t==0)
                {
                    segm.BoundaryMarker="";
                }
                else if (t<= (int)lineproplist.size())
                {
                    segm.BoundaryMarker=lineproplist[t-1].BdryName;
                }

                linelist.push_back(segm);
            }
            q[0] = NULL;
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
                int Hidden = 0;
                //v=ParseInt(v,&asegm.Hidden);
                v = ParseInt(v,&Hidden);
                asegm.Hidden = Hidden;
                v = ParseInt(v,&asegm.InGroup);

                if(t==0)
                {
                    asegm.BoundaryMarker = "";
                }
                else if (t<= (int) lineproplist.size())
                {
                    asegm.BoundaryMarker = lineproplist[t-1].BdryName;
                }

                arclist.push_back(asegm);
            }
            q[0] = NULL;
        }


        // read in list of holes;
        if(_strnicmp(q,"[numholes]",13)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&k);
            if(k>0)
            {
                blk.BlockType="<No Mesh>";
                blk.MaxArea=0;
                blk.InGroup=0;
                for(i=0; i<k; i++)
                {
                    fgets(s,1024,fp);
                    v=ParseDbl(s,&blk.x);
                    v=ParseDbl(v,&blk.y);
                    v=ParseInt(v,&blk.InGroup);

                    blocklist.push_back(blk);
                }
            }
            q[0] = NULL;
        }

        // read in regional attributes
        if(_strnicmp(q,"[numblocklabels]",13)==0)
        {
            v = StripKey(s);
            sscanf(v,"%i",&k);
            for(i=0; i<k; i++)
            {
                fgets(s,1024,fp);

                //some defaults
                t = 0;
                blk.MaxArea = 0.;
                blk.MagDir = 0.;
                blk.MagDirFctn.clear();
                blk.Turns = 1;
                blk.InCircuit = "<None>";
                blk.InGroup = 0;
                blk.IsExternal = 0;

                // scan in data
                v = ParseDbl(s,&blk.x);
                v = ParseDbl(v,&blk.y);

                v = ParseInt(v,&t);
                if (t==0)
                {
                    blk.BlockType = "<None>";
                }
                else if(t <= (int)blockproplist.size())
                {
                    blk.BlockType = blockproplist[t-1].BlockName;
                }

                v = ParseDbl(v,&blk.MaxArea);
                if (blk.MaxArea<0)
                {
                    blk.MaxArea = 0;
                }
                else
                {
                    blk.MaxArea = PI * blk.MaxArea * blk.MaxArea / 4.0;
                }

                v = ParseInt(v,&t);
                if (t==0)
                {
                    blk.InCircuit = "<None>";
                }
                else if(t <= (int)circproplist.size())
                {
                    blk.InCircuit = circproplist[t-1].CircName;
                }

                v = ParseDbl(v,&blk.MagDir);
                v = ParseInt(v,&blk.InGroup);
                v = ParseInt(v,&blk.Turns);
                int tIsExternal = 0;
                //v=ParseInt(v,&blk.IsExternal);
                v = ParseInt(v, &tIsExternal);
                blk.IsExternal = tIsExternal;
                v = ParseString(v,&blk.MagDirFctn);

                blocklist.push_back(blk);
            }
            q[0] = NULL;
        }
    }

    fclose(fp);

    if(Depth==-1)
    {
        // if this is a v3.2 file we are importing, make the length
        // equal to 1 meter, because 3.2 was all per-meter calculations
        switch(LengthUnits)
        {
        case 1:
            Depth = 1000.;		//mm
            break;
        case 2:
            Depth = 100.;			//cm
            break;
        case 3:
            Depth = 1.;			//m
            break;
        case 4:
            Depth = 1000./0.0254; // mils
            break;
        case 5:
            Depth = 1.e6;			// microns
            break;
        default:
            Depth = 1./0.0254;	// inches
            break;
        }
    }

    FirstDraw = true;

    return true;
}


bool FMesher::SaveFEMFile(string PathName)
{
    // TODO: Add your specialized code here and/or call the base class
    FILE *fp;
    unsigned int i, j;
    int k,t;
    string s;

    // check to see if we are ready to write a datafile;

    if ((fp = fopen(PathName.c_str(),"wt"))==NULL)
    {
        AfxMessageBox((std::string)"Couldn't write to specified file.\nPerhaps the file is write-protected?");
        return false;
    }

    fprintf(fp,"[Format]      =  4.0\n");
    fprintf(fp,"[Frequency]   =  %.17g\n",Frequency);
    fprintf(fp,"[Precision]   =  %.17g\n",Precision);
    fprintf(fp,"[MinAngle]    =  %.17g\n",MinAngle);
    fprintf(fp,"[Depth]       =  %.17g\n",Depth);
    fprintf(fp,"[LengthUnits] =  ");
    switch(LengthUnits)
    {
    case 1:
        fprintf(fp,"millimeters\n");
        break;
    case 2:
        fprintf(fp,"centimeters\n");
        break;
    case 3:
        fprintf(fp,"meters\n");
        break;
    case 4:
        fprintf(fp,"mils\n");
        break;
    case 5:
        fprintf(fp,"microns\n");
        break;
    default:
        fprintf(fp,"inches\n");
        break;
    }

    if (ProblemType==0) fprintf(fp,"[ProblemType] =  planar\n");
    else
    {
        fprintf(fp,"[ProblemType] =  axisymmetric\n");
        if ((extRo!=0) && (extRi!=0))
        {
            fprintf(fp,"[extZo] = %.17g\n",extZo);
            fprintf(fp,"[extRo] = %.17g\n",extRo);
            fprintf(fp,"[extRi] = %.17g\n",extRi);
        }
    }

    if (Coords==0) fprintf(fp,"[Coordinates] =  cartesian\n");
    else fprintf(fp,"[Coordinates] =  polar\n");

    s=ProblemNote;
    for(i=0; i< (unsigned int)ProblemNote.length(); i++)
    {
        if (s[i]==13) s[i] = '\\';
        if (s[i]==10) s[i] = 'n';
    }

    fprintf(fp, "[ACSolver]    =  %i\n", ACSolver);

    fprintf(fp, "[Comment]     =  \"%s\"\n", s.c_str());

    // write out materials properties stuff...
    fprintf(fp,"[PointProps]   = %i\n", (int) nodeproplist.size());
    for(i=0; i<nodeproplist.size(); i++)
    {
        fprintf(fp,"  <BeginPoint>\n");
        fprintf(fp,"    <PointName> = \"%s\"\n",nodeproplist[i].PointName.c_str());
        fprintf(fp,"    <I_re> = %.17g\n",nodeproplist[i].Jp.re);
        fprintf(fp,"    <I_im> = %.17g\n",nodeproplist[i].Jp.im);
        fprintf(fp,"    <A_re> = %.17g\n",nodeproplist[i].Ap.re);
        fprintf(fp,"    <A_im> = %.17g\n",nodeproplist[i].Ap.im);
        fprintf(fp,"  <EndPoint>\n");
    }

    fprintf(fp,"[BdryProps]   = %i\n", (int) lineproplist.size());
    for(i=0; i<lineproplist.size(); i++)
    {
        fprintf(fp,"  <BeginBdry>\n");
        fprintf(fp,"    <BdryName> = \"%s\"\n", lineproplist[i].BdryName.c_str());
        fprintf(fp,"    <BdryType> = %i\n",lineproplist[i].BdryFormat);
        fprintf(fp,"    <A_0> = %.17g\n",lineproplist[i].A0);
        fprintf(fp,"    <A_1> = %.17g\n",lineproplist[i].A1);
        fprintf(fp,"    <A_2> = %.17g\n",lineproplist[i].A2);
        fprintf(fp,"    <Phi> = %.17g\n",lineproplist[i].phi);
        fprintf(fp,"    <c0> = %.17g\n",lineproplist[i].c0.re);
        fprintf(fp,"    <c0i> = %.17g\n",lineproplist[i].c0.im);
        fprintf(fp,"    <c1> = %.17g\n",lineproplist[i].c1.re);
        fprintf(fp,"    <c1i> = %.17g\n",lineproplist[i].c1.im);
        fprintf(fp,"    <Mu_ssd> = %.17g\n",lineproplist[i].Mu);
        fprintf(fp,"    <Sigma_ssd> = %.17g\n",lineproplist[i].Sig);
        fprintf(fp,"  <EndBdry>\n");
    }

    fprintf(fp,"[BlockProps]  = %i\n", (int) blockproplist.size());
    for(i=0; i<blockproplist.size(); i++)
    {
        fprintf(fp,"  <BeginBlock>\n");
        fprintf(fp,"    <BlockName> = \"%s\"\n",blockproplist[i].BlockName.c_str());
        fprintf(fp,"    <Mu_x> = %.17g\n",blockproplist[i].mu_x);
        fprintf(fp,"    <Mu_y> = %.17g\n",blockproplist[i].mu_y);
        fprintf(fp,"    <H_c> = %.17g\n",blockproplist[i].H_c);
        fprintf(fp,"    <H_cAngle> = %.17g\n",blockproplist[i].Theta_m);
        fprintf(fp,"    <J_re> = %.17g\n",blockproplist[i].Jsrc.re);
        fprintf(fp,"    <J_im> = %.17g\n",blockproplist[i].Jsrc.im);
        fprintf(fp,"    <Sigma> = %.17g\n",blockproplist[i].Cduct);
        fprintf(fp,"    <d_lam> = %.17g\n",blockproplist[i].Lam_d);
        fprintf(fp,"    <Phi_h> = %.17g\n",blockproplist[i].Theta_hn);
        fprintf(fp,"    <Phi_hx> = %.17g\n",blockproplist[i].Theta_hx);
        fprintf(fp,"    <Phi_hy> = %.17g\n",blockproplist[i].Theta_hy);
        fprintf(fp,"    <LamType> = %i\n",blockproplist[i].LamType);
        fprintf(fp,"    <LamFill> = %.17g\n",blockproplist[i].LamFill);
        fprintf(fp,"    <NStrands> = %i\n",blockproplist[i].NStrands);
        fprintf(fp,"    <WireD> = %.17g\n",blockproplist[i].WireD);
        fprintf(fp,"    <BHPoints> = %i\n",blockproplist[i].BHpoints);
        for(j=0; j<(unsigned int)blockproplist[i].BHpoints; j++)
            fprintf(fp,"      %.17g	%.17g\n",blockproplist[i].BHdata[j].re,
                    blockproplist[i].BHdata[j].im);
        fprintf(fp,"  <EndBlock>\n");
    }

    fprintf(fp,"[CircuitProps]  = %i\n", (int) circproplist.size());
    for(i=0; i<circproplist.size(); i++)
    {
        fprintf(fp,"  <BeginCircuit>\n");
        fprintf(fp,"    <CircuitName> = \"%s\"\n",circproplist[i].CircName.c_str());
        fprintf(fp,"    <TotalAmps_re> = %.17g\n",circproplist[i].Amps.Re());
        fprintf(fp,"    <TotalAmps_im> = %.17g\n",circproplist[i].Amps.Im());
        fprintf(fp,"    <CircuitType> = %i\n",circproplist[i].CircType);
        fprintf(fp,"  <EndCircuit>\n");
    }

    // write out node list
    fprintf(fp,"[NumPoints] = %i\n", (int) nodelist.size());
    for(i=0; i<nodelist.size(); i++)
    {
        for(j=0,t=0; j<nodeproplist.size(); j++)
            if(nodeproplist[j].PointName==nodelist[i].BoundaryMarker) t=j+1;
        fprintf(fp,"%.17g	%.17g	%i	%i\n",nodelist[i].x,nodelist[i].y,t,
                nodelist[i].InGroup);
    }

    // write out segment list
    fprintf(fp,"[NumSegments] = %i\n", (int) linelist.size());
    for(i=0; i<linelist.size(); i++)
    {
        for(j=0,t=0; j<lineproplist.size(); j++)
            if(lineproplist[j].BdryName==linelist[i].BoundaryMarker) t=j+1;
        fprintf(fp,"%i	%i	",linelist[i].n0,linelist[i].n1);
        if(linelist[i].MaxSideLength<0) fprintf(fp,"-1	");
        else fprintf(fp,"%.17g	",linelist[i].MaxSideLength);
        fprintf(fp,"%i	%i	%i\n",t,linelist[i].Hidden,linelist[i].InGroup);
    }

    // write out arc segment list
    fprintf(fp,"[NumArcSegments] = %i\n", (int) arclist.size());
    for(i=0; i<arclist.size(); i++)
    {
        for(j=0,t=0; j<lineproplist.size(); j++)
            if(lineproplist[j].BdryName==arclist[i].BoundaryMarker) t=j+1;
        fprintf(fp,"%i	%i	%.17g	%.17g	%i	%i	%i\n",arclist[i].n0,arclist[i].n1,
                arclist[i].ArcLength,arclist[i].MaxSideLength,t,
                arclist[i].Hidden,arclist[i].InGroup);
    }

    // write out list of holes;
    for(i=0,j=0; i<blocklist.size(); i++)
        if(blocklist[i].BlockType=="<No Mesh>") j++;
    fprintf(fp,"[NumHoles] = %i\n",j);
    for(i=0,k=0; i<blocklist.size(); i++)
        if(blocklist[i].BlockType=="<No Mesh>")
        {
            fprintf(fp,"%.17g	%.17g	%i\n",blocklist[i].x,blocklist[i].y,
                    blocklist[i].InGroup);
            k++;
        }

    // write out regional attributes
    fprintf(fp,"[NumBlockLabels] = %i\n", (int) blocklist.size()-j);
    for(i=0,k=0; i<blocklist.size(); i++)
        if(blocklist[i].BlockType!="<No Mesh>")
        {
            fprintf(fp,"%.17g	%.17g	",blocklist[i].x,blocklist[i].y);
            for(j=0,t=0; j<blockproplist.size(); j++)
                if(blockproplist[j].BlockName==blocklist[i].BlockType) t=j+1;
            fprintf(fp,"%i	",t);
            if (blocklist[i].MaxArea>0)
                fprintf(fp,"%.17g	",sqrt(4.*blocklist[i].MaxArea/PI));
            else fprintf(fp,"-1	");
            for(j=0,t=0; j<circproplist.size(); j++)
                if(circproplist[j].CircName==blocklist[i].InCircuit) t=j+1;
            fprintf(fp,"%i	%.17g	%i	%i	%i",t,blocklist[i].MagDir,
                    blocklist[i].InGroup,blocklist[i].Turns,blocklist[i].IsExternal);
            if (blocklist[i].MagDirFctn.length()>0)
                fprintf(fp,"	\"%s\"",blocklist[i].MagDirFctn.c_str());
            fprintf(fp,"\n");

            k++;
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
        AfxMessageBox((std::string)"No mesh to display");
        return false;
    }

    rootname = pathname.substr(0,pathname.find_last_of('.'));

    //read meshnodes;
    infile = rootname + ".node";
    if((fp=fopen(infile.c_str(),"rt"))==NULL)
    {
        AfxMessageBox((std::string)"No mesh to display");
        return false;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    meshnode.resize(k);
    CNode node;
    for(i=0; i<k; i++)
    {
        fgets(s,1024,fp);
        sscanf(s,"%i	%lf	%lf",&j,&node.x,&node.y);
        meshnode[i] = node;
    }
    fclose(fp);

    //read meshlines;
    infile = rootname + ".edge";
    if((fp=fopen(infile.c_str(),"rt"))==NULL)
    {
        AfxMessageBox((std::string)"No mesh to display");
        return false;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    meshline.resize(k);
    fclose(fp);

    infile = rootname + ".ele";
    if((fp=fopen(infile.c_str(),"rt"))==NULL)
    {
        AfxMessageBox((std::string)"No mesh to display");
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
//			  q[0] = NULL;
//			}
//
//			if( _strnicmp(q,"<MinAngle>",10)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_minangle);
//			  q[0] = NULL;
//			}
//
//			if( _strnicmp(q,"<Frequency>",11)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_freq);
//			  q[0] = NULL;
//			}
//
//			if( _strnicmp(q,"<Depth>",7)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_depth);
//			  q[0] = NULL;
//			}
//
//			if( _strnicmp(q,"<Coordinates>",13)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_coord);
//			  q[0] = NULL;
//			}
//
//			if( _strnicmp(q,"<LengthUnits>",13)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_length);
//			  q[0] = NULL;
//			}
//
//			if( _strnicmp(q,"<ProblemType>",13)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_type);
//			  q[0] = NULL;
//			}
//
//			if( _strnicmp(q,"<ACSolver>",8)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_solver);
//			  q[0] = NULL;
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
