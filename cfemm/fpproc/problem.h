#ifndef PROBLEM_H
#define PROBLEM_H

#include <string>
#include "femmcomplex.h"

namespace FPProcdata{
/////////////////////////////////////////////////////////////////////////////
// CNode -- structure that holds information about each control point.

using std::string;

class CNode
{
    public:
        CNode();

        double x,y;
        int xs,ys;
        bool IsSelected;
        int BoundaryMarker;

        double GetDistance(double xo, double yo);
        CComplex CC();
        void ToggleSelect();

    private:

};

class CMeshNode
{
    public:
        CMeshNode();

        double x,y;
        CComplex A;
        double msk;
        int xs,ys;

        double GetDistance(double xo, double yo);
        CComplex CC();

    private:

};
/////////////////////////////////////////////////////////////////////////////
// CSegment -- structure that holds information about lines joining control pts

class CSegment
{
    public:
        CSegment();

        int n0,n1;
        double MaxSideLength;
        bool IsSelected;
        bool Hidden;
        int BoundaryMarker;

        void ToggleSelect();

    private:

};

class CArcSegment
{
    public:
        CArcSegment();

        int n0,n1;
        bool IsSelected;
        bool Hidden;
        double MaxSideLength,ArcLength;
        int BoundaryMarker;

        void ToggleSelect();

    private:

};
/////////////////////////////////////////////////////////////////////////////
// CBlockLabel -- structure that holds block label information

class CBlockLabel
{
    public:
        CBlockLabel();

        double x,y;
        double MaxArea;
        double MagDir;
        string MagDirFctn;
        int    Turns;
        bool IsSelected;
        int InCircuit;
        int BlockType;
        int InGroup;
        bool IsExternal;

        int Case;
        CComplex  J,dVolts;

        // attributes used to keep track of wound coil properties...
        double FillFactor;
        CComplex o,mu;

        void ToggleSelect();
        double GetDistance(double xo, double yo);

    private:

};

class CMaterialProp
{
    public:

        CMaterialProp();
        ~CMaterialProp();
        // copy constructor
        CMaterialProp( const CMaterialProp& other );

        void GetSlopes(double omega);
        CComplex LaminatedBH(double w, int i);

        double GetH(double b);
        CComplex GetH(CComplex b);            // ``raw'' results
        CComplex GetdHdB(double B);
        double GetB(double h);

        void GetMu(double b1,double b2,double &mu1, double &mu2);
        void GetMu(CComplex b1,CComplex b2,CComplex &mu1, CComplex &mu2);
        double GetEnergy(double b);        // straight from the
        double GetCoEnergy(double b);    // BH curve data

        // routines that are actually called to get
        // energy and coenergy.  These catch and take
        // care of all of the weird special cases
        // that arise with laminated materials.
        double DoEnergy(double bx, double by);
        double DoCoEnergy(double bx, double by);
        double DoEnergy(CComplex bx, CComplex by);
        double DoCoEnergy(CComplex b1, CComplex b2);

        string BlockName;
        double mu_x,mu_y;       // permeabilities, relative
        int BHpoints;           // number of points in the BH curve...
        double   *Bdata;
        CComplex *Hdata;        // entries in B-H curve;
        CComplex *slope;        // slopes used in interpolation
                                // of BHdata
        int    LamType;         // flag that tells how block is laminated;
                                //  0 = not laminated or laminated in plane;
                                //  1 = laminated in the x-direction;
                                //  2 = laminated in the y-direction;
        double LamFill;         // lamination fill factor, dimensionless;
        double H_c;             // magnetization, A/m
        double Nrg;
        double Jr,Ji;           // applied current density, MA/m^2
        double Cduct;           // conductivity of the material, MS/m
        double Lam_d;           // lamination thickness, mm
        double Theta_hn;        // hysteresis angle, degrees
        double Theta_hx;        // hysteresis angle, degrees
        double Theta_hy;        // hysteresis angle, degrees
        int    NStrands;        // number of strands per wire
        double WireD;           // strand diameter, mm

        CComplex mu_fdx,mu_fdy; // complex permeability for harmonic problems;

    private:
};

class CBoundaryProp
{
    public:

        CBoundaryProp();

        string BdryName;
        int BdryFormat;            // type of boundary condition we are applying
                                // 0 = constant value of A
                                // 1 = Small skin depth eddy current BC
                                // 2 = Mixed BC

        double A0,A1,A2,phi;    // set value of A for BdryFormat=0;

        double Mu,Sig;            // material properties necessary to apply
                                // eddy current BC

        CComplex c0,c1;            // coefficients for mixed BC

    private:
};

class CPointProp
{
    public:

        CPointProp();

        string PointName;
        double Jr,Ji;            // applied point current, A
        double Ar,Ai;                // prescribed nodal value;

    private:
};

class CCircuit
{
    public:

        CCircuit();

        string CircName;
        CComplex  Amps;
        int        CircType;

    private:

};

class CElement
{
    public:

        int p[3];
        int blk,lbl;
        CComplex B1,B2;
        CComplex b1[3],b2[3];
        double magdir;
        CComplex ctr;
        double rsqr;
        int n[3];  // 3 ints to store elem's neighbouring elements

    private:
};

class CPointVals
{
    public:

        CComplex A;            // vector potential
        CComplex B1,B2;        // flux density
        CComplex mu1,mu2;    // permeability
        CComplex H1,H2;        // field intensity
        CComplex Je,Js;        // eddy current and source current densities
        CComplex Hc;        // Magnetization for regions with a PM.
        double c;            // conductivity
        double E;            // energy stored in the magnetic field
        double Ph;            // power dissipated by hysteresis
        double Pe;            // power dissipated by eddy currents
        double ff;            // winding fill factor

        CPointVals();

    private:
};

}
#endif

using namespace FPProcdata;
