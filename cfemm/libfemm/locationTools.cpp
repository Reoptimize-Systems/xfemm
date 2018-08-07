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
#include "locationTools.h"

namespace {

#ifdef WIN32
//constexpr char DIRECTORY_SEPARATOR = '\\';
constexpr char PATH_SEPARATOR = ';';
#else
//constexpr char DIRECTORY_SEPARATOR = '/';
constexpr char PATH_SEPARATOR = ':';
#endif

/**
 * @brief get the value of an environment variable or a fallback if not set.
 * @param envVar the name of the environment variable
 * @param fallback
 * @return
 */
std::string getEnv(const char* envVar, const std::string &fallback)
{
    char *value = std::getenv(envVar);
    if (value)
        return std::string( value );
    else
        return fallback;
}

/**
 * @brief Split a string into a vector of substrings using a separator
 * Empty parts are ignored.
 * @param destination
 * @param text
 * @param sep
 */
void splitInto( std::vector<std::string> &destination, const std::string &text, char sep )
{
    std::istringstream stream(text);
    std::string part;
    while (std::getline(stream, part, sep))
        if (! part.empty())
            destination.push_back(part);
}
} // namespace


std::vector<std::string> location::standardDirectories(location::LocationType type)
{
    std::vector<std::string> result;
    const std::string homeDir = getEnv("HOME","");
    switch (type) {
    case LocationType::SystemData:
    {
        std::string dirs = getEnv("XDG_DATA_DIRS", "/usr/local/share/:/usr/share/");
        splitInto( result, dirs, PATH_SEPARATOR);
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
