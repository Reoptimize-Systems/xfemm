
#include "hmesh.h"

CHMaterialProp::CHMaterialProp()
{
    Kx=Ky=1;
    Kt=0;
    qv=0;
}

CHMaterialProp::~CHMaterialProp()
{

}

CHMaterialProp::CHMaterialProp( const CHMaterialProp & other)
{
    Kx = other.Kx;
    Ky = other.Kx;
	Kt = other.Kt;  // volumetric heat capacity
	qv = other.qv;  // volume heat generation

	// properties for nonlinear conductivity
	npts = other.npts;			// number of points in the nonlinear conductivity curve

	for (int i = 0; i < npts; i++)
	{
	    // copy the thermal conductivity data points.
        Kn[i] = other.Kn[i];
	}
}

CComplex CHMaterialProp::GetK(double t)
{
	int i,j;

	// Kx returned as real part;
	// Ky returned as imag part

	if (npts==0) return (Kx+I*Ky);
	if (npts==1) return (Im(Kn[0])*(1+I));
	if (t<=Re(Kn[0])) return (Im(Kn[0])*(1+I));
	if (t>=Re(Kn[npts-1])) return (Im(Kn[npts-1])*(1+I));

	for(i=0,j=1;j<npts;i++,j++)
	{
		if((t>=Re(Kn[i])) && (t<=Re(Kn[j])))
		{
			return (1+I)*(Im(Kn[i])+Im(Kn[j]-Kn[i])*Re(t-Kn[i])/Re(Kn[j]-Kn[i]));
		}
	}

	return (Kx+I*Ky);
}

