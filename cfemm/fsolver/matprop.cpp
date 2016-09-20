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

#include <stdio.h>
#include <math.h>
#include "malloc.h"
#include "femmcomplex.h"
#include "femmconstants.h"
#include "mmesh.h"
#include "fullmatrix.h"

#define ElementsPerSkinDepth 10


CMMaterialProp::~CMMaterialProp()
{
    delete[] Bdata;
    delete[] Hdata;
    delete[] slope;
}

// Constructor
CMMaterialProp::CMMaterialProp()
{
    mu_x=1.;
    mu_y=1.;            // permeabilities, relative
    H_c=0.;                // magnetization, A/m
    Cduct=0.;            // conductivity of the material, MS/m
    Lam_d=0.;            // lamination thickness, mm
    Theta_hn=0.;            // hysteresis angle, degrees
    Theta_hx=0.;            // hysteresis angle, degrees
    Theta_hy=0.;            // hysteresis angle, degrees
    Theta_m=0.;            // magnetization direction, degrees;
    LamFill=1.;            // lamination fill factor;
    LamType=0;            // type of lamination;
    WireD=0;            // strand diameter, mm
    NStrands=0;            // number of strands per wire
    Jr = 0;
    Ji = 0;			// applied current density, MA/m^2

    slope=NULL;
    Bdata=NULL;
    Hdata=NULL;
}

// Copy Constructor
CMMaterialProp::CMMaterialProp( const CMMaterialProp &other )
{
    mu_x = other.mu_x;
    mu_y = other.mu_y;          // permeabilities, relative
    H_c = other.H_c;            // magnetization, A/m
    Cduct = other.Cduct;        // conductivity of the material, MS/m
    Lam_d = other.Lam_d;        // lamination thickness, mm
    Theta_hn = other.Theta_hn;  // hysteresis angle, degrees
    Theta_hx = other.Theta_hx;  // hysteresis angle, degrees
    Theta_hy = other.Theta_hy;  // hysteresis angle, degrees
    Theta_m = other.Theta_m;    // magnetization direction, degrees;
    LamFill = other.LamFill;    // lamination fill factor;
    LamType = other.LamType;    // type of lamination;
    WireD = other.WireD;        // strand diameter, mm
    NStrands = other.NStrands;  // number of strands per wire
    Jr  = other.Jr;
    Ji  = other.Ji;			// applied current density, MA/m^2

    BHpoints = other.BHpoints;

    if ((BHpoints > 0) && (other.slope != NULL))
    {
        slope = (CComplex *)calloc(BHpoints,sizeof(CComplex));

        for (int i=0; i<BHpoints; i++)
        {
            slope[i] = other.slope[i];
        }
    }
    else
    {
        slope = NULL;
    }

    if ((BHpoints > 0) && (other.Bdata != NULL))
    {
        Bdata = (double *)  calloc(BHpoints,sizeof(double));

        for (int i=0; i<BHpoints; i++)
        {
            Bdata[i] = other.Bdata[i];
        }
    }
    else
    {
        Bdata = NULL;
    }

    if ((BHpoints > 0) && (other.Hdata != NULL))
    {
        Hdata = (CComplex *)calloc(BHpoints,sizeof(CComplex));

        for (int i=0; i<BHpoints; i++)
        {
            Hdata[i] = other.Hdata[i];
        }
    }
    else
    {
        Hdata = NULL;
    }
}

void CMMaterialProp::GetSlopes()
{
    GetSlopes(0);
}

