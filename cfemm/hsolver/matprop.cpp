
#include "hmesh.h"
#include "fparse.h"

#include <string>
#include <iostream>
using std::string;
using namespace femm;

CHMaterialProp::CHMaterialProp()
    : Kx(1)
    , Ky(1)
    , Kt(0)
    , qv(0)
    , npts(0)
{
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

CHMaterialProp CHMaterialProp::fromStream(std::istream &input, std::ostream &err)
{
    CHMaterialProp prop;

    if( parseToken(input, "<beginblock>", err) )
    {
        string token;
        while (input.good() && token != "<endblock>")
        {
            nextToken(input,&token);

            if( token == "<kx>" )
            {
                expectChar(input, '=', err);
                input >> prop.Kx;
                continue;
            }

            if( token == "<ky>" )
            {
                expectChar(input, '=', err);
                input >> prop.Ky;
                continue;
            }

            if( token == "<kt>" )
            {
                expectChar(input, '=', err);
                input >> prop.Kt;
                continue;
            }

            if( token == "<qv>" )
            {
                expectChar(input, '=', err);
                input >> prop.qv;
                continue;
            }

            if( token == "<blockname>" )
            {
                expectChar(input, '=', err);
                ParseString(input, &prop.name);
                continue;
            }

            if( token == "<tkpoints>" )
            {
                expectChar(input, '=', err);
                input >> prop.npts;
                if (prop.npts > 0)
                {
                    // FIXME: make Kn variable size
                    if (prop.npts>128)
                    {
                        err << "MaterialProp exceeds maximum size! File a bug report...";
                        prop.npts = 128;
                    }
                    for(int i=0; i<prop.npts; i++)
                    {
                        input >> prop.Kn[i].re >> prop.Kn[i].im;
                    }
                }
                continue;
            }
            if( token != "<endblock>")
                err << "\nUnexpected token: "<<token;
        }
    }

    return prop;
}




CHPointProp CHPointProp::fromStream(std::istream &input, std::ostream &err)
{
    CHPointProp prop;

    if( parseToken(input, "<beginpoint>", err) )
    {
        string token;
        while (input.good() && token != "<endpoint>")
        {
            nextToken(input, &token);

            if( token == "<tp>" )
            {
                expectChar(input, '=', err);
                input >> prop.V;
                continue;
            }

            if( token == "<qp>" )
            {
                expectChar(input, '=', err);
                input >> prop.qp;
                continue;
            }
            err << "\nUnexpected token: "<<token;
        }
    }

    return prop;
}

CHConductor CHConductor::fromStream(std::istream &input, std::ostream &err)
{
    CHConductor prop;

    if( parseToken(input, "<beginconductor>", err) )
    {
        string token;
        while (input.good() && token != "<endconductor>")
        {
            nextToken(input, &token);

            if( token == "<tc>" )
            {
                expectChar(input, '=', err);
                input >> prop.V;
                continue;
            }

            if( token == "<qc>" )
            {
                expectChar(input, '=', err);
                input >> prop.q;
                continue;
            }

            if( token == "<conductortype>" )
            {
                expectChar(input, '=', err);
                input >> prop.CircType;
                continue;
            }

            err << "\nUnexpected token: "<<token;
        }
    }

    return prop;
}
