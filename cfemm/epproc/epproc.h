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

class ElectrostaticsPostProcessor
        : public femm::PostProcessor
        , public femm::SolutionReader
{
public:
    ElectrostaticsPostProcessor();
    virtual ~ElectrostaticsPostProcessor();
    femm::ParserResult parseSolution( std::istream &input, std::ostream &err = std::cerr ) override;
    bool OpenDocument( std::string solutionFile ) override;

    /**
     * @brief  Calculate a block integral for the selected blocks.
     *
     * intType | Integral type
     * --------|--------------
     * 0       | Stored Energy
     * 1       | Block Cross-section
     * 2       | Block Volume
     * 3       | Average D over the block
     * 4       | Average E over the block
     * 5       | Weighted Stress Tensor Force
     * 6       | Weighted Stress Tensor Torque
     *
     * @param inttype
     * @return the result of the computation
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::BlockIntegral(int)}
     * \endinternal
     */
    CComplex blockIntegral(int inttype) const;
    void clearSelection() override;

    const femmsolver::CHSElement *getMeshElement(int idx) const override;
    const femmsolver::CSMeshNode *getMeshNode(int idx) const override;

    bool getPointValues(double x, double y, femm::CSPointVals &u) const;
    void getPointValues(double x, double y, int k, femm::CSPointVals &u) const;

    bool isSelectionOnAxis() const override;

    /**
     * @brief Calculate the line integral for the defined contour.
     * intType | results[0] | results[1]
     * --------|------------|-----------
     *    0    | E.t        | n/a
     *    1    | D.n        | average D.n over the surface
     *    2    | Contour length       | Contour area
     *    3    | Stress Tensor Force  | Stress Tensor Force (imaginary part)
     *    4    | Stress Tensor Torque | n/a
     * @param intType
     * @param results
     */
    void lineIntegral(int intType, double (&results)[2]) const;
private:
    /**
     * @brief Calculate the average electric field density for the given element.
     * @param k
     * @return the field density
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::E(int)}
     * \endinternal
     */
    CComplex E(const femmsolver::CHSElement *elem) const;

    /**
     * @brief GetElementD
     * Compute and fill in the D value of a mesh element.
     * @param k
     * \internal
     * - \femm42{femm/belaviewDoc.cpp,CbelaviewDoc::GetElementD(int)}
     * \endinternal
     */
    void getElementD(int k);

    double A_High;
    double A_Low;
    double A_lb;
    double A_ub;
};

#endif
