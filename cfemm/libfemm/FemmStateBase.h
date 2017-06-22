/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
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

#ifndef FEMMSTATEBASE_H
#define FEMMSTATEBASE_H

namespace femm
{
/**
 * @brief The FemmStateBase class holds the various femm documents.
 *
 * Having this class allows us to have the core Lua functionality in libfemm, while
 * we can extend the class so we can store various document handles not known to LuaInstance.
 */
class FemmStateBase
{
public:
    virtual ~FemmStateBase();
};

} /* namespace */

#endif /* FEMMSTATEBASE_H */
// vi:expandtab:tabstop=4 shiftwidth=4:
