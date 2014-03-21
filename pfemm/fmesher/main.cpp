#include <iostream>
#include <string.h>
#include "fmesher.h"

using namespace femm;

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
    MeshObj.filetype = FMesher::GetFileType (FilePath);

    int status = MeshObj.LoadFEMFile(FilePath);

    if (status != FMesher::F_FILE_OK)
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
