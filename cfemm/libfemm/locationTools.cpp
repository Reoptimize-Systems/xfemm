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

#include <iostream>

#ifdef XFEMM_HAVE_STAT
// room for improvement: stat instead of open
#else
# include <fstream>
#endif

#ifdef DEBUG_LOCATION_TOOLS
#define debug std::cerr
#else
#define debug while(false) std::cerr
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
    switch (type) {
    case LocationType::SystemData:
    {
#ifdef WIN32
        // default install is "%ProgramFiles%\xfemm"
        std::string dirs = getEnv("ProgramFiles", "");
        dirs += "\\xfemm";
#else
        std::string dirs = getEnv("XDG_DATA_DIRS", "/usr/local/share/:/usr/share/");
#endif
        splitInto( result, dirs, pathSeparator());
    }
        /* FALLTHRU */
    case LocationType::UserData:
#ifdef WIN32
        result.insert(result.begin(), getEnv("APPDATA", ""));
        result.insert(result.begin(), getEnv("LOCALAPPDATA", ""));
#else
        const std::string homeDir = getEnv("HOME","");
        result.insert(result.begin(), getEnv("XDG_DATA_HOME", homeDir+"/.local/share"));
#endif
        break;
    }
    return result;
}

std::string location::locateFile(location::LocationType type, const std::string &appName, const std::string &path)
{
    for (std::string filename: baseDirectories(type))
    {
        filename = filename + directorySeparator() + appName + directorySeparator() + path;
        debug << "locateFile: " << filename << "\n";
        if (fileExists(filename))
        {
            debug << "locateFile: *exists*\n";
            return filename;
        }
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

bool location::fileExists(const std::string &path)
{
#ifdef XFEMM_HAVE_STAT
#else
    // fall-back in case there's no usable stat
    std::ifstream ifs;
    ifs.open(path);
    return ifs.good();
#endif
}

// vi:expandtab:tabstop=4 shiftwidth=4:
