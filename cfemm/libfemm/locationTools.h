/* Copyright 2018 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of
 * Mechatronics GmbH (LCM)
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */
#ifndef FEMM_LOCATIONTOOLS_H
#define FEMM_LOCATIONTOOLS_H

#include <cassert>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>

/**
 * \file locationTools.h
 * \brief Functions to deal with standard locations in xfemm.
 * For Linux and similar platforms, these functions act according to the XDG base directory standard.
 * For other platforms, sensible defaults are used.
 *
 * ## References
 *  - https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
 *  - https://stackoverflow.com/questions/43853548/xdg-basedir-directories-for-windows
 */

namespace location
{

/**
 * @brief The LocationType enum corresponds to XDG directory types.
 * Even though not the full XDG types are defined here, if required we can easily extend this accordingly.
 *
 * Context: the XDG basedir spec defines several types of base directories (data, config, cache, ...),
 * and differentiates between preinstalled read-only defaults and user-specific (writable) locations.
 * User-specific locations have higher priority than the defaults; i.e. querying for a user directory
 * always returns a writable location, while querying for a system directory returns either the user-specific
 * location or a system location.
 */
enum class LocationType {
    UserData
    , SystemData
};


/**
 * @brief standardDirectories gives a list of standard directories suitable for the given type.
 * On Linux and unix systems, this conforms to the XDG basedir spec.
 * On Windows, a suitable location is chosen.
 * @param type
 * @return
 */
std::vector<std::string> baseDirectories( LocationType type);

/**
 * @brief directorySeparator
 * @return the directory separator for the current platform ('\\' on Windows, '/' everywhere else)
 */
constexpr char directorySeparator();

/**
 * @brief Checks if the file specified by \c path exists and is readable.
 * @param path
 * @return \c true, if the file exists, \c false otherwise
 */
bool fileExists( const std::string &path);

/**
 * @brief Searches the first matching file within the standardDirectories.
 *
 * E.g. \code getFile(LocationType::UserData, "xfemm", "debug/matlib.dat")\endcode
 * may return the string "/home/username/.share/xfemm/debug/matlib.dat" or "C:\Users\username\AppData\Local\debug\matlib.dat".
 *
 * @param type
 * @param appName the name of the application (e.g. "xfemm")
 * @param path the file name relative to the base directory
 * @return a file name, or an empty string if the file does not exist
 */
std::string locateFile( LocationType type, const std::string &appName, const std::string &path);

/**
 * @brief The path separator separates different file names or path names in PATH-style environment variables.
 * @return the path separator for the current platform (';' on Windows, ':' everywhere else)
 */
constexpr char pathSeparator();

} //namespace
#endif