void CMMaterialProp::GetSlopes(double omega)
{
    if (BHpoints==0) return; // catch trivial case;
    if (slope!=NULL) return; // already have computed the slopes;

    int i,k;
    int CurveOK=false;
    int ProcessedLams=false;
    CComplexFullMatrix L;
    double l1,l2;
    CComplex *hn;
    double *bn;
    CComplex mu;

    L.Create(BHpoints);
    bn   =(double *)  calloc(BHpoints,sizeof(double));
    hn   =(CComplex *)calloc(BHpoints,sizeof(CComplex));
    slope=(CComplex *)calloc(BHpoints,sizeof(CComplex));


    // strip off some info that we can use during the first
    // nonlinear iteration;
    mu_x = Bdata[1]/(muo*abs(Hdata[1]));
    mu_y = mu_x;
    Theta_hx=Theta_hn;
    Theta_hy=Theta_hn;

    // first, we need to doctor the curve if the problem is
    // being evaluated at a nonzero frequency.
    if(omega!=0)
    {
        // Make an effective B-H curve for harmonic problems.
        // this one convolves B(H) where H is sinusoidal with
        // a sine at the same frequency to get the effective
        // amplitude of B
        double munow,mumax=0;
        for(i=1; i<BHpoints; i++)
        {
            hn[i] = Hdata[i];
            for(k=1,bn[i]=0; k<=i; k++)
            {
                bn[i] += Re((4.*(Hdata[k]*Bdata[k-1] -
                                 Hdata[k-1]*Bdata[k])*(-cos((Hdata[k-1]*PI)/(2.*Hdata[i])) +
                                 cos((Hdata[k]*PI) / (2.*Hdata[i]))) + (-Bdata[k-1] +
                                 Bdata[k]) * ((Hdata[k-1] - Hdata[k])*PI +
                                 Hdata[i] * (-sin((Hdata[k-1]*PI) / Hdata[i]) +
                                 sin((Hdata[k]*PI) / Hdata[i])))) / ((Hdata[k-1] - Hdata[k])*PI));
            }
        }

        for(i=1; i<BHpoints; i++)
        {
            Bdata[i] = bn[i];
            Hdata[i] = hn[i];
            munow = Re(Bdata[i]/Hdata[i]);
            if (munow>mumax) mumax=munow;
        }

        // apply complex permeability to approximate the
        // effects of hysteresis.  We will follow a kludge
        // suggested by O'Kelly where hysteresis angle
        // is proportional to permeability.  This implies
        // that loss goes with B^2
        for(i=1; i<BHpoints; i++)
        {
            Hdata[i]*=exp(I*Bdata[i]*Theta_hn*DEG/(Hdata[i]*mumax));
        }

    }

    while(CurveOK!=true)
    {
        // make sure that the space for computing slopes is cleared out
        L.Wipe();

        // impose natural BC on the `left'
        l1=Bdata[1]-Bdata[0];
        L.M[0][0] = 4./l1;
        L.M[0][1] = 2./l1;
        L.b[0] = 6. * (Hdata[1]-Hdata[0]) / (l1*l1);

        // impose natural BC on the `right'
        int n=BHpoints;
        l1=Bdata[n-1]-Bdata[n-2];
        L.M[n-1][n-1]=4./l1;
        L.M[n-1][n-2]=2./l1;
        L.b[n-1]=6.*(Hdata[n-1]-Hdata[n-2])/(l1*l1);

        for(i=1; i<BHpoints-1; i++)
        {
            l1=Bdata[i]-Bdata[i-1];
            l2=Bdata[i+1]-Bdata[i];

            L.M[i][i-1]=2./l1;
            L.M[i][i]=4.*(l1+l2)/(l1*l2);
            L.M[i][i+1]=2./l2;

            L.b[i]=6.*(Hdata[i]-Hdata[i-1])/(l1*l1) +
                   6.*(Hdata[i+1]-Hdata[i])/(l2*l2);
        }

        L.GaussSolve();
        for(i=0; i<BHpoints; i++) slope[i]=L.b[i];

        // now, test to see if there are any "bad" segments in there.
        // it is probably sufficient to do this test just on the
        // real part of the BH curve...
        for(i=1,CurveOK=true; i<BHpoints; i++)
        {
            double L,c0,c1,c2,d0,d1,u0,u1,X0,X1;

            // it is probably sufficient to do this test just on the
            // real part of the BH curve.  We do the test on just the
            // real part of the curve by taking the real parts of
            // slope and Hdata
            d0=slope[i-1].re;
            d1=slope[i].re;
            u0=Hdata[i-1].re;
            u1=Hdata[i].re;
            L=Bdata[i]-Bdata[i-1];

            c0=d0;
            c1= -(2.*(2.*d0*L + d1*L + 3.*u0 - 3.*u1))/(L*L);
            c2= (3.*(d0*L + d1*L + 2.*u0 - 2.*u1))/(L*L*L);
            X0=-1.;
            X1=-1.;

            u0=c1*c1-4.*c0*c2;
            // check out degenerate cases
            if(c2==0)
            {
                if(c1!=0) X0=-c0/c1;
            }
            else if(u0>0)
            {
                u0=sqrt(u0);
                X0=-(c1 + u0)/(2.*c2);
                X1=(-c1 + u0)/(2.*c2);
            }

            //now, see if we've struck gold!
            if (((X0>=0.)&&(X0<=L))||((X1>=0.)&&(X1<=L)))
                CurveOK=false;
        }

        if(CurveOK!=true)  //remedial action
        {
            // Smooth out input points
            // to get rid of rapid transitions;
            // Uses a 3-point moving average
            for(i=1; i<BHpoints-1; i++)
            {
                bn[i]=(Bdata[i-1]+Bdata[i]+Bdata[i+1])/3.;
                hn[i]=(Hdata[i-1]+Hdata[i]+Hdata[i+1])/3.;
            }

            for(i=1; i<BHpoints-1; i++)
            {
                Hdata[i]=hn[i];
                Bdata[i]=bn[i];
            }
        }


        if((CurveOK==true) && (ProcessedLams==false))
        {
            // if the material is laminated and has a non-zero conductivity,
            // we have to do some work to find the "right" apparent BH
            // curve for the material for AC problems.  Essentially, we have
            // to solve a 1-D finite element problem at each B-H point to
            // figure out how much flux the lamination is really carrying.
            if((omega!=0) && (Lam_d!=0) && (Cduct!=0))
            {
                // Calculate a new apparent b and h for each point on the
                // curve to account for the laminations.
                for(i=1; i<BHpoints; i++)
                {
                    mu=LaminatedBH(omega,i);
                    bn[i]=abs(mu*Hdata[i]);
                    hn[i]=bn[i]/mu;
                }

                // Replace the original BH points with the apparent ones
                for(i=1; i<BHpoints; i++)
                {
                    Bdata[i]=bn[i];
                    Hdata[i]=hn[i];
                }

                // Make the program check the consistency and recompute the
                // proper slopes of the new BH curve one more time;
                CurveOK=false;
            }

            // take care of LamType=0 situation by changing the apparent B-H curve.
            if((LamType==0) && (LamFill!=1))
            {
                // this is changed from the previous version so that
                // a complex-valued H can be properly accommodated
                // in the algorithm.
                for(i=1; i<BHpoints; i++)
                {
                    mu=LamFill*Bdata[i]/Hdata[i]+(1.-LamFill)*muo;
                    Bdata[i]=abs(mu*Hdata[i]);
                    Hdata[i]=Bdata[i]/mu;
                }
                // Make the program check the consistency and recompute the
                // proper slopes of the new BH curve one more time;
                CurveOK=false;
            }

            ProcessedLams=true;
        }

    }


//	FILE *fp;
//	fp=fopen("c:\\temp\\bhjunk.txt","wt");
//	for(i=1;i<BHpoints;i++)
//		fprintf(fp,"%g	%g	%g\n",abs(Hdata[i]),Re(Bdata[i]*abs(Hdata[i])/Hdata[i]),Im(Bdata[i]*abs(Hdata[i])/Hdata[i]));
//	fclose(fp);

    free(bn);
    free(hn);
    return;
}


