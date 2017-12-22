/* Copyright 2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
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

#ifndef LIBFEMM_CLITOOLS_H
#define LIBFEMM_CLITOOLS_H

#include <string>

namespace femmutils {

/**
 * @brief splitArg splits a c string into two parts and appends it to two std::strings.
 * @param cstr the source c string (e.g. "--arg=value")
 * @param arg an existing string where the first part of cstr (e.g. "--arg") is appended.
 * @param value an existing string where the second part of cstr (e.g. "value") is appended.
 */
void splitArg(const char cstr[], std::string &arg, std::string &value);

} //namespace

#endif
