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

// fsolver.cpp : implementation of the FSolver class
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <malloc.h>
#include "lua.h"
#include "femmcomplex.h"
#include "spars.h"
#include "fparse.h"
#include "fsolver.h"
#include "mmesh.h"
#include "CNode.h"

#ifndef _MSC_VER
#define _strnicmp strncasecmp
#endif

using namespace std;
using namespace femm;

/////////////////////////////////////////////////////////////////////////////
// FSolver construction/destruction

FSolver::FSolver()
{
    Frequency = 0.0;
    Relax = 0.0;
    ACSolver=0;
    NumCircPropsOrig = 0;

    meshnode = NULL;
    blockproplist = NULL;
    nodeproplist = NULL;
    circproplist = NULL;
    labellist = NULL;
    lineproplist = NULL;

    extRo = extRi = extZo = 0.0;

    // initialise the warning message box function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;
}

FSolver::~FSolver()
{
    CleanUp();
}

void FSolver::CleanUp()
{
    delete[] meshnode;
    meshnode = NULL;
    delete[] blockproplist;
    blockproplist = NULL;
    delete[] nodeproplist;
    nodeproplist = NULL;
    delete[] circproplist;
    circproplist = NULL;
    delete[] lineproplist;
    lineproplist = NULL;
    delete[] labellist;
    labellist = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// FSolver commands

void FSolver::MsgBox(const char* message)
{
    printf("%s\n", message);
}

int FSolver::LoadProblemFile ()
{
    FILE *fp;
    int j,k,ic;
    char s[1024],q[1024];
    char *v;
    CMPointProp       PProp;
    CMBoundaryProp   BProp;
    CMMaterialProp    MProp;
    CMCircuit        CProp;
    CMBlockLabel     blk;

    sprintf(s,"%s.fem", PathName.c_str() );
    if ((fp=fopen(s,"rt"))==NULL)
    {
        printf("Couldn't read from specified .fem file\n");
        return false;
    }

    // define some defaults
    Frequency=0.;
    Precision=1.e-08;
    Relax=1.;
    ProblemType=0;
    Coords=0;
    NumPointProps=0;
    NumLineProps=0;
    NumBlockProps=0;
    NumCircProps=0;
    ACSolver=0;
    DoForceMaxMeshArea = false;

    // parse the file

    while (fgets(s,1024,fp)!=NULL)
    {
        sscanf(s,"%s",q);

        // Frequency of the problem
        if( _strnicmp(q,"[frequency]",11)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&Frequency);
            q[0] = '\0';
        }

        // Precision
        if( _strnicmp(q,"[precision]",11)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&Precision);
            q[0] = '\0';
        }

        // AC Solver Type
        if( _strnicmp(q,"[acsolver]",8)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&ACSolver);
            q[0] = '\0';
        }

        // Option to force use of default max mesh, overriding
        // user choice
        if( _strnicmp(q,"[forcemaxmesh]",13)==0)
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
        }

        // Units of length used by the problem
        if( _strnicmp(q,"[lengthunits]",13)==0)
        {
            v=StripKey(s);
            sscanf(v,"%s",q);
            if( _strnicmp(q,"inches",6)==0) LengthUnits=0;
            else if( _strnicmp(q,"millimeters",11)==0) LengthUnits=1;
            else if( _strnicmp(q,"centimeters",1)==0) LengthUnits=2;
            else if( _strnicmp(q,"mils",4)==0) LengthUnits=4;
            else if( _strnicmp(q,"microns",6)==0) LengthUnits=5;
            else if( _strnicmp(q,"meters",6)==0) LengthUnits=3;
            q[0] = '\0';
        }

        // Problem Type (planar or axisymmetric)
        if( _strnicmp(q,"[problemtype]",13)==0)
        {
            v=StripKey(s);
            sscanf(v,"%s",q);
            if( _strnicmp(q,"planar",6)==0) ProblemType=0;
            if( _strnicmp(q,"axisymmetric",3)==0) ProblemType=1;
            q[0] = '\0';
        }

        // Coordinates (cartesian or polar)
        if( _strnicmp(q,"[coordinates]",13)==0)
        {
            v=StripKey(s);
            sscanf(v,"%s",q);
            if ( _strnicmp(q,"cartesian",4)==0) Coords=0;
            if ( _strnicmp(q,"polar",5)==0) Coords=1;
            q[0] = '\0';
        }

        // properties for axisymmetric external region
        if( _strnicmp(q,"[extzo]",7)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&extZo);
            q[0] = '\0';
        }

        if( _strnicmp(q,"[extro]",7)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&extRo);
            q[0] = '\0';
        }

        if( _strnicmp(q,"[extri]",7)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&extRi);
            q[0] = '\0';
        }

        // Point Properties
        if( _strnicmp(q,"[pointprops]",12)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&k);
            if (k>0) nodeproplist=new CMPointProp[k];
            q[0] = '\0';
        }

        if( _strnicmp(q,"<beginpoint>",11)==0)
        {
            PProp.Jr=0.;
            PProp.Ji=0.;
            PProp.Ar=0.;
            PProp.Ai=0.;
            q[0] = '\0';
        }

        if( _strnicmp(q,"<A_re>",6)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&PProp.Ar);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<A_im>",6)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&PProp.Ai);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<I_re>",6)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&PProp.Jr);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<I_im>",6)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&PProp.Ji);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<endpoint>",9)==0)
        {
            nodeproplist[NumPointProps]=PProp;
            NumPointProps++;
            q[0] = '\0';
        }

        // Boundary Properties;
        if( _strnicmp(q,"[bdryprops]",11)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&k);
            if (k>0) lineproplist=new CMBoundaryProp[k];
            q[0] = '\0';
        }

        if( _strnicmp(q,"<beginbdry>",11)==0)
        {
            BProp.BdryFormat=0;
            BProp.A0=0.;
            BProp.A1=0.;
            BProp.A2=0.;
            BProp.phi=0.;
            BProp.Mu=0.;
            BProp.Sig=0.;
            BProp.c0=0.;
            BProp.c1=0.;
            q[0] = '\0';
        }

        if( _strnicmp(q,"<bdrytype>",10)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&BProp.BdryFormat);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<mu_ssd>",8)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.Mu);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<sigma_ssd>",11)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.Sig);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<A_0>",5)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.A0);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<A_1>",5)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.A1);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<A_2>",5)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.A2);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<phi>",5)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.phi);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<c0>",4)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.c0.re);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<c1>",4)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.c1.re);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<c0i>",5)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.c0.im);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<c1i>",5)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&BProp.c1.im);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<endbdry>",9)==0)
        {
            lineproplist[NumLineProps]=BProp;
            NumLineProps++;
            q[0] = '\0';
        }

        // Block Properties;
        if( _strnicmp(q,"[blockprops]",12)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&k);
            if (k>0) blockproplist=new CMMaterialProp[k];
            q[0] = '\0';
        }

        if( _strnicmp(q,"<beginblock>",12)==0)
        {
            MProp.mu_x=1.;
            MProp.mu_y=1.;            // permeabilities, relative
            MProp.H_c=0.;                // magnetization, A/m
            MProp.Jr=0.;
            MProp.Ji=0.;                // applied current density, MA/m^2
            MProp.Cduct=0.;            // conductivity of the material, MS/m
            MProp.Lam_d=0.;            // lamination thickness, mm
            MProp.Theta_hn=0.;            // hysteresis angle, degrees
            MProp.Theta_hx=0.;            // hysteresis angle, degrees
            MProp.Theta_hy=0.;            // hysteresis angle, degrees
            MProp.Theta_m=0.;            // magnetization direction, degrees;
            MProp.LamFill=1.;            // lamination fill factor;
            MProp.LamType=0;            // type of lamination;
            MProp.NStrands=0;
            MProp.WireD=0;
            MProp.BHpoints=0;
            MProp.Bdata=NULL;
            MProp.Hdata=NULL;
            q[0] = '\0';
        }

        if( _strnicmp(q,"<mu_x>",6)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.mu_x);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<mu_y>",6)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.mu_y);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<H_c>",5)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.H_c);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<H_cAngle>",10)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.Theta_m);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<J_re>",6)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.Jr);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<J_im>",6)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.Ji);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<sigma>",7)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.Cduct);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<phi_h>",7)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.Theta_hn);
            q[0] = '\0';
        }


        if( _strnicmp(q,"<phi_hx>",7)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.Theta_hx);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<phi_hy>",8)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.Theta_hy);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<d_lam>",7)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.Lam_d);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<LamFill>",8)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.LamFill);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<WireD>",7)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&MProp.WireD);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<LamType>",9)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&MProp.LamType);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<NStrands>",10)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&MProp.NStrands);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<BHPoints>",10)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&MProp.BHpoints);
            if (MProp.BHpoints>0)
            {
                MProp.Hdata=new CComplex[MProp.BHpoints];
                MProp.Bdata=new double[MProp.BHpoints];
                for(j=0; j<MProp.BHpoints; j++)
                {
                    fgets(s,1024,fp);
                    sscanf(s,"%lf\t%lf",&MProp.Bdata[j],&MProp.Hdata[j].re);
                    MProp.Hdata[j].im=0;
                }
            }
            q[0] = '\0';
        }

        if( _strnicmp(q,"<endblock>",9)==0)
        {
            blockproplist[NumBlockProps]=MProp;
            blockproplist[NumBlockProps].GetSlopes(Frequency*2.*PI);
            NumBlockProps++;
            MProp.BHpoints=0;
            MProp.Bdata=NULL;
            MProp.Hdata=NULL;
            q[0] = '\0';
        }

        // Circuit Properties
        if( _strnicmp(q,"[circuitprops]",15)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&k);
            if(k>0) circproplist=new CMCircuit[k];
            q[0] = '\0';
        }

        if( _strnicmp(q,"<begincircuit>",14)==0)
        {
            CProp.dVolts_re=0.;
            CProp.dVolts_im=0.;
            CProp.Amps_re=0.;
            CProp.Amps_im=0.;
            CProp.CircType=0;
            q[0] = '\0';
        }

        if( _strnicmp(q,"<voltgradient_re>",17)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&CProp.dVolts_re);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<voltgradient_im>",17)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&CProp.dVolts_im);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<totalamps_re>",14)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&CProp.Amps_re);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<totalamps_im>",14)==0)
        {
            v=StripKey(s);
            sscanf(v,"%lf",&CProp.Amps_im);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<circuittype>",13)==0)
        {
            v=StripKey(s);
            sscanf(v,"%i",&CProp.CircType);
            q[0] = '\0';
        }

        if( _strnicmp(q,"<endcircuit>",12)==0)
        {
            circproplist[NumCircProps]=CProp;
            NumCircProps++;
            q[0] = '\0';
        }


        // read in regional attributes
        if(_strnicmp(q,"[numblocklabels]",13)==0)
        {
            int i;
            v=StripKey(s);
            sscanf(v,"%i",&k);
            if (k>0) labellist=new CMBlockLabel[k];
            NumBlockLabels=k;
            for(i=0; i<k; i++)
            {
                fgets(s,1024,fp);

//sscanf(s,"%lf%lf	%i	%lf	%i	%lf	%i	%i	%i",&blk.x,&blk.y,&blk.BlockType,&blk.MaxArea,
//					&blk.InCircuit,&blk.MagDir,&blk.InGroup,&blk.Turns,&blk.IsExternal);

                //some defaults
                blk.x=0;
                blk.y=0;
                blk.BlockType=0;
                blk.MaxArea=0.;
                blk.MagDir=0.;
                blk.Turns=1;
                blk.InCircuit=0;
                blk.InGroup=0;
                blk.IsExternal=0;
                blk.IsDefault=0;
                blk.MagDirFctn.clear();

                // scan in data
                v = ParseDbl(s,&blk.x);
                v = ParseDbl(v,&blk.y);
                v = ParseInt(v,&blk.BlockType);
                v = ParseDbl(v,&blk.MaxArea);
                v = ParseInt(v,&blk.InCircuit);
                v = ParseDbl(v,&blk.MagDir);
                v = ParseInt(v,&blk.InGroup);
                v = ParseInt(v,&blk.Turns);
                int extDefault=0;
                v = ParseInt(v,&extDefault);
                // second last bit in extDefault flag, we mask the other bits
                // and take the resulting value, if not zero it will evaluate to true
                blk.IsDefault  = extDefault & 2;
                // last bit in extDefault flag, we mask the other bits
                // and take the resulting value, if not zero it will evaluate to true
                blk.IsExternal = extDefault & 1;
                v = ParseString(v,&blk.MagDirFctn);
                blk.BlockType--;
                blk.InCircuit--;
                labellist[i]=blk;
            }
            q[0] = '\0';
        }
    }

    // need to set these so that valid BH data doesn't get wiped
    // by the destructor of MProp
    MProp.BHpoints = 0;
    MProp.Bdata = NULL;
    MProp.Hdata = NULL;

    fclose(fp);

    if (NumCircProps==0) return true;

    // Process circuits for serial connections.
    // The program deals with serial "circuits" by making a separate
    // circuit property for each block in the serial circuit.  Then,
    // each of this larger number of circuits can be processed using
    // the previous approach which considered all circuits to be
    // parallel connected.

    // first, make enough space for all possible circuits;
    CMCircuit *temp=(CMCircuit *)calloc(NumCircProps+NumBlockLabels,sizeof(CMCircuit));
    for(k=0; k<NumCircProps; k++)
    {
        temp[k]=circproplist[k];
        temp[k].OrigCirc=-1;
    }
    free(circproplist);
    circproplist=temp;
    NumCircPropsOrig=NumCircProps;

    // now, go through the block label list and make a new "circuit"
    // for every block label that is an element of a "serial" circuit.
    CMCircuit ncirc;
    for(k=0; k<NumBlockLabels; k++)
        if(labellist[k].InCircuit>=0)
        {
            ic=labellist[k].InCircuit;
            if(circproplist[ic].CircType==1)
            {
                ncirc=circproplist[ic];
                ncirc.OrigCirc=ic;
                ncirc.Amps_im*=labellist[k].Turns;
                ncirc.Amps_re*=labellist[k].Turns;
                circproplist[NumCircProps]=ncirc;
                labellist[k].InCircuit=NumCircProps;
                NumCircProps++;
            }
        }

    // now, all "circuits" look like parallel circuits, so
    for(k=0; k<NumCircProps; k++)
        if(circproplist[k].CircType==1) circproplist[k].CircType=0;

//    // Check to see if any regions are multiply defined
//    // (i.e. tagged by more than one block label). If so,
//    // display a warnign and mark bMultiplyDefinedLabels true.
//    for(k=0,bMultiplyDefinedLabels=false; k<NumBlockLabels; k++)
//    {
//        // test if the label is inside the meshed region, by attempting to find
//        // which triangle it is in, if it's outside the problem region it will
//        // be ignored anyway
//        if((i = InTriangle(labellist[k].x,labellist[k].y)) >= 0)
//        {
//            // it's in the problem region,
//            if(meshele[i].lbl != k)
//            {
//                labellist[meshelem[i].lbl].IsSelected = true;
//                if (!bMultiplyDefinedLabels)
//                {
//
//                    string msg = "Some regions in the problem have been defined\n";
//                    msg += "by more than one block label.  These potentially\n";
//                    msg += "problematic regions will appear as selected in\n";
//                    msg += "the initial view.";
//                    WarnMessage(msg.c_str());
//                    bMultiplyDefinedLabels=true;
//                }
//            }
//        }
//    }

    return true;
}

int FSolver::LoadMesh(bool deleteFiles)
{
    int i,j,k,q,n0,n1;
    char infile[256];
    FILE *fp;
    char s[1024];
    double c[]= {2.54,0.1,1.,100.,0.00254,1.e-04};


    //read meshnodes;
    sprintf(infile,"%s.node",PathName.c_str());
    if((fp=fopen(infile,"rt"))==NULL)
    {
        return BADELEMENTFILE;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    NumNodes = k;

    meshnode = new CNode[k];
    CNode node;
    for(i=0; i<k; i++)
    {
        fscanf(fp,"%i",&j);
        fscanf(fp,"%lf",&node.x);
        fscanf(fp,"%lf",&node.y);
        fscanf(fp,"%i",&j);
        if(j>1) j=j-2;
        else j=-1;
        node.BoundaryMarker=j;

        // convert all lengths to centimeters (better conditioning this way...)
        node.x *= c[LengthUnits];
        node.y *= c[LengthUnits];

        meshnode[i] = node;
    }
    fclose(fp);

    //read in periodic boundary conditions;
    sprintf(infile,"%s.pbc",PathName.c_str());
    if((fp=fopen(infile,"rt"))==NULL)
    {
        return BADPBCFILE;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    NumPBCs = k;

    if (k!=0) pbclist = (CCommonPoint *)calloc(k,sizeof(CCommonPoint));
    CCommonPoint pbc;
    for(i=0; i<k; i++)
    {
        fscanf(fp,"%i",&j);
        fscanf(fp,"%i",&pbc.x);
        fscanf(fp,"%i",&pbc.y);
        fscanf(fp,"%i",&pbc.t);
        pbclist[i] = pbc;
    }
    fclose(fp);

    // read in elements;
    sprintf(infile,"%s.ele",PathName.c_str());
    if((fp=fopen(infile,"rt"))==NULL)
    {
        return BADELEMENTFILE;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    NumEls = k;

    meshele = (CElement *)calloc(k,sizeof(CElement));
    CElement elm;

    // get the default label for unlabelled blocks
    int defaultLabel;
    for(i=0,defaultLabel=-1; i<NumBlockLabels; i++)
    {
        if (labellist[i].IsDefault)
        {
            defaultLabel = i;
        }
    }

    for(i=0; i<k; i++)
    {
        fscanf(fp,"%i",&j);
        fscanf(fp,"%i",&elm.p[0]);
        fscanf(fp,"%i",&elm.p[1]);
        fscanf(fp,"%i",&elm.p[2]);
        fscanf(fp,"%i",&elm.lbl);
        elm.lbl--;

        if(elm.lbl<0)
        {
            elm.lbl = defaultLabel;
        }

        if(elm.lbl<0)
        {
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
        elm.blk = labellist[elm.lbl].BlockType;

        meshele[i] = elm;
    }
    fclose(fp);

    // initialize edge bc's and element permeabilities;
    for(i=0; i<NumEls; i++)
        for(j=0; j<3; j++)
        {
            meshele[i].e[j] = -1;
            meshele[i].mu1  = -1.;
            meshele[i].mu2  = -1.;
        }

    // read in edges to which boundary conditions are applied;

    // first, do a little bookkeeping so that element
    // associated with a given edge can be identified fast
    int *nmbr;
    int **mbr;

    nmbr = (int *)calloc(NumNodes,sizeof(int));

    // Make a list of how many elements that tells how
    // many elements to which each node belongs.
    for(i=0; i<NumEls; i++)
        for(j=0; j<3; j++)
            nmbr[meshele[i].p[j]]++;

    // mete out some memory to build a list of the
    // connectivity...
    mbr = (int **)calloc(NumNodes,sizeof(int *));
    for(i=0; i<NumNodes; i++)
    {
        mbr[i] = (int *)calloc(nmbr[i],sizeof(int));
        nmbr[i] = 0;
    }

    // fill up the connectivity information;
    for(i=0; i<NumEls; i++)
        for(j=0; j<3; j++)
        {
            k = meshele[i].p[j];
            mbr[k][nmbr[k]] = i;
            nmbr[k]++;
        }

    sprintf(infile,"%s.edge",PathName.c_str());
    if((fp=fopen(infile,"rt"))==NULL)
    {
        return BADEDGEFILE;
    }
    fscanf(fp,"%i",&k);// read in number of lines

    fscanf(fp,"%i",&j);// read in boundarymarker flag;
    for(i=0; i<k; i++)
    {
        fscanf(fp,"%i",&j);
        fscanf(fp,"%i",&n0);
        fscanf(fp,"%i",&n1);
        fscanf(fp,"%i",&j);

        if(j<0)
        {
            j = -(j+2);
            // search through elements to find one containing the line;
            // set corresponding edge equal to the bc number.
            for(q=0; q<nmbr[n0]; q++)
            {
                elm=meshele[mbr[n0][q]];

                if ((elm.p[0] == n0) && (elm.p[1] == n1)) elm.e[0]=j;
                if ((elm.p[0] == n1) && (elm.p[1] == n0)) elm.e[0]=j;

                if ((elm.p[1] == n0) && (elm.p[2] == n1)) elm.e[1]=j;
                if ((elm.p[1] == n1) && (elm.p[2] == n0)) elm.e[1]=j;

                if ((elm.p[2] == n0) && (elm.p[0] == n1)) elm.e[2]=j;
                if ((elm.p[2] == n1) && (elm.p[0] == n0)) elm.e[2]=j;

                meshele[mbr[n0][q]]=elm;
            }
        }

    }
    fclose(fp);

    // free up the connectivity information
    free(nmbr);
    for(i=0; i<NumNodes; i++) free(mbr[i]);
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

void FSolver::GetFillFactor(int lbl)
{
    // Get the fill factor associated with a stranded and
    // current-carrying region.  For AC problems, also compute
    // the apparent conductivity and permeability for use in
    // post-processing the voltage.

    CMMaterialProp* bp= &blockproplist[labellist[lbl].BlockType];
    CMBlockLabel* bl= &labellist[lbl];
    double atot,awire=0,d,o,fill,dd,W,R=0,c1,c2;
    int i,wiretype;
    CComplex ufd,ofd;

    if ((abs(bl->Turns)>1) || (blockproplist[labellist[lbl].BlockType].LamType>2))
        bl->bIsWound=true;
    else
        bl->bIsWound=false;

    if ((Frequency==0) || (blockproplist[labellist[lbl].BlockType].LamType<3))
    {
        bl->ProximityMu=1.;
        return;
    }

    // compute total area of associated block
    for(i=0,atot=0; i<NumEls; i++)
        if(meshele[i].lbl==lbl) atot+=ElmArea(i);

    if (atot==0) return;

    // "non-physical" case where the wire has a zero conductivity
    if (bp->Cduct==0)
    {
        bl->ProximityMu=1;
        return;
    }

    wiretype=bp->LamType-3;
    // wiretype = 0 for magnet wire
    // wiretype = 1 for stranded but non-litz wire
    // wiretype = 2 for litz wire
    // wiretype = 3 for rectangular wire

    if(wiretype==3) // rectangular wire
    {
        W=2.*PI*Frequency;
        d=bp->WireD*0.001;
        fill=fabs(d*d*((double) bl->Turns)/atot);
        dd=d/sqrt(fill);// foil pitch
        fill=d/dd;                    // fill for purposes of equivalent foil analysis
        o=bp->Cduct*(d/dd)*1.e6;    // effective foil conductivity in S/m

        // effective permeability for the equivalent foil
        ufd=muo*tanh(sqrt(I*W*o*muo)*d/2.)/(sqrt(I*W*o*muo)*d/2.);
        bl->ProximityMu=(fill*ufd+(1.-fill)*muo)/muo;
        return;
    }

    // procedure for round wires;
    switch (wiretype)
    {
        // wiretype = 0 for magnet wire
    case 0:
        R=bp->WireD*0.0005;
        awire=PI*R*R*((double) bp->NStrands)*((double) bl->Turns);
        break;

        // wiretype = 1 for stranded but non-litz wire
    case 1:
        R=bp->WireD*0.0005*sqrt((double) bp->NStrands);
        awire=PI*R*R*((double) bl->Turns);
        break;

        // wiretype = 2 for litz wire
    case 2:
        R=bp->WireD*0.0005;
        awire=PI*R*R*((double) bp->NStrands)*((double) bl->Turns);
        break;
    }
    fill=fabs(awire/atot);

    // preliminary definitions
    o=bp->Cduct*1.e6;                        // conductivity in S/m
    W=2.*PI*Frequency*o*muo*R*R/2.;            // non-dimensionalized frequency

    // fit for frequency-dependent permeability...
    c1=0.7756067409818643 + fill*(0.6873854335408803 + fill*(0.06841584481674128 -0.07143732702512284*fill));
    c2=1.5*fill/c1;
    ufd=c2*(tanh(sqrt(c1*I*W))/sqrt(c1*I*W))+(1.-c2); // relative frequency-dependent permeability
    bl->ProximityMu=ufd;


}
/*
void FSolver::GetFillFactor(int lbl)
{
	// Get the fill factor associated with a stranded and
	// current-carrying region.  For AC problems, also compute
	// the apparent conductivity and permeability for use in
	// post-processing the voltage.

	CMaterialProp* bp= &blockproplist[labellist[lbl].BlockType];
	CMBlockLabel* bl= &labellist[lbl];
	double atot,awire,r,FillFactor;
	int i,wiretype;
	CComplex ufd;
	double W=2.*PI*Frequency;

	if ((Frequency==0) || (blockproplist[labellist[lbl].BlockType].LamType<3))
	{
		bl->ProximityMu=0;
		return;
	}

	wiretype=bp->LamType-3;
	// wiretype = 0 for magnet wire
	// wiretype = 1 for stranded but non-litz wire
	// wiretype = 2 for litz wire
	// wiretype = 3 for rectangular wire
	r=bp->WireD*0.0005;

	for(i=0,atot=0;i<NumEls;i++)
		  if(meshele[i].lbl==lbl) atot+=ElmArea(i);

	awire=PI*r*r;
	if (wiretype==3) awire*=(4./PI); // if rectangular wire;
	awire*=((double) bp->NStrands);
	awire*=((double) bl->Turns);

	if (atot==0) return;
	FillFactor=fabs(awire/atot);

	double w,d,h,o,fill,dd;

	// if stranded but non-litz, use an effective wire radius that
	// gives the same cross-section as total stranded area
	if (wiretype==1) r*=sqrt((double) bp->NStrands);

	if (wiretype!=3){
		d=r*sqrt(3.);
		h=PI/sqrt(3.)*r;
		w=r*sqrt(PI/(2.*sqrt(3.)*FillFactor));
		dd=sqrt(3.)*w;
	}
	else{
		d=2.*r;
		h=2.*r;
		w=r/sqrt(FillFactor);
		dd=2.*w;
	}
	o=bp->Cduct*(h/w)*5.e5; // conductivity in S/m
	fill=d/dd; //fill for purposes of equivalent foil analysis

	// At this point, sanity-check the fill factor;
	if (fill>1)
	{
		CStdString mymsg;
		mymsg.Format("Block label at (%g,%g) has a fill factor",bl->x,bl->y);
		mymsg +=     "greater than the theoretical maximum.  Couldn't solve the problem.";
		WarnMessage(mymsg.c_str());
		exit(5);
	}

	// effective permeability for the equivalent foil.  Note that this is
	// the same equation as effective permeability of a lamination...
	if (o!=0) ufd=muo*tanh(sqrt(I*W*o*muo)*d/2.)/(sqrt(I*W*o*muo)*d/2.);
	else ufd=0;

	// relative complex permeability
	bl->ProximityMu=(fill*ufd+(1.-fill)*muo)/muo;
}
*/

double FSolver::ElmArea(int i)
{
    // returns element cross-section area in meter^2
    int j,n[3];
    double b0,b1,c0,c1;

    for(j=0; j<3; j++) n[j] = meshele[i].p[j];

    b0 = meshnode[n[1]].y - meshnode[n[2]].y;
    b1 = meshnode[n[2]].y - meshnode[n[0]].y;
    c0 = meshnode[n[2]].x - meshnode[n[1]].x;
    c1 = meshnode[n[0]].x - meshnode[n[2]].x;

    return 0.0001 * (b0*c1 - b1*c0) / 2.;

}

// SortNodes: sorts mesh nodes based on a new numbering
void FSolver::SortNodes (int* newnum)
{
    int j = 0;
    int n = 0;

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
