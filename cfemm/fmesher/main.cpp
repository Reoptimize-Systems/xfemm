#include "fmesher.h"
#include "FemmReader.h"
#include "femmversion.h"

#include <triangle_version.h>

#include <iostream>
#include <string.h>
using namespace femm;
using namespace fmesher;

int main(int argc, char ** argv)
{

    std::string FilePath;
    bool writePoly = false;

    if (argc < 2)
    {
        // request the file name from the user
        std::cout << "Enter file name:" << std::endl;
        getline(std::cin,FilePath);
    }
    else {
        for(int i=1; i<argc; i++)
        {
            const std::string arg { argv[i] };

            if (arg[0] != '-')
            {
                // positional argument
                if (! FilePath.empty())
                {
                    std::cout << "Too many input arguments" << std::endl;
                    return -4;
                }
                FilePath = arg;
            } else {
                if ( arg == "--write-poly")
                    writePoly = true;
                if ( arg == "--version" )
                {
                    std::cout << "fmesher version " << FEMM_VERSION_STRING << "\n";
                    std::cout << "triangle version " << TRIANGLE_VERSION << "\n";
#ifndef NDEBUG
                    std::cout << "assertions enabled\n";
#endif
                    return 0;
                }
                if ( arg == "--help" || arg == "-h" )
                {
                    std::cout << "Usage: " << argv[0] << " [--write-poly] <femfile>\n";
                    std::cout << "       " << argv[0] << " [-h|--help] [--version]\n";
                    std::cout << "\n";
                    return 0;
                }
            }
        }
    }

    FMesher MeshObj;
    MeshObj.writePolyFiles = writePoly;
    // attempt to discover the file type from the file name
    MeshObj.problem->filetype = FMesher::GetFileType (FilePath);
    ParserResult status = F_FILE_UNKNOWN_TYPE;

    if (MeshObj.problem->filetype == FileType::MagneticsFile)
    {
        MagneticsReader femReader (MeshObj.problem, std::cerr);
        status = femReader.parse(FilePath);
    } else if (MeshObj.problem->filetype == FileType::HeatFlowFile)
    {
        HeatFlowReader fehReader (MeshObj.problem, std::cerr);
        status = fehReader.parse(FilePath);
    } else if (MeshObj.problem->filetype == FileType::ElectrostaticsFile)
    {
        ElectrostaticsReader feeReader (MeshObj.problem, std::cerr);
        status = feeReader.parse(FilePath);
    } else {
        std::cout << "File type not supported!" << std::endl;
    }

    if (status != F_FILE_OK)
    {
        return status;
    }

    if (MeshObj.HasPeriodicBC() == true)
    {
        if (MeshObj.DoPeriodicBCTriangulation(FilePath) != 0)
        {
            return -2;
        }
    }
    else
    {
        if (MeshObj.DoNonPeriodicBCTriangulation(FilePath) != 0)
        {
            return -3;
        }
    }

    std::cout << "No errors" << std::endl;
    return 0;

}
