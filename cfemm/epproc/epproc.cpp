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
double sqr(double x)
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

const CSElement *ElectrostaticsPostProcessor::getMeshElement(int idx) const
{
    if (idx <0 || idx >= (int)meshelems.size())
        return nullptr;
    // we *know* that only CSElements are in meshelems
    return reinterpret_cast<CSElement*>(meshelems[idx].get());
}

double ElectrostaticsPostProcessor::AECF(int k)
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

double ElectrostaticsPostProcessor::AECF(int k, CComplex p)
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
