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

#include "FemmProblem.h"
#include "fmesher.h"
#include "fsolver.h"
#include "fpproc.h"

#include <memory>

namespace femmcli
{

/**
 * @brief The FemmState class holds the various femm documents.
 *
 * Data model
 * ----------
 *
 * ### "Ideal model"
 * This is where I want to get to:
 *
 *  * The main place for data is the FemmProblem.<br/>
 *    When data is added or modified, everything happens here.
 *  * The mesher, solver, and postprocessors each use the same FemmProblem to communicate.
 *
 * ### Current state
 *
 *  * Lua code writes data directly or indirectly (e.g.
 *    via fmesher::FMesher::AddNode()) to the FemmProblem.
 *  * The FMesher directly uses the same shared FemmProblem,
 *    but stores mesh data internally and saves it to disk.
 *  * The FSolver reads and writes data from/to disk.
 *  * The FPProc post processor reads and writes data from/to disk.
 *
 * Data flow
 * ---------
 *
 * 1. Data is input through a file (LuaBaseCommands::luaOpenDocument)
 *    or using lua commands (mi_*).<br/>
 *    → the data is in memory in FemmDocument
 * 2. LuaMagneticsCommands::luaSaveDocument() explicitly, or some other methods implicitly save the data to disk.<br/>
 *    → a \c .fem file is stored
 * 3. LuaMagneticsCommands::luaAnalyze() or LuaMagneticsCommands::luaCreateMesh() mesh the data
 *    and store the result to disk.<br/>
 *    → additional mesh files (\c .edge, \c .ele, \c .node, \c .pbc) are generated
 * 4. LuaMagneticsCommands::luaAnalyze() runs the solver and stores the solution to disk.<br/>
 *    → the mesh files are removed, and a solution file \c .ans is generated
 * 5. LuaMagneticsCommands::luaLoadSolution() reads the solution file into memory.
 *    The solution data is available for lua commands (mo_*).<br/>
 *    → the data is in memory in FPProc
 */
class FemmState : public femm::FemmStateBase
{
public:
    std::shared_ptr<femm::FemmProblem> femmDocument;

    /**
     * @brief Returns the current FPProc.
     * If FPProc was not yet initialized, a new FPProc is initialized.
     * @return
     */
    std::shared_ptr<FPProc> getFPProc();

    /**
     * @brief Returns the current FMesher
     * If FMesher was not yet initialized, a new FMesher is initialized.
     * The mesher is initialized using the femmDocument as its data handle.
     *
     * Note: if FMesher::Initialize is called (e.g. via FMesher::LoadFEMFile),
     * the mesher generates a new FemmProblem that is not connected to this one.
     * In this case, calling getMesher generates a new mesher.
     * @return
     */
    std::shared_ptr<fmesher::FMesher> getMesher();

    /**
     * @brief Invalidate the solution data stored in FPProc.
     * When a new solution is available, call this method before loading it.
     */
    void invalidateSolutionData();
private:
    std::shared_ptr<FPProc> theFPProc;
    std::shared_ptr<fmesher::FMesher> theFMesher;
};

} /* namespace */

#endif /* FEMMSTATE_H */
// vi:expandtab:tabstop=4 shiftwidth=4:
