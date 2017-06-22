/* Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of
 * Mechatronics GmbH (LCM)
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

#include "FemmState.h"

#include "fmesher.h"
#include "fpproc.h"

#include <memory>


const std::shared_ptr<femm::FemmProblem> femmcli::FemmState::femmDocument()
{
    return current.document;
}

void femmcli::FemmState::setDocument(std::shared_ptr<femm::FemmProblem> doc)
{
    // invalidate current state
    close();
    current.document = doc;
}

const std::shared_ptr<FPProc> femmcli::FemmState::getFPProc()
{
    if (!current.postprocessor)
    {
        current.postprocessor = std::make_shared<FPProc>();
    }
    return current.postprocessor;
}

const std::shared_ptr<fmesher::FMesher> femmcli::FemmState::getMesher()
{
    if (!current.mesher || current.mesher->problem != current.document)
    {
        current.mesher = std::make_shared<fmesher::FMesher>(current.document);
    }
    return current.mesher;
}

void femmcli::FemmState::invalidateSolutionData()
{
    current.postprocessor.reset();
}

void femmcli::FemmState::close()
{
    current.document.reset();
    current.mesher.reset();
    current.postprocessor.reset();
}

void femmcli::FemmState::deactivateProblemSet()
{
    inactiveMagneticsProblems.push_back(current);
    current = MagneticsProblemSet();
}

bool femmcli::FemmState::activateProblemSet(const std::string &title)
{
    for (auto it=inactiveMagneticsProblems.cbegin()
         , end = inactiveMagneticsProblems.cend()
         ; it != end
         ; ++it)
    {
        if (it->document->getTitle() == title)
        {
            // deactivate current problem set
            inactiveMagneticsProblems.push_back(current);
            // activate problem set
            current = *it;
            inactiveMagneticsProblems.erase(it);
            return true;
        }
    }
    return false;
}

bool femmcli::FemmState::isValid() const
{
    return (nullptr != current.document.get());
}
