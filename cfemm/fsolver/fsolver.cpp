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
   By: Richard Crozier
       Johannes Zarl-Zierl
   Contact:
        richard.crozier@yahoo.co.uk
        johannes.zarl-zierl@jku.at

   Contributions by Johannes Zarl-Zierl were funded by
   Linz Center of Mechatronics GmbH (LCM)
*/

// fsolver.cpp : implementation of the FSolver class
//

#include <CElement.h>
#include <CAirGapElement.h>
#include <CNode.h>
#include <femmcomplex.h>
#include <fparse.h>
#include <fsolver.h>
#include <LuaInstance.h>
#include <spars.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <malloc.h>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

// template instantiation:
#include "../libfemm/feasolver.cpp"
#include "../libfemm/cuthill.cpp"
template class FEASolver<
        femm::CMPointProp
        , femm::CMBoundaryProp
        , femm::CMSolverMaterialProp
        , femm::CMCircuit
        , femm::CMBlockLabel
        , femmsolver::CMElement
        , femmsolver::CAirGapElement
        >;

#ifndef _MSC_VER
#define _strnicmp strncasecmp
#endif

#ifdef DEBUG_FSOLVER
#define debug std::cerr << __func__ << "(): "
#else
#define debug while(false) std::cerr
#endif


using namespace std;
using namespace femm;
using namespace femmsolver;

/////////////////////////////////////////////////////////////////////////////
// FSolver construction/destruction

FSolver::FSolver()
    : theLua(new LuaInstance)
{
    Frequency = 0.0;
    Relax = 0.0;
    ACSolver=0;
    NumCircPropsOrig = 0;

    //meshnode = NULL;

    // initialise the warning message box function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;

    bMultiplyDefinedLabels = false;
}

FSolver::~FSolver()
{
    delete theLua;
    CleanUp();
}

void FSolver::CleanUp()
{
    FEASolver_type::CleanUp();
    //delete[] meshnode;
    //meshnode = NULL;
    //delete []Aprev;
    //Aprev = nullptr;
}

void FSolver::getPrevAxiB(int k, double &B1p, double &B2p) const
{
	// for axisymmetric incremental problem,
	// get flux density from the previous solution.
	// Code cribbed from CFemmviewDoc::GetElementB(CElement &elm) in femm source
	int i,n[3];
	double b[3],c[3],da;
	double v[6],dp,dq;
	double R[3],r;
	//double Z[3];

	// since all node positions were converted to units of cm
    // the proper LengthConv converts centimeters to meters
    double LengthConv = 0.01;

	for(i=0;i<3;i++) n[i]=meshele[k].p[i];

	b[0]=meshnode[n[1]].y - meshnode[n[2]].y;
	b[1]=meshnode[n[2]].y - meshnode[n[0]].y;
	b[2]=meshnode[n[0]].y - meshnode[n[1]].y;
	c[0]=meshnode[n[2]].x - meshnode[n[1]].x;
	c[1]=meshnode[n[0]].x - meshnode[n[2]].x;
	c[2]=meshnode[n[1]].x - meshnode[n[0]].x;

	for(i=0,r=0;i<3;i++){
		R[i]=meshnode[n[i]].x;
		//Z[i]=meshnode[n[i]].y;
		r+=R[i]/3.;
	}

	// corner nodes
	v[0]=Aprev[n[0]];
	v[2]=Aprev[n[1]];
	v[4]=Aprev[n[2]];

	// construct values for mid-side nodes;
	if ((R[0]<1.e-06) && (R[1]<1.e-06)) v[1]=(v[0]+v[2])/2.;
	else v[1]=(R[1]*(3.*v[0] + v[2]) + R[0]*(v[0] + 3.*v[2]))/
		 (4.*(R[0] + R[1]));

	if ((R[1]<1.e-06) && (R[2]<1.e-06)) v[3]=(v[2]+v[4])/2.;
	else v[3]=(R[2]*(3.*v[2] + v[4]) + R[1]*(v[2] + 3.*v[4]))/
		 (4.*(R[1] + R[2]));

	if ((R[2]<1.e-06) && (R[0]<1.e-06)) v[5]=(v[4]+v[0])/2.;
	else v[5]=(R[0]*(3.*v[4] + v[0]) + R[2]*(v[4] + 3.*v[0]))/
		(4.*(R[2] + R[0]));

	// derivatives w.r.t. p and q:
	dp=(-v[0] + v[2] + 4.*v[3] - 4.*v[5])/3.;
	dq=(-v[0] - 4.*v[1] + 4.*v[3] + v[4])/3.;

	// now, compute flux.
	da=(b[0]*c[1]-b[1]*c[0]);
	da*=2.*PI*r*LengthConv*LengthConv;
	B1p=Re(-(c[1]*dp+c[2]*dq)/da);
	B2p=Re( (b[1]*dp+b[2]*dq)/da);
}

void FSolver::getPrev2DB(int k, double &B1p, double &B2p) const
{
    int n[3];
    for(int i=0;i<3;i++) n[i]=meshele[k].p[i];

    double b[3],c[3];
    b[0]=meshnode[n[1]].y - meshnode[n[2]].y;
    b[1]=meshnode[n[2]].y - meshnode[n[0]].y;
    b[2]=meshnode[n[0]].y - meshnode[n[1]].y;
    c[0]=meshnode[n[2]].x - meshnode[n[1]].x;
    c[1]=meshnode[n[0]].x - meshnode[n[2]].x;
    c[2]=meshnode[n[1]].x - meshnode[n[0]].x;
    double da=(b[0]*c[1]-b[1]*c[0]);

    B1p=0;
    B2p=0;

    for(int i=0;i<3;i++)
    {
        B1p+=Aprev[n[i]]*c[i]/(da*LengthConvMeters[LengthUnits]);
        B2p-=Aprev[n[i]]*b[i]/(da*LengthConvMeters[LengthUnits]);
    }
}

/////////////////////////////////////////////////////////////////////////////
// FSolver commands

bool FSolver::LoadProblemFile ()
{
    // meshLoadedFromPrevSolution will be set to tru in loadPreviousSolution if
    // a mesh is successfully loaded from a previous solution file. The LoadMesh
    // method checks this before attempting to load a mesh
    meshLoadedFromPrevSolution = false;

    // define some defaults
    Relax=1.;

    // parse the file, unlike in the original femm we do this *before* reading
    // any previous mesh so we know whether to bother loading the previous
    // solution data as well as just the mesh
    std::string femFile = PathName+".fem";
    if (!FEASolver_type::LoadProblemFile(femFile))
    {
        return false;
    }

    // if there's a "previous solution" specified, slurp of the mesh and
    // possibly the previous vector potential values out of that file.
    if (!previousSolutionFile.empty())
    {
        bool loadAprev;

        if (PrevType == 0)
        {
            // not incremental permeability, so we don't bother storing Aprev
            loadAprev = false;
        }
        else
        {
            loadAprev = true;
        }

        return loadPreviousSolution(loadAprev);
    }

    // do some precomputations
    for (auto &prop : blockproplist)
    {
        debug << "doing precomputations for material " << prop.BlockName << "\n";
        if (!previousSolutionFile.empty() && Frequency>0 && PrevType != 0)
        {
            // first time through was just to get MuMax from AC curve...
            // -> backup Hdata and Bdata:
            std::vector<double> oldBdata;
            std::vector<CComplex> oldHdata;
            oldBdata.reserve(prop.BHpoints);
            oldHdata.reserve(prop.BHpoints);
            std::copy(prop.Bdata.begin(), prop.Bdata.end(), oldBdata.begin());
            std::copy(prop.Hdata.begin(), prop.Hdata.end(), oldHdata.begin());

            prop.GetSlopes(Frequency*2.*PI);

            std::copy(oldBdata.begin(), oldBdata.end(), prop.Bdata.begin());
            std::copy(oldHdata.begin(), oldHdata.end(), prop.Hdata.begin());
            prop.clearSlopes();

            // second time through is to get the DC curve
            prop.GetSlopes(0);
        } else {
            prop.GetSlopes(Frequency*2.*PI);
        }
    }


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
    for(int k=0; k<NumBlockLabels; k++)
        if(labellist[k].InCircuit>=0)
        {
            int ic=labellist[k].InCircuit;
            assert(ic < (int)circproplist.size());
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
    for(int k=0; k<NumCircProps; k++)
        if(circproplist[k].CircType==1) circproplist[k].CircType=0;

//    // Check to see if any regions are multiply defined
//    // (i.e. tagged by more than one block label). If so,
//    // display a warnign and mark bMultiplyDefinedLabels true.
//    for(k=0,bMultiplyDefinedLabels=false; k<NumBlockLabels; k++)
//    {
//        // test if the label is inside the meshed region, by attempting to find
//        // which triangle it is in, if it's outside the problem region it will
//        // be ignored anyway
//        if((i = InTriangle(labels[k].x,labels[k].y)) >= 0)
//        {
//            // it's in the problem region,
//            if(meshele[i].lbl != k)
//            {
//                labels[meshelem[i].lbl].IsSelected = true;
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

LoadMeshErr FSolver::LoadMesh(bool deleteFiles)
{
    int i,j,k,q,n0,n1;
    char infile[256];
    FILE *fp;
    char s[1024];

    if (meshLoadedFromPrevSolution)
    {
        return NOERROR;
    }

    //read meshnodes;
    sprintf(infile,"%s.node",PathName.c_str());
    if((fp=fopen(infile,"rt"))==NULL)
    {
        return BADNODEFILE;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    NumNodes = k;

    meshnode.clear();
    meshnode.shrink_to_fit();
    meshnode.reserve(k);
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
        node.x *= 100 * LengthConvMeters[LengthUnits];
        node.y *= 100 * LengthConvMeters[LengthUnits];

        meshnode.push_back (node);
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

    if (k!=0)
    {
        pbclist.clear();
        pbclist.shrink_to_fit();
        pbclist.reserve(k);
    }
    CCommonPoint pbc;
    for(i=0; i<k; i++)
    {
        fscanf(fp,"%i",&j);
        fscanf(fp,"%i",&pbc.x);
        fscanf(fp,"%i",&pbc.y);
        fscanf(fp,"%i",&pbc.t);
        pbclist.push_back(pbc);
    }


	// read in air gap element info
	fgets(s,1024,fp);
	sscanf(s,"%i", &NumAirGapElems);

	CAirGapElement age;

	agelist.clear();
	agelist.shrink_to_fit();
	agelist.reserve(NumAirGapElems);

	for(i=0;i<NumAirGapElems;i++)
    {
		fgets(s,80,fp);

		age.BdryName = s;

		fgets(s,1024,fp);

		sscanf(s,"%i %lf %lf %lf %lf %lf %lf %lf %i %lf %lf",
			&age.BdryFormat,&age.InnerAngle,&age.OuterAngle,
			&age.ri,&age.ro,&age.totalArcLength,
			&age.agc.re,&age.agc.im,&age.totalArcElements,&age.InnerShift,&age.OuterShift);

        age.quadNode.clear();
        age.quadNode.shrink_to_fit();
        age.quadNode.reserve(age.totalArcElements+1);

		for(k=0;k<=age.totalArcElements;k++)
        {
			fgets(s,1024,fp);

			sscanf(s,"%i %lf %i %lf %i %lf %i %lf",
				&age.quadNode[k].n0, &age.quadNode[k].w0,
				&age.quadNode[k].n1, &age.quadNode[k].w1,
				&age.quadNode[k].n2, &age.quadNode[k].w2,
				&age.quadNode[k].n3, &age.quadNode[k].w3);
		}
		agelist.push_back (age);
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

    meshele.clear();
    meshele.shrink_to_fit();
    meshele.reserve(k);
    femmsolver::CMElement elm;

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
        assert(elm.lbl < (int)labellist.size());
        // look up block type out of the list of block labels
        elm.blk = labellist[elm.lbl].BlockType;

        meshele.push_back(elm);
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

    return NOERROR;
}


//bool FSolver::LoadMeshFromPrevSolution(bool loadAprev)
//{
//	if (PrevSoln.GetLength()==0) return false;
//
//	FILE *fp;
//	int i,k;
//	char s[1024],q[256];
//	char *v;
//	double prevFreq=0;
//	double c[]={2.54,0.1,1.,100.,0.00254,1.e-04};
//	if ((fp=fopen(PrevSoln,"rt"))==NULL){
//		MsgBox("Couldn't read from specified previous solution\n");
//		return false;
//	}
//
//	// parse the file
//	k=0;
//	while (fgets(s,1024,fp)!=NULL)
//	{
//		sscanf(s,"%s",q);
//
//		// Frequency of the problem
//		if( _strnicmp(q,"[frequency]",11)==0){
//			v=StripKey(s);
//			sscanf(v,"%lf",&prevFreq);
//			q[0]=NULL;
//		}
//
//		sscanf(s,"%s",q);
//		if( _strnicmp(q,"[solution]",11)==0){
//			k=1;
//			break;
//		}
//	}
//
//	// case where the solution is never found.
//	if (k==0)
//	{
//		fclose(fp);
//		MsgBox("Couldn't read mesh from specified previous solution\n");
//		return false;
//	}
//
//	if (loadAprev)
//    {
//        // case were previous solution is an AC problem.
//        // only DC  previous solutions are presently supported
//        if (prevFreq!=0)
//        {
//            fclose(fp);
//            MsgBox("Only DC previous solutions are presently supported\n");
//            return false;
//        }
//    }
//
//	////////////////////////////
//	// read in the mesh from previous solution!!!
//	///////////////////////////
//
//	// read in nodes
//	LoadMeshNodesFromPrevSolution(loadAprev, fp);
//
//	// read elements
//	LoadMeshElementsFromPrevSolution(fp);
//
//	// scroll through block label info
//	fgets(s,1024,fp);
//	sscanf(s,"%i",&k);
//	for(i=0;i<k;i++) fgets(s,1024,fp);
//
//	// read in PBC list
//	LoadPBCFromPrevSolution(fp);
//
//	// read in air gap elements
//    LoadAGEsFromPrevSolution(fp);
//
//	fclose(fp);
//	return true;
//}

bool FSolver::LoadMeshNodesFromSolution(bool loadAprev, FILE* fp)
{
    double tmpAprev;
    char s[1024];

    // read in nodes
    fgets(s,1024,fp);
    sscanf(s,"%i",&NumNodes);

    if (loadAprev)
    {
        Aprev.clear();
        Aprev.shrink_to_fit();
        Aprev.reserve(NumNodes);
    }

    meshnode.clear ();
    meshnode.shrink_to_fit();
    meshnode.reserve(NumNodes);
    CNode node;
    for(int i=0;i<NumNodes;i++)
    {
        fgets(s,1024,fp);
        sscanf(s,"%lf   %lf     %lf     %i\n",&node.x,&node.y,&tmpAprev,&node.BoundaryMarker);

        // convert all lengths to centimeters (better conditioning this way...)
        node.x *= 100 * LengthConvMeters[LengthUnits];
        node.y *= 100 * LengthConvMeters[LengthUnits];

        if (loadAprev)
        {
            Aprev.push_back(tmpAprev);
        }

        meshnode.push_back(node);
    }

    return true;
}

bool FSolver::LoadMeshElementsFromSolution(FILE* fp)
{
    char s[1024];

    fgets (s, 1024, fp);

    sscanf (s,"%i", &NumEls);

    using CMElement = femmsolver::CMElement;

    meshele.clear();
    meshele.shrink_to_fit();
    meshele.reserve (NumEls);

    for(int i=0; i<NumEls; i++)
    {
        CMElement elm;

        fgets(s,1024,fp);

        sscanf ( s,
                 "%i    %i      %i      %i      %i      %i      %i      %lf\n",
                 &elm.p[0],
                 &elm.p[1],
                 &elm.p[2],
                 &elm.lbl,
                 &elm.e[0],
                 &elm.e[1],
                 &elm.e[2],
                 &elm.Jprev );

        // look up block type out of the list of block labels
        elm.blk = labellist[elm.lbl].BlockType;

        meshele.push_back(elm);
    }

    return true;
}

bool FSolver::LoadPBCFromSolution(FILE* fp)
{
    char s[1024];

    if (fgets(s,1024,fp)!=0)
    {
        sscanf(s,"%i",&NumPBCs);

        // clear the existing pbc list
        pbclist.clear();

        // remove any previously reserved capacity
        pbclist.shrink_to_fit();

        // reserve enough capacity for the declared number of pbc's in the file
        pbclist.reserve(NumPBCs);

        for(int i=0;i<NumPBCs;i++)
        {
            CCommonPoint pbc;
            fgets(s,1024,fp);
            sscanf(s,"%i    %i      %i\n",&pbc.x,&pbc.y,&pbc.t);
            pbclist.push_back(pbc);
        }
    }

    return true;
}

bool FSolver::LoadAGEsFromSolution(FILE* fp)
{
    char s[1024];
    CAirGapElement age;

	fgets(s,1024,fp);
	sscanf(s,"%i",&NumAirGapElems);

	agelist.clear();
	agelist.shrink_to_fit();
	agelist.reserve(NumAirGapElems);

	for(int i=0; i<NumAirGapElems; i++)
    {

		fgets(s,80,fp);

		age.BdryName = std::string (s);

		fgets(s,1024,fp);

		sscanf( s, "%i %lf %lf %lf %lf %lf %lf %lf %i %lf %lf",
                &age.BdryFormat,
                &age.InnerAngle,
                &age.OuterAngle,
                &age.ri,
                &age.ro,
                &age.totalArcLength,
                &age.agc.re,
                &age.agc.im,
                &age.totalArcElements,
                &age.InnerShift,
                &age.OuterShift );

		age.quadNode.reserve (age.totalArcElements+1);

		for(int k=0; k<=age.totalArcElements; k++)
		{
		    CQuadPoint qp;

			fgets(s,1024,fp);
			sscanf ( s,"%i %lf %i %lf %i %lf %i %lf",
                     &age.quadNode[k].n0,
                     &age.quadNode[k].w0,
                     &age.quadNode[k].n1,
                     &age.quadNode[k].w1,
                     &age.quadNode[k].n2,
                     &age.quadNode[k].w2,
                     &age.quadNode[k].n3,
                     &age.quadNode[k].w3);

            age.quadNode.push_back (qp);

		}
		agelist.push_back(age);
	}

    return true;
}

bool FSolver::loadPreviousSolution(bool loadAprev)
{
    if (previousSolutionFile.empty())
    {
        return false;
    }

    FILE *fp;
    if ((fp=fopen(previousSolutionFile.c_str(),"rt"))==NULL){
        WarnMessage("Couldn't read from specified previous solution\n");
        return false;
    }

    // parse the file
    bool hasSolution=false;
    char s[1024];
    while (fgets(s,1024,fp)!=0)
    {
        char q[256];
        sscanf(s,"%s",q);

        // Frequency of the problem
        if( _strnicmp(q,"[frequency]",11)==0){
            double prevFreq=0;
            char *v=StripKey(s);
            sscanf(v,"%lf",&prevFreq);

            // case were previous solution is an AC problem.
            // only DC  previous solutions are presently supported
            if (prevFreq!=0)
            {
                fclose(fp);
                WarnMessage("Only DC previous solutions are presently supported\n");
                return false;
            }
        }

        sscanf(s,"%s",q);
        if( _strnicmp(q,"[solution]",11)==0){
            hasSolution=true;
            break;
        }
    }

    // case where the solution is never found.
    if (!hasSolution)
    {
        fclose(fp);
        WarnMessage("Couldn't read from specified previous solution\n");
        return false;
    }

    ////////////////////////////
    // read in the previous solution!!!
    ///////////////////////////

    // read in nodes
    LoadMeshNodesFromSolution(loadAprev, fp);

    // read elements
    LoadMeshElementsFromSolution(fp);

    // scroll through block label info
    fgets(s,1024,fp);
    int numLabels;
    sscanf(s,"%i",&numLabels);
    for(int i=0;i<numLabels;i++) fgets(s,1024,fp);

    // read in PBC list
    LoadPBCFromSolution(fp);

    // read in air gap elements
    LoadAGEsFromSolution(fp);

    fclose(fp);

    meshLoadedFromPrevSolution = true;

    return true;
}

void FSolver::GetFillFactor(int lbl)
{
    // Get the fill factor associated with a stranded and
    // current-carrying region.  For AC problems, also compute
    // the apparent conductivity and permeability for use in
    // post-processing the voltage.

    CMSolverMaterialProp* bp= &blockproplist[labellist[lbl].BlockType];
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

bool FSolver::runSolver(bool verbose)
{
    // load mesh
    LoadMeshErr err = LoadMesh();
    if (err != NOERROR)
    {
        WarnMessage(getErrorString(err).c_str());
        return false;
    }

    // renumber using Cuthill-McKee
    if (verbose)
        PrintMessage("renumbering nodes\n");
    if (!Cuthill())
    {
        WarnMessage("problem renumbering node points\n");
        return false;
    }

    if (verbose)
    {
        PrintMessage("solving...\n");
        std::string stats = "Problem Statistics:\n";
        stats += to_string(NumNodes) + " nodes\n";
        stats += to_string(NumEls) + " elements\n";
        stats += "Precision: " + to_string(Precision) + "\n";
        PrintMessage(stats.c_str());
    }

    if (Frequency == 0)
    {
        if (!previousSolutionFile.empty())
        {
            WarnMessage("Cannot handle incremental permeability problems with frequency 0.\n");
            return false;
        }
        CBigLinProb L;
        L.Precision = Precision;

        // initialize the problem, allocating the space required to solve it.
        if (L.Create(NumNodes, BandWidth) == false)
        {
            WarnMessage("couldn't allocate enough space for matrices\n");
            return false;
        }

        // Create element matrices and solve the problem;
        if (ProblemType == PLANAR)
        {

            if (Static2D(L) == false)
            {
                WarnMessage("Couldn't solve the problem\n");
                return false;
            }
            if (verbose)
                PrintMessage("Static 2-D problem solved\n");
        } else {
            if (StaticAxisymmetric(L) == false)
            {
                WarnMessage("Couldn't solve the problem");
                return false;
            }
            if (verbose)
                PrintMessage("Static axisymmetric problem solved\n");
        }

        if (WriteStatic2D(L) == false)
        {
            WarnMessage("couldn't write results to disk\n");
            return false;
        }
        if (verbose)
            PrintMessage("results written to disk\n");
    } else {
        CBigComplexLinProb L;
        L.Precision = Precision;

        // initialize the problem, allocating the space required to solve it.
        if (!L.Create(NumNodes+NumCircProps, BandWidth, NumNodes))
        {
            WarnMessage("couldn't allocate enough space for matrices\n");
            return false;
        }

        // Create element matrices and solve the problem;
        if (ProblemType == PLANAR)
        {
            if (!previousSolutionFile.empty())
            {
                WarnMessage("Harmonic planar incremental permeability problems are work in progress. RESULTS WON'T BE VALID!\n");
            }
            if (!Harmonic2D(L))
            {
                WarnMessage("Couldn't solve the problem\n");
                return false;
            }
            if (verbose)
                PrintMessage("Harmonic 2-D problem solved\n");
        } else {
            if (!previousSolutionFile.empty())
            {
                WarnMessage("Cannot handle harmonic axisymmetric incremental problems.\n");
                return false;
            }
            if (!HarmonicAxisymmetric(L))
            {
                WarnMessage("Couldn't solve the problem\n");
                return false;
            }
            if (verbose)
                PrintMessage("Harmonic axisymmetric problem solved\n");
        }

        if (!WriteHarmonic2D(L))
        {
            WarnMessage("couldn't write results to disk\n");
            return false;
        }
        if (verbose)
            PrintMessage("results written to disk.\n");
    }
    return true;
}

// SortNodes: sorts mesh nodes based on a new numbering
void FSolver::SortNodes (int* newnum)
{
    // sort mesh nodes based on newnum;
    for(int i = 0; i < NumNodes; i++)
    {
        while(newnum[i] != i)
        {
            int j = newnum[i];
            swap(newnum[i],newnum[j]);
            swap(meshnode[i],meshnode[j]);
        }
    }
}

bool FSolver::handleToken(const string &token, istream &input, ostream &err)
{
    // Frequency of the problem
    if( token == "[frequency]")
    {
        expectChar(input, '=',err);
        parseValue(input, Frequency, err);
        return true;
    }

    return false;
}
