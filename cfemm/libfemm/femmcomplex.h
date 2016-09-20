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

#ifndef CCOMPLEX_H
#define CCOMPLEX_H
#include "femmconstants.h"


class CComplex
{
public:
    // data members
    double re,im;

    // member functions
    CComplex();
    CComplex(double x);
    CComplex(int x);
    CComplex(long x);
    CComplex(double x, double y);
    CComplex Sqrt();
    CComplex Conj();
    CComplex Inv();
    void Set(double x, double y);
    double Abs();
    double Arg();
    double Re();
    double Im();
    char* ToString(char *s);
    char* ToStringAlt(char *s);

    //operator redefinition
    //Addition
    CComplex operator+( const CComplex& z );
    CComplex operator+(double z);
    CComplex operator+(int z);
    friend CComplex operator+( int x,  const CComplex& y );
    friend CComplex operator+( double x,  const CComplex& y );
    friend CComplex operator+( const CComplex& x,  const CComplex& y );
    void operator+=( const CComplex& z);
    void operator+=(double z);
    void operator+=(int z);

    //Subtraction
    CComplex operator-();
    CComplex operator-( const CComplex& z );
    CComplex operator-(double z);
    CComplex operator-(int z);
    friend CComplex operator-( int x,  const CComplex& y );
    friend CComplex operator-( double x,  const CComplex& y );
    friend CComplex operator-( const CComplex& x,  const CComplex& y );
    friend CComplex operator-( const CComplex& x );
    void operator-=( const CComplex& z);
    void operator-=(double z);
    void operator-=(int z);

    //Multiplication
    CComplex operator*( const CComplex& z );
    CComplex operator*(double z);
    CComplex operator*(int z);
    friend CComplex operator*( int x,  const CComplex& y );
    friend CComplex operator*( double x,  const CComplex& y );
    friend CComplex operator*( const CComplex& x,  const CComplex& y );
    void operator*=( const CComplex& z);
    void operator*=(double z);
    void operator*=(int z);

    //Division
    CComplex operator/( const CComplex& z );
    CComplex operator/(double z);
    CComplex operator/(int z);
    friend CComplex operator/( int x,  const CComplex& y );
    friend CComplex operator/( double x,  const CComplex& y );
    friend CComplex operator/( const CComplex &x,  const CComplex& y );
    void operator/=( const CComplex& z);
    void operator/=(double z);
    void operator/=(int z);

    //Equals
    void operator=(double z);
    void operator=(int z);
    void operator=(long z);

    //Tests
    bool operator==( const CComplex& z);
    bool operator==(double z);
    bool operator==(int z);

    bool operator!=( const CComplex& z);
    bool operator!=(double z);
    bool operator!=(int z);

    bool operator<( const CComplex& z);
    bool operator<( double z);
    bool operator<( int z);

    bool operator<=( const CComplex& z);
    bool operator<=( double z);
    bool operator<=( int z);

    bool operator>( const CComplex& z);
    bool operator>( double z);
    bool operator>( int z);

    bool operator>=( const CComplex& z);
    bool operator>=( double z);
    bool operator>=( int z);


private:

};

// useful functions...
#define I CComplex(0,1)
double Re( const CComplex& a);
double Im( const CComplex& a);
double abs( const CComplex& x );
double absq( const CComplex& x );
double arg( const CComplex& x );
CComplex conj( const CComplex& x);
CComplex exp( const CComplex& x );
CComplex sqrt( const CComplex& x );
CComplex tanh( const CComplex& x );
CComplex sinh( const CComplex& x );
CComplex cosh( const CComplex& x );
CComplex cos( const CComplex& x );
CComplex acos( const CComplex& x );
CComplex sin( const CComplex& x );
CComplex asin( const CComplex& x );
CComplex tan( const CComplex& x );
CComplex atan( const CComplex& x );
CComplex atan2( const CComplex& y, const CComplex& x);
CComplex log( const CComplex& x );
CComplex pow( const CComplex& x, int y);
CComplex pow( const CComplex& x, double y);
CComplex pow( const CComplex& x,  const CComplex& y);
CComplex Chop( const CComplex& a, double tol=1.e-12);




#endif // CCOMPLEX check
