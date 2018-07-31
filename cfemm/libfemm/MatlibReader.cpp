/* Copyright 2018 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
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

#include "MatlibReader.h"

#include "CMaterialProp.h"
#include "make_unique.h"
#include "stringTools.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace femm;

MatlibParseResult MatlibReader::parse(const std::string &libraryFile, std::ostream &err, const std::string &filter)
{
    std::ifstream input;
    input.open(libraryFile.c_str(), std::ifstream::in);
    if (!input.is_open())
    {
        err << "Couldn't open " + libraryFile + "\n";
        return MatlibParseResult::FileError;
    }

    std::stringstream err_internal;
    while (input && err_internal.str().empty())
    {
        std::string line;
        std::getline(input, line);
        trim(line);
        if (line.empty())
            continue;
        to_lower(line);
        if ( begins_with(line,"<beginfolder>")
             || begins_with(line, "<foldername>")
             || begins_with(line, "<folderurl>")
             || begins_with(line, "<foldervendor>")
             || begins_with(line, "<endfolder>"))
            continue;

        if ( line != "<beginblock>" )
        {
            err << "Invalid material library file: expected '<beginblock>', but got '"
                << line << "'!\n";
            return MatlibParseResult::ParseFolderError;
        }
        std::unique_ptr<CMaterialProp> prop;
        // in .fem files, material properties are identified by context;
        // in matlib.dat files, we need to read the beginBlock line, requiring the fromStream method to go without that line.
        prop = MAKE_UNIQUE<CMSolverMaterialProp>(CMSolverMaterialProp::fromStream(input, err_internal, PropertyParseMode::NoBeginBlock));
        if ( ! err_internal.str().empty() )
        {
            err << err_internal.str();
            return  MatlibParseResult::ParseMaterialError;
        }
        if (filter.empty() || prop->BlockName == filter)
        {
            m_library[prop->BlockName] = std::move(prop);
        }
    }
    return  MatlibParseResult::OK;
}

const CMaterialProp *MatlibReader::getMaterial(const std::string &materialName) const
{
    const auto entry = m_library.find(materialName);
    if (entry == m_library.end())
        return nullptr;
    else
        return entry->second.get();
}

CMaterialProp *MatlibReader::takeMaterial(const std::string &materialName)
{
    auto entry = m_library.find(materialName);
    if (entry == m_library.end())
        return nullptr;
    else
    {
        CMaterialProp *result = entry->second.release();
        m_library.erase(entry);
        return result;
    }
}

// vi:expandtab:tabstop=4 shiftwidth=4:
