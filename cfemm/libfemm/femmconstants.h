/* Copyright 2016 Johannes Zarl-Zierl <johannes@zarl-zierl.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of
 * Mechatronics GmbH (LCM)
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>,
 * and modifications done by Richard Crozier <richard.crozier@yahoo.co.uk>
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

#ifndef LIBFEMM_CONSTANTS_H
#define LIBFEMM_CONSTANTS_H

#define DEG 0.01745329251994329576923690768
#define DEGREE 0.017453292519943295
#define eo 8.85418781762e-12  ///< epsilon0; electric permittivity constant (in vacuum)
#define Golden 0.3819660112501051517954131656
#define Ksb 5.67032e-8 ///< Stefan-Boltzmann Constant
#define muo 1.2566370614359173e-6
#define PI 3.141592653589793238462643383
#define SmallNo 1.e-14

#define DEFAULT_MINIMUM_ANGLE 30.0
#define MINANGLE_BUMP 3
#define MINANGLE_MAX  33.8
#define CLOSE_ENOUGH 1.e-06 ///< preprocessor tolerance

#endif
