#include "FemmProblem.h"

#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>

femm::FemmProblem::~FemmProblem()
{
}

bool femm::FemmProblem::saveFEMFile(std::string &filename) const
{
    std::cerr << "savin file " << filename <<"\n";
    if ( filetype == UnknownFile )
    {
        // can't save if file type is not set
        std::cerr << "Cannot save file because file type is unknown!\n";
        return false;
    }
    std::ofstream fem (filename);
    if ( fem.fail() )
    {
        // open failed
        std::cerr << "Opening file " << filename << " for writing failed!\n";
        return false;
    }

    // set floating point precision once for the whole stream
    fem << std::setprecision(17);

    // ***** write properties ****
    fem << "[Format] = 4.0\n";
    fem << "[Frequency] = " << Frequency << "\n";
    fem << "[Precision] = " << Precision << "\n";
    fem << "[MinAngle] = " << MinAngle << "\n";
    fem << "[Depth] = " << Depth << "\n";
    fem << "[LengthUnits] = ";
    switch (LengthUnits) {
    case femm::LengthMillimeters:
        fem << "millimeters\n";
        break;
    case femm::LengthCentimeters:
        fem << "centimeters\n";
        break;
    case femm::LengthMeters:
        fem << "meters\n";
        break;
    case femm::LengthMils:
        fem << "mils\n";
        break;
    case femm::LengthMicrometers:
        fem << "microns\n";
        break;
    case femm::LengthInches:
        fem << "inches\n";
        break;
    }

    if (ProblemType == femm::PLANAR)
    {
        fem << "[ProblemType] = planar\n";
    } else {
        fem << "[ProblemType] = axisymmetric\n";
        if ( extRo != 0 && extRi != 0)
        {
            fem << "[extZo]" << extZo << "\n";
            fem << "[extRo]" << extRo << "\n";
            fem << "[extRi]" << extRi << "\n";
        }
    }
    if (Coords == femm::CART)
    {
        fem << "[Coordinates] = cartesian\n";
    } else {
        fem << "[Coordinates] = polar\n";
    }

    fem << "[ACSolver] = " << ACSolver <<"\n";
    fem << "[PrevSoln] = " << PrevSoln <<"\n";
    fem << "[PrevSoln] = " << PrevSoln <<"\n";

    std::string commentString (comment);
    // escape line-breaks
    size_t pos = commentString.find('\n');
    while (pos != std::string::npos)
        commentString.replace(pos,1,"\\n");
    fem << "[Comment] = \"" << commentString <<"\"\n";

    // ***** write data ****
    fem << "[PointProps] = " << nodeproplist.size() <<"\n";
    for( const auto &prop: nodeproplist)
    {
        prop->toStream(fem);
    }

    fem << "[BdryProps] = " << lineproplist.size() <<"\n";
    for( const auto &prop: lineproplist)
    {
        prop->toStream(fem);
    }

    fem << "[BlockProps] = " << blockproplist.size() <<"\n";
    for( const auto &prop: blockproplist)
    {
        prop->toStream(fem);
    }

    std::string circuitHeader;
    if (filetype==MagneticsFile)
        circuitHeader = "[CircuitProps]";
    else if (filetype==HeatFlowFile)
        circuitHeader = "[ConductorProps]";
    fem << circuitHeader << " = " << circproplist.size() <<"\n";
    for( const auto &prop: circproplist)
    {
        prop->toStream(fem);
    }

    // the solver does not even read this, but write it anyways...
    fem << "[NumPoints] = " << nodelist.size() <<"\n";
    for (const auto &node: nodelist)
    {
        int boundaryMarkerIdx=0;
        for (int i=0; i<(int)nodeproplist.size(); i++)
            if (nodeproplist[i]->PointName==node->BoundaryMarkerName)
            {
                // Note(ZaJ): if we don't want to save between mesh and solve, we would need to put this in a dedicated method.
                boundaryMarkerIdx=i+1;
            }

        fem << node->x << "\t" << node->y
            << "\t" << boundaryMarkerIdx << "\t" << node->InGroup;

        if (filetype == HeatFlowFile)
        {
            int conductorIdx=0;
            for (int i=0; i<(int)circproplist.size(); i++)
                if (circproplist[i]->CircName==node->InConductorName)
                {
                    // Note(ZaJ): if we don't want to save between mesh and solve, we would need to put this in a dedicated method.
                    conductorIdx=i+1;
                }
            fem << "\t" << conductorIdx;
        }
        fem << "\n";
    }
    // the solver does not even read this, but write it anyways...
    fem << "[NumSegments] = " << linelist.size() <<"\n";
    for (const auto &line: linelist)
    {
        fem << line->n0 << "\t" << line->n1;
        if (line->MaxSideLength < 0)
        {
            fem << "\t-1";
        } else {
            fem << "\t" << line->MaxSideLength;
        }

        int boundaryMarkerIdx=0;
        for (int i=0; i<(int)lineproplist.size(); i++)
            if (lineproplist[i]->BdryName==line->BoundaryMarkerName)
            {
                // Note(ZaJ): if we don't want to save between mesh and solve, we would need to put this in a dedicated method.
                boundaryMarkerIdx=i+1;
            }

        fem << "\t" << boundaryMarkerIdx
            << "\t" << (int)line->Hidden
            << "\t" << line->InGroup;

        if (filetype == HeatFlowFile)
        {
            int conductorIdx=0;
            for (int i=0; i<(int)circproplist.size(); i++)
                if (circproplist[i]->CircName==line->InConductorName)
                {
                    // Note(ZaJ): if we don't want to save between mesh and solve, we would need to put this in a dedicated method.
                    conductorIdx=i+1;
                }
            fem << "\t" << conductorIdx;
        }
        fem << "\n";
    }
    // the solver does not even read this, but write it anyways...
    fem << "[NumArcSegments] = " << arclist.size() <<"\n";
    for (const auto &arc: arclist)
    {
        int boundaryMarkerIdx=0;
        for (int i=0; i<(int)lineproplist.size(); i++)
            if (lineproplist[i]->BdryName==arc->BoundaryMarkerName)
            {
                // Note(ZaJ): if we don't want to save between mesh and solve, we would need to put this in a dedicated method.
                boundaryMarkerIdx=i+1;
            }

        fem << arc->n0 << "\t" << arc->n1
            << "\t" << arc->ArcLength << "\t" << arc->MaxSideLength
            << "\t" << boundaryMarkerIdx
            << "\t" << (int)arc->Hidden
            << "\t" << arc->InGroup;


        if (filetype == HeatFlowFile)
        {
            int conductorIdx=0;
            for (int i=0; i<(int)circproplist.size(); i++)
                if (circproplist[i]->CircName==arc->InConductorName)
                {
                    // Note(ZaJ): if we don't want to save between mesh and solve, we would need to put this in a dedicated method.
                    conductorIdx=i+1;
                }
            fem << "\t" << conductorIdx;
        }
        fem << "\n";
    }
    // write out list of holes;
    int numHoles=0;
    for (const auto &label: labellist)
    {
        // count holes
        if(label->BlockTypeName=="<No Mesh>")
            numHoles++;
    }
    fem << "[NumHoles] = " << numHoles << "\n";

    return true;
}

femm::FemmProblem::FemmProblem(FileType ftype)
    : FileFormat(-1)
    , Frequency(0.0)
    , Precision(1.e-08)
    , MinAngle(0)
    , Depth(-1)
    , LengthUnits(LengthInches)
    , Coords(CART)
    , ProblemType(PLANAR)
    , extZo(0)
    , extRo(0)
    , extRi(0)
    , comment()
    , ACSolver(0)
    , dT(0)
    , PrevSoln()
    , DoForceMaxMeshArea(false)
    , nodelist()
    , linelist()
    , arclist()
    , labellist()
    , nodeproplist()
    , lineproplist()
    , blockproplist()
    , circproplist()
    , solved(false)
    , meshnodes()
    , meshelems()
    , pathName()
    //, solutionFile()
    , filetype(ftype)
{}
