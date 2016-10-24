#ifndef FEMM_CMATERIALPROP_H
#define FEMM_CMATERIALPROP_H

#include "femmcomplex.h"
#include <iostream>
#include <string>
#include <vector>

namespace femm {
class CMaterialProp
{
public:

    CMaterialProp();
    virtual ~CMaterialProp();
    // copy constructor
    CMaterialProp( const CMaterialProp& other );

    virtual void GetSlopes(double omega=0.);
    virtual CComplex LaminatedBH(double w, int i);

    double GetH(double b);
    CComplex GetH(CComplex b);            // ``raw'' results
    virtual CComplex GetdHdB(double B);
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

    std::string BlockName;
    double mu_x,mu_y;       // permeabilities, relative
    int BHpoints;           // number of points in the BH curve...
    std::vector<double>   Bdata;
    std::vector<CComplex> Hdata;        // entries in B-H curve;
    std::vector<CComplex> slope;        // slopes used in interpolation
    // of BHdata
    int    LamType;         // flag that tells how block is laminated;
    //  0 = not laminated or laminated in plane;
    //  1 = laminated in the x-direction;
    //  2 = laminated in the y-direction;
    double LamFill;         // lamination fill factor, dimensionless;
    double H_c;             // magnetization, A/m
    double Nrg;
    CComplex J;             // applied current density, MA/m^2
    double Cduct;           // conductivity of the material, MS/m
    double Lam_d;           // lamination thickness, mm
    double Theta_hn;        // hysteresis angle, degrees
    double Theta_hx;        // hysteresis angle, degrees
    double Theta_hy;        // hysteresis angle, degrees
    int    NStrands;        // number of strands per wire
    double WireD;           // strand diameter, mm

    CComplex mu_fdx,mu_fdy; // complex permeability for harmonic problems;


    /**
     * @brief toStream serializes the data and inserts it into \p out.
     * This virtual method is called by the \c operator<<() and
     * needs to be overridden by any subclass.
     *
     * Unless \c NDEBUG is defined, this dummy implementation in the base class will call \c assert(false).
     *
     * @param out
     */
    virtual void toStream( std::ostream &out ) const;
private:

};

/**
 * @brief operator << serializes the data in \prop and inserts it into \p os
 * @param os
 * @param prop
 * @return \p os
 */
std::ostream& operator<< (std::ostream& os, const CMaterialProp& prop);

}
#endif
