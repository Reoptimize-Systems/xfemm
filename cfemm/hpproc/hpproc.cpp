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

   Date Modified: 2017
   By:  Emoke Szelitzky
        Tibor Szelitzky
        Richard Crozier
        Johannes Zarl-Zierl
   Contact:
        szelitzkye@gmail.com
        sztibi82@gmail.com
        richard.crozier@yahoo.co.uk
        johannes.zarl-zierl@jku.at

   Contributions by Johannes Zarl-Zierl were funded by
   Linz Center of Mechatronics GmbH (LCM)
*/

// hpproc.cpp : implementation of the HPProc class
//
#include "hpproc.h"

#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"
#include "stringTools.h"

#include <cassert>
#include <cmath>
#include <string>
#include <sstream>

#ifndef _MSC_VER
#define _strnicmp strncasecmp
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//using namespace std;
using namespace femm;
using std::string;

using femmsolver::CHSElement;
using femmsolver::CHMeshNode;

// HPProc construction/destruction

namespace {
constexpr double sqr(double x)
{
    return x*x;
}
} // anon namespace

/**
 * Constuctor for the HPProc class.
 */

HPProc::HPProc()
{
	A_High=0.;
	A_Low=0.;
	A_lb=0.;
	A_ub=0.;
	for(int i=0;i<4;i++)
		d_PlotBounds[i][0]=d_PlotBounds[i][1]=
		PlotBounds[i][0]=PlotBounds[i][1]=0;

}

HPProc::~HPProc()
{
}

const CHSElement *HPProc::getMeshElement(int idx) const
{
    return reinterpret_cast<CHSElement*>(meshelems[idx].get());
}

const CHMeshNode *HPProc::getMeshNode(int idx) const
{
    return reinterpret_cast<CHMeshNode*>(meshnodes[idx].get());
}


/////////////////////////////////////////////////////////////////////////////
// HPProc serialization

//void HPProc::Serialize(CArchive& ar)
//{
//	if (ar.IsStoring())
//	{
//		// TODO: add storing code here
//	}
//	else
//	{
//		// TODO: add loading code here
//	}
//}

/////////////////////////////////////////////////////////////////////////////
// HPProc diagnostics
//
//#ifdef _DEBUG
//void HPProc::AssertValid() const
//{
//	CDocument::AssertValid();
//}
//
//void HPProc::Dump(CDumpContext& dc) const
//{
//	CDocument::Dump(dc);
//}
//#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// HPProc commands

// char* StripKey(char *c);

bool HPProc::OpenDocument(string solutionFile)
{
    std::stringstream err;
    problem = std::make_shared<FemmProblem>(FileType::HeatFlowFile);
    problem->Depth=1/0.0254; // FemmProblem default is 1

    // read data from file
    HeatFlowReader reader(problem,this,err);
    if (reader.parse(solutionFile) != F_FILE_OK)
    {
        PrintWarningMsg(err.str().c_str());
        return false;
    }

    if (problem->FileFormat != 1)
    {
        WarnMessage("This file is from a different version of FEMM\nRe-analyze the problem using the current version.");
        return false;
    }

	// scale depth to meters for internal computations;
    if(problem->Depth==-1) problem->Depth=1; else problem->Depth*=LengthConv[problem->LengthUnits];

	// element centroids and radii;
    for(int i=0;i<(int)meshelems.size();i++)
	{
        // reinterpret_cast possible because there can only be CSElements for our problem type
        CHSElement *e = reinterpret_cast<CHSElement*>(meshelems[i].get());
        e->ctr=Ctr(i);
        e->rsqr=0;
        for(int j=0;j<3;j++)
		{
            double b=sqr(meshnodes[e->p[j]]->x-e->ctr.re)+
              sqr(meshnodes[e->p[j]]->y-e->ctr.im);
            if(b>e->rsqr) e->rsqr=b;
		}
	}

	// Find flux density in each element;
    for(int i=0;i<(int)meshelems.size();i++)
        getElementD(i);

	// Find extreme values of A;
    CHMeshNode *node = reinterpret_cast<CHMeshNode*>(meshnodes[0].get());
    A_Low=node->T;
    A_High=node->T;
    for(int i=1;i<(int)meshnodes.size();i++)
	{
        node = reinterpret_cast<CHMeshNode*>(meshnodes[i].get());
        if (node->T>A_High) A_High=node->T;
        if (node->T<A_Low)  A_Low =node->T;
	}
	// save default values for extremes of A
	A_lb=A_Low;
	A_ub=A_High;

	// build list of elements connected to each node;
	// allocate connections list;
    NumList=(int *)calloc(meshnodes.size(),sizeof(int));
    ConList=(int **)calloc(meshnodes.size(),sizeof(int *));
	// find out number of connections to each node;
    for(int i=0;i<(int)meshelems.size();i++)
        for(int j=0;j<3;j++)
            NumList[meshelems[i]->p[j]]++;

	// allocate space for connections lists;
    for(int i=0;i<(int)meshnodes.size();i++)
		ConList[i]=(int *)calloc(NumList[i],sizeof(int));
	// build list;
    for(int i=0;i<(int)meshnodes.size();i++) NumList[i]=0;
    for(int i=0;i<(int)meshelems.size();i++)
    {
        for(int j=0;j<3;j++){
            int k=meshelems[i]->p[j];
            ConList[k][NumList[k]]=i;
            NumList[k]++;
        }
    }

	// sort each connection list so that the elements are
	// arranged in a counter-clockwise order
    for(int i=0;i<(int)meshnodes.size();i++)
	{
        for(int j=0;j<NumList[i];j++)
		{
            bool swapped=false;
            for(int k=0;k<NumList[i]-j-1;k++)
			{
                CComplex u0=meshelems[ConList[i][k]]->ctr  -meshnodes[i]->CC();
                CComplex u1=meshelems[ConList[i][k+1]]->ctr-meshnodes[i]->CC();
				if(arg(u0)>arg(u1))
				{
                    std::swap(ConList[i][k],ConList[i][k+1]);
                    swapped=true;
				}
			}
            if(!swapped) j=NumList[i];
		}
	}

	// Find extreme values of potential
	d_PlotBounds[0][0]=A_Low;
	d_PlotBounds[0][1]=A_High;
	PlotBounds[0][0]=d_PlotBounds[0][0];
	PlotBounds[0][1]=d_PlotBounds[0][1];

    for(int i=0;i<(int)meshelems.size();i++)
    {
        auto elem = reinterpret_cast<CHSElement*>(meshelems[i].get());
        getNodalD(elem->d,i);
    }

	// Find extreme values of D and E;
    const auto &labellist = problem->labellist;
    int externalElements=0;
    for (const auto& elem : meshelems)
    {
        const auto sElem = reinterpret_cast<CHSElement*>(elem.get());
        if (labellist[sElem->lbl]->IsExternal)
            externalElements++;
    }
    d_PlotBounds[1][0]=abs(getMeshElement(externalElements)->D);
    d_PlotBounds[1][1]=d_PlotBounds[1][0];
    d_PlotBounds[2][0]=abs(E(getMeshElement(0)));
    d_PlotBounds[2][1]=d_PlotBounds[2][0];
    for (const auto& elem : meshelems)
    {
        const auto sElem = reinterpret_cast<CHSElement*>(elem.get());
        if(!labellist[sElem->lbl]->IsExternal){
            double b=abs(sElem->D);
            if(b>d_PlotBounds[1][1]) d_PlotBounds[1][1]=b;
            if(b<d_PlotBounds[1][0]) d_PlotBounds[1][0]=b;

            b=abs(E(sElem));
            if(b>d_PlotBounds[2][1]) d_PlotBounds[2][1]=b;
            if(b<d_PlotBounds[2][0]) d_PlotBounds[2][0]=b;
        }
    }
	PlotBounds[1][0]=d_PlotBounds[1][0];
	PlotBounds[1][1]=d_PlotBounds[1][1];
	PlotBounds[2][0]=d_PlotBounds[2][0];
	PlotBounds[2][1]=d_PlotBounds[2][1];

	// Choose bounds based on the type of contour plot
	// currently in play
    //POSITION pos = GetFirstViewPosition();
    //ChviewView *theView=(ChviewView *)GetNextView(pos);

	// Build adjacency information for each element.
	FindBoundaryEdges();

	// Check to see if any regions are multiply defined
	// (i.e. tagged by more than one block label). If so,
	// display an error message and mark the problem blocks.
    bMultiplyDefinedLabels = false;
    for(int k=0;k<(int)problem->labellist.size();k++)
    {
        assert(labellist[k]);
        const auto label = reinterpret_cast<CHBlockLabel*>(labellist[k].get());
        int i=InTriangle(label->x,label->y);
        if(i>=0)
		{
            const auto& elem = getMeshElement(i);
            if(elem->lbl!=k)
			{
                auto elemLabel = reinterpret_cast<CHBlockLabel*>(labellist[elem->lbl].get());
                elemLabel->IsSelected=true;
				if (!bMultiplyDefinedLabels)
				{
					string msg;
					msg ="Some regions in the problem have been defined\n";
					msg+="by more than one block label.  These potentially\n";
					msg+="problematic regions will appear as selected in\n";
					msg+="the initial view.";
					WarnMessage(msg.c_str());
                    bMultiplyDefinedLabels=true;
				}
			}
		}
	}

    return true;
}

bool HPProc::getPointValues(double x, double y, CHPointVals &u)
{
	int k;
	k=InTriangle(x,y);
    if (k<0) return false;
    getPointValues(x,y,k,u);
    return true;
}

bool HPProc::getPointValues(double x, double y, int k, CHPointVals &u)
{
	int i,n[3];
    double a[3],b[3],c[3],da;
    // double ravg;

    for(i=0;i<3;i++) n[i]=meshelems[k]->p[i];
    a[0]=meshnodes[n[1]]->x * meshnodes[n[2]]->y - meshnodes[n[2]]->x * meshnodes[n[1]]->y;
    a[1]=meshnodes[n[2]]->x * meshnodes[n[0]]->y - meshnodes[n[0]]->x * meshnodes[n[2]]->y;
    a[2]=meshnodes[n[0]]->x * meshnodes[n[1]]->y - meshnodes[n[1]]->x * meshnodes[n[0]]->y;
    b[0]=meshnodes[n[1]]->y - meshnodes[n[2]]->y;
    b[1]=meshnodes[n[2]]->y - meshnodes[n[0]]->y;
    b[2]=meshnodes[n[0]]->y - meshnodes[n[1]]->y;
    c[0]=meshnodes[n[2]]->x - meshnodes[n[1]]->x;
    c[1]=meshnodes[n[0]]->x - meshnodes[n[2]]->x;
    c[2]=meshnodes[n[1]]->x - meshnodes[n[0]]->x;
	da=(b[0]*c[1]-b[1]*c[0]);
    //ravg=LengthConv[LengthUnits]*
    //	(meshnode[n[0]]->x + meshnode[n[1]]->x + meshnode[n[2]]->x)/3.;

    auto elem=getMeshElement(k);
    getPointD(x,y,u.F,*elem);

	u.T=0;
    for(i=0;i<3;i++) u.T+=getMeshNode(n[i])->T*(a[i]+b[i]*x+c[i]*y)/(da);

    const CHMaterialProp *mat = dynamic_cast<CHMaterialProp *>(problem->blockproplist[elem->blk].get());
    u.K=mat->GetK(u.T);
    u.K/=AECF(elem,x+I*y);

	u.G.re = u.F.re/(u.K.re);
	u.G.im = u.F.im/(u.K.im);

    return true;
}

void HPProc::getElementD(int k)
{
    auto elem = reinterpret_cast<CHSElement*>(meshelems[k].get());
    int n[3];
    for(int i=0;i<3;i++) n[i]=elem->p[i];

    double b[3],c[3];
    b[0]=meshnodes[n[1]]->y - meshnodes[n[2]]->y;
    b[1]=meshnodes[n[2]]->y - meshnodes[n[0]]->y;
    b[2]=meshnodes[n[0]]->y - meshnodes[n[1]]->y;
    c[0]=meshnodes[n[2]]->x - meshnodes[n[1]]->x;
    c[1]=meshnodes[n[0]]->x - meshnodes[n[2]]->x;
    c[2]=meshnodes[n[1]]->x - meshnodes[n[0]]->x;
    double da=(b[0]*c[1]-b[1]*c[0]);

    CComplex E(0);
    CComplex kn(0);
    for(int i=0;i<3;i++)
	{
        auto node = getMeshNode(elem->p[i]);
        E-=node->T*(b[i]+I*c[i])/(da*LengthConv[problem->LengthUnits]);

        auto bprop = dynamic_cast<CHMaterialProp*>(problem->blockproplist[elem->blk].get());
        assert(bprop);
        kn+=bprop->GetK(node->T)/3.;
    }
    elem->D=(E.re*kn.re + I*E.im*kn.im)/AECF(elem);
}


//void HPProc::OnReload()
//{
	// TODO: Add your command handler code here
//	CString pname = GetPathName();
//	if(pname.GetLength()>0)
//	{
//		OnNewDocument();
//		SetPathName(pname,false);
//		OnOpenDocument(pname);
//	}
//}

//void HPProc::GetLineValues(CXYPlot &p,int PlotType,int NumPlotPoints)
//{
//	double *q,z,u,dz;
//	CComplex pt,n,t;
//	int i,j,k,m,elm;
//	CPointVals v;
//	bool flag;
//
//	q=(double *)calloc(contour.size(),sizeof(double));
//	for(i=1,z=0.;i<contour.size();i++)
//	{
//		z+=abs(contour[i]-contour[i-1]);
//		q[i]=z;
//	}
//	dz=z/(NumPlotPoints-1);//
//
//	/*
//		m_XYPlotType.AddString("Potential");
//		m_XYPlotType.AddString("|B|        (Magnitude of flux density)");
//		m_XYPlotType.AddString("B . n      (Normal flux density)");
//		m_XYPlotType.AddString("B . t      (Tangential flux density)");
//		m_XYPlotType.AddString("|H|        (Magnitude of field intensity)");
//		m_XYPlotType.AddString("H . n      (Normal field intensity)");
//		m_XYPlotType.AddString("H . t      (Tangential field intensity)");
//		m_XYPlotType.AddString("J_eddy
//	*/
//
//
//	switch (PlotType)
//	{
//		case 0:
//			p.Create(NumPlotPoints,2);
//			strcpy(p.lbls[1],"Temperature, K");
//			break;
//		case 1:
//			p.Create(NumPlotPoints,2);
//			strcpy(p.lbls[1],"|F|, W/m^2");
//			break;
//		case 2:
//			p.Create(NumPlotPoints,2);
//			strcpy(p.lbls[1],"F.n, W/m^2");
//			break;
//		case 3:
//			p.Create(NumPlotPoints,2);
//			strcpy(p.lbls[1],"F.t, W/m^2");
//			break;
//		case 4:
//			p.Create(NumPlotPoints,2);
//			strcpy(p.lbls[1],"|G|, K/m");
//			break;
//		case 5:
//			p.Create(NumPlotPoints,2);
//			strcpy(p.lbls[1],"G.n, K/m");
//			break;
//		case 6:
//			p.Create(NumPlotPoints,2);
//			strcpy(p.lbls[1],"G.t, K/m");
//			break;
//		default:
//			p.Create(NumPlotPoints,2);
//			break;
//	}



//	switch(LengthUnits){
//		case 1:
//			strcpy(p.lbls[0],"Length, mm");
//			break;
//		case 2:
//			strcpy(p.lbls[0],"Length, cm");
//			break;
//		case 3:
//			strcpy(p.lbls[0],"Length, m");
//			break;
//		case 4:
//			strcpy(p.lbls[0],"Length, mils");
//			break;
//		case 5:
//			strcpy(p.lbls[0],"Length, um");
//			break;
//		default:
//			strcpy(p.lbls[0],"Length, inches");
//			break;
//	}

//	for(i=0,k=1,z=0,elm=-1;i<NumPlotPoints;i++,z+=dz)
//	{
//		while((z>q[k]) && (k<(contour.size()-1))) k++;
//		u=(z-q[k-1])/(q[k]-q[k-1]);
//		pt=contour[k-1]+u*(contour[k]-contour[k-1]);
//		t=contour[k]-contour[k-1];
//		t/=abs(t);
//		n = I*t;
//		pt+=(n*1.e-06);

//		if (elm<0) elm=InTriangle(pt.re,pt.im);
//		else if (InTriangleTest(pt.re,pt.im,elm)==false)
//		{
//			flag=false;
//			for(j=0;j<3;j++)
//				for(m=0;m<NumList[meshelem[elm]->p[j]];m++)
//				{
//					elm=ConList[meshelem[elm]->p[j]][m];
//					if (InTriangleTest(pt.re,pt.im,elm)==true)
//					{
//						flag=true;
//						m=100;
//						j=3;
//					}
//				}
//			if (flag==false) elm=InTriangle(pt.re,pt.im);
//		}
//		if(elm>=0)
//			flag=GetPointValues(pt.re,pt.im,elm,v);
//		else flag=false;

//		p.M[i][0]=z;
//		if(flag!=false)
//		{
//			switch (PlotType)
//			{
//				case 0:
//					p.M[i][1]=v.T;
//					break;
//				case 1:
//					p.M[i][1]=abs(v.F);
//					break;
//				case 2:
//					p.M[i][1]=Re(v.F/n);
//					break;
//				case 3:
//					p.M[i][1]=Re(v.F/t);
//					break;
//				case 4:
//					p.M[i][1]=abs(v.G);
//					break;
//				case 5:
//					p.M[i][1]=Re(v.G/n);
//					break;
//				case 6:
//					p.M[i][1]=Re(v.G/t);
//					break;
//				default:
//					p.M[i][1]=0;
//					break;
//			}
//		}
//	}

//	free(q);
//}

bool HPProc::InTriangleTest(double x, double y, int i) const
{
	int j,k;
	double z;
	bool InFlag;

    if(i<0) return false;

    for(j=0,InFlag=true;((j<3) && (InFlag==true));j++)
	{
		k=j+1; if(k==3) k=0;
        z=(meshnodes[meshelems[i]->p[k]]->x-meshnodes[meshelems[i]->p[j]]->x)*
          (y-meshnodes[meshelems[i]->p[j]]->y) -
          (meshnodes[meshelems[i]->p[k]]->y-meshnodes[meshelems[i]->p[j]]->y)*
          (x-meshnodes[meshelems[i]->p[j]]->x);
        if(z<0) InFlag=false;
	}

	return InFlag;
}

CComplex HPProc::blockIntegral(int inttype)
{
	CComplex c,z;
	double T;
	double a,R;
	double r[3];

    R=0;
    z=0;
    for(int i=0;i<(int)meshelems.size();i++)
	{
        if(problem->labellist[meshelems[i]->lbl]->IsSelected==true)
		{
			// compute some useful quantities employed by most integrals...
            a=ElmArea(i)*pow(LengthConv[problem->LengthUnits],2.);
            if(problem->problemType==1){
                for(int k=0;k<3;k++)
                    r[k]=meshnodes[meshelems[i]->p[k]]->x*LengthConv[problem->LengthUnits];
				R=(r[0]+r[1]+r[2])/3.;
			}

			// now, compute the desired integral;
			switch(inttype)
			{
				case 0: // T
                    if(problem->problemType==1) a*=(2.*PI*R); else a*=problem->Depth;
                    T=0;
                    for (int k=0;k<3;k++)
                        T+=getMeshNode(meshelems[i]->p[k])->T/3.;
					z+=a*T;
					break;

				case 1: // cross-section area
					z+=a;
					break;

				case 2: // volume
                    if(problem->problemType==1) a*=(2.*PI*R); else a*=problem->Depth;
					z+=a;
					break;

				case 3: // F
                    if(problem->problemType==1) a*=(2.*PI*R); else a*=problem->Depth;
                    z+=a*getMeshElement(i)->D;
					break;

				case 4: // G
                    if(problem->problemType==1) a*=(2.*PI*R); else a*=problem->Depth;
                    z+=a*E(getMeshElement(i));
					break;

				default:
					break;
			}
		}
	}

	// Integrals 0, 3 and 4 are averages over the selected volume;
	// Need to divide by the block volme to get the average.
    if((inttype==0) || (inttype==3) || (inttype==4)) z/=blockIntegral(2);

	return z;
}

void HPProc::lineIntegral(int inttype, double *z)
{
// inttype	Integral
//		0	G.t
//		1	F.n
//		2	Contour length
//		3   Average Temp.

	// inttype==0 => G.t
	if(inttype==0){
		CHPointVals u;
		int k;

		k=(int) contour.size();
        getPointValues(contour[0].re,contour[0].im, u);
		z[0] = u.T;
        getPointValues(contour[k-1].re,contour[k-1].im,u);
		z[0]-= u.T;
	}

	// inttype==1 => F.n
	if(inttype==1){
		CComplex n,t,pt;
		CHPointVals v;
		double dz,u,d,Fn;
		int NumPlotPoints=d_LineIntegralPoints;
		bool flag;

		z[0]=0;
		z[1]=0;
        for(int k=1;k<(int)contour.size();k++)
		{
			dz=abs(contour[k]-contour[k-1])/((double) NumPlotPoints);
            for(int i=0,elm=-1;i<NumPlotPoints;i++)
			{
				u=(((double) i)+0.5)/((double) NumPlotPoints);
				pt=contour[k-1] + u*(contour[k] - contour[k-1]);
				t=contour[k]-contour[k-1];
				t/=abs(t);
				n=I*t;
				pt+=n*1.e-06;

				if (elm<0) elm=InTriangle(pt.re,pt.im);
                else if (InTriangleTest(pt.re,pt.im,elm)==false)
				{
                    flag=false;
                    for(int j=0;j<3;j++)
                        for(int m=0;m<NumList[meshelems[elm]->p[j]];m++)
						{
                            elm=ConList[meshelems[elm]->p[j]][m];
                            if (InTriangleTest(pt.re,pt.im,elm)==true)
							{
                                flag=true;
								m=100;
								j=3;
							}
						}
                    if (flag==false) elm=InTriangle(pt.re,pt.im);
				}
				if(elm>=0)
                    flag=getPointValues(pt.re,pt.im,elm,v);
                else flag=false;

                if(flag==true){
					Fn = Re(v.F/n);

                    if (problem->problemType==AXISYMMETRIC)
                        d=2.*PI*pt.re*sqr(LengthConv[problem->LengthUnits]);
					else
                        d=problem->Depth*LengthConv[problem->LengthUnits];

					z[0]+=(Fn*dz*d);
					z[1]+=dz*d;
				}
			}
		}
		z[1]=z[0]/z[1]; // Average F.n over the surface;
	}

	// inttype==2 => Contour Length
	if(inttype==2){
		int i,k;
		k=(int) contour.size();
		for(i=0,z[0]=0;i<k-1;i++)
			z[0]+=abs(contour[i+1]-contour[i]);
        z[0]*=LengthConv[problem->LengthUnits];

        if(problem->problemType==1){
			for(i=0,z[1]=0;i<k-1;i++)
				z[1]+=(PI*(contour[i].re+contour[i+1].re)*
						abs(contour[i+1]-contour[i]));
            z[1]*=pow(LengthConv[problem->LengthUnits],2.);
		}
		else{
            z[1]=z[0]*problem->Depth;
		}
	}

	// inttype==3 => Average Temperature
	if(inttype==3){
		CComplex pt;
		CHPointVals v;
		double dz,u,d;
		int NumPlotPoints=d_LineIntegralPoints;
		bool flag;

		z[0]=0;
		z[1]=0;
        for(int k=1;k<(int)contour.size();k++)
		{
			dz=abs(contour[k]-contour[k-1])/((double) NumPlotPoints);
            for(int i=0,elm=-1;i<NumPlotPoints;i++)
			{
				u=(((double) i)+0.5)/((double) NumPlotPoints);
				pt=contour[k-1] + u*(contour[k] - contour[k-1]);

				if (elm<0) elm=InTriangle(pt.re,pt.im);
                else if (InTriangleTest(pt.re,pt.im,elm)==false)
				{
                    flag=false;
                    for(int j=0;j<3;j++)
                        for(int m=0;m<NumList[meshelems[elm]->p[j]];m++)
						{
                            elm=ConList[meshelems[elm]->p[j]][m];
                            if (InTriangleTest(pt.re,pt.im,elm)==true)
							{
                                flag=true;
								m=100;
								j=3;
							}
						}
                    if (flag==false) elm=InTriangle(pt.re,pt.im);
				}
				if(elm>=0)
                    flag=getPointValues(pt.re,pt.im,elm,v);
                else flag=false;

                if(flag==true){
                    if (problem->problemType==AXISYMMETRIC)
                        d=2.*PI*pt.re*sqr(LengthConv[problem->LengthUnits]);
					else
                        d=problem->Depth*LengthConv[problem->LengthUnits];

					z[0]+=(v.T*dz*d);
					z[1]+=dz*d;
				}
			}
		}
		z[0]=z[0]/z[1]; // Average F.n over the surface;
	}

    return;
}

ParserResult HPProc::parseSolution(std::istream &input, std::ostream &err)
{
    using femmsolver::CHMeshNode;
    using femmsolver::CHSElement;

    int k;
    // read in meshnodes;
    parseValue(input, k, err);
    meshnodes.reserve(k);
    for(int i=0;i<k;i++)
    {
        meshnodes.push_back(std::make_unique<CHMeshNode>(CHMeshNode::fromStream(input,err)));
    }

    // read in elements;
    parseValue(input, k, err);
    meshelems.reserve(k);
    auto &labellist = problem->labellist;
    for(int i=0;i<k;i++)
    {
        CHSElement elm = CHSElement::fromStream(input,err);
        elm.blk = labellist[elm.lbl]->BlockType;
        meshelems.push_back(std::make_unique<CHSElement>(elm));
    }

    // read in circuit data;
    auto &circproplist = problem->circproplist;
    parseValue(input, k, err);
    for(int i=0;i<k;i++)
    {
        auto circuit = reinterpret_cast<CHConductor*>(circproplist[i].get());
        // partially overwrite circuit data:
        input >> circuit->V;
        input >> circuit->q;
    }
    return femm::F_FILE_OK;
}

double HPProc::getA_High() const
{
    return A_High;
}

double HPProc::getA_Low() const
{
    return A_Low;
}


CComplex HPProc::E(const femmsolver::CHSElement *elem) const
{
	// return average electric field intensity for the kth element
    CComplex kn=0;

    const auto mat = reinterpret_cast<CHMaterialProp*>(problem->blockproplist[elem->blk].get());
    for(int i=0;i<3;i++)
        kn+=mat->GetK(getMeshNode(elem->p[i])->T)/3.;

    return (elem->D.re/Re(kn) + I*elem->D.im/Im(kn)) * AECF(elem);

	// AECF(k) part corrects permittivity for axisymmetric external region;
}

CComplex HPProc::e(const femmsolver::CHSElement *elem, int i) const
{
	// return nodal temperature gradient for the ith node of the kth element
	double aecf=1;

    if((problem->problemType==AXISYMMETRIC) && (problem->labellist[elem->lbl]->IsExternal))
	{
		// correct for axisymmetric external region
        double x=meshnodes[elem->p[i]]->x;
        double y=meshnodes[elem->p[i]]->y-problem->extZo;
        aecf=(x*x+y*y)/(problem->extRi*problem->extRo);
	}

    const auto mat = reinterpret_cast<CHMaterialProp*>(problem->blockproplist[elem->blk].get());
    CComplex kn=mat->GetK(getMeshNode(elem->p[i])->T);

    return (elem->d[i].re/Re(kn) +
          I*elem->d[i].im/Im(kn)) * aecf;
}
