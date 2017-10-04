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
#include "stringTools.h"

#include <cassert>
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

    // FemmReader also reads holes -> we need to purge them:
    problem->purgeHoles();

    int k;
    // read in meshnodes;
    parseValue(input, k, err);
    meshnodes.reserve(k);
    for(int i=0;i<k;i++)
    {
        meshnodes.push_back(std::make_unique<CSMeshNode>(CSMeshNode::fromStream(input,err)));
    }

    // read in elements;
    parseValue(input, k, err);
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
    problem = std::make_shared<FemmProblem>(FileType::ElectrostaticsFile);

    // read data from file
    ElectrostaticsReader reader(problem,this,err);
    if (reader.parse(solutionFile) != F_FILE_OK)
    {
        PrintWarningMsg(err.str().c_str());
        return false;
    }

    // scale depth to meters for internal computations;
    if(problem->Depth==-1) problem->Depth=1; else problem->Depth*=LengthConv[problem->LengthUnits];

    // element centroids and radii;
    for(int i=0; i<(int)meshelems.size(); i++)
    {
        // reinterpret_cast possible because there can only be CSElements for our problem type
        CSElement *e = reinterpret_cast<CSElement*>(meshelems[i].get());
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
        getElementD(i);

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

    // Find extreme values of potential
    d_PlotBounds[0][0]=A_Low;
    d_PlotBounds[0][1]=A_High;
    PlotBounds[0][0]=d_PlotBounds[0][0];
    PlotBounds[0][1]=d_PlotBounds[0][1];

    for(int i=0;i<(int)meshelems.size();i++)
    {
        auto elem = reinterpret_cast<CSElement*>(meshelems[i].get());
        getNodalD(elem->d,i);
    }

    // Find extreme values of D and E;
    const auto &labellist = problem->labellist;
    int externalElements=0;
    for (const auto& elem : meshelems)
    {
        const auto sElem = reinterpret_cast<CSElement*>(elem.get());
        if (labellist[sElem->lbl]->IsExternal)
            externalElements++;
    }
    d_PlotBounds[1][0]=abs(getMeshElement(externalElements)->D);
    d_PlotBounds[1][1]=d_PlotBounds[1][0];
    d_PlotBounds[2][0]=abs(E(getMeshElement(0)));
    d_PlotBounds[2][1]=d_PlotBounds[2][0];
    for (const auto& elem : meshelems)
    {
        const auto sElem = reinterpret_cast<CSElement*>(elem.get());
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

    // // Choose bounds based on the type of contour plot
    // // currently in play
    // POSITION pos = GetFirstViewPosition();
    // CbelaviewView *theView=(CbelaviewView *)GetNextView(pos);

    // Build adjacency information for each element.
    FindBoundaryEdges();

    // Check to see if any regions are multiply defined
    // (i.e. tagged by more than one block label). If so,
    // display an error message and mark the problem blocks.
    bMultiplyDefinedLabels = false;
    for(int k=0;k<(int)labellist.size();k++)
    {
        assert(labellist[k]);
        const auto label = reinterpret_cast<CSBlockLabel*>(labellist[k].get());
        int i=InTriangle(label->x,label->y);
        if(i>=0)
        {
            const auto& elem = getMeshElement(i);
            if(elem->lbl!=k)
            {
                auto elemLabel = reinterpret_cast<CSBlockLabel*>(labellist[elem->lbl].get());
                elemLabel->IsSelected=true;
                if (!bMultiplyDefinedLabels)
                {
                    std::string msg;
                    msg = "Some regions in the problem have been defined\n"
                          "by more than one block label.  These potentially\n"
                          "problematic regions will appear as selected in\n"
                          "the initial view.";
                    PrintWarningMsg(msg.c_str());
                    bMultiplyDefinedLabels=true;
                }
            }
        }
    }

    return true;
}

CComplex ElectrostaticsPostProcessor::blockIntegral(int inttype) const
{
    CComplex result=0;
    for(int i=0;i<(int)meshelems.size();i++)
    {
        auto elem = getMeshElement(i);
        if((problem->labellist[elem->lbl]->IsSelected) && (inttype<5))
        {
            double R=0; // for axisymmetric problems
            // compute some useful quantities employed by most integrals...
            double a=ElmArea(i)*sqr(LengthConv[problem->LengthUnits]);
            if(problem->ProblemType==AXISYMMETRIC){
                double r[3];
                for(int k=0;k<3;k++)
                    r[k]=meshnodes[elem->p[k]]->x*LengthConv[problem->LengthUnits];
                R=(r[0]+r[1]+r[2])/3.;
            }

            // now, compute the desired integral;
            switch(inttype)
            {
            case 0: // stored energy
                if(problem->ProblemType==AXISYMMETRIC)
                    a*=(2.*PI*R);
                else
                    a*=problem->Depth;
                result+=a*Re(elem->D*conj(E(elem)))/2.;
                break;

            case 1: // cross-section area
                result+=a;
                break;

            case 2: // volume
                if(problem->ProblemType==AXISYMMETRIC)
                    a*=(2.*PI*R);
                else
                    a*=problem->Depth;
                result+=a;
                break;

            case 3: // D
                if(problem->ProblemType==AXISYMMETRIC)
                    a*=(2.*PI*R);
                else
                    a*=problem->Depth;
                result+=a*elem->D;
                break;

            case 4: // E
                if(problem->ProblemType==AXISYMMETRIC)
                    a*=(2.*PI*R);
                else a*=problem->Depth;
                result+=a*E(elem);
                break;

            default:
                break;
            }
        }

        // integrals that need to be evaluated over all elements,
        // regardless of which elements are actually selected.
        if(inttype>4)
        {
            double a=ElmArea(i)*sqr(LengthConv[problem->LengthUnits]);
            if(problem->ProblemType==AXISYMMETRIC){
                double r[3];
                for(int k=0;k<3;k++)
                    r[k]=meshnodes[elem->p[k]]->x*LengthConv[problem->LengthUnits];
                double R=(r[0]+r[1]+r[2])/3.;
                a*=(2.*PI*R);
            }
            else a*=problem->Depth;

            switch(inttype)
            {
            case 5:
            {
                double B1=Re(elem->D);
                double B2=Im(elem->D);
                CComplex c=HenrotteVector(i);

                // x (or r) direction Henrotte force, SS part.
                double y;
                if(problem->ProblemType==PLANAR){
                    y=(((B1*B1) - (B2*B2))*Re(c) + 2.*(B1*B2)*Im(c))/(2.*eo)*AECF(elem);
                    result.re += (a*y);
                } else {
                    // y (or z) direction Henrotte force, SS part
                    y=(((B2*B2) - (B1*B1))*Im(c) + 2.*(B1*B2)*Re(c))/(2.*eo)*AECF(elem);
                }
                result.im += (a*y);
                break;
            }

            case 6: // Henrotte torque, SS part.
            {
                if(problem->ProblemType!=PLANAR) break;
                double B1=Re(elem->D);
                double B2=Im(elem->D);
                CComplex c=HenrotteVector(i);

                double F1 = (((B1*B1) - (B2*B2))*Re(c) +
                      2.*(B1*B2)*Im(c))/(2.*eo);
                double F2 = (((B2*B2) - (B1*B1))*Im(c) +
                      2.*(B1*B2)*Re(c))/(2.*eo);

                c=0;
                for(int k=0;k<3;k++)
                    c+=meshnodes[elem->p[k]]->CC()*LengthConv[problem->LengthUnits]/3.;

                double y=Re(c)*F2 -Im(c)*F1;
                y*=AECF(elem);
                result+=(a*y);

                break;
            }
            default:
                break;
            }
        }
    }

    // Integrals 3 and 4 are averages over the selected volume;
    // Need to divide by the block volme to get the average.
    if((inttype==3) || (inttype==4)) result/=blockIntegral(2);

    return result;
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

void ElectrostaticsPostProcessor::getPointValues(double x, double y, int k, CSPointVals &u) const
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

    auto elem=getMeshElement(k);
    //std::cerr << "meshelems["<<k<<"] = " << *elem;
    getPointD(x,y,u.D,*elem);
    const CSMaterialProp *prop = dynamic_cast<CSMaterialProp *>(problem->blockproplist[elem->blk].get());
    u.e=prop->ex + I*prop->ey;
    u.e/=AECF(elem,x+I*y);

    u.V=0;
    for(int i=0;i<3;i++)
        u.V+=getMeshNode(i)->V*(a[i]+b[i]*x+c[i]*y)/(da);
    u.E.re = u.D.re/(u.e.re*eo);
    u.E.im = u.D.im/(u.e.im*eo);

    u.nrg=Re(u.D*conj(u.E))/2.;
}

bool ElectrostaticsPostProcessor::isSelectionOnAxis() const
{
    if (problem->ProblemType!=AXISYMMETRIC)
        return false;

    if (PostProcessor::isSelectionOnAxis())
        return true;
    for (auto &mnode: meshnodes)
    {
        // reinterpret_cast is safe because we know only CSMeshNodes are in meshnodes.
        CSMeshNode *snode = reinterpret_cast<CSMeshNode*>(mnode.get());
        if ((snode->IsSelected) && (snode->x<1.e-6))
            return true;
    }

    return false;
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

double ElectrostaticsPostProcessor::AECF(const CElement *elem) const
{
    // Computes the permeability correction factor for axisymmetric
    // external regions.  This is sort of a kludge, but it's the best
    // way I could fit it in.  The structure of the code wasn't really
    // designed to have a permeability that varies with position in a
    // continuous way.

    if (problem->ProblemType == PLANAR) return 1.; // no correction for planar problems
    if (!problem->labellist[elem->lbl]->IsExternal) return 1; // only need to correct for external regions

    double r=abs(elem->ctr-I*problem->extZo);
    return (r*r)/(problem->extRo*problem->extRi); // permeability gets divided by this factor;
}

double ElectrostaticsPostProcessor::AECF(const CElement *elem, CComplex p) const
{
    // Correction factor for a point within the element, rather than
    // for the center of the element.
    if (problem->ProblemType == PLANAR) return 1.; // no correction for planar problems
    if (!problem->labellist[elem->lbl]->IsExternal) return 1; // only need to correct for external regions
    double r=abs(p-I*problem->extZo);
    if (r==0)
        return AECF(elem);
    return (r*r)/(problem->extRo*problem->extRi); // permeability gets divided by this factor;
}

CComplex ElectrostaticsPostProcessor::E(const CSElement *elem) const
{
    const CSMaterialProp *mat = dynamic_cast<CSMaterialProp*>(problem->blockproplist[elem->blk].get());
    // return average electric field intensity for the kth element
    return (elem->D.re/mat->ex + I*elem->D.im/mat->ey)/eo * AECF(elem);

    // AECF(k) part corrects permittivity for axisymmetric external region;
}

void ElectrostaticsPostProcessor::getElementD(int k)
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
    assert(elem->blk >= 0);
    assert(elem->blk < (int)problem->blockproplist.size());
    CSMaterialProp *mat = dynamic_cast<CSMaterialProp*>(problem->blockproplist[elem->blk].get());
    assert(mat);
    elem->D = eo*(E.re*mat->ex + I*E.im*mat->ey)/AECF(elem);
}

// almost the same as hpproc::GetNodalD; differences noted by comments
void ElectrostaticsPostProcessor::getNodalD(CComplex *d, int N) const
{
    int i,j,k,n,m,p,eos,nos,qn;
    int lf,rt;
    double xi,yi,ii,xx,xy,yy,iv,xv,yv,dx,dy,dv,Ex,Ey,det;
    static int q[21];
    bool flag;

    const auto &elem = getMeshElement(N);
    for(i=0;i<3;i++)
    {
        j=elem->p[i];
        lf=rt=-1;
        flag=false;
        for(eos=0;eos<NumList[j];eos++) if(ConList[j][eos]==N) break;

        // scan ccw
        for(k=0,m=eos,qn=0;k<NumList[j];k++)
        {
            n=ConList[j][m];
            const auto &conElem = getMeshElement(n);
            if(!isSameMaterial(*elem,*conElem)) break;

            // figure out which node is the next one in the ccw direction
            // Note that the ConList has been sorted in ccw order,
            // and the nodes in each element are sorted in ccw order,
            // making this task a bit easier.  The next node
            // ends up in the variable p
            for(nos=0;nos<3;nos++) if(conElem->p[nos]==j) break;
            if(nos==3) break;
            nos--;
            if(nos<0) nos=2;
            p=conElem->p[nos];

            // add this node to the list.  We can have a max of 20 nodes,
            // which should never actually occur (usually about 6 to 8)
            if (qn<20) q[qn++]=p;

            // if this is a fixed boundary, get out of the loop;
            if ((meshnodes[j]->Q!=-2) && (meshnodes[p]->Q!=-2)){
                rt=p;
                break;
            }

            m++; if(m==NumList[j]) m=0;
        }

        // scan cw
        for(k=0,m=eos;k<NumList[j];k++)
        {
            n=ConList[j][m];
            const auto &conElem = getMeshElement(n);
            if(!isSameMaterial(*elem,*conElem)) break;

            // figure out which node is the next one in the cw direction
            // The next node ends up in the variable p
            for(nos=0;nos<3;nos++) if(conElem->p[nos]==j) break;
            if(nos==3) break;
            nos++;
            if(nos>2) nos=0;
            p=conElem->p[nos];

            // add this node to the list.  We can have a max of 20 nodes,
            // which should never actually occur (usually about 6 to 8)
            if (qn<20) q[qn++]=p;

            // if this node has a fixed definition, get out of the loop;
            if((meshnodes[j]->Q!=-2) &&(meshnodes[p]->Q!=-2)){
                lf=p;
                break;
            }

            m--; if(m<0) m=NumList[j]-1;
        }

        // catch some annoying special cases;
        if ((lf==rt) && (rt!=-1) && (meshnodes[j]->Q!=-2))
        {
            // The node of interest is at the end of a conductor; not much to
            // do but punt;
            d[i]=elem->D;
            flag=true;
        }
        else if ((rt!=-1) && (meshnodes[j]->Q!=-2) && (lf==-1))
        {
            // Another instance of a node at the
            // end of a conductor; punt!
            d[i]=elem->D;
            flag=true;
        }
        else if ((lf!=-1) && (meshnodes[j]->Q!=-2) && (rt==-1))
        {
            // Another instance of a node at the
            // end of a conductor; punt!
            d[i]=elem->D;
            flag=true;
        }
        else if((lf==-1) && (rt==-1) && (meshnodes[j]->Q!=-2))
        {
            // The node of interest is an isolated charge. Again, not much to
            // do but punt;
            d[i]=elem->D;
            flag=true;
        }
        else if((lf!=-1) && (rt!=-1) && (meshnodes[j]->Q!=-2))
        {

            // The node of interest is on some boundary where the charge is fixed.
            // if the angle is shallow enough, we can just do the regular thing;
            // Otherwise, we punt.
            CComplex x,y;
            x=meshnodes[lf]->CC()-meshnodes[j]->CC(); x/=abs(x);
            y=meshnodes[j]->CC()-meshnodes[rt]->CC(); y/=abs(y);
            if(std::abs(arg(x/y))>10.0001*PI/180.)
            {
                // if the angle is greater than 10 degrees, punt;
                d[i]=elem->D;
                flag=true;
            }
        }

        if(flag==false)
        {
            // The nominal case.
            // Fit a plane through the nodes in the list to solve for E.
            // Then, multiply by permittivity to get D.
            xi=yi=ii=xx=xy=yy=iv=xv=yv=0;

            q[qn++]=j;

            for(k=0;k<qn;k++)
            {
                dx=meshnodes[q[k]]->x-meshnodes[j]->x;
                dy=meshnodes[q[k]]->y-meshnodes[j]->y;
                // Note(ZaJ): hpproc -> dv=meshnodes[j]->T-meshnodes[q[k]]->T;
                dv=getMeshNode(j)->V-getMeshNode(q[k])->V;

                ii+=1.;
                xi+=dx;
                yi+=dy;
                xx+=dx*dx;
                xy+=dx*dy;
                yy+=dy*dy;
                iv+=dv;
                xv+=dx*dv;
                yv+=dy*dv;
            }
            det=(-(ii*xy*xy) + 2*xi*xy*yi - xx*yi*yi - xi*xi*yy + ii*xx*yy)*LengthConv[problem->LengthUnits];

            if (det==0) d[i]=elem->D;
            else{
                Ex=(iv*xy*yi - xv*yi*yi - ii*xy*yv + xi*yi*yv - iv*xi*yy + ii*xv*yy)/det;
                Ey=(iv*xi*xy - ii*xv*xy + xi*xv*yi - iv*xx*yi - xi*xi*yv + ii*xx*yv)/det;
                CComplex kn;
                // Note(ZaJ): hpproc:
                // kn=problem->blockproplist[elem->blk]->GetK(meshnodes[j]->T);
                auto bprop = reinterpret_cast<CSMaterialProp*>(problem->blockproplist[elem->blk].get());
                kn = bprop->ex*eo;
                d[i]= Re(kn)*Ex + I*Im(kn)*Ey;
                d[i]/=AECF(elem,meshnodes[j]->CC());
            }
        }
    }
}



void ElectrostaticsPostProcessor::getPointD(double x, double y, CComplex &D, const CSElement &elm) const
{
    // elm is a reference to the element that contains the point of interest.
    if(!Smooth){
        //std::cerr << "!Smooth -> D = " << elm.D.re << "+" << elm.D.im << "i\n";
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
    {
        //std::cerr << "D += " << elm.d[i] << " * ( "<<a[i]<<" + "<<b[i]<<" * "<<x<<" + "<<c[i]<<" * "<<y<<" ) / " <<da << "\n";
        D+=(elm.d[i]*(a[i]+b[i]*x+c[i]*y)/da);
    }
}

bool ElectrostaticsPostProcessor::isSameMaterial(const CSElement &e1, const CSElement &e2) const
{
    CSMaterialProp *m1 = reinterpret_cast<CSMaterialProp *>(problem->blockproplist[e1.blk].get());
    CSMaterialProp *m2 = reinterpret_cast<CSMaterialProp *>(problem->blockproplist[e2.blk].get());

    return ((m1->ex==m2->ex) &&  (m1->ey==m2->ey));
}
