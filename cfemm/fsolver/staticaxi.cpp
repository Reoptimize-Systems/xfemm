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

#include <string>
#include <cstdio>
#include <math.h>
#include <malloc.h>
#include "femmcomplex.h"
#include "mesh.h"
#include "spars.h"
#include "fsolver.h"
#include "lua.h"
//#include "boost/format.hpp"

#ifdef _MSC_VER
  #ifndef SNPRINTF
  #define SNPRINTF _snprintf
  #endif
#else
  #ifndef SNPRINTF
  #define SNPRINTF std::snprintf
  #endif
#endif

extern lua_State *lua;

int FSolver::StaticAxisymmetric(CBigLinProb &L)
{
    int i,j,k,s,w,sdi_iter,sdin;
    double Me[3][3],Mx[3][3],My[3][3],Mn[3][3];
    double l[3],p[3]={0.,0.,0.},q[3]={0.,0.,0.},g[3],be[3],u[3],v[3],res,lastres=0.,dv,vol;
    int n[3] = { 0, 0, 0}; // numbers of nodes for a particular element;
    double a,K,r,t=0.,x,y,B,mu,R,rn[3],a_hat,R_hat=0.,Cduct;
    double c=PI*4.e-05;
    double units[]= {2.54,0.1,1.,100.,0.00254,1.e-04};
    double *V_old=NULL,*V_sdi=NULL,*CircInt1=NULL,*CircInt2=NULL,*CircInt3=NULL;
    int flag,Iter=0,pctr;
    int LinearFlag=true;
    int SDIflag=false;
    res=0;

    femm::CElement *El;
    V_old=(double *) calloc(NumNodes,sizeof(double));

    for(i=0; i<NumBlockLabels; i++) GetFillFactor(i);

    extRo*=units[LengthUnits];
    extRi*=units[LengthUnits];
    extZo*=units[LengthUnits];

    // check to see if any circuits have been defined and process them;
    if (NumCircProps>0)
    {
        CircInt1=(double *)calloc(NumCircProps,sizeof(double));
        CircInt2=(double *)calloc(NumCircProps,sizeof(double));
        CircInt3=(double *)calloc(NumCircProps,sizeof(double));
        for(i=0; i<NumEls; i++)
        {
            if(meshele[i].lbl>=0)
                if(labellist[meshele[i].lbl].InCircuit!=-1)
                {
                    El=&meshele[i];

                    // get element area;
                    for(k=0; k<3; k++) n[k]=El->p[k];
                    p[0]=meshnode[n[1]].y - meshnode[n[2]].y;
                    p[1]=meshnode[n[2]].y - meshnode[n[0]].y;
                    p[2]=meshnode[n[0]].y - meshnode[n[1]].y;
                    q[0]=meshnode[n[2]].x - meshnode[n[1]].x;
                    q[1]=meshnode[n[0]].x - meshnode[n[2]].x;
                    q[2]=meshnode[n[1]].x - meshnode[n[0]].x;
                    a=(p[0]*q[1]-p[1]*q[0])/2.;
                    r=(meshnode[n[0]].x+meshnode[n[1]].x+meshnode[n[2]].x)/3.;

                    // if coils are wound, they act like they have
                    // a zero "bulk" conductivity...
                    Cduct=blockproplist[El->blk].Cduct;
                    if (labellist[El->lbl].bIsWound) Cduct=0;

                    // evaluate integrals;
                    CircInt1[labellist[El->lbl].InCircuit]+=a;

                    CircInt2[labellist[El->lbl].InCircuit]+=
                        100.*a*Cduct/r;

                    CircInt3[labellist[El->lbl].InCircuit]+=
                        blockproplist[El->blk].Jr*a*100.;
                }
        }

        for(i=0; i<NumCircProps; i++)
        {
            if (circproplist[i].CircType==0)
            {
                if(CircInt2[i]==0)
                {
                    circproplist[i].Case=1;
                    if(CircInt1[i]==0.) circproplist[i].J=0.;
                    else circproplist[i].J=0.01*(circproplist[i].Amps_re -
                                                     CircInt3[i])/CircInt1[i];
                }
                else
                {
                    circproplist[i].Case=0;
                    circproplist[i].dV=-0.01*(circproplist[i].Amps_re -
                                              CircInt3[i])/CircInt2[i];
                }
            }
            else
            {
                circproplist[i].Case=0;
                circproplist[i].dV=circproplist[i].dVolts_re;
            }
        }
    }

    // check to see if there are any SDI boundaries...
    // lineproplist[ meshele[i].e[j] ].BdryFormat==0
    for(i=0; i<NumLineProps; i++)
        if(lineproplist[i].BdryFormat==3) SDIflag=true;

    if(SDIflag==true)
    {
        // there is an SDI boundary defined; check to see if it is in use
        SDIflag=false;
        for(i=0; i<NumEls; i++)
            for(j=0; j<3; j++)
                if (lineproplist[meshele[i].e[j]].BdryFormat==3)
                {
                    SDIflag=true;
                    printf("Problem has SDI boundaries\n");
                    i=NumEls;
                    j=3;
                }
    }

    if (SDIflag==true)
    {
        V_sdi=(double *) calloc(NumNodes,sizeof(double));
        sdin=2;
    }
    else sdin=1;

    // first, need to define permeability in each block.  In nonlinear
    // case, this is sort of a hassle.  Linear permeability is simply
    // copied from the associated block definition, but nonlinear
    // permeability must be updated from iteration to iteration...

    // build element matrices using the matrices derived in Allaire's book.

    do
    {
        for(sdi_iter=0; sdi_iter<sdin; sdi_iter++)
        {
//	TheView->SetDlgItemText(IDC_FRAME1,"Matrix Construction");
//	TheView->m_prog1.SetPos(0);
            printf("Matrix Construction\n");
            pctr=0;

            if(Iter>0) L.Wipe();

            for(i=0; i<NumEls; i++)
            {

                // update ``building matrix'' progress bar...
                j=(i*20)/NumEls+1;
                if(j>pctr)
                {
                    j=pctr*5;
                    if (j>100) j=100;
//			TheView->m_prog1.SetPos(j);
                    pctr++;
                }

                // zero out Me, be;
                for(j=0; j<3; j++)
                {
                    for(k=0; k<3; k++)
                    {
                        Me[j][k]=0.;
                        Mx[j][k]=0;
                        My[j][k]=0;
                        Mn[j][k]=0;
                    }
                    be[j]=0.;
                }

                // Determine shape parameters.
                // l == element side lengths;
                // p corresponds to the `b' parameter in Allaire
                // q corresponds to the `c' parameter in Allaire
                El=&meshele[i];

                for(k=0; k<3; k++)
                {
                    n[k]=El->p[k];
                    rn[k]=meshnode[n[k]].x;
                }

                p[0]=meshnode[n[1]].y - meshnode[n[2]].y;
                p[1]=meshnode[n[2]].y - meshnode[n[0]].y;
                p[2]=meshnode[n[0]].y - meshnode[n[1]].y;
                q[0]=meshnode[n[2]].x - meshnode[n[1]].x;
                q[1]=meshnode[n[0]].x - meshnode[n[2]].x;
                q[2]=meshnode[n[1]].x - meshnode[n[0]].x;
                g[0]=(meshnode[n[2]].x + meshnode[n[1]].x)/2.;
                g[1]=(meshnode[n[0]].x + meshnode[n[2]].x)/2.;
                g[2]=(meshnode[n[1]].x + meshnode[n[0]].x)/2.;

                for(j=0,k=1; j<3; k++,j++)
                {
                    if (k==3) k=0;
                    l[j]=sqrt( pow(meshnode[n[k]].x-meshnode[n[j]].x,2.) +
                               pow(meshnode[n[k]].y-meshnode[n[j]].y,2.) );
                }

                a=(p[0]*q[1]-p[1]*q[0])/2.;
                R=(meshnode[n[0]].x+meshnode[n[1]].x+meshnode[n[2]].x)/3.;

                for(j=0,a_hat=0; j<3; j++) a_hat+=(rn[j]*rn[j]*p[j]/(4.*R));
                vol=2.*R*a_hat;

                for(j=0,flag=0; j<3; j++) if(rn[j]<1.e-06) flag++;
                switch(flag)
                {
                case 2:
                    R_hat=R;

                    break;

                case 1:

                    if(rn[0]<1.e-06)
                    {
                        if (fabs(rn[1]-rn[2])<1.e-06) R_hat=rn[2]/2.;
                        else R_hat=(rn[1] - rn[2])/(2.*log(rn[1]) - 2.*log(rn[2]));
                    }
                    if(rn[1]<1.e-06)
                    {
                        if (fabs(rn[2]-rn[0])<1.e-06) R_hat=rn[0]/2.;
                        else R_hat=(rn[2] - rn[0])/(2.*log(rn[2]) - 2.*log(rn[0]));
                    }
                    if(rn[2]<1.e-06)
                    {
                        if (fabs(rn[0]-rn[1])<1.e-06) R_hat=rn[1]/2.;
                        else R_hat=(rn[0] - rn[1])/(2.*log(rn[0]) - 2.*log(rn[1]));
                    }

                    break;

                default:

                    if (fabs(q[0])<1.e-06)
                        R_hat=(q[1]*q[1])/(2.*(-q[1] + rn[0]*log(rn[0]/rn[2])));
                    else if (fabs(q[1])<1.e-06)
                        R_hat=(q[2]*q[2])/(2.*(-q[2] + rn[1]*log(rn[1]/rn[0])));
                    else if (fabs(q[2])<1.e-06)
                        R_hat=(q[0]*q[0])/(2.*(-q[0] + rn[2]*log(rn[2]/rn[1])));
                    else
                        R_hat=-(q[0]*q[1]*q[2])/
                              (2.*(q[0]*rn[0]*log(rn[0]) +
                                   q[1]*rn[1]*log(rn[1]) +
                                   q[2]*rn[2]*log(rn[2])));

                    break;
                }

                // Mr Contribution
                // Derived from flux formulation with c0 + c1 r^2 + c2 z
                // interpolation in the element.
                K=(-1./(2.*a_hat*R));
                for(j=0; j<3; j++)
                    for(k=j; k<3; k++)
                        Mx[j][k] += K*p[j]*rn[j]*p[k]*rn[k];

                // need this loop to avoid singularities.  This just puts something
                // on the main diagonal of nodes that are on the r=0 line.
                // The program later sets these nodes to zero, but it's good to
                // for scaling reasons to grab entries from the neighboring diagonals
                // rather than just setting these entries to 1 or something....
                for(j=0; j<3; j++)
                    if (rn[j]<1.e-06) Mx[j][j]+=Mx[0][0]+Mx[1][1]+Mx[2][2];

                // Mz Contribution;
                // Derived from flux formulation with c0 + c1 r^2 + c2 z
                // interpolation in the element.
                K=(-1./(2.*a_hat*R_hat));
                for(j=0; j<3; j++)
                    for(k=j; k<3; k++)
                        My[j][k] += K*(q[j]*rn[j])*(q[k]*rn[k])*
                                    (g[j]/R)*(g[k]/R);

                // Fill out rest of entries of Mx and My;
                Mx[1][0]=Mx[0][1];
                Mx[2][0]=Mx[0][2];
                Mx[2][1]=Mx[1][2];
                My[1][0]=My[0][1];
                My[2][0]=My[0][2];
                My[2][1]=My[1][2];

                // contributions to Me, be from derivative boundary conditions;
                for(j=0; j<3; j++)
                {
                    if (El->e[j] >= 0)
                        if (lineproplist[El->e[j]].BdryFormat==2)
                        {
                            // conversion factor is 10^(-4) (I think...)
                            k=j+1;
                            if(k==3) k=0;
                            r=(meshnode[n[j]].x+meshnode[n[k]].x)/2.;
                            K=-0.0001*c*2.*r*lineproplist[ El->e[j] ].c0.re*l[j]/6.;
                            k=j+1;
                            if(k==3) k=0;
                            Me[j][j]+=K*2.;
                            Me[k][k]+=K*2.;
                            Me[j][k]+=K;
                            Me[k][j]+=K;

                            K=(lineproplist[ El->e[j] ].c1.re*l[j]/2.)*0.0001*2*r;
                            be[j]+=K;
                            be[k]+=K;
                        }
                }


                // contribution to be from current density in the block
                for(j=0; j<3; j++)
                {
                    if(labellist[El->lbl].InCircuit>=0)
                    {
                        k=labellist[El->lbl].InCircuit;
                        if(circproplist[k].Case==1) t=circproplist[k].J.Re();
                        if(circproplist[k].Case==0)
                            t=-100.*circproplist[k].dV.Re()*blockproplist[El->blk].Cduct/R;
                    }
                    else t=0;
                    K=-2.*R*(blockproplist[El->blk].Jr+t)*a/3.;
                    be[j]+=K;
                }

                // contribution to be from magnetization in the block;
                t=labellist[El->lbl].MagDir;
                // create the formatter object in case of a lua defined mag direction
//                boost::format fmatter("r=%.17g\nz=%.17g\nx=r\ny=z\ntheta=%.17g\nR=%.17g\nreturn %s");
                if (!labellist[El->lbl].MagDirFctn.empty()) // functional magnetization direction
                {
                    char magbuff[4096];
                    std::string str;
                    CComplex X;
                    int top1,top2;
                    for (j=0,X=0; j<3; j++) X+=(meshnode[n[j]].x + I*meshnode[n[j]].y);
                    X=X/units[LengthUnits]/3.;
                    // generate the string using boost::format
//                    fmatter % (X.re) % (X.im) % (arg(X)*180/PI) % (abs(X)) % (labellist[El->lbl].MagDirFctn);
                    // get the created string
//                    str = fmatter.str();
                    SNPRINTF(magbuff, sizeof magbuff, "r=%.17g\nz=%.17g\nx=r\ny=z\ntheta=%.17g\nR=%.17g\nreturn %s",
                                 (X.re) , (X.im) , (arg(X)*180/PI) , (abs(X)) , (labellist[El->lbl].MagDirFctn.c_str()));
                    str = magbuff;
                    top1=lua_gettop(lua);
                    if(lua_dostring(lua,str.c_str())!=0)
                    {
                        //MsgBox("Lua error evaluating \"%s\"",labellist[El->lbl].MagDirFctn);
                        printf("Lua error evaluating \"%s\"",labellist[El->lbl].MagDirFctn.c_str());
                        //exit(7);
                        return -7;
                    }
                    top2=lua_gettop(lua);
                    if (top2!=top1)
                    {
                        str=lua_tostring(lua,-1);
                        if (str.length()==0)
                        {
//					MsgBox( "\"%s\" does not evaluate to a numerical value",
//						labellist[El->lbl].MagDirFctn);
                            printf("\"%s\" does not evaluate to a numerical value",
                                   labellist[El->lbl].MagDirFctn.c_str());
//					exit(7);
                            return -7;
                        }
                        else t=Re(lua_tonumber(lua,-1));
                    }
                }
                for(j=0; j<3; j++)
                {
                    k=j+1;
                    if(k==3) k=0;
                    r=(meshnode[n[j]].x+meshnode[n[k]].x)/2.;
                    // need to scale so that everything is in proper units...
                    // conversion is 0.0001
                    K=-0.0001*r*blockproplist[El->blk].H_c*(
                          cos(t*PI/180.)*(meshnode[n[k]].x-meshnode[n[j]].x) +
                          sin(t*PI/180.)*(meshnode[n[k]].y-meshnode[n[j]].y) );
                    be[j]+=K;
                    be[k]+=K;
                }

                // update permeability for the element;
                if (Iter==0)
                {
                    k=meshele[i].blk;

                    if (blockproplist[k].BHpoints != 0) LinearFlag=false;

                    if (blockproplist[k].LamType==0)
                    {
                        mu=blockproplist[k].LamFill;
                        meshele[i].mu1=blockproplist[k].mu_x*mu;
                        meshele[i].mu2=blockproplist[k].mu_y*mu;
                    }
                    if (blockproplist[k].LamType==1)
                    {
                        mu=blockproplist[k].LamFill;
                        K=blockproplist[k].mu_x;
                        meshele[i].mu1=K*mu + (1.-mu);
                        meshele[i].mu2=K/(mu + K*(1.-mu));
                    }
                    if (blockproplist[k].LamType==2)
                    {
                        mu=blockproplist[k].LamFill;
                        K=blockproplist[k].mu_y;
                        meshele[i].mu1=K*mu + (1.-mu);
                        meshele[i].mu2=K/(mu + K*(1.-mu));
                    }
                    if (blockproplist[k].LamType>2)
                    {
                        meshele[i].mu1=1;
                        meshele[i].mu2=1;
                    }
                }
                else
                {
                    k=meshele[i].blk;

                    if ((blockproplist[k].LamType==0) &&
                            (meshele[i].mu1==meshele[i].mu2)
                            &&(blockproplist[k].BHpoints>0))
                    {
                        //	Derive B directly from energy;
                        v[0]=0;
                        v[1]=0;
                        v[2]=0;
                        for(j=0; j<3; j++)
                            for(w=0; w<3; w++)
                                v[j]+=(Mx[j][w]+My[j][w])*L.V[n[w]];
                        for(j=0,dv=0; j<3; j++) dv+=L.V[n[j]]*v[j];
                        dv*=(10000.*c*c/vol);
                        B=sqrt(fabs(dv));

                        // find out new mu from saturation curve;
                        blockproplist[k].GetBHProps(B,mu,dv);
                        mu=1./(muo*mu);
                        meshele[i].mu1=mu;
                        meshele[i].mu2=mu;
                        for(j=0; j<3; j++)
                        {
                            for(w=0,v[j]=0; w<3; w++)
                                v[j]+=(Mx[j][w]+My[j][w])*L.V[n[w]];
                        }

                        K=-200.*c*c*c*dv/vol;
                        for(j=0; j<3; j++)
                            for(w=0; w<3; w++)
                                Mn[j][w]=K*v[j]*v[w];
                    }

                    if ((blockproplist[k].LamType==1) && (blockproplist[k].BHpoints>0))
                    {

                        //	Derive B directly from energy;
                        t=blockproplist[k].LamFill;
                        v[0]=0;
                        v[1]=0;
                        v[2]=0;
                        for(j=0; j<3; j++)
                            for(w=0; w<3; w++)
                                v[j]+=(Mx[j][w]+My[j][w]/(t*t))*L.V[n[w]];
                        for(j=0,dv=0; j<3; j++) dv+=L.V[n[j]]*v[j];
                        dv*=(10000.*c*c/vol);
                        B=sqrt(fabs(dv));

                        // Evaluate BH curve
                        blockproplist[k].GetBHProps(B,mu,dv);
                        mu=1./(muo*mu);
                        meshele[i].mu1=mu*t;
                        meshele[i].mu2=mu/(t+mu*(1.-t));
                        for(j=0; j<3; j++)
                        {
                            for(w=0,v[j]=0,u[j]=0; w<3; w++)
                            {
                                v[j]+=(My[j][w]/t+Mx[j][w])*L.V[n[w]];
                                u[j]+=(My[j][w]/t + t*Mx[j][w])*L.V[n[w]];
                            }
                        }
                        K=-100.*c*c*c*dv/(vol);
                        for(j=0; j<3; j++)
                            for(w=0; w<3; w++)
                                Mn[j][w]=K*(v[j]*u[w]+v[w]*u[j]);
                    }
                    if ((blockproplist[k].LamType==2) && (blockproplist[k].BHpoints>0))
                    {

                        //	Derive B directly from energy;
                        t=blockproplist[k].LamFill;
                        v[0]=0;
                        v[1]=0;
                        v[2]=0;
                        for(j=0; j<3; j++)
                            for(w=0; w<3; w++)
                                v[j]+=(Mx[j][w]/(t*t)+My[j][w])*L.V[n[w]];
                        for(j=0,dv=0; j<3; j++) dv+=L.V[n[j]]*v[j];
                        dv*=(10000.*c*c/vol);
                        B=sqrt(fabs(dv));

                        // Evaluate BH curve
                        blockproplist[k].GetBHProps(B,mu,dv);
                        mu=1./(muo*mu);
                        meshele[i].mu2=mu*t;
                        meshele[i].mu1=mu/(t+mu*(1.-t));

                        for(j=0; j<3; j++)
                        {
                            for(w=0,v[j]=0,u[j]=0; w<3; w++)
                            {
                                v[j]+=(Mx[j][w]/t + My[j][w])*L.V[n[w]];
                                u[j]+=(Mx[j][w]/t + t*My[j][w])*L.V[n[w]];
                            }
                        }
                        K=-100.*c*c*c*dv/(vol);
                        for(j=0; j<3; j++)
                            for(w=0; w<3; w++)
                                Mn[j][w]=K*(v[j]*u[w]+v[w]*u[j]);

                    }
                }

                // "Warp" the permeability of this element if part of
                // the conformally mapped external region
                if((labellist[meshele[i].lbl].IsExternal) && (Iter==0) && (sdi_iter==0))
                {
                    double Z=(meshnode[n[0]].y+meshnode[n[1]].y+meshnode[n[2]].y)/3. - extZo;
                    double kludge=(R*R+Z*Z)*extRi/(extRo*extRo*extRo);
                    meshele[i].mu1/=kludge;
                    meshele[i].mu2/=kludge;
                }

                // combine block matrices into global matrices;
                for(j=0; j<3; j++)
                    for(k=0; k<3; k++)
                    {
                        Me[j][k]+= (Mx[j][k]/Re(El->mu2) + My[j][k]/Re(El->mu1) + Mn[j][k]);
                        be[j]+=Mn[j][k]*L.V[n[k]];
                    }

                for (j=0; j<3; j++)
                {
                    for (k=j; k<3; k++)
                        L.Put(L.Get(n[j],n[k])-Me[j][k],n[j],n[k]);
                    L.b[n[j]]-=be[j];
                }
            }

            // add in contribution from point currents;
            for(i=0; i<NumNodes; i++)
                if(meshnode[i].BoundaryMarker>=0)
                {
                    r=meshnode[i].x;
                    L.b[i]+=(0.01*nodeproplist[meshnode[i].BoundaryMarker].Jr*2.*r);
                }

            // apply fixed boundary conditions at points;
            for(i=0; i<NumNodes; i++)
            {
                if(fabs(meshnode[i].x)<(units[LengthUnits]*1.e-06)) L.SetValue(i,0.);
                else if(meshnode[i].BoundaryMarker >=0)
                    if((nodeproplist[meshnode[i].BoundaryMarker].Jr==0) &&
                            (nodeproplist[meshnode[i].BoundaryMarker].Ji==0) && (sdi_iter==0))
                        L.SetValue(i,nodeproplist[meshnode[i].BoundaryMarker].Ar/c);
            }

            // apply fixed boundary conditions along segments;
            for(i=0; i<NumEls; i++)
                for(j=0; j<3; j++)
                {
                    k=j+1;
                    if(k==3) k=0;
                    if(meshele[i].e[j]>=0)
                        if(lineproplist[ meshele[i].e[j] ].BdryFormat==0)
                        {
                            if(Coords==0)
                            {
                                // first point on the side;
                                x=meshnode[meshele[i].p[j]].x;
                                y=meshnode[meshele[i].p[j]].y;
                                x/=units[LengthUnits];
                                y/=units[LengthUnits];
                                s=meshele[i].e[j];
                                a=lineproplist[s].A0 + x*lineproplist[s].A1 +
                                  y*lineproplist[s].A2;
                                // just take ``real'' component.
                                a*=cos(lineproplist[s].phi*DEG);
                                if(x!=0) L.SetValue(meshele[i].p[j],a/c);

                                // second point on the side;
                                x=meshnode[meshele[i].p[k]].x;
                                y=meshnode[meshele[i].p[k]].y;
                                x/=units[LengthUnits];
                                y/=units[LengthUnits];
                                s=meshele[i].e[j];
                                a=lineproplist[s].A0 + x*lineproplist[s].A1 +
                                  y*lineproplist[s].A2;
                                // just take``real'' component.
                                a*=cos(lineproplist[s].phi*DEG);
                                if (x!=0) L.SetValue(meshele[i].p[k],a/c);
                            }
                            else
                            {
                                // first point on the side;
                                x=meshnode[meshele[i].p[j]].x;
                                y=meshnode[meshele[i].p[j]].y;
                                r=sqrt(x*x+y*y);
                                if((x==0)&&(y==0)) t=0;
                                else t=atan2(y,x)/DEG;
                                r/=units[LengthUnits];
                                s=meshele[i].e[j];
                                a=lineproplist[s].A0 + r*lineproplist[s].A1 +
                                  t*lineproplist[s].A2;
                                a*=cos(lineproplist[s].phi*DEG); // just take ``real'' component.
                                if (x!=0) L.SetValue(meshele[i].p[j],a/c);

                                // second point on the side;
                                x=meshnode[meshele[i].p[k]].x;
                                y=meshnode[meshele[i].p[k]].y;
                                r=sqrt(x*x+y*y);
                                if((x==0) && (y==0)) t=0;
                                else t=atan2(y,x)/DEG;
                                r/=units[LengthUnits];
                                s=meshele[i].e[j];
                                a=lineproplist[s].A0 + r*lineproplist[s].A1 +
                                  t*lineproplist[s].A2;
                                a*=cos(lineproplist[s].phi*DEG); // just take ``real'' component.
                                if (x!=0) L.SetValue(meshele[i].p[k],a/c);
                            }

                        }
                }

            if ((SDIflag==true) && (sdi_iter==1)) for(i=0; i<NumEls; i++)
                    for(j=0; j<3; j++)
                    {
                        k=j+1;
                        if(k==3) k=0;
                        if(meshele[i].e[j]>=0)
                            if(lineproplist[ meshele[i].e[j] ].BdryFormat==3)
                            {
                                L.SetValue(meshele[i].p[j],0.);
                                L.SetValue(meshele[i].p[k],0.);
                            }
                    }

            // Apply any periodicity/antiperiodicity boundary conditions that we have
            for(k=0; k<NumPBCs; k++)
            {
                if (pbclist[k].t==0) L.Periodicity(pbclist[k].x,pbclist[k].y);
                if (pbclist[k].t==1) L.AntiPeriodicity(pbclist[k].x,pbclist[k].y);
            }

            // solve the problem;
            if (SDIflag==false) for(j=0; j<NumNodes; j++) V_old[j]=L.V[j];
            else
            {
                if(sdi_iter==0)
                    for(j=0; j<NumNodes; j++) V_sdi[j]=L.V[j];
                else
                    for(j=0; j<NumNodes; j++)
                    {
                        V_old[j]=V_sdi[j];
                        V_sdi[j]=L.V[j];
                    }
            }

            if (L.PCGSolve(Iter+sdi_iter)==false) return false;

            if(sdi_iter==1)
                for(j=0; j<NumNodes; j++) L.V[j]=(V_sdi[j]+L.V[j])/2.;

        } // end of SDI iteration loop;
        if (LinearFlag==false)
        {

            for(j=0,x=0,y=0; j<NumNodes; j++)
            {
                x+=(L.V[j]-V_old[j])*(L.V[j]-V_old[j]);
                y+=(L.V[j]*L.V[j]);
            }

            if (y==0) LinearFlag=true;
            else
            {
                lastres=res;
                res=sqrt(x/y);
            }


            // relaxation if we need it
            if(Iter>5)
            {
                if ((res>lastres) && (Relax>0.125)) Relax/=2.;
                else Relax+= 0.1 * (1. - Relax);

                for(j=0; j<NumNodes; j++) L.V[j]=Relax*L.V[j]+(1.0-Relax)*V_old[j];
            }


            // report some results
            char outstr[256];
            sprintf(outstr,"Newton Iteration(%i) Relax=%.4g\n",Iter,Relax);
//        TheView->SetDlgItemText(IDC_FRAME2,outstr);
            printf("%s", outstr);
            j=(int)  (100.*log10(res)/(log10(Precision)+2.));
            if (j>100) j=100;
//        TheView->m_prog2.SetPos(j);
        }

        // nonlinear iteration has to have a looser tolerance
        // than the linear solver--otherwise, things can't ever
        // converge.  Arbitrarily choose 100*tolerance.
        if((res<100.*Precision) && Iter>0) LinearFlag=true;

        Iter++;

    }
    while(LinearFlag==false);

    // convert answer back to Webers for plotting purposes.
    for (i=0; i<NumNodes; i++)
    {
        L.b[i]=L.V[i]*c;
        L.b[i]*=(meshnode[i].x*0.01*2*PI);
    }

    free(V_old);
    if (SDIflag==true) free(V_sdi);
    if(NumCircProps>0)
    {
        free(CircInt1);
        free(CircInt2);
        free(CircInt3);
    }

    return true;
}
