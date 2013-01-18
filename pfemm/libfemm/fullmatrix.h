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

#ifndef FULLMATRIX_H
#define FULLMATRIX_H
class CFullMatrix{

	public:

		// data members
		double **M;			// Matrix on LHS
		double *b;			// vector on RHS
		long n;				// dimension of the matrix

		// member functions
		CFullMatrix();
		CFullMatrix(int d);
		~CFullMatrix();
		void Wipe();
		int Create(int d);
		int GaussSolve();

	private:

};

class CComplexFullMatrix{

    public:

        // data members
        CComplex **M;           // Matrix on LHS
        CComplex *b;            // vector on RHS
        int n;              // dimension of the matrix

        // member functions
        CComplexFullMatrix();
        CComplexFullMatrix(int d);
        ~CComplexFullMatrix();
        void Wipe();
        int Create(int d);
        int GaussSolve();

    private:

};

#endif
