/* Copyright 2017 Johannes Zarl-Zierl <johannes@zarl-zierl.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of
 * Mechatronics GmbH (LCM)
 * Copyright 1998-2016 David Meeker <dmeeker@ieee.org>
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

#include "LuaCommonCommands.h"

#include "CPointVals.h"
#include "femmenums.h"
#include "FemmState.h"
#include "LuaInstance.h"

#include <lua.h>

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifdef DEBUG_FEMMLUA
#define debug std::cerr
#else
#define debug while(false) std::cerr
#endif

using namespace femm;
using std::swap;

/**
 * @brief Unselect all selected nodes, blocks, segments and arc segments.
 * @param L
 * @return 0
 * \ingroup LuaCommon
 *
 * \internal
 * ### Implements:
 * - \lua{mi_clearselected()}
 * - \lua{ei_clearselected()}
 *
 * ### FEMM source:
 * - \femm42{femm/femmeLua.cpp,lua_clearselected()}
 * - \femm42{femm/beladrawLua.cpp,lua_clearselected()}
 * \endinternal
 */
int femmcli::LuaCommonCommands::luaClearSelected(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());

    femmState->getMesher()->UnselectAll();
    return 0;
}
