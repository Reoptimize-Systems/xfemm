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
#ifndef MATLIB_READER_H
#define MATLIB_READER_H

#include <memory>
#include <string>
#include <unordered_map>

namespace femm {

class CMaterialProp;

enum class MatlibParseResult
{
    OK,
    FileError,
    ParseFolderError,
    ParseMaterialError
};

/**
 * @brief The MatlibReader class can parse matlib.dat-style files.
 * Folders are handled, although currently the folder properties are discarded while parsing.
 */
class MatlibReader
{
public:
    /**
     * @brief parse the given material library file and optionally apply a filter.
     * Before parsing, any existing material data is cleared.
     *
     * If a filter is given, only the matching material is stored while other materials are discarded.
     * @param libraryFile a file name
     * @param filter the name of a material
     * @return
     */
    MatlibParseResult parse(const std::string &libraryFile, std::ostream &err, const std::string &filter="");

    /**
     * @brief Get the material matching \c materialName.
     * @param materialName
     * @return the material, or \c nullptr if no matching material exists.
     */
    const CMaterialProp *getMaterial(const std::string &materialName) const;
    /**
     * @brief Get the material matching \c materialName and remove it afterwards.
     * @param materialName
     * @return
     */
    CMaterialProp *takeMaterial(const std::string &materialName);
private:
    std::unordered_map<std::string,std::unique_ptr<CMaterialProp>> m_library;
};
}

#endif
// vi:expandtab:tabstop=4 shiftwidth=4:
