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

   Additional changes:
   Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
   Contributions by Johannes Zarl-Zierl were funded by Linz Center of
   Mechatronics GmbH (LCM)
*/

// fmesher.cpp : implementation of FMesher Class
//

#include "femmconstants.h"
#include "fmesher.h"
#include "fparse.h"
#include "IntPoint.h"

#include "triangle.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace femm;
using namespace fmesher;

namespace {
// set up some default behaviors
constexpr double DEFAULT_MINANGLE=30.;
}

FMesher::FMesher()
{
    // initialise the warning message function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;

    TriMessage = NULL;

    Verbose = true;

    // initialize the problem data structures
    // and default behaviour etc.
    Initialize(femm::FileType::Unknown);

}


FMesher::FMesher(std::shared_ptr<FemmProblem> p)
    : d_EditMode(EditMode::Invalid)
    , problem(p)
    , Verbose(true)
    , WarnMessage(&PrintWarningMsg)
    , TriMessage(nullptr)
{
}

bool FMesher::Initialize(femm::FileType t)
{

    // fire up lua
    //initalise_lua();

    // clear out all current lines, nodes, and block labels
    meshnode.clear ();
    meshline.clear ();
    greymeshline.clear ();
    probdescstrings.clear ();

    problem = std::make_shared<femm::FemmProblem>(t);
    // set problem attributes to generic ones;
    problem->MinAngle = DEFAULT_MINANGLE;

    return true;
}


void FMesher::EnforcePSLG(double tol)
{
    std::vector< std::unique_ptr<CNode>> newnodelist;
    std::vector< std::unique_ptr<CSegment>> newlinelist;
    std::vector< std::unique_ptr<CArcSegment>> newarclist;
    std::vector< std::unique_ptr<CBlockLabel>> newlabellist;

    // save existing objects into new*list:
    newnodelist.swap(problem->nodelist);
    newlinelist.swap(problem->linelist);
    newarclist.swap(problem->arclist);
    newlabellist.swap(problem->labellist);

    // find out what tolerance is so that there are not nodes right on
    // top of each other;
    double d = tol;
    if (tol==0)
    {
        if (newnodelist.size()==1)
            d = 1.e-08;
        else
        {
            CComplex p0 = newnodelist[0]->CC();
            CComplex p1 = p0;
            for (int i=1; i<(int)newnodelist.size(); i++)
            {
                if(newnodelist[i]->x<p0.re) p0.re = newnodelist[i]->x;
                if(newnodelist[i]->x>p1.re) p1.re = newnodelist[i]->x;
                if(newnodelist[i]->y<p0.im) p0.im = newnodelist[i]->y;
                if(newnodelist[i]->y>p1.im) p1.im = newnodelist[i]->y;
            }
            d = abs(p1-p0)*CLOSE_ENOUGH;
        }
    }

    // put in all of the nodes;
    for (auto &node: newnodelist)
    {
        problem->addNode(std::move(node), d);
    }

    // put in all of the lines;
    for (const auto &line: newlinelist)
    {
        // Note(ZaJ): the original code uses a variant of AddSegment that uses coordinates instead of node indices.
        // since I don't see any advantage to that, I just use the regular AddSegment method:
        //CComplex p0 (newnodelist[line->n0]->x, newnodelist[line->n0]->y);
        //CComplex p1 (newnodelist[line->n1]->x, newnodelist[line->n1]->y);
        //AddSegment(p0,p1,newlinelist[i]);

        // using the raw pointer is ok here, because AddSegment creates a copy anyways
        problem->addSegment(line->n0, line->n1, line.get(), d);
    }

    // put in all of the arcs;
    for (const auto &arc: newarclist)
    {
        // Note(ZaJ): the original code uses a variant of AddArcSegment that uses coordinates instead of the node indices
        // since I don't see any advantage to that, I just use the regular AddArcSegment method:
        //CComplex p0 (newnodelist[arc->n0]->x, newnodelist[arc->n0]->y);
        //CComplex p1 (newnodelist[arc->n1]->x, newnodelist[arc->n1]->y);
        //AddArcSegment(p0,p1,newarclist[i]);

        // using the raw pointer is ok here, because AddArcSegment creates a copy anyways
        problem->addArcSegment(*arc.get(), d);
    }

    // put in all of the block labels;
    for (auto &label: newlabellist)
    {
        AddBlockLabel(std::move(label), d);
    }

    problem->unselectAll();
    return;
}

femm::FileType FMesher::GetFileType (string PathName)
{
    // find the position of the last '.' in the string
    size_t dotpos = PathName.rfind ('.');

    if (dotpos == string::npos)
    {
        // no '.' found
        return femm::FileType::Unknown;
    }

    // compare different file extensions and return the appropriate string
    if ( PathName.compare (dotpos, string::npos, ".fem") == 0 )
    {
        return femm::FileType::MagneticsFile;
    }
    else if ( PathName.compare (dotpos, string::npos, ".feh") == 0 )
    {
        return femm::FileType::HeatFlowFile;
    }
    else if ( PathName.compare(dotpos, string::npos, ".fee") == 0)
    {
        return femm::FileType::ElectrostaticsFile;
    }
    else
    {
        return femm::FileType::Unknown;
    }

}

void FMesher::addFileStr (char *s)
{
    probdescstrings.push_back (string (s));
}

bool FMesher::SaveFEMFile(string PathName)
{
    FILE *fp;
    unsigned int i, j;
    int k,t;
    string s;

    // check to see if we are ready to write a datafile;
    if ((fp = fopen(PathName.c_str(),"wt"))==NULL)
    {
        WarnMessage("Couldn't write to specified file.\nPerhaps the file is write-protected?");
        return false;
    }

    // echo the start of the input file
    for (i = 0; i < probdescstrings.size (); i++)
    {
        fprintf(fp,"%s", probdescstrings[i].c_str () );
    }

    // write out node list
    fprintf(fp,"[NumPoints] = %i\n", (int) problem->nodelist.size());
    for(i=0; i<problem->nodelist.size(); i++)
    {
        for(j=0,t=0; j<problem->nodeproplist.size(); j++)
            if(problem->nodeproplist[j]->PointName==problem->nodelist[i]->BoundaryMarkerName) t=j+1;
        fprintf(fp,"%.17g\t%.17g\t%i\t%i",problem->nodelist[i]->x,problem->nodelist[i]->y,t,
                problem->nodelist[i]->InGroup);

        if (problem->filetype == femm::FileType::HeatFlowFile
                || problem->filetype == femm::FileType::ElectrostaticsFile )
        {
            // find and write number of conductor property group
            for (j=0,t=0; j<problem->circproplist.size (); j++)
                if (problem->circproplist[j]->CircName==problem->nodelist[i]->InConductorName) t=j+1;

            fprintf(fp,"\t%i",t);
        }

        fprintf(fp,"\n");
    }

    // write out segment list
    fprintf(fp,"[NumSegments] = %i\n", (int) problem->linelist.size());
    for(i=0; i<problem->linelist.size(); i++)
    {
        for(j=0,t=0; j<problem->lineproplist.size(); j++)
            if(problem->lineproplist[j]->BdryName==problem->linelist[i]->BoundaryMarkerName) t=j+1;

        fprintf(fp,"%i\t%i\t",problem->linelist[i]->n0,problem->linelist[i]->n1);

        if(problem->linelist[i]->MaxSideLength<0)
        {
            fprintf(fp,"-1\t");
        }
        else
        {
            fprintf(fp,"%.17g\t",problem->linelist[i]->MaxSideLength);
        }

        fprintf(fp,"%i\t%i\t%i",t,problem->linelist[i]->Hidden,problem->linelist[i]->InGroup);

        if (problem->filetype == femm::FileType::HeatFlowFile
                || problem->filetype == femm::FileType::ElectrostaticsFile )
        {
            // find and write number of conductor property group
            for(j=0,t=0;j<problem->circproplist.size ();j++)
            {
                if(problem->circproplist[j]->CircName==problem->linelist[i]->InConductorName) t = j + 1;
            }
            fprintf(fp,"\t%i",t);
        }

        fprintf(fp,"\n");
    }

    // write out arc segment list
    fprintf(fp,"[NumArcSegments] = %i\n", (int) problem->arclist.size());
    for(i=0; i<problem->arclist.size(); i++)
    {
        for(j=0,t=0; j<problem->lineproplist.size(); j++)
            if(problem->lineproplist[j]->BdryName==problem->arclist[i]->BoundaryMarkerName) t=j+1;
        fprintf(fp,"%i\t%i\t%.17g\t%.17g\t%i\t%i\t%i",problem->arclist[i]->n0,problem->arclist[i]->n1,
                problem->arclist[i]->ArcLength,problem->arclist[i]->MaxSideLength,t,
                problem->arclist[i]->Hidden,problem->arclist[i]->InGroup);

        if (problem->filetype == femm::FileType::HeatFlowFile
                || problem->filetype == femm::FileType::ElectrostaticsFile )
        {
            // find and write number of conductor property group
            for(j=0,t=0;j<problem->circproplist.size ();j++)
                if(problem->circproplist[j]->CircName==problem->arclist[i]->InConductorName) t=j+1;
            fprintf(fp,"\t%i",t);
        }
        fprintf(fp,"\n");
    }

    // write out list of holes;
    for(i=0,j=0; i<problem->labellist.size(); i++)
    {
        if(problem->labellist[i]->BlockTypeName=="<No Mesh>")
        {
            j++;
        }
    }

    fprintf(fp,"[NumHoles] = %i\n",j);
    for(i=0,k=0; i<problem->labellist.size(); i++)
    {
        if(problem->labellist[i]->BlockTypeName=="<No Mesh>")
        {
            fprintf(fp,"%.17g\t%.17g\t%i\n",problem->labellist[i]->x,problem->labellist[i]->y,
                    problem->labellist[i]->InGroup);
            k++;
        }
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
        WarnMessage("No mesh to display");
        return false;
    }

    rootname = pathname.substr(0,pathname.find_last_of('.'));

    //read meshnodes;
    infile = rootname + ".node";
    if((fp=fopen(infile.c_str(),"rt"))==NULL)
    {
        WarnMessage("No mesh to display");
        return false;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    meshnode.resize(k);
    CNode node;
    for(i=0; i<k; i++)
    {
        fgets(s,1024,fp);
        sscanf(s,"%i\t%lf\t%lf",&j,&node.x,&node.y);
        meshnode[i] = std::make_unique<CNode>(node);
    }
    fclose(fp);

    //read meshlines;
    infile = rootname + ".edge";
    if((fp=fopen(infile.c_str(),"rt"))==NULL)
    {
        WarnMessage("No mesh to display");
        return false;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);
    meshline.resize(k);
    fclose(fp);

    infile = rootname + ".ele";
    if((fp=fopen(infile.c_str(),"rt"))==NULL)
    {
        WarnMessage("No mesh to display");
        return false;
    }
    fgets(s,1024,fp);
    sscanf(s,"%i",&k);

    IntPoint segm;
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
                    meshline[nl++] = std::make_unique<femm::IntPoint>(segm);
                }
                else
                {
                    greymeshline.push_back(std::make_unique<femm::IntPoint>(segm));
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
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<MinAngle>",10)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_minangle);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<Frequency>",11)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_freq);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<Depth>",7)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%lf",&d_depth);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<Coordinates>",13)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_coord);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<LengthUnits>",13)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_length);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<ProblemType>",13)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_type);
//			  q[0] = '\0';
//			}
//
//			if( _strnicmp(q,"<ACSolver>",8)==0)
//			{
//			  v = StripKey(s);
//			  sscanf(v,"%i",&d_solver);
//			  q[0] = '\0';
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


void FMesher::MirrorCopy(double x0, double y0, double x1, double y1, EditMode selector)
{
    assert(selector != EditMode::Invalid);
    CComplex x=x0 + I*y0;
    CComplex p=(x1-x0) + I*(y1-y0);
    if(abs(p)==0)
        return;
    p/=abs(p);

    if (selector==EditMode::EditNodes || selector == EditMode::EditGroup)
    {
        for (const auto &node: problem->nodelist)
        {
            if (node->IsSelected)
            {
                CComplex y (node->x,node->y);
                y = (y-x) / p;
                y = p*y.Conj()+x;
                // create copy
                std::unique_ptr<CNode> newnode = std::make_unique<CNode>(*node);
                // overwrite coordinates in copy
                newnode->x = y.re;
                newnode->y = y.im;
                newnode->IsSelected = false;
                problem->nodelist.push_back(std::move(newnode));
            }
        }
    }
    if (selector == EditMode::EditLines || selector == EditMode::EditGroup)
    {
        for (const auto &line: problem->linelist)
        {
            if (line->IsSelected)
            {
                // copy endpoints
                std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[line->n0]);
                CComplex y0 (n0->x,n0->y);
                y0 = (y0-x) / p;
                y0 = p*y0.Conj()+x;
                n0->x = y0.re;
                n0->y = y0.im;
                n0->IsSelected = false;

                std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[line->n1]);
                CComplex y1 (n1->x,n1->y);
                y1 = (y1-x) / p;
                y1 = p*y1.Conj()+x;
                n1->x = y1.re;
                n1->y = y1.im;
                n1->IsSelected = false;

                // copy line (with identical endpoints)
                std::unique_ptr<CSegment> newline = std::make_unique<CSegment>(*line);
                newline->IsSelected = false;
                // set endpoints
                newline->n0 = (int)problem->nodelist.size();
                problem->nodelist.push_back(std::move(n0));
                newline->n1 = (int)problem->nodelist.size();
                problem->nodelist.push_back(std::move(n1));
                problem->linelist.push_back(std::move(newline));
            }
        }
    }

    if (selector == EditMode::EditLabels || selector == EditMode::EditGroup)
    {
        for (const auto &label: problem->labellist)
        {
            if (label->IsSelected)
            {
                std::unique_ptr<CBlockLabel> newlabel = label->clone();
                CComplex y (label->x,label->y);
                y = (y-x) / p;
                y = p*y.Conj()+x;
                newlabel->x = y.re;
                newlabel->y = y.im;
                newlabel->IsSelected = false;
                // set specific problem parameters:
                if (CMBlockLabel *ptr=dynamic_cast<CMBlockLabel*>(newlabel.get()))
                    ptr->MagDir = (180./PI)*arg(p*conj(exp(I*ptr->MagDir*PI/180.)/p));

                problem->labellist.push_back(std::move(newlabel));
            }
        }
    }
    if (selector == EditMode::EditArcs || selector == EditMode::EditGroup)
    {
        for (const auto &arc: problem->arclist)
        {
            if (arc->IsSelected)
            {
                // copy endpoints
                std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[arc->n0]);
                CComplex y0 (n0->x,n0->y);
                y0 = (y0-x) / p;
                y0 = p*y0.Conj()+x;
                n0->x = y0.re;
                n0->y = y0.im;
                n0->IsSelected = false;

                std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[arc->n1]);
                CComplex y1 (n1->x,n1->y);
                y1 = (y1-x) / p;
                y1 = p*y1.Conj()+x;
                n1->x = y1.re;
                n1->y = y1.im;
                n1->IsSelected = false;

                // copy arc (with identical endpoints)
                std::unique_ptr<CArcSegment> newarc = std::make_unique<CArcSegment>(*arc);
                newarc->IsSelected = false;
                // set endpoints
                newarc->n0 = (int)problem->nodelist.size();
                problem->nodelist.push_back(std::move(n0));
                newarc->n1 = (int)problem->nodelist.size();
                problem->nodelist.push_back(std::move(n1));
                problem->arclist.push_back(std::move(newarc));
            }
        }
    }
    EnforcePSLG();
}

void FMesher::RotateCopy(CComplex c, double dt, int ncopies, femm::EditMode selector)
{
    assert(selector != EditMode::Invalid);
    for(int nc=0; nc<ncopies; nc++)
    {
        // accumulated angle
        double t = ((double) (nc+1))*dt;

        CComplex z = exp(I*t*PI/180);

        if (selector==EditMode::EditNodes || selector == EditMode::EditGroup)
        {
            for (const auto &node: problem->nodelist)
            {
                if (node->IsSelected)
                {
                    CComplex x (node->x, node->y);
                    x=(x-c)*z+c;
                    // create copy
                    std::unique_ptr<CNode> newnode = std::make_unique<CNode>(*node);
                    // overwrite coordinates in copy
                    newnode->x = x.re;
                    newnode->y = x.im;
                    newnode->IsSelected = false;
                    problem->nodelist.push_back(std::move(newnode));
                }
            }
        }

        if (selector == EditMode::EditLines || selector == EditMode::EditGroup)
        {
            for (const auto &line: problem->linelist)
            {
                if (line->IsSelected)
                {
                    // copy endpoints
                    std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[line->n0]);
                    CComplex x0 (n0->x,n0->y);
                    x0 = (x0-c)*z+c;
                    n0->x = x0.re;
                    n0->y = x0.im;
                    n0->IsSelected = false;

                    std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[line->n1]);
                    CComplex x1 (n1->x,n1->y);
                    x1 = (x1-c)*z+c;
                    n1->x = x1.re;
                    n1->y = x1.im;
                    n1->IsSelected = false;

                    // copy line (with identical endpoints)
                    std::unique_ptr<CSegment> newline = std::make_unique<CSegment>(*line);
                    newline->IsSelected = false;
                    // set endpoints
                    newline->n0 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n0));
                    newline->n1 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n1));
                    problem->linelist.push_back(std::move(newline));
                }
            }
        }

        if (selector == EditMode::EditArcs || selector == EditMode::EditGroup)
        {
            for (const auto &arc: problem->arclist)
            {
                if (arc->IsSelected)
                {
                    // copy endpoints
                    std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[arc->n0]);
                    CComplex x0 (n0->x,n0->y);
                    x0 = (x0-c)*z+c;
                    n0->x = x0.re;
                    n0->y = x0.im;
                    n0->IsSelected = false;

                    std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[arc->n1]);
                    CComplex x1 (n1->x,n1->y);
                    x1 = (x1-c)*z+c;
                    n1->x = x1.re;
                    n1->y = x1.im;
                    n1->IsSelected = false;

                    // copy arc (with identical endpoints)
                    std::unique_ptr<CArcSegment> newarc = std::make_unique<CArcSegment>(*arc);
                    newarc->IsSelected = false;
                    // set endpoints
                    newarc->n0 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n0));
                    newarc->n1 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n1));
                    problem->arclist.push_back(std::move(newarc));
                }
            }
        }

        if (selector == EditMode::EditLabels || selector == EditMode::EditGroup)
        {
            for (const auto &label: problem->labellist)
            {
                if (label->IsSelected)
                {
                    std::unique_ptr<CBlockLabel> newlabel = label->clone();
                    CComplex x(label->x,label->y);
                    x = (x-c)*z+c;
                    newlabel->x = x.re;
                    newlabel->y = x.im;
                    newlabel->IsSelected = false;

                    for (const auto &bprop: problem->blockproplist)
                    {
                        CMMaterialProp *prop = dynamic_cast<CMMaterialProp*>(bprop.get());
                        if (prop
                                && prop->BlockName == newlabel->BlockTypeName
                                && prop->H_c != 0)
                        {
                            if (CMBlockLabel *ptr=dynamic_cast<CMBlockLabel*>(newlabel.get()))
                                ptr->MagDir += t;
                        }
                    }

                    problem->labellist.push_back(std::move(newlabel));
                }
            }
        }
    }

    EnforcePSLG();
}

void FMesher::RotateMove(CComplex c, double t, femm::EditMode selector)
{
    assert(selector != EditMode::Invalid);
    bool processNodes = (selector == EditMode::EditNodes);

    const CComplex z = exp(I*t*PI/180);

    if(selector==EditMode::EditLines || selector==EditMode::EditGroup)
    {
        for (const auto &line: problem->linelist)
        {
            if (line->IsSelected)
            {
                problem->nodelist[line->n0]->IsSelected = true;
                problem->nodelist[line->n1]->IsSelected = true;
            }
        }
        processNodes = true;
    }

    if(selector==EditMode::EditArcs || selector==EditMode::EditGroup)
    {
        for (const auto &arc: problem->arclist)
        {
            if (arc->IsSelected)
            {
                problem->nodelist[arc->n0]->IsSelected = true;
                problem->nodelist[arc->n1]->IsSelected = true;
            }
        }
        processNodes = true;
    }

    if(selector==EditMode::EditLabels || selector==EditMode::EditGroup)
    {
        for (auto &label: problem->labellist)
        {
            if (label->IsSelected)
            {
                CComplex x (label->x, label->y);
                x = (x-c)*z+c;
                label->x = x.re;
                label->y = x.im;

                for (const auto &bprop : problem->blockproplist)
                {
                    CMMaterialProp *prop = dynamic_cast<CMMaterialProp*>(bprop.get());
                    if (prop
                            && prop->BlockName == label->BlockTypeName
                            && prop->H_c != 0)
                    {
                        if (CMBlockLabel *ptr=dynamic_cast<CMBlockLabel*>(label.get()))
                            ptr->MagDir += t;
                    }
                }
            }
        }
    }

    if(processNodes)
    {
        for (auto &node : problem->nodelist)
        {
            if (node->IsSelected)
            {
                CComplex x(node->x,node->y);
                x = (x-c)*z+c;
                node->x = x.re;
                node->y = x.im;
            }
        }
    }
    EnforcePSLG();
}

void FMesher::ScaleMove(double bx, double by, double sf, EditMode selector)
{
    assert(selector != EditMode::Invalid);
    bool processNodes = (selector == EditMode::EditNodes);

    if (selector==EditMode::EditLines || selector==EditMode::EditGroup)
    {
        for (const auto& line: problem->linelist)
        {
            if (line->IsSelected)
            {
                problem->nodelist[line->n0]->IsSelected = true;
                problem->nodelist[line->n1]->IsSelected = true;
            }
        }
        processNodes = true;
    }

    if (selector==EditMode::EditArcs || selector==EditMode::EditGroup)
    {
        for (const auto &arc: problem->arclist)
        {
            if (arc->IsSelected)
            {
                problem->nodelist[arc->n0]->IsSelected = true;
                problem->nodelist[arc->n1]->IsSelected = true;
            }
        }
        processNodes = true;
    }

    if (selector==EditMode::EditLabels || selector==EditMode::EditGroup)
    {
        for (auto &label: problem->labellist)
        {
            if (label->IsSelected)
            {
                label->x = bx+sf*(label->x - bx);
                label->y = by+sf*(label->y - by);
                label->MaxArea *= (sf*sf);
            }
        }
    }

    if (processNodes)
    {
        for (auto &node : problem->nodelist)
        {
            if (node->IsSelected)
            {
                node->x = bx+sf*(node->x - bx);
                node->y = by+sf*(node->y - by);
            }
        }
    }
    EnforcePSLG();
}

void FMesher::TranslateCopy(double incx, double incy, int ncopies, femm::EditMode selector)
{
    assert(selector != EditMode::Invalid);
    for(int nc=0; nc<ncopies; nc++)
    {
        // accumulated offsets
        double dx=((double)(nc+1))*incx;
        double dy=((double)(nc+1))*incy;

        if (selector==EditMode::EditNodes || selector == EditMode::EditGroup)
        {
            for (const auto &node: problem->nodelist)
            {
                if (node->IsSelected)
                {
                    // create copy
                    std::unique_ptr<CNode> newnode = std::make_unique<CNode>(*node);
                    // overwrite coordinates in copy
                    newnode->x += dx;
                    newnode->y += dy;
                    newnode->IsSelected = false;
                    problem->nodelist.push_back(std::move(newnode));
                }
            }
        }

        if (selector == EditMode::EditLines || selector == EditMode::EditGroup)
        {
            for (const auto &line: problem->linelist)
            {
                if (line->IsSelected)
                {
                    // copy endpoints
                    std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[line->n0]);
                    n0->x += dx;
                    n0->y += dy;
                    n0->IsSelected = false;

                    std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[line->n1]);
                    n1->x += dx;
                    n1->y += dy;
                    n1->IsSelected = false;

                    // copy line (with identical endpoints)
                    std::unique_ptr<CSegment> newline = std::make_unique<CSegment>(*line);
                    newline->IsSelected = false;
                    // set endpoints
                    newline->n0 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n0));
                    newline->n1 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n1));
                    problem->linelist.push_back(std::move(newline));
                }
            }
        }

        if (selector == EditMode::EditLabels || selector == EditMode::EditGroup)
        {
            for (const auto &label: problem->labellist)
            {
                if (label->IsSelected)
                {
                    std::unique_ptr<CBlockLabel> newlabel = label->clone();
                    newlabel->x += dx;
                    newlabel->y += dy;
                    newlabel->IsSelected = false;

                    problem->labellist.push_back(std::move(newlabel));
                }
            }
        }

        if (selector == EditMode::EditArcs || selector == EditMode::EditGroup)
        {
            for (const auto &arc: problem->arclist)
            {
                if (arc->IsSelected)
                {
                    // copy endpoints
                    std::unique_ptr<CNode> n0 = std::make_unique<CNode>(*problem->nodelist[arc->n0]);
                    n0->x += dx;
                    n0->y += dy;
                    n0->IsSelected = false;

                    std::unique_ptr<CNode> n1 = std::make_unique<CNode>(*problem->nodelist[arc->n1]);
                    n1->x += dx;
                    n1->y += dy;
                    n1->IsSelected = false;

                    // copy arc (with identical endpoints)
                    std::unique_ptr<CArcSegment> newarc = std::make_unique<CArcSegment>(*arc);
                    newarc->IsSelected = false;
                    // set endpoints
                    newarc->n0 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n0));
                    newarc->n1 = (int)problem->nodelist.size();
                    problem->nodelist.push_back(std::move(n1));
                    problem->arclist.push_back(std::move(newarc));
                }
            }
        }
    }

    EnforcePSLG();
}

void FMesher::TranslateMove(double dx, double dy, femm::EditMode selector)
{
    assert(selector != EditMode::Invalid);
    bool processNodes = (selector == EditMode::EditNodes);

    if (selector == EditMode::EditLines || selector == EditMode::EditGroup)
    {
        // select end points of selected lines:
        for (auto &line: problem->linelist)
        {
            if (line->IsSelected)
            {
                problem->nodelist[line->n0]->IsSelected = true;
                problem->nodelist[line->n1]->IsSelected = true;
            }
        }
        // make sure to translate endpoints
        processNodes = true;
    }
    if (selector == EditMode::EditArcs || selector == EditMode::EditGroup)
    {
        // select end points of selected arcs:
        for (auto &arc: problem->arclist)
        {
            if (arc->IsSelected)
            {
                problem->nodelist[arc->n0]->IsSelected = true;
                problem->nodelist[arc->n1]->IsSelected = true;
            }
        }
        // make sure to translate endpoints
        processNodes = true;
    }

    if (selector == EditMode::EditLabels || selector == EditMode::EditGroup)
    {
        for (auto &lbl: problem->labellist)
        {
            if (lbl->IsSelected)
            {
                lbl->x += dx;
                lbl->y += dy;
            }
        }
    }
    if (processNodes)
    {
        for (auto &node: problem->nodelist)
        {
            if (node->IsSelected)
            {
                node->x += dx;
                node->y += dy;
            }
        }
    }
    EnforcePSLG();
}


bool FMesher::AddBlockLabel(double x, double y, double d)
{
    std::unique_ptr<CBlockLabel> pt;
    switch (problem->filetype) {
    case FileType::MagneticsFile:
        pt = std::make_unique<CMBlockLabel>();
        break;
    case FileType::HeatFlowFile:
        pt = std::make_unique<CHBlockLabel>();
        break;
    case FileType::ElectrostaticsFile:
        pt = std::make_unique<CSBlockLabel>();
        break;
    default:
        assert(false && "Unhandled file type");
        break;
    }
    pt->x = x;
    pt->y = y;

    return AddBlockLabel(std::move(pt), d);
}

bool FMesher::AddBlockLabel(std::unique_ptr<CBlockLabel> &&label, double d)
{
    double x = label->x;
    double y = label->y;

    // can't put a block label on top of an existing node...
    for (int i=0; i<(int)problem->nodelist.size(); i++)
        if(problem->nodelist[i]->GetDistance(x,y)<d) return false;

    // can't put a block label on a line, either...
    for (int i=0; i<(int)problem->linelist.size(); i++)
        if(problem->shortestDistanceFromSegment(x,y,i)<d) return false;

    // test to see if ``too close'' to existing node...
    bool exists=false;
    for (int i=0; i<(int)problem->labellist.size(); i++)
        if(problem->labellist[i]->GetDistance(x,y)<d) {
            exists=true;
            break;
        }

    // if all is OK, add point in to the node list...
    if(!exists){
        problem->labellist.push_back(std::move(label));
    }

    return true;
}


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
//			else if(blocklist[i].BlockTypeName!=blocklist[k].BlockTypeName)
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
//		if(blocklist[k].BlockTypeName=="<No Mesh>") zDlg.cursel=1;
//		else for(i=0,zDlg.cursel=0;i<blockproplist.size();i++)
//			if (blockproplist[i].BlockName==blocklist[k].BlockTypeName)
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
//				if (zDlg.cursel==0) blocklist[i].BlockTypeName="<None>";
//				else if(zDlg.cursel==1) blocklist[i].BlockTypeName="<No Mesh>";
//				else blocklist[i].BlockTypeName=blockproplist[zDlg.cursel-2].BlockName;
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
//			else if(nodelist[i].BoundaryMarkerName!=nodelist[k].BoundaryMarkerName)
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
//			if (nodeproplist[i].PointName==nodelist[k].BoundaryMarkerName)
//				zDlg.cursel=i+1;
//	}
//	else zDlg.cursel=0;
//
//	if (zDlg.DoModal()==IDOK){
//		for(i=0;i<nodelist.size();i++)
//		{
//			if(nodelist[i].IsSelected!=0){
//				if (zDlg.cursel==0) nodelist[i].BoundaryMarkerName="<None>";
//				else nodelist[i].BoundaryMarkerName=nodeproplist[zDlg.cursel-1].PointName;
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
//			else if(linelist[i].BoundaryMarkerName!=linelist[k].BoundaryMarkerName)
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
//			if (lineproplist[i].BdryName==linelist[k].BoundaryMarkerName)
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
//				if (zDlg.cursel==0) linelist[i].BoundaryMarkerName="<None>";
//				else linelist[i].BoundaryMarkerName=lineproplist[zDlg.cursel-1].BdryName;
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
//			else if(arclist[i].BoundaryMarkerName!=arclist[k].BoundaryMarkerName)
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
//			if (lineproplist[i].BdryName==arclist[k].BoundaryMarkerName)
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
//				if (zDlg.cursel==0) arclist[i].BoundaryMarkerName="<None>";
//				else arclist[i].BoundaryMarkerName=lineproplist[zDlg.cursel-1].BdryName;
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
