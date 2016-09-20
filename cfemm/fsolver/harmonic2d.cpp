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

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<algorithm>
#include <malloc.h>
#include "femmcomplex.h"
#include "mesh.h"
#include "spars.h"
#include "fsolver.h"

// #define NEWTON

int FSolver::Harmonic2D(CBigComplexLinProb &L)
{
    int i,j,k,ww,s,sdin,sdi_iter,pctr;
    CComplex Mx[3][3],My[3][3];
    CComplex Me[3][3],be[3];		// element matrices;
    double l[3],p[3],q[3];		// element shape parameters;
    int n[3];					// numbers of nodes for a particular element;
    double a,r,t,x,y,B,w,res,lastres,ds,Cduct;
    CComplex K,mu,dv,B1,B2,v[3],mu1,mu2,lag,halflag,deg45,Jv; //u[3],
    CComplex **Mu,*V_sdi,*V_old;
    double c=PI*4.e-05;
    double units[]= {2.54,0.1,1.,100.,0.00254,1.e-04};
    femm::CElement *El;
    int Iter=0;
    int SDIflag=false;
    int LinearFlag=true;

    res=0;

// #ifndef NEWTON
    CComplex murel,muinc;
// #else;
    CComplex Mnh[3][3];
    CComplex Mna[3][3];
    CComplex Mns[3][3];
// #endif

    CComplex Mn[3][3];

    deg45=1+I;
    w=Frequency*2.*PI;

    CComplex *CircInt1,*CircInt2,*CircInt3;



    // Can't handle LamType==1 or LamType==2 in AC problems.
    // Detect if this is being attempted.
    for(i=0; i<NumEls; i++)
    {
        if( (blockproplist[meshele[i].blk].LamType==1) ||
                (blockproplist[meshele[i].blk].LamType==2) )
        {
            WarnMessage("On-edge lamination not supported in AC analyses");
            return false;
        }
    }

    // Go through and evaluate permeability for regions subject to prox effects
    for(i=0; i<NumBlockLabels; i++) GetFillFactor(i);

    V_old=(CComplex *) calloc(NumNodes+NumCircProps,sizeof(CComplex));

    // check to see if any circuits have been defined and process them;
    if (NumCircProps>0)
    {
        CircInt1=(CComplex *)calloc(NumCircProps,sizeof(CComplex));
        CircInt2=(CComplex *)calloc(NumCircProps,sizeof(CComplex));
        CircInt3=(CComplex *)calloc(NumCircProps,sizeof(CComplex));
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
                    //	r=(meshnode[n[0]].x+meshnode[n[1]].x+meshnode[n[2]].x)/3.;

                    // if coils are wound, they act like they have
                    // a zero "bulk" conductivity...
                    Cduct=blockproplist[El->blk].Cduct;
                    if (labellist[El->lbl].bIsWound) Cduct=0;

                    // evaluate integrals;

                    // total cross-section of circuit;
                    CircInt1[labellist[El->lbl].InCircuit]+=a;

                    // integral of conductivity over the circuit;
                    CircInt2[labellist[El->lbl].InCircuit]+=a*Cduct;

                    // integral of applied J over current;
                    CircInt3[labellist[El->lbl].InCircuit]+=
                        (blockproplist[El->blk].Jr+I*blockproplist[El->blk].Ji)*a*100.;
                }
        }

        for(i=0; i<NumCircProps; i++)
        {
            // Case 0 :: a priori known voltage gradient is applied to the region;
            // Case 1 :: flat current density is applied to the region;
            // Case 2 :: voltage gradient applied to the region, but we gotta solve for it...

            if (circproplist[i].CircType==0) // specified current
            {
                if(CircInt2[i]==0)  //circuit composed of zero cond. materials
                {
                    circproplist[i].Case=1;
                    if (CircInt1[i]==0.) circproplist[i].J=0.;
                    else circproplist[i].J=0.01*(
                                                   (circproplist[i].Amps_re+I*circproplist[i].Amps_im) -
                                                   CircInt3[i])/CircInt1[i];
                }
                else
                {
                    circproplist[i].Case=2; // need to include an extra
                    // entry in matrix to solve for
                    // voltage grad in the circuit
                }
            }
            else
            {
                // case where voltage gradient is specified a priori...
                circproplist[i].Case=0;
                circproplist[i].dV=circproplist[i].dVolts_re +
                                   I*circproplist[i].dVolts_im;
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
        V_sdi=(CComplex *) calloc(NumNodes+NumCircProps,sizeof(CComplex));
        sdin=2;
    }
    else sdin=1;

    // compute effective permeability for each block type;
    Mu=(CComplex **)calloc(NumBlockProps,sizeof(CComplex *));
    for(i=0; i<NumBlockProps; i++) Mu[i]=(CComplex *)calloc(2,sizeof(CComplex));

    for(k=0; k<NumBlockProps; k++)
    {

        if (blockproplist[k].LamType==0)
        {
            Mu[k][0]=blockproplist[k].mu_x*exp(-I*blockproplist[k].Theta_hx*DEG);
            Mu[k][1]=blockproplist[k].mu_y*exp(-I*blockproplist[k].Theta_hy*DEG);

            if(blockproplist[k].Lam_d!=0)
            {
                if (blockproplist[k].Cduct != 0)
                {
                    halflag=exp(-I*blockproplist[k].Theta_hx*DEG/2.);
                    ds=sqrt(2./(0.4*PI*w*blockproplist[k].Cduct*blockproplist[k].mu_x));
                    K=halflag*deg45*blockproplist[k].Lam_d*0.001/(2.*ds);
                    Mu[k][0]=((Mu[k][0]*tanh(K))/K)*blockproplist[k].LamFill +
                             (1.- blockproplist[k].LamFill);

                    halflag=exp(-I*blockproplist[k].Theta_hy*DEG/2.);
                    ds=sqrt(2./(0.4*PI*w*blockproplist[k].Cduct*blockproplist[k].mu_y));
                    K=halflag*deg45*blockproplist[k].Lam_d*0.001/(2.*ds);
                    Mu[k][1]=((Mu[k][1]*tanh(K))/K)*blockproplist[k].LamFill +
                             (1. - blockproplist[k].LamFill);
                }
                else
                {
                    Mu[k][0]=Mu[k][0]*blockproplist[k].LamFill +
                             (1.- blockproplist[k].LamFill);
                    Mu[k][1]=Mu[k][1]*blockproplist[k].LamFill +
                             (1. - blockproplist[k].LamFill);
                }
            }
        }
        else
        {
            Mu[k][0]=1;
            Mu[k][1]=1;
        }

    }

    do
    {
        for(sdi_iter=0; sdi_iter<sdin; sdi_iter++)
        {
//		TheView->SetDlgItemText(IDC_FRAME1,"Matrix Construction");
//		TheView->m_prog1.SetPos(0);
            printf("Matrix Construction\n");
            pctr=0;

            if(Iter>0) L.Wipe();

            // build element matrices using the matrices derived in Allaire's book.
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
                        Me[j][k]=0;
                        Mx[j][k]=0;
                        My[j][k]=0;
//#ifdef NEWTON
                        if (ACSolver==1)
                        {
                            Mnh[j][k]=0;
                            Mna[j][k]=0;
                            Mns[j][k]=0;
                        }
//#endif
                        Mn[j][k]=0;
                    }
                    be[j]=0;
                }

                // Determine shape parameters.
                // l == element side lengths;
                // p corresponds to the `b' parameter in Allaire
                // q corresponds to the `c' parameter in Allaire
                El=&meshele[i];

                for(k=0; k<3; k++) n[k]=El->p[k];
                p[0]=meshnode[n[1]].y - meshnode[n[2]].y;
                p[1]=meshnode[n[2]].y - meshnode[n[0]].y;
                p[2]=meshnode[n[0]].y - meshnode[n[1]].y;
                q[0]=meshnode[n[2]].x - meshnode[n[1]].x;
                q[1]=meshnode[n[0]].x - meshnode[n[2]].x;
                q[2]=meshnode[n[1]].x - meshnode[n[0]].x;
                for(j=0,k=1; j<3; k++,j++)
                {
                    if (k==3) k=0;
                    l[j]=sqrt( pow(meshnode[n[k]].x-meshnode[n[j]].x,2.) +
                               pow(meshnode[n[k]].y-meshnode[n[j]].y,2.) );
                }
                a=(p[0]*q[1]-p[1]*q[0])/2.;

                // x-contribution;
                K = (-1./(4.*a));
                for(j=0; j<3; j++)
                    for(k=j; k<3; k++)
                    {
                        Mx[j][k] += K*p[j]*p[k];
                        if (j!=k) Mx[k][j]+=K*p[j]*p[k];
                    }

                // y-contribution;
                K = (-1./(4.*a));
                for(j=0; j<3; j++)
                    for(k=j; k<3; k++)
                    {
                        My[j][k] +=K*q[j]*q[k];
                        if (j!=k) My[k][j]+=K*q[j]*q[k];
                    }

                // contribution from eddy currents;
                K=-I*a*w*blockproplist[meshele[i].blk].Cduct*c/12.;

                // in-plane laminated blocks appear to have no conductivity;
                // eddy currents are accounted for in these elements by their
                // frequency-dependent permeability.
                if((blockproplist[El->blk].LamType==0) &&
                        (blockproplist[El->blk].Lam_d>0)) K=0;

                // if this element is part of a wound coil,
                // it should have a zero "bulk" conductivity...
                if(labellist[El->lbl].bIsWound) K=0;

                for(j=0; j<3; j++)
                {
                    for(k=j; k<3; k++)
                    {
                        Me[j][k]+=K;
                        Me[k][j]+=K;
                    }
                }

                // contributions to Me, be from derivative boundary conditions;
                for(j=0; j<3; j++)
                {
                    if (El->e[j] >= 0)
                    {
                        if (lineproplist[El->e[j]].BdryFormat==2)
                        {
                            // conversion factor is 10^(-4) (I think...)
                            K=(-0.0001*c*lineproplist[ El->e[j] ].c0*l[j]/6.);
                            k=j+1;
                            if(k==3) k=0;
                            Me[j][j]+=2*K;
                            Me[k][k]+=2*K;
                            Me[j][k]+=K;
                            Me[k][j]+=K;

                            K=(lineproplist[ El->e[j] ].c1*l[j]/2.)*0.0001;
                            be[j]+=K;
                            be[k]+=K;
                        }

                        if (lineproplist[El->e[j]].BdryFormat==1)
                        {
                            ds=sqrt(2./(0.4*PI*w*lineproplist[El->e[j]].Sig*
                                        lineproplist[El->e[j]].Mu));
                            K=deg45/(-ds*lineproplist[El->e[j]].Mu*100.);
                            K*=(l[j]/6.);
                            k=j+1;
                            if(k==3) k=0;
                            Me[j][j]+=2*K;
                            Me[k][k]+=2*K;
                            Me[j][k]+=K;
                            Me[k][j]+=K;
                        }
                    }
                }

                // contribution to be from current density in the block
                for(j=0; j<3; j++)
                {
                    Jv=0;
                    if(labellist[El->lbl].InCircuit>=0)
                    {
                        k=labellist[El->lbl].InCircuit;
                        if(circproplist[k].Case==1) Jv=circproplist[k].J;
                        if(circproplist[k].Case==0)
                            Jv=-circproplist[k].dV*blockproplist[El->blk].Cduct;
                    }
                    K=-(blockproplist[El->blk].Jr+I*blockproplist[El->blk].Ji+Jv)*a/3.;
                    be[j]+=K;

                    if(labellist[El->lbl].InCircuit>=0)
                    {
                        k=labellist[El->lbl].InCircuit;
                        if(circproplist[k].Case==2) L.b[NumNodes+k]+=K;
                    }
                }

                // do Case 2 circuit stuff for element
                if(labellist[El->lbl].InCircuit>=0)
                {
                    k=labellist[El->lbl].InCircuit;
                    if(circproplist[k].Case==2)
                    {
                        K=-I*a*w*blockproplist[meshele[i].blk].Cduct*c;
                        for(j=0; j<3; j++) L.Put(L.Get(n[j],NumNodes+k)+K/3.,n[j],NumNodes+k);
                        L.Put(L.Get(NumNodes+k,NumNodes+k)+K,NumNodes+k,NumNodes+k);
                    }
                }


///////////////////////////////////////////////////////////////
//
//	New Nonlinear stuff
//
///////////////////////////////////////////////////////////////

                // update permeability for the element;
                if (Iter==0)
                {
                    k=meshele[i].blk;
                    if (blockproplist[k].BHpoints != 0) LinearFlag=false;
                    meshele[i].mu1=Mu[k][0];
                    meshele[i].mu2=Mu[k][1];
                }
                else
                {

                    k=meshele[i].blk;

                    if ((blockproplist[k].LamType==0) &&
                            (meshele[i].mu1==meshele[i].mu2)
                            &&(blockproplist[k].BHpoints>0))
                    {
                        for(j=0,B1=0.,B2=0.; j<3; j++)
                        {
                            B1+=L.V[n[j]]*q[j];
                            B2+=L.V[n[j]]*p[j];
                        }
                        B=c*sqrt(abs(B1*conj(B1))+abs(B2*conj(B2)))/(0.02*a);
                        // correction for lengths in cm of 1/0.02

// #ifdef NEWTON
                        if(ACSolver==1)
                        {
                            // find out new mu from saturation curve;
                            blockproplist[k].GetBHProps(B,mu,dv);
                            mu=1./(muo*mu);
                            meshele[i].mu1=mu;
                            meshele[i].mu2=mu;
                            for(j=0; j<3; j++)
                            {
                                for(ww=0,v[j]=0; ww<3; ww++)
                                    v[j]+=(Mx[j][ww]+My[j][ww])*L.V[n[ww]];
                            }

                            //Newton-like Iteration
                            //Comment out for successive approx
                            K=-200.*c*c*c*dv/a;
                            for(j=0; j<3; j++)
                                for(ww=0; ww<3; ww++)
                                {
                                    // Still compute Mn, the approximate N-R matrix used in
                                    // the complex-symmetric approx.  This will be useful
                                    // w.r.t. preconditioning.  However, subtract it off of Mnh and Mna
                                    // so that there is no net addition.
                                    Mn[j][ww] =K*Re(v[j]*conj(v[ww]));
                                    Mnh[j][ww]=  0.5*Re(K)*v[j]*conj(v[ww])-Re(Mn[j][ww]);
                                    Mna[j][ww]=I*0.5*Im(K)*v[j]*conj(v[ww])-I*Im(Mn[j][ww]);
                                    Mns[j][ww]=  0.5*K*v[j]*v[ww];
                                }
                        }
//#else
                        else
                        {
                            // find out new mu from saturation curve;
                            murel=1./(muo*blockproplist[k].Get_v(B));
                            muinc=1./(muo*blockproplist[k].GetdHdB(B));

                            // successive approximation;
                            //		       K=muinc;                            // total incremental
                            //			   K=murel;                            // total updated
                            K=2.*murel*muinc/(murel+muinc);     // averaged
                            meshele[i].mu1=K;
                            meshele[i].mu2=K;
                            K=-(1./murel - 1/K);
                            for(j=0; j<3; j++)
                                for(ww=0; ww<3; ww++)
                                    Mn[j][ww]=K*(Mx[j][ww]+My[j][ww]);
                        }
//#endif

                    }
                }

                // Apply correction for elements subject to prox effects
                if((blockproplist[meshele[i].blk].LamType>2) && (Iter==0) && (sdi_iter==0))
                {
                    meshele[i].mu1=labellist[meshele[i].lbl].ProximityMu;
                    meshele[i].mu2=labellist[meshele[i].lbl].ProximityMu;
                }

                // combine block matrices into global matrices;
                for(j=0; j<3; j++)
                    for(k=0; k<3; k++)
                    {

// #ifdef NEWTON
                        if (ACSolver==1)
                        {
                            Me[j][k]+= (Mx[j][k]/(El->mu2) + My[j][k]/(El->mu1) + Mn[j][k] );
                            be[j]+=(Mnh[j][k]+Mna[j][k]+Mn[j][k])*L.V[n[k]];
                            be[j]+=Mns[j][k]*L.V[n[k]].Conj();
                        }
// #else
                        else
                        {
                            Me[j][k]+= (Mx[j][k]/(El->mu2) + My[j][k]/(El->mu1) );
                            be[j]+=Mn[j][k]*L.V[n[k]];
                        }
// #endif
                    }

                for (j=0; j<3; j++)
                {
                    for (k=j; k<3; k++)
                    {
                        L.Put(L.Get(n[j],n[k]) + Me[j][k],n[j],n[k]);
//#ifdef NEWTON
                        if (ACSolver==1)
                        {
                            if (Mnh[j][k]!=0) L.Put(L.Get(n[j],n[k],1) + Mnh[j][k],n[j],n[k],1);
                            if (Mns[j][k]!=0) L.Put(L.Get(n[j],n[k],2) + Mns[j][k],n[j],n[k],2);
                            if (Mna[j][k]!=0) L.Put(L.Get(n[j],n[k],3) + Mna[j][k],n[j],n[k],3);
                        }
//#endif
                    }
                    L.b[n[j]]+=be[j];
                }
            }

            // add in contribution from point currents;
            for(i=0; i<NumNodes; i++)
                if(meshnode[i].BoundaryMarker>=0)
                {
                    K=0.01*(nodeproplist[meshnode[i].BoundaryMarker].Jr
                            +I*nodeproplist[meshnode[i].BoundaryMarker].Ji);
                    L.b[i]+=(-K);
                }

            // add in total current constraints for circuits;
            for(i=0; i<NumCircProps; i++)
                if (circproplist[i].Case==2)
                {
                    L.b[NumNodes+i]+=0.01*(circproplist[i].Amps_re +
                                           I*circproplist[i].Amps_im);
                }

            // apply fixed boundary conditions at points;
            for(i=0; i<NumNodes; i++)
                if(meshnode[i].BoundaryMarker >=0)
                    if((nodeproplist[meshnode[i].BoundaryMarker].Jr==0) &&
                            (nodeproplist[meshnode[i].BoundaryMarker].Ji==0) && (sdi_iter==0))
                    {
                        K= (nodeproplist[meshnode[i].BoundaryMarker].Ar +
                            I*nodeproplist[meshnode[i].BoundaryMarker].Ai)/c;
                        L.SetValue(i,K);
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
                                K=(a/c)*exp(I*lineproplist[s].phi*DEG);
                                L.SetValue(meshele[i].p[j],K);

                                // second point on the side;
                                x=meshnode[meshele[i].p[k]].x;
                                y=meshnode[meshele[i].p[k]].y;
                                x/=units[LengthUnits];
                                y/=units[LengthUnits];
                                s=meshele[i].e[j];
                                a=lineproplist[s].A0 + x*lineproplist[s].A1 +
                                  y*lineproplist[s].A2;
                                K=(a/c)*exp(I*lineproplist[s].phi*DEG);
                                L.SetValue(meshele[i].p[k],K);
                            }
                            else
                            {
                                // first point on the side;
                                x=meshnode[meshele[i].p[j]].x;
                                y=meshnode[meshele[i].p[j]].y;
                                r=sqrt(x*x+y*y);
                                if ((x==0) && (y==0)) t=0;
                                else t=atan2(y,x)/DEG;
                                r/=units[LengthUnits];
                                s=meshele[i].e[j];
                                a=lineproplist[s].A0 + r*lineproplist[s].A1 +
                                  t*lineproplist[s].A2;
                                K=(a/c)*exp(I*lineproplist[s].phi*DEG);
                                L.SetValue(meshele[i].p[j],K);

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
                                K=(a/c)*exp(I*lineproplist[s].phi*DEG);
                                L.SetValue(meshele[i].p[k],K);
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
                                L.SetValue(meshele[i].p[j],0.*I);
                                L.SetValue(meshele[i].p[k],0.*I);
                            }
                    }

            // "fix" diagonal entries associated with circuits that have
            // applied current or voltage that is known a priori
            // so that solver doesn't throw a "singular" flag
            for(j=0; j<NumCircProps; j++)
                if (circproplist[j].Case<2)	L.Put(L.Get(0,0),NumNodes+j,NumNodes+j);

            for(k=0; k<NumPBCs; k++)
            {
                if (pbclist[k].t==0) L.Periodicity(pbclist[k].x,pbclist[k].y);
                if (pbclist[k].t==1) L.AntiPeriodicity(pbclist[k].x,pbclist[k].y);
            }

            // solve the problem;
            if (SDIflag==false) for(j=0; j<NumNodes+NumCircProps; j++) V_old[j]=L.V[j];
            else
            {
                if(sdi_iter==0)
                    for(j=0; j<NumNodes+NumCircProps; j++) V_sdi[j]=L.V[j];
                else
                    for(j=0; j<NumNodes+NumCircProps; j++)
                    {
                        V_old[j]=V_sdi[j];
                        V_sdi[j]=L.V[j];
                    }
            }

            if (L.bNewton)
            {
                L.Precision=std::min(1.e-4,0.001*res);
                if (L.Precision<Precision) L.Precision=Precision;
            }
            if (L.PBCGSolveMod(Iter+sdi_iter)==false) return false;

            if(sdi_iter==1)
                for (i=0; i<NumNodes+NumCircProps; i++)
                    L.V[i]=(V_sdi[i]+L.V[i])/2.;

        } //end of SDI loop;

        if (LinearFlag==false)
        {

            for(j=0,x=0,y=0; j<NumNodes; j++)
            {
                x+=Re((L.V[j]-V_old[j])*conj(L.V[j]-V_old[j]));
                y+=Re(L.V[j]*conj(L.V[j]));
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
                if ((res>lastres) && (Relax>0.1)) Relax/=2.;
                else Relax+= 0.1 * (1. - Relax);

                for(j=0; j<NumNodes+NumCircProps; j++) L.V[j]=Relax*L.V[j]+(1.0-Relax)*V_old[j];
            }


            // report some results
            char outstr[256];
// #ifdef NEWTON
            if (ACSolver==1) sprintf(outstr,"Newton Iteration(%i) Relax=%.4g\n",Iter,Relax);
// #else
            else sprintf(outstr,"Successive Approx(%i) Relax=%.4g",Iter,Relax);
// #endif
//        TheView->SetDlgItemText(IDC_FRAME2,outstr);
            printf("%s\n", outstr);
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

    for (i=0; i<NumNodes; i++) L.b[i]=(L.V[i]*c);	// convert answer back to AMPS
    for (i=0; i<NumCircProps; i++)
        L.b[NumNodes+i]=(I*c*w*L.V[NumNodes+i]);
    // free up space allocated in this routine
    for(k=0; k<NumBlockProps; k++) free(Mu[k]);
    free(Mu);
    free(V_old);
    if(SDIflag==true) free(V_sdi);
    if(NumCircProps>0)
    {
        free(CircInt1);
        free(CircInt2);
        free(CircInt3);
    }

    return true;
}

int FSolver::WriteHarmonic2D(CBigComplexLinProb &L)
{
    // write solution to disk;

    char c[1024];
    FILE *fp,*fz;
    int i,k;
    double cf;
    double unitconv[]= {2.54,0.1,1.,100.,0.00254,1.e-04};

    // first, echo input .fem file to the .ans file;
    sprintf(c,"%s.fem",PathName.c_str());
    fz = fopen(c,"rt");
    if(fz==NULL)
    {
        //MsgBox("Couldn't open %s.fem\n",PathName);
        printf("Couldn't open %s.fem\n",PathName.c_str());
        return false;
    }

    sprintf(c,"%s.ans",PathName.c_str());
    fp = fopen(c,"wt");
    if(fp==NULL)
    {
        if (fz != NULL) fclose(fz);
        //MsgBox("Couldn't write to %s.ans\n",PathName.c_str());
        printf("Couldn't write to %s.ans\n",PathName.c_str());
        return false;
    }

    while(fgets(c,1024,fz)!=NULL) fputs(c,fp);
    fclose(fz);

    // then print out node, line, and element information
    fprintf(fp,"[Solution]\n");
    cf=unitconv[LengthUnits];
    fprintf(fp,"%i\n",NumNodes);
    for(i=0; i<NumNodes; i++)
        fprintf(fp,"%.17g\t%.17g\t%.17g\t%.17g\n",meshnode[i].x/cf,
                meshnode[i].y/cf,L.b[i].re,L.b[i].im);
    fprintf(fp,"%i\n",NumEls);
    for(i=0; i<NumEls; i++)
        fprintf(fp,"%i\t%i\t%i\t%i\n",
                meshele[i].p[0],meshele[i].p[1],meshele[i].p[2],meshele[i].lbl);

    /*
    	// print out circuit info
    	fprintf(fp,"%i\n",NumCircPropsOrig);
    	for(i=0;i<NumCircPropsOrig;i++){
    		if (circproplist[i].Case==0)
    			fprintf(fp,"0	%.17g	%.17g\n",circproplist[i].dV.Re(),
    								      circproplist[i].dV.Im());
    		if (circproplist[i].Case==1)
    			fprintf(fp,"1	%.17g	%.17g\n",circproplist[i].J.Re(),
    									  circproplist[i].J.Im());

    		if (circproplist[i].Case==2)
    			fprintf(fp,"0	%.17g	%.17g\n",L.b[NumNodes+i].Re(),
    									  L.b[NumNodes+i].Im());
    	}
    */
    // print out circuit info on a blocklabel by blocklabel basis;
    fprintf(fp,"%i\n",NumBlockLabels);
    for(k=0; k<NumBlockLabels; k++)
    {
        i=labellist[k].InCircuit;
        if(i<0) // if block not associated with any particular circuit
        {
            // print out some "dummy" propeties that say that
            // there is a fixed additional current density,
            // but that that additional current density is zero.
            fprintf(fp,"1\t0\t0\n");
        }
        else
        {
            if (circproplist[i].Case==0)
                fprintf(fp,"0\t%.17g\t%.17g\n",circproplist[i].dV.Re(),
                        circproplist[i].dV.Im());
            if (circproplist[i].Case==1)
                fprintf(fp,"1\t%.17g\t%.17g\n",circproplist[i].J.Re(),
                        circproplist[i].J.Im());

            if (circproplist[i].Case==2)
                fprintf(fp,"0\t%.17g\t%.17g\n",L.b[NumNodes+i].Re(),
                        L.b[NumNodes+i].Im());
        }
    }

    fclose(fp);
    return true;
}




