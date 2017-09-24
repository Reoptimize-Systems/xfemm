/* Copyright 2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by
 * Linz Center of Mechatronics GmbH (LCM)
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#include "epproc.h"

#include "CMeshNode.h"
#include "femmconstants.h"
#include "FemmProblem.h"
#include "FemmReader.h"

#include <string>
#include <sstream>

using namespace femm;
using namespace femmsolver;

namespace {
constexpr double sqr(double x)
{
    return x*x;
}
} // anonymous namespace

ElectrostaticsPostProcessor::ElectrostaticsPostProcessor()
    : PostProcessor()
{
}

ElectrostaticsPostProcessor::~ElectrostaticsPostProcessor()
{
}

femm::ParserResult ElectrostaticsPostProcessor::parseSolution(std::istream &input, std::ostream &err)
{
    using femmsolver::CSMeshNode;
    using femmsolver::CSElement;
    // read in meshnodes;
    int k;
    input >> k;
    meshnodes.reserve(k);
    for(int i=0;i<k;i++)
    {
        meshnodes.push_back(std::make_unique<CSMeshNode>(CSMeshNode::fromStream(input,err)));
    }

    // read in elements;
    input >> k;
    meshelems.reserve(k);
    auto &labellist = problem->labellist;
    for(int i=0;i<k;i++)
    {
        CSElement elm = CSElement::fromStream(input,err);
        elm.blk = labellist[elm.lbl]->BlockType;
        meshelems.push_back(std::make_unique<CSElement>(elm));
    }

    // read in circuit data;
    // first clear original circuit data:
    auto &circproplist = problem->circproplist;
    circproplist.clear();
    input >> k;
    circproplist.reserve(k);
    for(int i=0;i<k;i++)
    {
        circproplist.push_back(
                    std::make_unique<CSCircuit>(CSCircuit::fromStream(input,err))
                    );
    }

    return femm::F_FILE_OK;
}

bool ElectrostaticsPostProcessor::OpenDocument(std::string solutionFile)
{
    std::stringstream err;
    problem = std::make_unique<FemmProblem>(FileType::ElectrostaticsFile);

    // read data from file
    ElectrostaticsReader reader(problem.get(),this,err);
    if (reader.parse(solutionFile) != F_FILE_OK)
        return false;

    // scale depth to meters for internal computations;
    if(problem->Depth==-1) problem->Depth=1; else problem->Depth*=LengthConv[problem->LengthUnits];

    // element centroids and radii;
    for(int i=0; i<(int)meshelems.size(); i++)
    {
        CSElement *e = dynamic_cast<CSElement*>(meshelems[i].get());
        e->ctr=Ctr(i);
        e->rsqr=0;
        for(int j=0;j<3;j++)
        {
            double b=sqr(meshnodes[e->p[j]]->x-e->ctr.re)+
                    sqr(meshnodes[e->p[j]]->y-e->ctr.im);
            if(b>e->rsqr)
                e->rsqr=b;
        }
    }

    // Find flux density in each element;
    for(int i=0;i<(int)meshelems.size();i++)
        GetElementD(i);

    // Find extreme values of A;
    CSMeshNode *node = dynamic_cast<CSMeshNode*>(meshnodes[0].get());
    A_Low=node->V;
    A_High=node->V;
    for(int i=1;i<(int)meshnodes.size();i++)
    {
        node = dynamic_cast<CSMeshNode*>(meshnodes[i].get());
        if (node->V>A_High) A_High=node->V;
        if (node->V<A_Low)  A_Low =node->V;
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
        for(int j=0;j<3;j++){
            int k=meshelems[i]->p[j];
            ConList[k][NumList[k]]=i;
            NumList[k]++;
        }

    // sort each connection list so that the elements are
    // arranged in a counter-clockwise order
    for(int i=0;i<(int)meshnodes.size();i++)
    {
        for(int j=0;j<NumList[i];j++)
        {
            bool swapped = false;
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

    return true;
}

void ElectrostaticsPostProcessor::clearSelection()
{
    PostProcessor::clearSelection();
    for (auto &node: meshnodes) {
        // reinterpret_cast is safe because we know only CSMeshNodes are in meshnodes.
        CSMeshNode *snode = reinterpret_cast<CSMeshNode*>(node.get());
        snode->IsSelected = false;
    }
}

const CSElement *ElectrostaticsPostProcessor::getMeshElement(int idx) const
{
    if (idx <0 || idx >= (int)meshelems.size())
        return nullptr;
    // we *know* that only CSElements are in meshelems
    return reinterpret_cast<CSElement*>(meshelems[idx].get());
}

const CSMeshNode *ElectrostaticsPostProcessor::getMeshNode(int idx) const
{
    if (idx<0 || idx>=(int)meshnodes.size())
        return nullptr;
    // we *know* that only CSMeshNodes are in meshnodes
    return reinterpret_cast<CSMeshNode*>(meshnodes[idx].get());
}

bool ElectrostaticsPostProcessor::getPointValues(double x, double y, CSPointVals &u) const
{
    int k = InTriangle(x,y);
    if (k<0)
        return false;
    getPointValues(x,y,k,u);
    return true;
}

void ElectrostaticsPostProcessor::getPointValues(double x, double y, double k, CSPointVals &u) const
{
    int n[3];
    for(int i=0; i<3; i++)
        n[i]=meshelems[k]->p[i];

    double a[3],b[3],c[3];
    const auto &n0 = meshnodes[n[0]];
    const auto &n1 = meshnodes[n[1]];
    const auto &n2 = meshnodes[n[2]];
    a[0]=n1->x * n2->y - n2->x * n1->y;
    a[1]=n2->x * n0->y - n0->x * n2->y;
    a[2]=n0->x * n1->y - n1->x * n0->y;
    b[0]=n1->y - n2->y;
    b[1]=n2->y - n0->y;
    b[2]=n0->y - n1->y;
    c[0]=n2->x - n1->x;
    c[1]=n0->x - n2->x;
    c[2]=n1->x - n0->x;

    double da=(b[0]*c[1]-b[1]*c[0]);

    getPointD(x,y,u.D,*getMeshElement(k));
    const CSMaterialProp *prop = dynamic_cast<CSMaterialProp *>(problem->blockproplist[meshelems[k]->blk].get());
    u.e=prop->ex + I*prop->ey;
    u.e/=AECF(k,x+I*y);

    u.V=0;
    for(int i=0;i<3;i++)
        u.V+=getMeshNode(i)->V*(a[i]+b[i]*x+c[i]*y)/(da);
    u.E.re = u.D.re/(u.e.re*eo);
    u.E.im = u.D.im/(u.e.im*eo);

    u.nrg=Re(u.D*conj(u.E))/2.;
}

void ElectrostaticsPostProcessor::selectConductor(int idx)
{
    for (auto &node: problem->nodelist)
        if (idx == node->InConductor)
            node->ToggleSelect();
    for (auto &line: problem->linelist)
        if (idx == line->InConductor)
            line->ToggleSelect();
    for (auto &arc: problem->arclist)
        if (idx == arc->InConductor)
            arc->ToggleSelect();
    for (auto &mnode: meshnodes)
    {
        // reinterpret_cast is safe because we know only CSMeshNodes are in meshnodes.
        CSMeshNode *snode = reinterpret_cast<CSMeshNode*>(mnode.get());
        if (idx == snode->Q)
            snode->IsSelected = ! snode->IsSelected;
    }
}

void ElectrostaticsPostProcessor::lineIntegral(int intType, double (&results)[2]) const
{
    // inttype  Integral
    //    0  E.t
    //    1  D.n
    //    2  Contour length
    //    3  Stress Tensor Force
    //    4  Stress Tensor Torque
    // inttype==0 => E.t
    if(intType==0){
        int k = (int)contour.size();

        CSPointVals u;
        getPointValues(contour[0].re,contour[0].im, u);
        results[0] = u.V;
        getPointValues(contour[k-1].re,contour[k-1].im,u);
        results[0]-= u.V;
    }

    // inttype==1 => D.n
    if(intType==1)
    {
        int NumPlotPoints=d_LineIntegralPoints;

        results[0]=0;
        results[1]=0;
        bool flag;
        for(int k=1; k<(int)contour.size();k++)
        {
            double dz = abs(contour[k]-contour[k-1])/((double) NumPlotPoints);
            int elm = -1;
            for(int i=0;i<NumPlotPoints;i++)
            {
                double u=(((double) i)+0.5)/((double) NumPlotPoints);
                CComplex pt=contour[k-1] + u*(contour[k] - contour[k-1]);
                CComplex t=contour[k]-contour[k-1];
                t/=abs(t);
                CComplex n=I*t;
                pt+=n*1.e-06;

                if (elm<0) elm=InTriangle(pt.re,pt.im);
                else if (! InTriangleTest(pt.re,pt.im,elm))
                {
                    flag=false;
                    for(int j=0;j<3;j++)
                    {
                        for(int m=0; m<NumList[meshelems[elm]->p[j]]; m++)
                        {
                            elm=ConList[meshelems[elm]->p[j]][m];
                            if (InTriangleTest(pt.re,pt.im,elm))
                            {
                                flag=true;
                                m=100;
                                j=3;
                            }
                        }
                    }
                    if (!flag)
                        elm=InTriangle(pt.re,pt.im);
                }
                CSPointVals v;
                if(elm>=0)
                {
                    getPointValues(pt.re,pt.im,elm,v);
                    flag=true;
                }
                else flag=false;

                if(flag){
                    double Dn = Re(v.D/n);

                    double d;
                    if (problem->ProblemType==AXISYMMETRIC)
                        d=2.*PI*pt.re*sqr(LengthConv[problem->LengthUnits]);
                    else
                        d=problem->Depth*LengthConv[problem->LengthUnits];

                    results[0]+=(Dn*dz*d);
                    results[1]+=dz*d;
                }
            }
        }
        results[1]=results[0]/results[1]; // Average D.n over the surface;
    }

    // inttype==2 => Contour Length
    if(intType==2)
    {
        results[0]=0;
        results[1]=0;
        int k=(int) contour.size();
        for(int i=0;i<k-1;i++)
            results[0]+=abs(contour[i+1]-contour[i]);
        results[0]*=LengthConv[problem->LengthUnits];

        if(problem->ProblemType==AXISYMMETRIC)
        {
            for(int i=0;i<k-1;i++)
            {
                results[1]+=(PI*(contour[i].re+contour[i+1].re)*
                        abs(contour[i+1]-contour[i]));
            }
            results[1]*=sqr(LengthConv[problem->LengthUnits]);
        } else {
            results[1]=results[0]*problem->Depth;
        }
    }

    // inttype==3 => Stress Tensor Force
    if(intType==3)
    {
        results[0]=0;
        results[1]=0;
        int NumPlotPoints=d_LineIntegralPoints;
        bool flag;

        for(int k=1;k<(int)contour.size();k++)
        {
            double dz=abs(contour[k]-contour[k-1])/((double) NumPlotPoints);
            int elm=-1;
            for(int i=0; i<NumPlotPoints;i++)
            {
                double u=(((double) i)+0.5)/((double) NumPlotPoints);
                CComplex pt=contour[k-1] + u*(contour[k] - contour[k-1]);
                CComplex t=contour[k]-contour[k-1];
                t/=abs(t);
                CComplex n=I*t;
                pt+=n*1.e-06;

                if (elm<0) elm=InTriangle(pt.re,pt.im);
                else if (!InTriangleTest(pt.re,pt.im,elm))
                {
                    flag=false;
                    for(int j=0;j<3;j++)
                        for(int m=0;m<NumList[meshelems[elm]->p[j]];m++)
                        {
                            elm=ConList[meshelems[elm]->p[j]][m];
                            if (InTriangleTest(pt.re,pt.im,elm))
                            {
                                flag=true;
                                m=100;
                                j=3;
                            }
                        }
                    if (!flag) elm=InTriangle(pt.re,pt.im);
                }
                CSPointVals v;
                if(elm>=0)
                {
                    getPointValues(pt.re,pt.im,elm,v);
                    flag = true;
                } else flag=false;

                if(flag)
                {
                    double Hn= Re(v.E/n);
                    double Bn= Re(v.D/n);
                    double BH= Re(v.D*conj(v.E));
                    double dF1=v.E.re*Bn + v.D.re*Hn - n.re*BH;
                    double dF2=v.E.im*Bn + v.D.im*Hn - n.im*BH;

                    double dza=dz*LengthConv[problem->LengthUnits];
                    if(problem->ProblemType==AXISYMMETRIC){
                        dza*=2.*PI*pt.re*LengthConv[problem->LengthUnits];
                        dF1=0;
                    }
                    else dza*=problem->Depth;

                    results[0]+=(dF1*dza/2.);
                    results[1]+=(dF2*dza/2.);
                }
            }
        }
    }

    // inttype==4 => Stress Tensor Torque
    if(intType==4)
    {
        int NumPlotPoints=d_LineIntegralPoints;
        bool flag;

        results[0]=results[1]=0;
        for(int k=1;k<(int)contour.size();k++)
        {
            double dz=abs(contour[k]-contour[k-1])/((double) NumPlotPoints);
            int elm=-1;
            for(int i=0;i<NumPlotPoints;i++)
            {
                double u=(((double) i)+0.5)/((double) NumPlotPoints);
                CComplex pt=contour[k-1]+ u*(contour[k] - contour[k-1]);
                CComplex t=contour[k]-contour[k-1];
                t/=abs(t);
                CComplex n=I*t;
                pt+=n*1.e-6;

                if (elm<0) elm=InTriangle(pt.re,pt.im);
                else if (!InTriangleTest(pt.re,pt.im,elm))
                {
                    flag=false;
                    for(int j=0;j<3;j++)
                        for(int m=0;m<NumList[meshelems[elm]->p[j]];m++)
                        {
                            elm=ConList[meshelems[elm]->p[j]][m];
                            if (InTriangleTest(pt.re,pt.im,elm))
                            {
                                flag=true;
                                m=100;
                                j=3;
                            }
                        }
                    if (!flag) elm=InTriangle(pt.re,pt.im);
                }
                CSPointVals v;
                if(elm>=0)
                {
                    getPointValues(pt.re,pt.im,elm,v);
                    flag = true;
                }
                else flag=false;

                if(flag)
                {
                    double Hn= Re(v.E/n);
                    double Bn= Re(v.D/n);
                    double BH= Re(v.D*conj(v.E));
                    double dF1=v.E.re*Bn + v.D.re*Hn - n.re*BH;
                    double dF2=v.E.im*Bn + v.D.im*Hn - n.im*BH;
                    double dT= pt.re*dF2 - dF1*pt.im;
                    double dza=dz*sqr(LengthConv[problem->LengthUnits]);

                    results[0]+=(dT*dza*problem->Depth/2.);
                }
            }
        }
    }
}

double ElectrostaticsPostProcessor::AECF(int k) const
{
    // Computes the permeability correction factor for axisymmetric
    // external regions.  This is sort of a kludge, but it's the best
    // way I could fit it in.  The structure of the code wasn't really
    // designed to have a permeability that varies with position in a
    // continuous way.

    if (problem->ProblemType == PLANAR) return 1.; // no correction for planar problems
    if (!problem->labellist[meshelems[k]->lbl]->IsExternal) return 1; // only need to correct for external regions

    double r=abs(meshelems[k]->ctr-I*problem->extZo);
    return (r*r)/(problem->extRo*problem->extRi); // permeability gets divided by this factor;
}

double ElectrostaticsPostProcessor::AECF(int k, CComplex p) const
{
    // Correction factor for a point within the element, rather than
    // for the center of the element.
    if (problem->ProblemType == PLANAR) return 1.; // no correction for planar problems
    if (!problem->labellist[meshelems[k]->lbl]->IsExternal) return 1; // only need to correct for external regions
    double r=abs(p-I*problem->extZo);
    if (r==0)
        return AECF(k);
    return (r*r)/(problem->extRo*problem->extRi); // permeability gets divided by this factor;
}

void ElectrostaticsPostProcessor::GetElementD(int k)
{
    int n[3];
    for(int i=0;i<3;i++)
        n[i]=meshelems[k]->p[i];

    double b[3],c[3];
    b[0]=meshnodes[n[1]]->y - meshnodes[n[2]]->y;
    b[1]=meshnodes[n[2]]->y - meshnodes[n[0]]->y;
    b[2]=meshnodes[n[0]]->y - meshnodes[n[1]]->y;
    c[0]=meshnodes[n[2]]->x - meshnodes[n[1]]->x;
    c[1]=meshnodes[n[0]]->x - meshnodes[n[2]]->x;
    c[2]=meshnodes[n[1]]->x - meshnodes[n[0]]->x;
    double da=(b[0]*c[1]-b[1]*c[0]);

    CComplex E(0);
    for(int i=0;i<3;i++)
    {
        CSMeshNode *node = dynamic_cast<CSMeshNode*>(meshnodes[n[i]].get());
        E-=node->V*(b[i]+I*c[i])/(da*LengthConv[problem->LengthUnits]);
    }

    CSElement *elem = dynamic_cast<CSElement*>(meshelems[k].get());
    CSMaterialProp *mat = dynamic_cast<CSMaterialProp*>(problem->blockproplist[elem->blk].get());
    elem->D = eo*(E.re*mat->ex + I*E.im*mat->ey)/AECF(k);
}

void ElectrostaticsPostProcessor::getPointD(double x, double y, CComplex &D, const CSElement &elm) const
{
    // elm is a reference to the element that contains the point of interest.
    if(!Smooth){
        D=elm.D;
        return;
    }

    const auto &n0 = meshnodes[elm.p[0]];
    const auto &n1 = meshnodes[elm.p[1]];
    const auto &n2 = meshnodes[elm.p[2]];
    double a[3],b[3],c[3];
    a[0]=n1->x * n2->y - n2->x * n1->y;
    a[1]=n2->x * n0->y - n0->x * n2->y;
    a[2]=n0->x * n1->y - n1->x * n0->y;
    b[0]=n1->y - n2->y;
    b[1]=n2->y - n0->y;
    b[2]=n0->y - n1->y;
    c[0]=n2->x - n1->x;
    c[1]=n0->x - n2->x;
    c[2]=n1->x - n0->x;
    double da=(b[0]*c[1]-b[1]*c[0]);

    D=0;
    for(int i=0;i<3;i++)
        D+=(elm.d[i]*(a[i]+b[i]*x+c[i]*y)/da);
}
