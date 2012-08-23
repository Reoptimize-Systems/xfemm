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

//#include "stdafx.h"
//#include <afx.h>
//#include <afxtempl.h>
//#include "stdio.h"
#include <vector>
#include <cmath>
#include <cstring>
#include "nosebl.h"
/////////////////////////////////////////////////////////////////////////////
// CNode construction

CNode::CNode()
{
	x=0.;
	y=0.;
	IsSelected=0;
	InGroup=0;
	BoundaryMarker="<None>";
}

double CNode::GetDistance(double xo, double yo)
{
	return std::sqrt((x-xo)*(x-xo) + (y-yo)*(y-yo));
}

CComplex CNode::CC()
{
	return CComplex(x,y);
}

void CNode::ToggleSelect()
{
	if (IsSelected!=0)
	{
        IsSelected=0;
	}
	else
	{
        IsSelected = 1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegment construction

CSegment::CSegment()
{
	n0=0;
	n1=0;
	IsSelected=0;
	Hidden=false;
	MaxSideLength=-1;
	BoundaryMarker="<None>";
	InGroup=0;
}

void CSegment::ToggleSelect()
{
	if (IsSelected!=0)
	{
        IsSelected=0;
	}
	else
	{
        IsSelected = 1;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CArcSegment construction

CArcSegment::CArcSegment()
{
	n0=0;
	n1=0;
	IsSelected=0;
	Hidden=false;
	ArcLength=90.;
	MaxSideLength=10.;
	BoundaryMarker="<None>";
	InGroup=0;
	NormalDirection=true;
}

void CArcSegment::ToggleSelect()
{
	if (IsSelected!=0)
	{
        IsSelected=0;
	}
	else
	{
        IsSelected = 1;
	}
}



/////////////////////////////////////////////////////////////////////////////
// CNode construction

CBlockLabel::CBlockLabel()
{
	x=0.;
	y=0.;
	MaxArea=0.;
	MagDir=0.;
	Turns=1;
	IsSelected=0;
	BlockType="<None>";
	InCircuit="<None>";
	InGroup=0;
	IsExternal=false;
}

void CBlockLabel::ToggleSelect()
{
	if (IsSelected!=0)
	{
        IsSelected=0;
	}
	else
	{
        IsSelected = 1;
	}
}

double CBlockLabel::GetDistance(double xo, double yo)
{
	return sqrt((x-xo)*(x-xo) + (y-yo)*(y-yo));
}

CMaterialProp::CMaterialProp()
{
		BlockName="New Material";
		mu_x=1.;
		mu_y=1.;			// permeabilities, relative
		H_c=0.;				// magnetization, A/m
		Jsrc=0;				// applied current density, MA/m^2
		Cduct=0.;		    // conductivity of the material, MS/m
		Lam_d=0.;			// lamination thickness, mm
		Theta_hn=0.;			// hysteresis angle, degrees
		Theta_hx=0.;			// hysteresis angle, degrees
		Theta_hy=0.;			// hysteresis angle, degrees
		Theta_m=0.;			// magnetization direction, degrees;
		LamFill=1.;			// lamination fill factor;
		LamType=0;			// type of lamination;
		WireD=0;			// strand diameter, mm
		NStrands=0;			// number of strands per wire

		BHpoints=0;

        BHdata.clear();
}

CMaterialProp::~CMaterialProp()
{
	//if(BHpoints>0)
 //       free(BHdata);
}

void CMaterialProp::StripBHData(CStdString &b, CStdString &h)
{
	int i,k;
	char *buff,*nptr,*endptr;
	double z;
	std::vector <double > B;
	std::vector <double > H;

	if (BHpoints>0) BHdata.clear();
	B.clear();
	H.clear();

	k=b.GetLength()+10;
	buff=(char *)calloc(k,sizeof(char));
	strcpy(buff,b);
	nptr=buff;
	while (sscanf(nptr,"%lf",&z)!=EOF){
		z=strtod(nptr,&endptr );
		if(nptr==endptr) nptr++; //catch special case
		else nptr=endptr;
		if(B.size()>0){ // enforce monotonicity
			if (z<=B[B.size()-1])
				break;
		}
		else if(z!=0) B.push_back(0);
		B.push_back(z);
	}
	free(buff);

	k=h.GetLength()+10;
	buff=(char *)calloc(k,sizeof(char));
	strcpy(buff,h);
	nptr=buff;
	while (sscanf(nptr,"%lf",&z)!=EOF){
		z=strtod(nptr,&endptr );
		if(nptr==endptr) nptr++;
		else nptr=endptr;
		if(H.size()>0){
			if (z<=H[H.size()-1])
				break;
		}
		else if(z!=0) H.push_back(0);
		H.push_back(z);
	}

	k=B.size();
	if (H.size()<(unsigned int)k) k=H.size();

	if (k>1){
		BHpoints = k;
        BHdata.resize(k);
		{
			//BHdata=(CComplex *)calloc(k,sizeof(CComplex));
			for(i=0;i<k;i++) BHdata[i].Set(B[i],H[i]);
		}
	}
	else BHpoints=0;
	free(buff);

	return;
}

void CMaterialProp::BHDataToCString(CStdString &b, CStdString &h)
{
	int i;
	char c[80];

	b.Empty();
	h.Empty();

	for(i=0;i<BHpoints;i++){
		sprintf(c,"%f%c%c",BHdata[i].re,0x0D,0x0A);
		b+=c;
		sprintf(c,"%f%c%c",BHdata[i].im,0x0D,0x0A);
		h+=c;
	}

	//b.AnsiToOem();
	//h.AnsiToOem();
}

CBoundaryProp::CBoundaryProp()
{
		BdryName="New Boundary";
		BdryFormat=0;				// type of boundary condition we are applying
									// 0 = constant value of A
									// 1 = Small skin depth eddy current BC
									// 2 = Mixed BC

		A0=0.; A1=0.;
		A2=0.; phi=0.;			// set value of A for BdryFormat=0;

		Mu=0.; Sig=0.;			// material properties necessary to apply
								// eddy current BC

		c0=0.; c1=0.;			// coefficients for mixed BC

}

CPointProp::CPointProp()
{
		PointName="New Point Property";
		Jp=0;					// applied point current, A
		Ap=0;					// prescribed nodal value;
}

CCircuit::CCircuit()
{
		CircName="New Circuit";
		Amps=0;
		CircType=1;
};

CPeriodicBoundary::CPeriodicBoundary()
{
		BdryName="";
		BdryFormat=0;
		nseg=0;
		narc=0;
		seg[0]=0;
		seg[1]=0;
}

CCommonPoint::CCommonPoint()
{
	x=y=t=0;
}

void CCommonPoint::Order()
{
	int z;

	if(x>y){
		z=y;
		y=x;
		x=z;
	}
}
