/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
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

#ifndef FEMMSTATE_H
#define FEMMSTATE_H

#include "FemmStateBase.h"
#include "fsolver.h"
#include "fpproc.h"
#include <memory>

namespace femmcli
{

/**
 * @brief The FemmState class holds the various femm documents.
 */
class FemmState : public femm::FemmStateBase
{
public:
    std::shared_ptr<FSolver> fSolverDocument;
    std::shared_ptr<FPProc> fPProcDocument;
};

} /* namespace */

#endif /* FEMMSTATE_H */
// vi:expandtab:tabstop=4 shiftwidth=4:
