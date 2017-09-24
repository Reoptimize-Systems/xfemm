/* Copyright 2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by
 * Linz Center of Mechatronics GmbH (LCM)
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#ifndef EPPROC_H
#define EPPROC_H

#include "CSPointVals.h"
#include "FemmReader.h"
#include "PostProcessor.h"

class ElectrostaticsPostProcessor :
        public femm::PostProcessor,
        public femm::SolutionReader
{
public:
    ElectrostaticsPostProcessor();
    virtual ~ElectrostaticsPostProcessor();
    femm::ParserResult parseSolution( std::istream &input, std::ostream &err = std::cerr ) override;
    bool OpenDocument( std::string solutionFile ) override;

    const femmsolver::CSElement *getMeshElement(int idx) const override;
    const femmsolver::CSMeshNode *getMeshNode(int idx) const override;

    bool getPointValues(double x, double y, femm::CSPointVals &u) const;
    void getPointValues(double x, double y, double k, femm::CSPointVals &u) const;
private:
    /**
     * @brief AECF
     * Computes the permeability correction factor for axisymmetric
     * external regions.  This is sort of a kludge, but it's the best
     * way I could fit it in.  The structure of the code wasn't really
     * designed to have a permeability that varies with position in a
     * continuous way.
     * @param k
     * @return
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::AECF(int)}
     * \endinternal
     */
    double AECF(int k) const ;
    /**
     * @brief AECF
     * Correction factor for a point within the element, rather than
     * for the center of the element.
     * @param k
     * @param p
     * @return
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::AECF(int,CComplex)}
     * \endinternal
     */
    double AECF(int k, CComplex p) const;
    /**
     * @brief GetElementD
     * @param k
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::GetElementD(int)}
     * \endinternal
     */
    void GetElementD(int k);

    /**
     * @brief getPointD
     * @param x
     * @param y
     * @param D
     * @param elm a reference to the element that contains the point of interest.
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::GetPointD()}
     * \endinternal
     */
    void getPointD(double x, double y, CComplex &D, const femmsolver::CSElement &elm) const;

    double A_High;
    double A_Low;
    double A_lb;
    double A_ub;
};

#endif
