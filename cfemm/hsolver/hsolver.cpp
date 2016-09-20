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

   Date Modified: 2014 - 03 - 21
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
   Contact:
        szelitzkye@gmail.com
        sztibi82@gmail.com
        richard.crozier@yahoo.co.uk
*/

// hsolver.cpp : implementation of the HSolver class
//


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <malloc.h>
#include "lua.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "hmesh.h"
#include "hspars.h"
#include "fparse.h"
#include "hsolver.h"

#ifndef _MSC_VER
#define _strnicmp strncasecmp
#endif

//conversion to internal working units of m
double units[]={0.0254,0.001,0.01,1,2.54e-5,1.e-6};
double sq(double x){ return x*x; }

//extern void lua_baselibopen (lua_State *L);
//extern void lua_iolibopen (lua_State *L);
//extern void lua_strlibopen (lua_State *L);
//extern void lua_mathlibopen (lua_State *L);
//extern void lua_dblibopen (lua_State *L);
//extern lua_State *lua; // the main lua object
//extern int bLinehook;
//extern int lua_byebye;
//
//lua_State *lua;
//int bLinehook;
//int lua_byebye;

using namespace std;
using namespace femm;


/////////////////////////////////////////////////////////////////////////////
// HSolver construction/destruction

HSolver::HSolver()
{
	meshnode=NULL;
	Tprev=NULL;
	blockproplist=NULL;
	nodeproplist=NULL;
	circproplist=NULL;
	labellist=NULL;

    // initialise the warning message box function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;

}

HSolver::~HSolver()
{
    CleanUp();
}

void HSolver::CleanUp()
{
    if (meshnode!=NULL)		 delete[] meshnode;
    if (blockproplist!=NULL) delete[] blockproplist;
    if (nodeproplist!=NULL)	 delete[] nodeproplist;
    if (circproplist!=NULL)	 delete[] circproplist;
    if (labellist!=NULL)	 delete[] labellist;
    if (Tprev!=NULL)		 delete[] Tprev;
}

/////////////////////////////////////////////////////////////////////////////
// HSolver commands

void HSolver::MsgBox(const char* message)
{
    printf("%s\n", message);
}

int HSolver::LoadProblemFile ()
{
	FILE *fp;
	int j,k;
	char s[1024],q[1024];
	char *v;
    CHPointProp	  PProp;
	CHBoundaryProp BProp;
    CHMaterialProp MProp;
	CHConductor	  CProp;
	CBlockLabel   blk;


	sprintf(s,"%s.feh",PathName.c_str());
	if ((fp=fopen(s,"rt"))==NULL){
		fprintf(stderr,"Couldn't read from specified .feh file\n");
        return false;
	}

	// define some defaults
	Precision=1.e-08;
	Depth=-1;
	ProblemType=0;
	Coords=0;
	NumPointProps=0;
	NumLineProps=0;
	NumBlockProps=0;
	NumCircProps=0;

	// parse the file

	while (fgets(s,1024,fp)!=NULL)
	{
		sscanf(s,"%s",q);

		// Precision
		if( _strnicmp(q,"[precision]",11)==0){
			v=StripKey(s);
			sscanf(v,"%lf",&Precision);
			q[0]='\0';
		}

		// Units of length used by the problem
		if( _strnicmp(q,"[lengthunits]",13)==0){
			v=StripKey(s);
			sscanf(v,"%s",q);
			if( _strnicmp(q,"inches",6)==0) LengthUnits=0;
			else if( _strnicmp(q,"millimeters",11)==0) LengthUnits=1;
			else if( _strnicmp(q,"centimeters",1)==0) LengthUnits=2;
			else if( _strnicmp(q,"mils",4)==0) LengthUnits=4;
			else if( _strnicmp(q,"microns",6)==0) LengthUnits=5;
			else if( _strnicmp(q,"meters",6)==0) LengthUnits=3;
			q[0]='\0';
		}

		// Depth for 2D planar problems;
		if( _strnicmp(q,"[depth]",7)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&Depth);
		   q[0]='\0';
		}

		// Problem Type (planar or axisymmetric)
		if( _strnicmp(q,"[problemtype]",13)==0){
			v=StripKey(s);
			sscanf(v,"%s",q);
			if( _strnicmp(q,"planar",6)==0) ProblemType=0;
			if( _strnicmp(q,"axisymmetric",3)==0) ProblemType=1;
			q[0]='\0';
		}

		// Coordinates (cartesian or polar)
		if( _strnicmp(q,"[coordinates]",13)==0){
			v=StripKey(s);
			sscanf(v,"%s",q);
			if ( _strnicmp(q,"cartesian",4)==0) Coords=0;
			if ( _strnicmp(q,"polar",5)==0) Coords=1;
			q[0]='\0';
		}

		// properties for axisymmetric external region
		if( _strnicmp(q,"[extzo]",7)==0){
			v=StripKey(s);
			sscanf(v,"%lf",&extZo);
			q[0]='\0';
		}

		if( _strnicmp(q,"[extro]",7)==0){
			v=StripKey(s);
			sscanf(v,"%lf",&extRo);
			q[0]='\0';
		}

		if( _strnicmp(q,"[extri]",7)==0){
			v=StripKey(s);
			sscanf(v,"%lf",&extRi);
			q[0]='\0';
		}

		// Point Properties
		if( _strnicmp(q,"[pointprops]",12)==0){
			v=StripKey(s);
			sscanf(v,"%i",&k);
            if (k>0) nodeproplist=new CHPointProp[k];
			q[0]='\0';
		}

		if( _strnicmp(q,"<beginpoint>",11)==0){
			PProp.V=0;
			PProp.qp=0;
			q[0]='\0';
		}

		if( _strnicmp(q,"<tp>",4)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&PProp.V);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<qp>",4)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&PProp.qp);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<endpoint>",9)==0){
			nodeproplist[NumPointProps]=PProp;
			NumPointProps++;
			q[0]='\0';
		}

		// Boundary Properties;
		if( _strnicmp(q,"[bdryprops]",11)==0){
			v=StripKey(s);
			sscanf(v,"%i",&k);
			if (k>0) lineproplist=(CHBoundaryProp *)calloc(k,sizeof(CHBoundaryProp));
			q[0]='\0';
		}

		if( _strnicmp(q,"<beginbdry>",11)==0){
			BProp.BdryFormat=0;
			BProp.Tset=0;
			BProp.Tinf=0;
			BProp.qs=0;
			BProp.beta=0;
			BProp.h=0;
			q[0]='\0';
		}

		if( _strnicmp(q,"<bdrytype>",10)==0){
		   v=StripKey(s);
		   sscanf(v,"%i",&BProp.BdryFormat);
		   q[0]='\0';
		}

        if( _strnicmp(q,"<Tset>",6)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&BProp.Tset);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<qs>",4)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&BProp.qs);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<beta>",6)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&BProp.beta);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<h>",3)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&BProp.h);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<Tinf>",6)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&BProp.Tinf);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<endbdry>",9)==0){
			lineproplist[NumLineProps]=BProp;
			NumLineProps++;
			q[0]='\0';
		}

		// Block Properties;
		if( _strnicmp(q,"[blockprops]",12)==0){
			v=StripKey(s);
			sscanf(v,"%i",&k);
            if (k>0) blockproplist=new CHMaterialProp[k];
			q[0]='\0';
		}

        // timestep
        if( _strnicmp(q,"[dt]",4)==0){
            v=StripKey(s);
            sscanf(v,"%lf",&dT);
            q[0]='\0';
        }

        // Previous Solution File
        if( _strnicmp(q,"[prevsoln]",10)==0){
			int i;
            v=StripKey(s);

            // have to do this carefully to accept a filename with spaces
            k=(int) strlen(v);
            for(i=0;i<k;i++)
                if(v[i]=='\"'){
                    v=v+i+1;
                    i=k;
                }

            k=(int) strlen(v);
            if(k>0) for(i=k-1;i>=0;i--){
                if(v[i]=='\"'){
                    v[i]=0;
                    i=-1;
                }
            }
            PrevSoln = (char *) calloc(k,sizeof(char));
            strcpy(PrevSoln, v);
            q[0]='\0';
        }

		if( _strnicmp(q,"<beginblock>",12)==0){
			MProp.Kx=1;
			MProp.Ky=1;			// permittivity, relative
			MProp.Kt=0;
			MProp.qv=0;			// charge density, C/m^3
			MProp.npts=0;
			q[0]='\0';
		}

		if( _strnicmp(q,"<Kx>",6)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&MProp.Kx);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<Ky>",6)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&MProp.Ky);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<Kt>",6)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&MProp.Kt);
		   MProp.Kt*=1.e6;
		   q[0]='\0';
		}

		if( _strnicmp(q,"<qv>",5)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&MProp.qv);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<TKPoints>",10)==0){
			v=StripKey(s);
			sscanf(v,"%i",&MProp.npts);
			if (MProp.npts>0)
			{
				for(j=0;j<MProp.npts;j++){
					fgets(s,1024,fp);
					sscanf(s,"%lf	%lf",&MProp.Kn[j].re,&MProp.Kn[j].im);
				}
			}
			q[0]='\0';
		}

		if( _strnicmp(q,"<endblock>",9)==0){
			blockproplist[NumBlockProps]=MProp;
			NumBlockProps++;
			q[0]='\0';
		}

		// Conductor Properties
		if( _strnicmp(q,"[conductorprops]",16)==0){
			v=StripKey(s);
			sscanf(v,"%i",&k);
            if(k>0) circproplist=new CHConductor[k];
			q[0]='\0';
		}

		if( _strnicmp(q,"<beginconductor>",16)==0){
			CProp.V=0;
			CProp.q=0;
			CProp.CircType=0;
			q[0]='\0';
		}

		if( _strnicmp(q,"<tc>",4)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&CProp.V);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<qc>",4)==0){
		   v=StripKey(s);
		   sscanf(v,"%lf",&CProp.q);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<conductortype>",15)==0){
		   v=StripKey(s);
		   sscanf(v,"%i",&CProp.CircType);
		   q[0]='\0';
		}

		if( _strnicmp(q,"<endconductor>",14)==0){
			circproplist[NumCircProps]=CProp;
			NumCircProps++;
			q[0]='\0';
		}

		// read in regional attributes
		if(_strnicmp(q,"[numblocklabels]",13)==0){
			int i;
			v=StripKey(s);
			sscanf(v,"%i",&k);
            if (k>0) labellist=new CBlockLabel[k];
			NumBlockLabels=k;
			for(i=0;i<k;i++)
			{
				fgets(s,1024,fp);
                int ext=0;
				sscanf(s,"%lf	%lf	%i	%lf	%i	%i",&blk.x,&blk.y,&blk.BlockType,
                    &blk.MaxArea,&blk.InGroup,&ext);
                blk.IsDefault  = ext & 2;
                blk.IsExternal = ext & 1;
				blk.BlockType--;
				labellist[i]=blk;
			}
			q[0]='\0';
		}
	}

	fclose(fp);

	return 1;
}

