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

#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "femmcomplex.h"
#include "spars.h"
#include "fparse.h"
#include "fsolver.h"
#include "mmesh.h"
#include "CNode.h"
#include "lua.h"

// template instantiation:
#include "../libfemm/feasolver.cpp"
#include "../libfemm/cuthill.cpp"
template class FEASolver<
        femm::CPointProp
        , femm::CMBoundaryProp
        , fsolver::CMMaterialProp
        , femm::CMCircuit
        , femm::CMSolverBlockLabel
        , femm::CSolverNode
        >;

#ifndef _MSC_VER
#define _strnicmp strncasecmp
#endif

using namespace std;
using namespace femm;
using fsolver::CMMaterialProp;

extern lua_State *lua; // the main lua object

extern void lua_baselibopen (lua_State *L);
extern void lua_iolibopen (lua_State *L);
extern void lua_strlibopen (lua_State *L);
extern void lua_mathlibopen (lua_State *L);
extern void lua_dblibopen (lua_State *L);
extern int bLinehook;
extern int lua_byebye;

lua_State *lua;
int bLinehook;
int lua_byebye;
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
    labellist = NULL;
    lineproplist = NULL;

    extRo = extRi = extZo = 0.0;

    // initialise the warning message box function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;

    // Initialize Lua
    bLinehook = false;
    bMultiplyDefinedLabels = false;
    lua = lua_open(4096);
    lua_baselibopen(lua);
    lua_strlibopen(lua);
    lua_mathlibopen(lua);
    lua_iolibopen(lua);
}

FSolver::~FSolver()
{
    lua_close(lua);
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
    circproplist.clear();
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
    std::ifstream input;
    int j,k,ic;
    char s[1024];
    CPointProp       PProp;
    CMBoundaryProp   BProp;
    CMMaterialProp    MProp;
    CMCircuit        CProp;
    CMSolverBlockLabel     blk;

    sprintf(s,"%s.fem", PathName.c_str() );
    input.open(s, std::ifstream::in);
    if (!input.is_open())
    {
        printf("Couldn't read from specified .fem file\n");
        return false;
    }

    // define some defaults
    Frequency=0.;
    Precision=1.e-08;
    Relax=1.;
    ProblemType=PLANAR;
    Coords=CART;
    NumPointProps=0;
    NumLineProps=0;
    NumBlockProps=0;
    NumCircProps=0;
    ACSolver=0;
    DoForceMaxMeshArea = false;

    // parse the file

    string token;
    while (input.good())
    {
        input >> token;
        // transform token to lower case
        transform(token.begin(), token.end(), token.begin(), ::tolower);

        // Frequency of the problem
        if(token == "[frequency]")
        {
            expectChar(input, '=', "frequency");
            input >> Frequency;
            continue;
        }

        // Precision
        if( token == "[precision]")
        {
            expectChar(input, '=');
            input >> Precision;
            continue;
        }

        // AC Solver Type
        if( token == "[acsolver]")
        {
            expectChar(input, '=');
            input >> ACSolver;
            continue;
        }

        // Option to force use of default max mesh, overriding
        // user choice
        if( token == "[forcemaxmesh]")
        {
            expectChar(input, '=');
            input >> DoForceMaxMeshArea;
            continue;
        }

        // Units of length used by the problem
        if( token == "[lengthunits]")
        {
            expectChar(input, '=');
            input >> token;
            transform(token.begin(), token.end(), token.begin(), ::tolower);

            if( token == "inches" ) LengthUnits=LengthInches;
            else if( token == "millimeters" ) LengthUnits=LengthMillimeters;
            else if( token == "centimeters" ) LengthUnits=LengthCentimeters;
            else if( token == "mils" ) LengthUnits=LengthMils;
            else if( token == "microns" ) LengthUnits=LengthMicrometers;
            else if( token == "meters" ) LengthUnits=LengthMeters;
            continue;
        }

        // Problem Type (planar or axisymmetric)
        if( token == "[problemtype]" )
        {
            expectChar(input, '=');
            input >> token;
            transform(token.begin(), token.end(), token.begin(), ::tolower);

            if( token == "planar" ) ProblemType=PLANAR;
            if( token == "axisymmetric" ) ProblemType=AXISYMMETRIC;
            continue;
        }

        // Coordinates (cartesian or polar)
        if( token == "[coordinates]" )
        {
            expectChar(input, '=');
            input >> token;
            transform(token.begin(), token.end(), token.begin(), ::tolower);

            if ( token == "cartesian" ) Coords=CART;
            if ( token == "polar" ) Coords=POLAR;
            continue;
        }

        // properties for axisymmetric external region
        if( token == "[extzo]" )
        {
            expectChar(input, '=');
            input >> extZo;
            continue;
        }

        if( token == "[extro]" )
        {
            expectChar(input, '=');
            input >> extRo;
            continue;
        }

        if( token == "[extri]" )
        {
            expectChar(input, '=');
            input >> extRi;
            continue;
        }

        // Point Properties
        if( token == "[pointprops]" )
        {
            expectChar(input, '=');
            input >> k;
            if (k>0) nodeproplist=new CPointProp[k];
            continue;
        }

        if( token == "<beginpoint>" )
        {
            PProp.J.re=0.;
            PProp.J.im=0.;
            PProp.A.re=0.;
            PProp.A.im=0.;
            continue;
        }

        if( token == "<a_re>" )
        {
            expectChar(input, '=');
            input >> PProp.A.re;
            continue;
        }

        if( token == "<a_im>" )
        {
            expectChar(input, '=');
            input >> PProp.A.im;
            continue;
        }

        if( token == "<i_re>" )
        {
            expectChar(input, '=');
            input >> PProp.J.re;
            continue;
        }

        if( token == "<i_im>" )
        {
            expectChar(input, '=');
            input >> PProp.J.im;
            continue;
        }

        if( token == "<endpoint>" )
        {
            nodeproplist[NumPointProps]=PProp;
            NumPointProps++;
            continue;
        }

        // Boundary Properties;
        if( token == "[bdryprops]" )
        {
            expectChar(input, '=');
            input >> k;
            if (k>0) lineproplist=new CMBoundaryProp[k];
            continue;
        }

        if( token == "<beginbdry>" )
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
            continue;
        }

        if( token == "<bdrytype>" )
        {
            expectChar(input, '=');
            input >> BProp.BdryFormat;
            continue;
        }

        if( token == "<mu_ssd>" )
        {
            expectChar(input, '=');
            input >> BProp.Mu;
            continue;
        }

        if( token == "<sigma_ssd>" )
        {
            expectChar(input, '=');
            input >> BProp.Sig;
            continue;
        }

        if( token == "<a_0>" )
        {
            expectChar(input, '=');
            input >> BProp.A0;
            continue;
        }

        if( token == "<a_1>" )
        {
            expectChar(input, '=');
            input >> BProp.A1;
            continue;
        }

        if( token == "<a_2>" )
        {
            expectChar(input, '=');
            input >> BProp.A2;
            continue;
        }

        if( token == "<phi>" )
        {
            expectChar(input, '=');
            input >> BProp.phi;
            continue;
        }

        if( token == "<c0>" )
        {
            expectChar(input, '=');
            input >> BProp.c0.re;
            continue;
        }

        if( token == "<c1>" )
        {
            expectChar(input, '=');
            input >> BProp.c1.re;
            continue;
        }

        if( token == "<c0i>" )
        {
            expectChar(input, '=');
            input >> BProp.c0.im;
            continue;
        }

        if( token == "<c1i>" )
        {
            expectChar(input, '=');
            input >> BProp.c1.im;
            continue;
        }

        if( token == "<endbdry>" )
        {
            lineproplist[NumLineProps]=BProp;
            NumLineProps++;
            continue;
        }

        // Block Properties;
        if( token == "[blockprops]" )
        {
            expectChar(input, '=');
            input >> k;
            if (k>0) blockproplist=new CMMaterialProp[k];
            continue;
        }

        if( token == "<beginblock>" )
        {
            MProp.mu_x=1.;
            MProp.mu_y=1.;            // permeabilities, relative
            MProp.H_c=0.;                // magnetization, A/m
            MProp.J.re=0.;
            MProp.J.im=0.;                // applied current density, MA/m^2
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
            continue;
        }

        if( token == "<mu_x>" )
        {
            expectChar(input, '=');
            input >> MProp.mu_x;
            continue;
        }

        if( token == "<mu_y>" )
        {
            expectChar(input, '=');
            input >> MProp.mu_y;
            continue;
        }

        if( token == "<h_c>" )
        {
            expectChar(input, '=');
            input >> MProp.H_c;
            continue;
        }

        if( token == "<h_cangle>" )
        {
            expectChar(input, '=');
            input >> MProp.Theta_m;
            continue;
        }

        if( token == "<j_re>" )
        {
            expectChar(input, '=');
            input >> MProp.J.re;
            continue;
        }

        if( token == "<j_im>" )
        {
            expectChar(input, '=');
            input >> MProp.J.im;
            continue;
        }

        if( token == "<sigma>" )
        {
            expectChar(input, '=');
            input >> MProp.Cduct;
            continue;
        }

        if( token == "<phi_h>" )
        {
            expectChar(input, '=');
            input >> MProp.Theta_hn;
            continue;
        }


        if( token == "<phi_hx>" )
        {
            expectChar(input, '=');
            input >> MProp.Theta_hx;
            continue;
        }

        if( token == "<phi_hy>" )
        {
            expectChar(input, '=');
            input >> MProp.Theta_hy;
            continue;
        }

        if( token == "<d_lam>" )
        {
            expectChar(input, '=');
            input >> MProp.Lam_d;
            continue;
        }

        if( token == "<lamfill>" )
        {
            expectChar(input, '=');
            input >> MProp.LamFill;
            continue;
        }

        if( token == "<wired>" )
        {
            expectChar(input, '=');
            input >> MProp.WireD;
            continue;
        }

        if( token == "<lamtype>" )
        {
            expectChar(input, '=');
            input >> MProp.LamType;
            continue;
        }

        if( token == "<nstrands>" )
        {
            expectChar(input, '=');
            input >> MProp.NStrands;
            continue;
        }

        if( token == "<bhpoints>" )
        {
            expectChar(input, '=');
            input >> MProp.BHpoints;
            if (MProp.BHpoints>0)
            {
                MProp.Hdata=new CComplex[MProp.BHpoints];
                MProp.Bdata=new double[MProp.BHpoints];
                for(j=0; j<MProp.BHpoints; j++)
                {
                    input >> MProp.Bdata[j] >> MProp.Hdata[j].re;
                    MProp.Hdata[j].im=0;
                }
            }
            continue;
        }

        if( token == "<endblock>" )
        {
            blockproplist[NumBlockProps]=MProp;
            blockproplist[NumBlockProps].GetSlopes(Frequency*2.*PI);
            NumBlockProps++;
            MProp.BHpoints=0;
            MProp.Bdata=NULL;
            MProp.Hdata=NULL;
            continue;
        }

        // Circuit Properties
        if( token == "[circuitprops]" )
        {
            expectChar(input, '=');
            input >> k;
            if(k>0) circproplist.reserve(k);
            continue;
        }

        if( token == "<begincircuit>" )
        {
            CProp.dVolts.re=0.;
            CProp.dVolts.im=0.;
            CProp.Amps.re=0.;
            CProp.Amps.im=0.;
            CProp.CircType=0;
            continue;
        }

        if( token == "<voltgradient_re>" )
        {
            expectChar(input, '=');
            input >> CProp.dVolts.re;
            continue;
        }

        if( token == "<voltgradient_im>" )
        {
            expectChar(input, '=');
            input >> CProp.dVolts.im;
            continue;
        }

        if( token == "<totalamps_re>" )
        {
            expectChar(input, '=');
            input >> CProp.Amps.re;
            continue;
        }

        if( token == "<totalamps_im>" )
        {
            expectChar(input, '=');
            input >> CProp.Amps.im;
            continue;
        }

        if( token == "<circuittype>" )
        {
            expectChar(input, '=');
            input >> CProp.CircType;
            continue;
        }

        if( token == "<endcircuit>" )
        {
            circproplist.push_back(CProp);
            NumCircProps++;
            continue;
        }


        // read in regional attributes
        if(token == "[numblocklabels]" )
        {
            expectChar(input, '=');
            input >> k;
            if (k>0) labellist=new CMSolverBlockLabel[k];
            NumBlockLabels=k;
            for(int i=0; i<k; i++)
            {
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
                input >> blk.x;
                input >> blk.y;
                input >> blk.BlockType;
                input >> blk.MaxArea;
                input >> blk.InCircuit;
                input >> blk.MagDir;
                input >> blk.InGroup;
                input >> blk.Turns;

                int extDefault;
                input >> extDefault;
                // second last bit in extDefault flag, we mask the other bits
                // and take the resulting value, if not zero it will evaluate to true
                blk.IsDefault  = extDefault & 2;
                // last bit in extDefault flag, we mask the other bits
                // and take the resulting value, if not zero it will evaluate to true
                blk.IsExternal = extDefault & 1;

                // MagDirFctn is an extra field not formally described in the .fem file format spec
                ParseString(input, &blk.MagDirFctn);

                blk.BlockType--;
                blk.InCircuit--;
                labellist[i]=blk;
            }
            continue;
        }
        // fall-through; token was not used
        // -> ignore rest of line
        //input.getline(s,1024);
        //std::cerr << "**unused: " <<token << " " << s << std::endl;
    }
    if (input.bad())
    {
        std::cerr << "Parse error while reading input file " << PathName << ".fem!";
        std::cerr << "Last token was: " << token;
    }

    // need to set these so that valid BH data doesn't get wiped
    // by the destructor of MProp
    MProp.BHpoints = 0;
    MProp.Bdata = NULL;
    MProp.Hdata = NULL;

    input.close();

    if (NumCircProps==0) return true;

    // Process circuits for serial connections.
    // The program deals with serial "circuits" by making a separate
    // circuit property for each block in the serial circuit.  Then,
    // each of this larger number of circuits can be processed using
    // the previous approach which considered all circuits to be
    // parallel connected.

    // first, make enough space for all possible circuits;
    circproplist.resize(NumCircProps+NumBlockLabels);
    for(int k=0; k<NumCircProps; k++)
    {
        circproplist[k].OrigCirc=-1;
    }
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
                ncirc.Amps.im*=labellist[k].Turns;
                ncirc.Amps.re*=labellist[k].Turns;
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


    //read meshnodes;
    sprintf(infile,"%s.node",PathName.c_str());
    if((fp=fopen(infile,"rt"))==NULL)
    {
        return BADELEMENTFILE;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    NumNodes = k;

    meshnode = new CSolverNode[k];
    CSolverNode node;
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
        node.x *= 100 * LengthConv[LengthUnits];
        node.y *= 100 * LengthConv[LengthUnits];

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
    CMSolverBlockLabel* bl= &labellist[lbl];
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
            CSolverNode swap;

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
