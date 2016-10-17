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

#include "femmcomplex.h"
#include "femmconstants.h"
#include "mmesh.h"
#include "fullmatrix.h"
#include "fparse.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctype.h>
#include <istream>

#define ElementsPerSkinDepth 10

using fsolver::CMMaterialProp;
using namespace std;

CMMaterialProp::~CMMaterialProp()
{
}

// Constructor
CMMaterialProp::CMMaterialProp()
    : CMaterialProp()
{
    Theta_m=0.;            // magnetization direction, degrees;
}

// Copy Constructor
CMMaterialProp::CMMaterialProp( const CMMaterialProp &other )
    : femm::CMaterialProp( other )
{
    Theta_m = other.Theta_m;    // magnetization direction, degrees;
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

// this can't be immediately merged with femm::CMaterialProp,
// because this uses the slightly different GetH implementation
// from fsolver
CComplex CMMaterialProp::LaminatedBH(double w, int i)
{
    int k,n,iter=0;
    CComplex *m0,*m1,*b,*x;
    double L,o,d,ds,B,lastres,res;
    res=0;
    double Relax=1;
    CComplex mu,vo,vi,c,H;
    CComplex Md,Mo;
    int Converged=false;

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
                vo=GetH(B)/B; // <- this is the GetH from fsolver
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

fsolver::CMMaterialProp CMMaterialProp::fromStream(std::istream &input, std::ostream &err)
{
    using namespace femm;
    CMMaterialProp prop;

    if( parseToken(input, "<beginblock>", err) )
    {
        string token;
        while (input.good() && token != "<endblock>")
        {
            nextToken(input,&token);

            if( token == "<mu_x>" )
            {
                expectChar(input, '=', err);
                input >> prop.mu_x;
                continue;
            }

            if( token == "<mu_y>" )
            {
                expectChar(input, '=', err);
                input >> prop.mu_y;
                continue;
            }

            if( token == "<h_c>" )
            {
                expectChar(input, '=', err);
                input >> prop.H_c;
                continue;
            }

            if( token == "<h_cangle>" )
            {
                expectChar(input, '=', err);
                input >> prop.Theta_m;
                continue;
            }

            if( token == "<j_re>" )
            {
                expectChar(input, '=', err);
                input >> prop.J.re;
                continue;
            }

            if( token == "<j_im>" )
            {
                expectChar(input, '=', err);
                input >> prop.J.im;
                continue;
            }

            if( token == "<sigma>" )
            {
                expectChar(input, '=', err);
                input >> prop.Cduct;
                continue;
            }

            if( token == "<phi_h>" )
            {
                expectChar(input, '=', err);
                input >> prop.Theta_hn;
                continue;
            }


            if( token == "<phi_hx>" )
            {
                expectChar(input, '=', err);
                input >> prop.Theta_hx;
                continue;
            }

            if( token == "<phi_hy>" )
            {
                expectChar(input, '=', err);
                input >> prop.Theta_hy;
                continue;
            }

            if( token == "<d_lam>" )
            {
                expectChar(input, '=', err);
                input >> prop.Lam_d;
                continue;
            }

            if( token == "<lamfill>" )
            {
                expectChar(input, '=', err);
                input >> prop.LamFill;
                continue;
            }

            if( token == "<wired>" )
            {
                expectChar(input, '=', err);
                input >> prop.WireD;
                continue;
            }

            if( token == "<lamtype>" )
            {
                expectChar(input, '=', err);
                input >> prop.LamType;
                continue;
            }

            if( token == "<nstrands>" )
            {
                expectChar(input, '=', err);
                input >> prop.NStrands;
                continue;
            }

            if( token == "<bhpoints>" )
            {
                expectChar(input, '=', err);
                input >> prop.BHpoints;
                if (prop.BHpoints > 0)
                {
                    prop.Hdata.reserve(prop.BHpoints);
                    prop.Bdata.reserve(prop.BHpoints);
                    for(int i=0; i<prop.BHpoints; i++)
                    {
                        double b;
                        CComplex h;
                        input >> b >> h.re;
                        prop.Bdata.push_back(b);
                        prop.Hdata.push_back(h);
                    }
                }
                continue;
            }
            err << "\nUnexpected token: "<<token;
        }
    }

    return prop;
}