int HSolver::LoadPrev()
{
    if (strlen(PrevSoln)==0) return true;

	FILE *fp;
    double x,y;
    int k;
	char s[1024],q[256];

    if ((fp=fopen(PrevSoln,"rt"))==NULL)
	{
		return BADELEMENTFILE;
	}

	// parse the file
	k=0;
	while (fgets(s,1024,fp)!=NULL)
	{
		sscanf(s,"%s",q);
		if( _strnicmp(q,"[solution]",11)==0){
			k=1;
			break;
		}
	}

	// case where the solution is never found.
	if (k==0)
	{
		fclose(fp);
		return BADELEMENTFILE;
	}

	// read in the solution
	fgets(s,1024,fp);
	sscanf(s,"%i",&k);
	if(k!=NumNodes)
	{
		fclose(fp);
		return BADELEMENTFILE;
	}

    Tprev=new double[NumNodes];

	for(k=0;k<NumNodes;k++)
	{
		fgets(s,1024,fp);
		sscanf(s,"%lf	%lf	%lf",&x,&y,&Tprev[k]);
	}

	return 0;
}

int HSolver::LoadMesh(bool deleteFiles)
{
	int i,j,k,q,n0,n1,n;
	char infile[256];
	FILE *fp;
	char s[1024];
    double c[]={0.0254,0.001,0.01,1,2.54e-5,1.e-6};


	//read meshnodes;
	sprintf(infile,"%s.node",PathName.c_str());
	if((fp=fopen(infile,"rt"))==NULL){
		return BADELEMENTFILE;
	}
	fgets(s,1024,fp);
	sscanf(s,"%i",&k);
	NumNodes = k;

    meshnode = new CNode[k];
    CNode node;
	for(i = 0; i < k; i++)
	{
		fscanf(fp,"%i",&j);
		fscanf(fp,"%lf",&node.x);
		fscanf(fp,"%lf",&node.y);
		fscanf(fp,"%i",&n);

		if (n > 1)
		{
			// strip off point BC number;
			j = n & 0xffff;
			j = j - 2;
			if (j<0)
			{
			    j = -1;
			}

			// strip off Conductor number
			n = (n - (n & 0xffff))/0x10000 - 1;
		}
		else
		{
			j = -1;
			n = -1;
		}
        node.BoundaryMarker = j;
		node.InConductor = n;

		// convert all lengths to internal working units of mm
		node.x *= c[LengthUnits];
		node.y *= c[LengthUnits];

		meshnode[i] = node;
	}
	fclose(fp);

	//read in periodic boundary conditions;
	sprintf(infile,"%s.pbc",PathName.c_str());
	if((fp=fopen(infile,"rt"))==NULL){
		return BADPBCFILE;
	}
	fgets(s,1024,fp);
	sscanf(s,"%i",&k);
	NumPBCs=k;

	if (k!=0) pbclist=(CCommonPoint *)calloc(k,sizeof(CCommonPoint));
	CCommonPoint pbc;
	for(i=0;i<k;i++){
		fscanf(fp,"%i",&j);
		fscanf(fp,"%i",&pbc.x);
		fscanf(fp,"%i",&pbc.y);
		fscanf(fp,"%i",&pbc.t);
		pbclist[i]=pbc;
	}
	fclose(fp);

	// read in elements;
	sprintf(infile,"%s.ele",PathName.c_str());
	if((fp=fopen(infile,"rt"))==NULL){
		return BADELEMENTFILE;
	}
	fgets(s,1024,fp);
	sscanf(s,"%i",&k); NumEls=k;

	meshele=(CElement *)calloc(k,sizeof(CElement));
	CElement elm;

	int defaultLabel;
	for(i=0,defaultLabel=-1;i<NumBlockLabels;i++)
		if (labellist[i].IsDefault) defaultLabel=i;

	for(i=0;i<k;i++){
		fscanf(fp,"%i",&j);
		fscanf(fp,"%i",&elm.p[0]);
		fscanf(fp,"%i",&elm.p[1]);
		fscanf(fp,"%i",&elm.p[2]);
		fscanf(fp,"%i",&elm.lbl);
		elm.lbl--;
		if(elm.lbl<0) elm.lbl=defaultLabel;
		if(elm.lbl<0){
		    string msg = "Material properties have not been defined for\n";
            msg += "all regions. Press the \"Run Mesh Generator\"\n";
            msg += "button to highlight the problem regions.";
            WarnMessage(msg.c_str());

			fclose(fp);
            if (deleteFiles)
            {
                sprintf(infile,"%s.ele",PathName.c_str());
                remove(infile);
                sprintf(infile,"%s.node",PathName.c_str());
                remove(infile);
                sprintf(infile,"%s.pbc",PathName.c_str());
                remove(infile);
                sprintf(infile,"%s.poly",PathName.c_str());
                remove(infile);
                sprintf(infile,"%s.edge",PathName.c_str());
                remove(infile);
            }
            return MISSINGMATPROPS;
		}
		// look up block type out of the list of block labels
		elm.blk=labellist[elm.lbl].BlockType;

		meshele[i]=elm;
	}
	fclose(fp);

	// initialize edge bc's and element permeabilities;
	for(i=0;i<NumEls;i++)
		for(j=0;j<3;j++)
			meshele[i].e[j] = -1;

	// read in edges to which boundary conditions are applied;

		// first, do a little bookkeeping so that element
		// associated with a give edge can be identified fast
		int *nmbr;
		int **mbr;

		nmbr=(int *)calloc(NumNodes,sizeof(int));

		// Make a list of how many elements that tells how
		// many elements to which each node belongs.
		for(i=0;i<NumEls;i++)
			for(j=0;j<3;j++)
				nmbr[meshele[i].p[j]]++;

		// mete out some memory to build a list of the
		// connectivity...
		mbr=(int **)calloc(NumNodes,sizeof(int *));
		for(i=0;i<NumNodes;i++){
			mbr[i]=(int *)calloc(nmbr[i],sizeof(int));
			nmbr[i]=0;
		}

		// fill up the connectivity information;
		for(i=0;i<NumEls;i++)
			for(j=0;j<3;j++)
			{
				k=meshele[i].p[j];
				mbr[k][nmbr[k]]=i;
				nmbr[k]++;
			}

	sprintf(infile,"%s.edge",PathName.c_str());
	if((fp=fopen(infile,"rt"))==NULL)
	{
		return BADEDGEFILE;
	}
	fscanf(fp,"%i",&k);	// read in number of lines

	fscanf(fp,"%i",&j);	// read in boundarymarker flag;
	for(i=0;i<k;i++)
	{
		fscanf(fp,"%i",&j);
		fscanf(fp,"%i",&n0);
		fscanf(fp,"%i",&n1);
		fscanf(fp,"%i",&n);

		// BC number;
		if (n<0)
		{
			n=(-n);
			j = (n & 0xffff) - 2;
			if (j<0) j = -1;

			// Conductor number;
			n= (n - (n & 0xffff))/0x10000 - 1;
			if (n>=0)
			{
				meshnode[n0].InConductor=n;
				meshnode[n1].InConductor=n;
			}
		}
		else j=-1;

		if (j>=0)
		{
			// search through elements to find one containing the line;
			// set corresponding edge equal to the bc number
            for(q=0,n=false;q<nmbr[n0];q++)
			{
				elm=meshele[mbr[n0][q]];

                if ((elm.p[0] == n0) && (elm.p[1] == n1)) {elm.e[0]=j; n=true;}
                if ((elm.p[0] == n1) && (elm.p[1] == n0)) {elm.e[0]=j; n=true;}

                if ((elm.p[1] == n0) && (elm.p[2] == n1)) {elm.e[1]=j; n=true;}
                if ((elm.p[1] == n1) && (elm.p[2] == n0)) {elm.e[1]=j; n=true;}

                if ((elm.p[2] == n0) && (elm.p[0] == n1)) {elm.e[2]=j; n=true;}
                if ((elm.p[2] == n1) && (elm.p[0] == n0)) {elm.e[2]=j; n=true;}

				meshele[mbr[n0][q]]=elm;

				//this is a little hack: line charge distributions should be applied
				//to at most one element;
				if((lineproplist[j].BdryFormat==2) && (n)) q=nmbr[n0];
			}
		}

	}
	fclose(fp);

	// free up the connectivity information
	free(nmbr);
	for(i=0;i<NumNodes;i++) free(mbr[i]);
	free(mbr);

    if (deleteFiles)
    {
        // clear out temporary files
        sprintf(infile,"%s.ele",PathName.c_str());
        remove(infile);
        sprintf(infile,"%s.node",PathName.c_str());
        remove(infile);
        sprintf(infile,"%s.pbc",PathName.c_str());
        remove(infile);
        sprintf(infile,"%s.poly",PathName.c_str());
        remove(infile);
    }

    return 0;
}

