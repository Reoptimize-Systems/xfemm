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

#ifndef CSPARS_H
#define CSPARS_H

class CComplexEntry
{
public:

    CComplex x;				// value stored in the entry
    int c;					// column that the entry lives in
    CComplexEntry *next;	// pointer to next entry in the row;
    CComplexEntry();

private:
};

class CBigComplexLinProb
{
public:

    // data members

    CComplex *P; 				// search direction
    CComplex *U;
    CComplex *R; 				// residual
    CComplex *V;
    CComplex *Z;
    CComplex *b;				// RHS of linear equation
    CComplex *uu;
    CComplex *vv;

    CComplexEntry **M;			// pointer to list of matrix entries;
    CComplexEntry **Mh;			// Hermitian matrix arising from N-R algorithm;
    CComplexEntry **Ma;			// Antihermitian matrix arising from N-R algorithm;
    CComplexEntry **Ms;			// Additional complex-symmetric matrix arising from N-R algorithm;
    int n;						// dimensions of the matrix;
    int bdw;					// optional bandwidth parameter;
    int bNewton;				// Flag which denotes whether or not there are entries in Mh or Ms;
    int NumNodes;
    double Precision;
    double Lambda;			// relaxation factor;

    // member functions

    CBigComplexLinProb();				// constructor
    ~CBigComplexLinProb();				// destructor
    int Create(int d, int bw, int nodes);	// initialize the problem
    void Put(CComplex v, int p, int q, int k=0); // use to create/set entries in the matrix
    CComplex Get(int p, int q, int k=0);
    void MultA(CComplex *X, CComplex *Y, int k=0);
    void MultConjA(CComplex *X, CComplex *Y, int k=0);
    CComplex Dot(CComplex *x, CComplex *y);
    CComplex ConjDot(CComplex *x, CComplex *y);
    void SetValue(int i, CComplex x);
    void Periodicity(int i, int j);
    void AntiPeriodicity(int i, int j);
    void Wipe();
    void MultPC(CComplex *X, CComplex *Y);
    void MultAPPA(CComplex *X, CComplex *Y);


    // flag==false initializes solution to zero
    // flag==true  starts from solution of previous call
    int PBCGSolveMod(int flag);	// Precondition Biconjugate Gradient
    int PCGSQStart();
    int PBCGSolve(int flag);
    int BiCGSTAB(int flag);
    int KludgeSolve(int flag);

//		CFknDlg *TheView;

private:

};

#endif
