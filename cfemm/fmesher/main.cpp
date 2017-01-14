#include <iostream>
#include <string.h>
#include "fmesher.h"
#include "FemmReader.h"

using namespace femm;
using namespace fmesher;

int main(int argc, char ** argv)
{

    FMesher MeshObj;
    std::string FilePath;

    if (argc < 2)
    {
        // request the file name from the user
        std::cout << "Enter file name:" << std::endl;
        getline(std::cin,FilePath);
    }
    else if(argc > 2)
    {
        std::cout << "Too many input arguments" << std::endl;
        return -4;
    }
    else
    {
        FilePath = argv[1];
    }

    // attempt to discover the file type from the file name
    MeshObj.problem->filetype = FMesher::GetFileType (FilePath);
    bool success=false;

    if (MeshObj.problem->filetype == MagneticsFile)
    {
        MagneticsReader femReader (MeshObj.problem, std::cerr);
        success = femReader.parse(FilePath);
    } else {
        std::cout << "File type not supported!" << std::endl;
    }

    if (!success)
    {
        return -1;
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
