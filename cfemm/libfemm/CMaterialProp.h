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
       johannes@zarl-zierl.at

   Contributions by Johannes Zarl-Zierl were funded by
	Linz Center of Mechatronics GmbH (LCM)
*/
#ifndef FEMM_CMATERIALPROP_H
#define FEMM_CMATERIALPROP_H

#include "femmcomplex.h"
#include <iostream>
#include <string>
#include <vector>

namespace femm {

/**
 * @brief The PropertyParseMode controls parsing in the ::fromStream methods.
 */
enum class PropertyParseMode {
    Normal,
    NoBeginBlock /// Don't expect the \c <beginBlock> token.
};

class CMaterialProp
{
public:
    virtual ~CMaterialProp();

    std::string BlockName;

    /**
     * @brief isAir is used by PostProcessor::makeMask().
     * @return \c true, if the material has the properties of air, \c false otherwise
     */
    virtual bool isAir() const = 0;

    /**
     * @brief isSameMaterialAs is used by IsSameMaterial in the postprocessor to determine if two block properties can be considered the same.
     * @param other
     * @return \c true, if both materials can be said to be the same, \c false otherwise.
     */
    virtual bool isSameMaterialAs(const CMaterialProp *other) const = 0;

    /**
     * @brief toStream serializes the data and inserts it into \p out.
     * This virtual method is called by the \c operator<<() and
     * needs to be overridden by any subclass.
     *
     * @param out
     */
    virtual void toStream( std::ostream &out ) const = 0;
protected:
    CMaterialProp();
};

class CMMaterialProp : public CMaterialProp
{
public:

    CMMaterialProp();
    virtual ~CMMaterialProp();
    // copy constructor
    CMMaterialProp( const CMMaterialProp& other );

    virtual void clearSlopes();
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

    double MuMax; // maximum permeability for AC problems

    virtual bool isAir() const override;
    /**
     * @brief this is not (yet?) used for magnetics problems
     * @param other
     * @return \c false
     */
    bool isSameMaterialAs(const CMaterialProp *other) const override;
    /**
     * @brief CMaterialProp is used in the fpproc and fpproc never uses the toStream method.
     * Therefore, this implementation just calls assert(false).
     * @param out
     */
    virtual void toStream( std::ostream &out ) const override;
private:

};

/**
 * @brief operator << serializes the data in \p prop and inserts it into \p os
 * @param os
 * @param prop
 * @return \p os
 */
std::ostream& operator<< (std::ostream& os, const CMMaterialProp& prop);

/**
 * @brief The CMMaterialProp class is used in fsolver.
 */
class CMSolverMaterialProp : public CMMaterialProp
{
    // Properties
public:

    double Theta_m;			// orientation of magnetization, degrees

    // Methods
public:

    CMSolverMaterialProp();
    virtual ~CMSolverMaterialProp();
    CMSolverMaterialProp( const CMSolverMaterialProp & );
    CComplex GetH(double B); // ill-matched override
    CComplex Get_dvB2(double B);
    CComplex Get_v(double B);
    void GetBHProps(double B, CComplex &v, CComplex &dv);
    void GetBHProps(double B, double &v, double &dv);
    /**
     * @brief Get the incremental permeability of a nonlinear material for use in incremental permeability formulation about DC offset.
     * @param B
     * @param w
     * @param mu1
     * @param mu2
     *
     * \internal
     * ### FEMM reference source:
     *  - \femm42{fkn/matprop.cpp,CMaterialProp::IncrementalPermeability()}
     * \endinternal
     */
    void incrementalPermeability(double B, double w, CComplex &mu1, CComplex &mu2);
    virtual CComplex LaminatedBH(double omega, int i) override;

    /**
     * @brief fromStream constructs a CMaterialProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @param mode
     * @return a CMaterialProp
     */
    static CMSolverMaterialProp fromStream( std::istream &input, std::ostream &err = std::cerr, PropertyParseMode mode = PropertyParseMode::Normal );
    virtual void toStream( std::ostream &out ) const override;
private:
};

class CHMaterialProp : public CMaterialProp
{
    // Properties
public:

    double Kx,Ky;		// thermal conductivity for linear (possibly anisotropic) regions
    double Kt;			// volumetric heat capacity
    double qv;			// volume heat generation

    // properties for nonlinear conductivity
    int npts;			// number of points in the nonlinear conductivity curve
    CComplex Kn[128];   // here, I'm being _very_ lazy by defining a fixed-length buffer for the
                        // thermal conductivity data points.

    // Methods
public:

    CHMaterialProp();
    virtual ~CHMaterialProp();
    CHMaterialProp( const CHMaterialProp & );
    CComplex GetK(double t) const;

    /**
     * @brief fromStream constructs a CHMaterialProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @param mode
     * @return a CHMaterialProp
     */
    static CHMaterialProp fromStream( std::istream &input, std::ostream &err = std::cerr, PropertyParseMode mode = PropertyParseMode::Normal );

    /**
     * @brief isAir is not relevant to heat flow problems and therefore always returns false.
     * @return \c false
     */
    bool isAir() const override;
    bool isSameMaterialAs(const CMaterialProp *other) const override;

    virtual void toStream( std::ostream &out ) const override;
private:
};

/**
 * @brief The CSMaterialProp class holds material properties in electrostatics problems.
 */
class CSMaterialProp : public CMaterialProp
{
public:

    CSMaterialProp();
    virtual ~CSMaterialProp();

    double ex;   ///< relative permittivity in x direction
    double ey;   ///< relative permittivity in y direction
    double qv;   ///< volume charge density, C/m^3

    /**
     * @brief fromStream constructs a CSMaterialProp from an input stream (usually an input file stream)
     * @param input
     * @param err output stream for error messages
     * @param mode
     * @return a CSMaterialProp
     */
    static CSMaterialProp fromStream( std::istream &input, std::ostream &err = std::cerr, PropertyParseMode mode = PropertyParseMode::Normal );
    bool isAir() const override;
    /**
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::IsSameMaterial(int,int)}
     * \endinternal
     */
    bool isSameMaterialAs(const CMaterialProp *other) const override;
    virtual void toStream( std::ostream &out ) const override;
private:
};

} //namespace
#endif
