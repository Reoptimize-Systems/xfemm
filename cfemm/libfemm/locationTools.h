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
#ifndef FEMM_STRINGTOOLS_H
#define FEMM_STRINGTOOLS_H

#include <cassert>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>

/**
 * \file locationTools.h
 * \brief Functions to deal with standard locations in xfemm.
 * For Linux, these functions act according to the XDG base directory standard.
 * For other platforms, sensible defaults are used.
 *
 * ## References
 *  - https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
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

inline std::string getEnv(const char* envVar, const std::string &fallback)
{
    char *value = std::getenv(envVar);
    if (value)
        return std::string( value );
    else
        return fallback;
}

#ifdef WIN32
constexpr char PLATFORM_SEPARATOR = ';';
#elif linux
constexpr char PLATFORM_SEPARATOR = ':';
#endif

inline void splitInto( std::vector<std::string> &destination, const std::string &text )
{
    std::istringstream stream(text);
    std::string part;
    while (std::getline(stream, part, PLATFORM_SEPARATOR))
        if (! part.empty())
            destination.push_back(part);
}

inline std::vector<std::string> standardDirectories( LocationType type)
{
    std::vector<std::string> result;
    const std::string homeDir = getEnv("HOME","~");
    switch (type) {
    case LocationType::SystemData:
    {
        std::string dirs = getEnv("XDG_DATA_DIRS", "/usr/local/share/:/usr/share/");
        splitInto( result, dirs);
    }
        /* FALL_THROUGH */
    case LocationType::UserData:
        result.insert(result.begin(), getEnv("XDG_DATA_HOME", homeDir+"/.local/.share"));
        break;
    default:
        assert(false && "Unknown LocationType!");
    }
    return result;
}

} //namespace
#endif