//CComplex CMaterialProp::GetK(double t)
//{
//	int i,j;
//
//	// Kx returned as real part;
//	// Ky returned as imag part
//
//	if (npts==0) return (Kx+I*Ky);
//	if (npts==1) return (Im(Kn[0])*(1+I));
//	if (t<=Re(Kn[0])) return (Im(Kn[0])*(1+I));
//	if (t>=Re(Kn[npts-1])) return (Im(Kn[npts-1])*(1+I));
//
//	for(i=0,j=1;j<npts;i++,j++)
//	{
//		if((t>=Re(Kn[i])) && (t<=Re(Kn[j])))
//		{
//			return (1+I)*(Im(Kn[i])+Im(Kn[j]-Kn[i])*Re(t-Kn[i])/Re(Kn[j]-Kn[i]));
//		}
//	}
//
//	return (Kx+I*Ky);
//}

//////////////////////////////////////////////////////////////////////////////////////////////

int HSolver::AnalyzeProblem(CHBigLinProb &L)
{
	int i,j,k,bf,pctr=0;
	double Me[3][3],be[3];		// element matrices;
	double l[3],p[3],q[3];		// element shape parameters;
	int n[3],ne[3];				// numbers of nodes for a particular element;
	double a,K,r,z,kludge;
	double bta,Tinf,Tlast,*Vo;
    int IsNonlinear=false;
	CElement *El;
	CComplex kn;
	int iter=0;

	Depth*=units[LengthUnits];
	extRo*=units[LengthUnits];
	extRi*=units[LengthUnits];
	extZo*=units[LengthUnits];
	kludge=1;

	//TheView->SetDlgItemText(IDC_FRAME1,"Matrix Construction");

	Vo=(double *) calloc(NumNodes,sizeof(double));

	// scan through the problem to see if there are any elements
	// with a nonlinear conductivity
	for(i=0;i<NumNodes;i++)
	{
		if (blockproplist[meshele[i].blk].npts>0){
            IsNonlinear=true;
			i=NumNodes;
		}
	}

	do{
		// copy old solution
		for(i=0;i<NumNodes;i++) Vo[i]=L.V[i];
		L.Wipe();

		// do some book-keeping related to fixed boundary conditions;
		// The P vector denotes which nodes have an assigned value
		// The V vector denotes the assigned value
		for(i=0;i<NumNodes;i++)
		{
			L.Q[i] = -2;
            if(meshnode[i].BoundaryMarker >= 0)
                if(nodeproplist[meshnode[i].BoundaryMarker].qp == 0)
				{
                    L.V[i] = nodeproplist[meshnode[i].BoundaryMarker].V;
					L.Q[i] = -1;
				}

			if(meshnode[i].InConductor >= 0)
				if(circproplist[meshnode[i].InConductor].CircType == 1)
				{
					L.V[i] = circproplist[meshnode[i].InConductor].V;
					L.Q[i] = meshnode[i].InConductor;
				}
		}

		// account for fixed boundary conditions along segments;
		for(i=0;i<NumEls;i++)
		{
			for(j=0;j<3;j++){
				k=j+1; if(k==3) k=0;
				if(meshele[i].e[j]>=0)
				{
					if(lineproplist[ meshele[i].e[j] ].BdryFormat==0)
					{
						L.V[meshele[i].p[j]]=lineproplist[meshele[i].e[j]].Tset;
						L.V[meshele[i].p[k]]=lineproplist[meshele[i].e[j]].Tset;
						L.Q[meshele[i].p[j]]=-1;
						L.Q[meshele[i].p[k]]=-1;
					}
				}
			}
		}


		// build element matrices using the matrices derived in Allaire's book.
		for(i=0;i<NumEls;i++)
		{
			// update progress bar
			j=5*((i*20)/NumEls);
			if(j!=pctr){ pctr=j;
               // TheView->m_prog1.SetPos(pctr);
            }

			// zero out Me, be;
			for(j=0;j<3;j++){
				for(k=0;k<3;k++) Me[j][k]=0;
				be[j]=0;
			}

			// Determine shape parameters.
			// l's are element side lengths;
			// p's corresponds to the `b' parameter in Allaire
			// q's corresponds to the `c' parameter in Allaire
			El=&meshele[i];

			for(k=0;k<3;k++) n[k]=El->p[k];
			p[0]=meshnode[n[1]].y - meshnode[n[2]].y;
			p[1]=meshnode[n[2]].y - meshnode[n[0]].y;
			p[2]=meshnode[n[0]].y - meshnode[n[1]].y;
			q[0]=meshnode[n[2]].x - meshnode[n[1]].x;
			q[1]=meshnode[n[0]].x - meshnode[n[2]].x;
			q[2]=meshnode[n[1]].x - meshnode[n[0]].x;
			for(j=0,k=1;j<3;k++,j++){
				if (k==3) k=0;
				l[j]=sqrt( sq(meshnode[n[k]].x-meshnode[n[j]].x) +
						   sq(meshnode[n[k]].y-meshnode[n[j]].y) );
			}
			a=(p[0]*q[1]-p[1]*q[0])/2.;
			r=(meshnode[n[0]].x+meshnode[n[1]].x+meshnode[n[2]].x)/3.;

			// get the thermal conductivites to use for this element;
			kn = (blockproplist[El->blk].GetK(Vo[n[0]]) +
				  blockproplist[El->blk].GetK(Vo[n[1]]) +
				  blockproplist[El->blk].GetK(Vo[n[2]]))/3.;

			if (ProblemType==AXISYMMETRIC){
				Depth=2.*PI*r;

				// "Warp" the permeability of this element is part of
				// the conformally mapped external region
				if(labellist[meshele[i].lbl].IsExternal)
				{
					z=(meshnode[n[0]].y+meshnode[n[1]].y+meshnode[n[2]].y)/3. - extZo;
					kludge=(r*r+z*z)/(extRi*extRo);
				}
				else kludge=1;
			}


			// x-contribution;
			K = -Depth*Re(kn)/(4.*a)/kludge;
			for(j=0;j<3;j++)
				for(k=j;k<3;k++)
				{
					Me[j][k] += K*p[j]*p[k];
					if (j!=k) Me[k][j]+=K*p[j]*p[k];
				}

			// y-contribution;
			K = -Depth*Im(kn)/(4.*a)/kludge;
			for(j=0;j<3;j++)
				for(k=j;k<3;k++)
				{
					Me[j][k] +=K*q[j]*q[k];
					if (j!=k) Me[k][j]+=K*q[j]*q[k];
				}

			// contribution to Me and be from time-transient term
/*			if (dT!=0)
			{
				K = -Depth*blockproplist[El->blk].Kt*a/(12.*dT);

				Me[0][0]+=2.*K;
				Me[1][1]+=2.*K;
				Me[2][2]+=2.*K;
				Me[0][1]+=K; Me[1][0]+=K;
				Me[0][2]+=K; Me[2][0]+=K;
				Me[1][2]+=K; Me[2][1]+=K;

				be[0]+=K*(2.*Tprev[n[0]] +    Tprev[n[1]] +    Tprev[n[2]]);
				be[1]+=K*(   Tprev[n[0]] + 2.*Tprev[n[1]] +    Tprev[n[2]]);
				be[2]+=K*(   Tprev[n[0]] +    Tprev[n[1]] + 2.*Tprev[n[2]]);
			} */

			if (dT!=0)
			{
				K = -Depth*blockproplist[El->blk].Kt*a/(3.*dT);

				Me[0][0]+=K;
				Me[1][1]+=K;
				Me[2][2]+=K;

				be[0]+=K*Tprev[n[0]];
				be[1]+=K*Tprev[n[1]];
				be[2]+=K*Tprev[n[2]];
			}

			// contribution to be[] from volume charge density
			for(j = 0;j<3;j++){
				K = -Depth*(blockproplist[El->blk].qv)*a/3.;
				be[j]+=K;
			}


			for(j=0;j<3;j++)
			{
				if (El->e[j] >= 0)
				{
					k=j+1; if(k==3) k=0;

					if (ProblemType==AXISYMMETRIC)
						Depth=PI*(meshnode[n[j]].x + meshnode[n[k]].x);

					// contributions to Me, be from derivative boundary conditions;
					// !!! need to put in contribution here for radiation....
					bf=lineproplist[El->e[j]].BdryFormat;
					if ((bf==1) || (bf==2) || (bf==3))
					{
						double c0,c1;

						switch(bf)
						{
							case 1:
								c1=lineproplist[El->e[j]].qs;
								c0=0;
								break;
							case 2:
								c0=lineproplist[El->e[j]].h;
								c1=-c0*lineproplist[El->e[j]].Tinf;
								break;
							case 3:
                                IsNonlinear=true;
								bta =lineproplist[El->e[j]].beta;
								Tinf=lineproplist[El->e[j]].Tinf;
								Tlast=(Vo[n[j]]+Vo[n[k]])/2.;

								c0 = 4.*bta*Ksb*pow(Tlast,3.);
								c1 = -(bta*Ksb*(pow(Tinf,4.) + 3.*pow(Tlast,4.)));

								break;
							default:
								break;
						}

						if (ProblemType==AXISYMMETRIC)
						{
							K =-2.*PI*c0*l[j]/6.;
							Me[j][j]+=K*2. *(3.*meshnode[n[j]].x + meshnode[n[k]].x)/4.;
							Me[k][k]+=K*2. *(meshnode[n[j]].x + 3.*meshnode[n[k]].x)/4.;
							Me[j][k]+=K    *(meshnode[n[j]].x + meshnode[n[k]].x)/2.;
							Me[k][j]+=K    *(meshnode[n[j]].x + meshnode[n[k]].x)/2.;

							K = 2.*PI*c1*l[j]/2.;
							be[j]+=K*(2.*meshnode[n[j]].x + meshnode[n[k]].x)/3.;
							be[k]+=K*(meshnode[n[j]].x + 2.*meshnode[n[k]].x)/3.;
						}
						else
						{
							K =-Depth*c0*l[j]/6.;
							Me[j][j]+=K*2.;
							Me[k][k]+=K*2.;
							Me[j][k]+=K;
							Me[k][j]+=K;

							K = Depth*c1*l[j]/2.;
							be[j]+=K;
							be[k]+=K;
						}
					}
				/*
					// contribution to be[] from surface heating
					if (lineproplist[El->e[j]].BdryFormat==2)
					{
						K =-Depth*lineproplist[El->e[j]].qs*l[j]/2.;
						be[j]+=K;
						be[k]+=K;
					}
				*/
				}
			}

			// process any prescribed nodal values;
			for(j=0;j<3;j++)
			{
				if(L.Q[n[j]]!=-2)
				{
					for(k=0;k<3;k++)
					{
						if(j!=k){
							be[k]-=Me[k][j]*L.V[n[j]];
							Me[k][j]=0;
							Me[j][k]=0;
						}
					}
					be[j]=L.V[n[j]]*Me[j][j];
				}
			}

			// combine block matrices into global matrices;
			for (j=0;j<3;j++)
			{
				ne[j]=n[j];
				if(meshnode[n[j]].InConductor>=0)
					if(circproplist[meshnode[n[j]].InConductor].CircType==0)
						ne[j]=meshnode[n[j]].InConductor+NumNodes;
			}
			for (j=0;j<3;j++){
				for (k=j;k<3;k++)
                L.Put(L.Get(ne[j],ne[k])-Me[j][k],ne[j],ne[k]);
				L.b[ne[j]]-=be[j];

				if(ne[j]!=n[j])
				{
					L.Put(L.Get(n[j],n[j])-Me[j][j],n[j],n[j]);
					L.Put(L.Get(n[j],ne[j])+Me[j][j],n[j],ne[j]);
				}
			}

		} // end of loop that builds element matrices

		// add in contribution from point charge density;
		for(i=0;i<NumNodes;i++)
		{
            if((meshnode[i].BoundaryMarker>=0) && (L.Q[i]==-2))
			{
				if (ProblemType==AXISYMMETRIC) Depth=2.*PI*meshnode[i].x;
                L.b[i]+=(Depth*nodeproplist[meshnode[i].BoundaryMarker].qp);
				L.Q[i]=-1;
			}

			// some bookkeeping to denote which nodes we can smooth over
			if(meshnode[i].InConductor>=0) L.Q[i]=meshnode[i].InConductor;
		}

		// Apply any periodicity/antiperiodicity boundary conditions that we have
		for(k=0,pctr=0;k<NumPBCs;k++)
		{
			if (pbclist[k].t==0) L.Periodicity(pbclist[k].x,pbclist[k].y);
			if (pbclist[k].t==1) L.AntiPeriodicity(pbclist[k].x,pbclist[k].y);
		}

		// Finish building the equations that assign conductor voltage;
		for(i=0;i<NumCircProps;i++)
		{
			// put a placeholder on the main diagonal;
			k=NumNodes+i;

			if (circproplist[i].CircType==1)
			{
				K=L.Get(0,0);
				L.Put(K,k,k);
				L.b[k]=K*circproplist[i].V;
			}

			if(circproplist[i].CircType==0)
			{
				for(j=0,K=0;j<L.n;j++) if(j!=k) K+=L.Get(k,j);
				if(K!=0){
					L.Put(-K,k,k);
					L.b[k]=circproplist[i].q;
				}
				else L.Put(L.Get(0,0),k,k);


			}
		}

		// solve the problem;
        if (L.PCGSolve(iter++)==false){
			free(Vo);
            return false;
		}

        if (IsNonlinear == true)
		{
			double e1=0;
			double e2=0;
			int prog;
			char fmsg[256];

			sprintf(fmsg,"Iteration(%i) ",iter);
            printf("%s", fmsg);
			//TheView->SetDlgItemText(IDC_FRAME2,fmsg);

			for(i=0;i<NumNodes;i++){
				e1+=(L.V[i]-Vo[i])*(L.V[i]-Vo[i]);
				e2+=(Vo[i]*Vo[i]);
			}
			if(e2!=0)
			{
				// test to see if we have converged.
                if(sqrt(e1/e2) < Precision*100.) IsNonlinear=false;
				prog=(int)  (100.*log10(e1/e2)/(log10(Precision)+2.));
				if (prog>100) prog=100;
			//	TheView->m_prog2.SetPos(prog);

			}
		}

    }while(IsNonlinear == true);

	// compute total charge on conductors
	// with a specified voltage
	for(i=0;i<NumCircProps;i++)
		if(circproplist[i].CircType==1)
			circproplist[i].q=ChargeOnConductor(i,L);

	free(Vo);
    return true;
}

//=========================================================================
//=========================================================================

int HSolver::WriteResults(CHBigLinProb &L)
{
	// write solution to disk;

	char c[1024];
	FILE *fp,*fz;
	int i;
	double cf;
	// first, echo input .feh file to the .anh file;
	sprintf(c,"%s.feh",PathName.c_str());

	fz=fopen(c,"rt");
	if(fz==NULL)
    {
		printf("Couldn't open %s.feh\n", PathName.c_str());
        return false;
	}

    sprintf(c,"%s.anh",PathName.c_str());
    fp=fopen(c,"wt");
	if(fp==NULL)
    {
		printf("Couldn't write to %s.anh",PathName.c_str());
        return false;
	}

	while(fgets(c,1024,fz)!=NULL)
    {
        fputs(c,fp);
    }
	fclose(fz);

	// then print out node, line, and element information
	fprintf(fp,"[Solution]\n");
    // get conversion factor for conversion from internal working units of 
    // mm to the specified length units
	cf = units[LengthUnits];
	fprintf(fp,"%i\n",NumNodes);
	for(i=0;i<NumNodes;i++)
    {
		fprintf(fp,"%.17g	%.17g	%.17g	%i\n",meshnode[i].x/cf,meshnode[i].y/cf,L.V[i],L.Q[i]);
    }
    
	fprintf(fp,"%i\n",NumEls);
    
	for(i=0;i<NumEls;i++)
    {
		fprintf(fp,"%i	%i	%i	%i\n",
			meshele[i].p[0],meshele[i].p[1],meshele[i].p[2],meshele[i].lbl);
    }

	// print out circuit info
	fprintf(fp,"%i\n",NumCircProps);
	for(i=0;i<NumCircProps;i++)
    {
		fprintf(fp,"%.17g	%.17g\n",L.V[NumNodes+i],circproplist[i].q);
    }

	fclose(fp);
    return true;
}

//=========================================================================
//=========================================================================


double HSolver::ChargeOnConductor(int u, CHBigLinProb &L)
{
	int i,k;
	double b[3],c[3];		// element shape parameters;
	int n[3];				// numbers of nodes for a particular element;
	double a,da,Dx,Dy,vx,vy,Z;
	CComplex kn;

	for(i=0;i<NumNodes;i++)
		if(meshnode[i].InConductor==u) L.P[i]=1;
		else L.P[i]=0;

	// build element matrices using the matrices derived in Allaire's book.
	for(i=0,Z=0;i<NumEls;i++)
	{
		for(k=0;k<3;k++) n[k]=meshele[i].p[k];

		if((L.P[n[0]]!=0) || (L.P[n[1]]!=0) || (L.P[n[2]]!=0))
		{
			// Determine shape parameters.
			b[0]=meshnode[n[1]].y - meshnode[n[2]].y;
			b[1]=meshnode[n[2]].y - meshnode[n[0]].y;
			b[2]=meshnode[n[0]].y - meshnode[n[1]].y;
			c[0]=meshnode[n[2]].x - meshnode[n[1]].x;
			c[1]=meshnode[n[0]].x - meshnode[n[2]].x;
			c[2]=meshnode[n[1]].x - meshnode[n[0]].x;
			da=(b[0]*c[1]-b[1]*c[0]);
			a=da/2.;
			if (ProblemType==AXISYMMETRIC)
				a*=(2.*PI*(meshnode[n[0]].x+meshnode[n[1]].x+meshnode[n[2]].x)/3.);
			else a*=Depth;
			// get normal vector and element flux density;
			for(k=0,kn=0,vx=0,vy=0,Dx=0,Dy=0;k<3;k++)
			{
				vx-=(L.P[n[k]]*b[k])/da;
				vy-=(L.P[n[k]]*c[k])/da;
				Dx-=(L.V[n[k]]*b[k])/da;
				Dy-=(L.V[n[k]]*c[k])/da;
				kn+=blockproplist[meshele[i].blk].GetK(L.V[n[k]])/3.;
			}
			Dx*=Re(kn);
			Dy*=Im(kn);

			Z+=a*(Dx*vx+Dy*vy);
		}
	}

	return Z;
}


// SortNodes: sorts mesh nodes based on a new numbering
void HSolver::SortNodes (int* newnum)
{
    int j=0,n=0;

    // sort mesh nodes based on newnum;
    for(int i = 0; i < NumNodes; i++)
    {
        while(newnum[i] != i)
        {
            CNode swap;

            j = newnum[i];
            n = newnum[j];
            newnum[j] = newnum[i];
            newnum[i] = n;
            swap = meshnode[j];
            meshnode[j] = meshnode[i];
            meshnode[i] = swap;
        }
    }
}