CComplex CMMaterialProp::GetH(double B)
{
    double b,z,z2,l;
    CComplex h;
    int i;

    b=fabs(B);

    if(BHpoints==0)	return CComplex(b/(mu_x*muo));

    if(b>Bdata[BHpoints-1])
        return (Hdata[BHpoints-1] + slope[BHpoints-1]*(b-Bdata[BHpoints-1]));

    for(i=0; i<BHpoints-1; i++)
        if((b>=Bdata[i]) && (b<=Bdata[i+1]))
        {
            l=(Bdata[i+1]-Bdata[i]);
            z=(b-Bdata[i])/l;
            z2=z*z;
            h=(1.-3.*z2+2.*z2*z)*Hdata[i] +
              z*(1.-2.*z+z2)*l*slope[i] +
              z2*(3.-2.*z)*Hdata[i+1] +
              z2*(z-1.)*l*slope[i+1];
            return h;
        }

    return CComplex(0);
}

CComplex CMMaterialProp::GetdHdB(double B)
{
    double b,z,l;
    CComplex h;
    int i;

    b=fabs(B);

    if(BHpoints==0)	return CComplex(b/(mu_x*muo));

    if(b>Bdata[BHpoints-1])
        return slope[BHpoints-1];

    for(i=0; i<BHpoints-1; i++)
        if((b>=Bdata[i]) && (b<=Bdata[i+1]))
        {
            l=(Bdata[i+1]-Bdata[i]);
            z=(b-Bdata[i])/l;
            h=6.*z*(z-1.)*Hdata[i]/l +
              (1.-4.*z+3.*z*z)*slope[i] +
              6.*z*(1.-z)*Hdata[i+1]/l +
              z*(3.*z-2.)*slope[i+1];
            return h;
        }

    return CComplex(0);
}

CComplex CMMaterialProp::Get_v(double B)
{
    if (B==0) return slope[0];

    return (GetH(B)/B);
}

CComplex CMMaterialProp::Get_dvB2(double B)
{
    if (B==0) return 0;

    return 0.5*(GetdHdB(B)/(B*B) - GetH(B)/(B*B*B));
}

void CMMaterialProp::GetBHProps(double B, double &v, double &dv)
{
    // version to use in the magnetostatic case in
    // which we know that v and dv ought to be real-valued.
    CComplex vc,dvc;

    GetBHProps(B,vc,dvc);
    v =Re(vc);
    dv=Re(dvc);
}

