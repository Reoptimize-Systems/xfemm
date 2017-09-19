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

#include "FemmReader.h"
#include "PostProcessor.h"

class ElectrostaticsPostProcessor :
        public femm::PostProcessor,
        public femm::SolutionReader
{
public:
    ElectrostaticsPostProcessor();
    virtual ~ElectrostaticsPostProcessor();
    virtual femm::ParserResult parseSolution( std::istream &input, std::ostream &err = std::cerr ) override;
};

#endif
