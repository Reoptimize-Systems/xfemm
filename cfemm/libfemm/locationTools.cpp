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

#ifdef WIN32

#define USERDATA_ENV "LOCALAPPDATA"
#define USERDATA_DEFAULT ""
#define SYSTEMDATA_ENV "APPDATA"
#define SYSTEMDATA_DEFAULT ""
#else

#define USERDATA_ENV "XDG_DATA_HOME"
#define USERDATA_DEFAULT (homeDir+"/.local/.share")
#define SYSTEMDATA_ENV "XDG_DATA_DIRS"
#define SYSTEMDATA_DEFAULT "/usr/local/share/:/usr/share/"
#endif

namespace {

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


std::vector<std::string> location::baseDirectories(location::LocationType type)
{
    std::vector<std::string> result;
    const std::string homeDir = getEnv("HOME","");
    switch (type) {
    case LocationType::SystemData:
    {
        std::string dirs = getEnv(SYSTEMDATA_ENV, SYSTEMDATA_DEFAULT);
        splitInto( result, dirs, pathSeparator());
    }
        /* FALLTHRU */
    case LocationType::UserData:
        result.insert(result.begin(), getEnv(USERDATA_ENV, USERDATA_DEFAULT));
        break;
    }
    return result;
}

std::string location::locateFile(location::LocationType type, const std::string &appName, const std::string &path)
{
    for (std::string filename: baseDirectories(type))
    {
        filename = filename + directorySeparator() + appName + directorySeparator() + path;
        // TODO: stat file, return iff found
        assert(false && "STUB" );
    }
    return "";
}

constexpr char location::pathSeparator()
{
#ifdef WIN32
    return ';';
#else
    return ':';
#endif
}

constexpr char location::directorySeparator()
{
#ifdef WIN32
    return '\\';
#else
    return '/';
#endif
}