void CMMaterialProp::GetBHProps(double B, CComplex &v, CComplex &dv)
{
    double b,z,z2,l;
    CComplex h,dh;
    int i;

    b=fabs(B);

    if(BHpoints==0)
    {
        v=mu_x;
        dv=0;
        return;
    }

    if(b==0)
    {
        v=slope[0];
        dv=0;
        return;
    }

    if(b>Bdata[BHpoints-1])
    {
        h=(Hdata[BHpoints-1] + slope[BHpoints-1]*(b-Bdata[BHpoints-1]));
        dh=slope[BHpoints-1];
        v=h/b;
        dv=0.5*(dh/(b*b) - h/(b*b*b));
        return;
    }

    for(i=0; i<BHpoints-1; i++)
        if((b>=Bdata[i]) && (b<=Bdata[i+1]))
        {
            l=(Bdata[i+1]-Bdata[i]);
            z=(b-Bdata[i])/l;
            z2=z*z;
            h=(1.-3.*z2+2.*z2*z)*Hdata[i] +
              z*(1.-2.*z+z2)*l*slope[i] +
              z2*(3.-2.*z)*Hdata[i+1] +
              z2*(z-1.)*l*slope[i+1];
            dh=6.*z*(z-1.)*Hdata[i]/l +
               (1.-4.*z+3.*z*z)*slope[i] +
               6.*z*(1.-z)*Hdata[i+1]/l +
               z*(3.*z-2.)*slope[i+1];
            v=h/b;
            dv=0.5*(dh/(b*b) - h/(b*b*b));
            return;
        }
}

CComplex CMMaterialProp::LaminatedBH(double w, int i)
{
    int k,n,iter=0;
    CComplex *m0,*m1,*b,*x;
    double L,o,d,ds,B,lastres,res;
    double Relax=1;
    CComplex mu,vo,vi,c,H;
    CComplex Md,Mo;
    int Converged=false;
    res=0;

    // Base the required element spacing on the skin depth
    // at the surface of the material
    mu=Bdata[i]/Hdata[i];
    o=Cduct*1.e6;
    d=(Lam_d*0.001)/2.;
    ds=sqrt(2/(w*o*abs(mu)));
    n= ElementsPerSkinDepth * ((int) ceil(d/ds));
    L=d/((double) n);

    x =(CComplex *)calloc(n+1,sizeof(CComplex));
    b =(CComplex *)calloc(n+1,sizeof(CComplex));
    m0=(CComplex *)calloc(n+1,sizeof(CComplex));
    m1=(CComplex *)calloc(n+1,sizeof(CComplex));

    do
    {
        // make sure that the old stuff is wiped out;
        for(k=0; k<=n; k++)
        {
            m0[k]=0;
            m1[k]=0;
            b[k] =0;
        }

        // build matrix
        for(k=0; k<n; k++)
        {
            if(iter!=0)
            {
                B=abs(x[k+1]-x[k])/L;
                vi=GetdHdB(B);
                vo=GetH(B)/B;
            }
            else
            {
                vo=1./mu;
                vi=1./mu;
            }

            Md = ( (vi+vo)/(2.*L) + I*w*o*L/4.);
            Mo = (-(vi+vo)/(2.*L) + I*w*o*L/4.);
            m0[k]   += Md;
            m1[k]   += Mo;
            m0[k+1] += Md;

            Md = ( (vi-vo)/(2.*L));
            Mo = (-(vi-vo)/(2.*L));
            b[k]    += (Md*x[k] + Mo*x[k+1]);
            b[k+1]  += (Mo*x[k] + Md*x[k+1]);
        }

        // set boundary conditions
        m1[0] = 0;
        b[0]  = 0;
        b[n] += Hdata[i];

        // solve tridiagonal equation
        // solution ends up in b;
        for(k = 0; k < n; k++)
        {
            c = m1[k]/m0[k];
            m0[k+1] -= m1[k]*c;
            b[k+1]  -= b[k]*c;
        }
        b[n] = b[n]/m0[n];
        for(k = n-1; k >= 0; k--)
            b[k] = (b[k] - m1[k]*b[k + 1])/m0[k];

        iter++;

        lastres=res;
        res=abs(b[n]-x[n])/d;

        if (res<1.e-8) Converged=true;

        // Do the same relaxation scheme as is implemented
        // in the solver to make sure that this effective
        // lamination permeability calculation converges
        if(iter>5)
        {
            if ((res>lastres) && (Relax>0.1)) Relax/=2.;
            else Relax+= 0.1 * (1. - Relax);
        }

        for(k=0; k<=n; k++) x[k]=Relax*b[k]+(1.0-Relax)*x[k];

    }
    while(Converged!=true);

    mu = x[n]/(Hdata[i]*d);

    free(x );
    free(b );
    free(m0);
    free(m1);

    return mu;
}
