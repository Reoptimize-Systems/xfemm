#include <iostream>
#include "stdio.h"
#include "stdstring.h"
#include "fmesher.h"


int main(int argc, char ** argv)
{

    FMesher MeshObj;
    bool MeshUpToDate;
    CStdString FilePath;
    char tempFilePath[1024];

    if (argc < 2)
    {
        // request the file name from the user
        printf("Enter fem file name:\n");

        //scanf("%s", tempFilePath);

        gets(tempFilePath);

        FilePath = tempFilePath;

    }
    else if(argc > 2)
    {
        printf("Too many arguments");
    }
    else
    {
        FilePath = argv[1];
    }

    if (MeshObj.LoadFEMFile(FilePath) == false)
    {
        //EndWaitCursor();
        return -1;
    }

    if (MeshObj.HasPeriodicBC() == true)
    {
        if (MeshObj.DoPeriodicBCTriangulation(FilePath) == false)
        {
            //EndWaitCursor();
            return -2;
        }
    }
    else
    {
        if (MeshObj.DoNonPeriodicBCTriangulation(FilePath) == false)
        {
            //EndWaitCursor();
            return -3;
        }
    }

    //bool LoadMesh = MeshObj.LoadMesh(FilePath);

    //EndWaitCursor();

    //if(LoadMesh == true){

    //    //MeshUpToDate = true;

    //    //if(MeshFlag == false)
    //    //{
    //    //    OnShowMesh();
    //    //}
    //    //else
    //    //{
    //    //    InvalidateRect(NULL);
    //    //}

    //    //CStdString s;

    //    //s.Format("Created mesh with %i nodes", MeshObj.meshnode.size());

    //    //if (TheDoc->greymeshline.size() != 0)
    //    //    s += "\nGrey mesh lines denote regions\nthat have no block label.";
    //
    //    //if(bLinehook==false)
    //    //{
    //    //    AfxMessageBox(s,MB_ICONINFORMATION);
    //    //}
    //    //else
    //    //{
    //    //    lua_pushnumber(lua,TheDoc->meshnode.size());
    //    //}

    //}

    printf("\nNo errors");
    return 0;

}
