#include "FemmProblem.h"

#include "femmconstants.h"

#include <ctgmath>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>

femm::FemmProblem::~FemmProblem()
{
}

bool femm::FemmProblem::saveFEMFile(std::string &filename) const
{
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
    // when filling to a width, adjust to the left
    fem.setf(std::ios::left);
    // width and fill-direction are only used to emulate the output of femm42 as close as possible.

    // ***** write properties ****
    fem << "[Format]      =  4.0\n";
    fem.width(12);
    fem << "[Frequency]" << "  =  " << Frequency << "\n";
    fem.width(12);
    fem << "[Precision]" << "  =  " << Precision << "\n";
    fem.width(12);
    fem << "[MinAngle]" << "  =  " << MinAngle << "\n";
    fem.width(12);
    fem << "[Depth]" << "  =  " << Depth << "\n";
    fem << "[LengthUnits] =  ";
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
        fem << "[ProblemType] =  planar\n";
    } else {
        fem << "[ProblemType] =  axisymmetric\n";
        if ( extRo != 0 && extRi != 0)
        {
            fem.width(12);
            fem << "[extZo]" << "  =  " << extZo << "\n";
            fem.width(12);
            fem << "[extRo]" << "  =  " << extRo << "\n";
            fem.width(12);
            fem << "[extRi]" << "  =  " << extRi << "\n";
        }
    }
    if (Coords == femm::CART)
    {
        fem << "[Coordinates] =  cartesian\n";
    } else {
        fem << "[Coordinates] =  polar\n";
    }

    fem.width(12);
    fem << "[ACSolver]" << "  =  " << ACSolver <<"\n";
    if (!PrevSoln.empty())
    {
        fem.width(12);
        fem << "[PrevSoln]" << "  = \"" << PrevSoln <<"\"\n";
    }

    std::string commentString (comment);
    // escape line-breaks
    size_t pos = commentString.find('\n');
    while (pos != std::string::npos)
        commentString.replace(pos,1,"\\n");
    fem.width(12);
    fem << "[Comment]" << "  =  \"" << commentString <<"\"\n";

    // ***** write data ****
    fem.width(12);
    fem << "[PointProps]" << "  =  " << nodeproplist.size() <<"\n";
    for( const auto &prop: nodeproplist)
    {
        prop->toStream(fem);
    }

    fem.width(12);
    fem << "[BdryProps]" << "  = " << lineproplist.size() <<"\n";
    for( const auto &prop: lineproplist)
    {
        prop->toStream(fem);
    }

    fem.width(12);
    fem << "[BlockProps]" << "  = " << blockproplist.size() <<"\n";
    for( const auto &prop: blockproplist)
    {
        prop->toStream(fem);
    }

    std::string circuitHeader;
    if (filetype==MagneticsFile)
        circuitHeader =  "[CircuitProps]";
    else if (filetype==HeatFlowFile)
        circuitHeader =  "[ConductorProps]";
    fem.width(12);
    fem << circuitHeader << "  = " << circproplist.size() <<"\n";
    for( const auto &prop: circproplist)
    {
        prop->toStream(fem);
    }

    // the solver does not even read this, but write it anyways...
    fem << "[NumPoints] = " << nodelist.size() <<"\n";
    for (const auto &node: nodelist)
    {
        fem << node->x << "\t" << node->y
            << "\t" << node->BoundaryMarker+1 << "\t" << node->InGroup;

        if (filetype == HeatFlowFile)
        {
            fem << "\t" << node->InConductor+1;
        }
        fem << "\n";
    }
    // the solver does not even read this, but write it anyways...
    fem << "[NumSegments] = " << linelist.size() <<"\n";
    for (const auto &line: linelist)
    {
        // n0 n1 MaxSideLength BoundaryMarker Hidden Group [InConductor]
        fem << line->n0 << "\t" << line->n1;
        if (line->MaxSideLength < 0)
        {
            fem << "\t-1";
        } else {
            fem << "\t" << line->MaxSideLength;
        }

        fem << "\t" << line->BoundaryMarker+1
            << "\t" << (int)line->Hidden
            << "\t" << line->InGroup;

        if (filetype == HeatFlowFile)
        {
            fem << "\t" << line->InConductor+1;
        }
        fem << "\n";
    }
    // the solver does not even read this, but write it anyways...
    fem << "[NumArcSegments] = " << arclist.size() <<"\n";
    for (const auto &arc: arclist)
    {
        // n0 n1 ArcLength MaxSideLength BoundaryMarker Hidden Group [InConductor]
        fem << arc->n0 << "\t" << arc->n1
            << "\t" << arc->ArcLength << "\t" << arc->MaxSideLength
            << "\t" << arc->BoundaryMarker+1
            << "\t" << (int)arc->Hidden
            << "\t" << arc->InGroup;


        if (filetype == HeatFlowFile)
        {
            fem << "\t" << arc->InConductor+1;
        }
        fem << "\n";
    }
    // write out list of holes;
    int numHoles=0;
    for (const auto &label: labellist)
    {
        // count holes
        if(!label->hasBlockType())
            numHoles++;
    }
    fem << "[NumHoles] = " << numHoles << "\n";
    for (const auto &label: labellist)
    {
        if(!label->hasBlockType())
            fem << label->x << " " << label->y << " " << label->InGroup << "\n";
    }

    fem << "[NumBlockLabels] = " << labellist.size()-numHoles << "\n";
    for (const auto &label: labellist)
    {
        if(label->hasBlockType())
        {
            label->toStream(fem);
        }
    }

    return true;
}

void femm::FemmProblem::updateLabelsFromIndex()
{
    // block labels
    for (auto &label: labellist)
    {
        if (label->hasBlockType())
            label->BlockTypeName = blockproplist[label->BlockType]->BlockName;
        if (label->isInCircuit())
            label->InCircuitName = circproplist[label->InCircuit]->CircName;
    }
    // points
    for (auto &node: nodelist)
    {
        if ( node->hasBoundaryMarker())
            node->BoundaryMarkerName = nodeproplist[node->BoundaryMarker]->PointName;
        if ( node->isInConductor())
            node->InConductorName = circproplist[node->InConductor]->CircName;
    }
    // segments
    for (auto &segm: linelist)
    {
        if (segm->hasBoundaryMarker())
            segm->BoundaryMarkerName = lineproplist[segm->BoundaryMarker]->BdryName;
        if (segm->isInConductor())
            segm->InConductorName = circproplist[segm->InConductor]->CircName;
    }
    // arc segments
    for (auto &asegm: arclist)
    {
        if (asegm->hasBoundaryMarker())
            asegm->BoundaryMarkerName = lineproplist[asegm->BoundaryMarker]->BdryName;
        if (asegm->isInConductor())
            asegm->InConductorName = circproplist[asegm->InConductor]->CircName;
    }
}

void femm::FemmProblem::updateBlockMap()
{
    int idx=0;
    blockMap.clear();
    for (const auto &bprop: blockproplist)
    {
        blockMap[bprop->BlockName] = idx++;
    }
}

void femm::FemmProblem::updateCircuitMap()
{
    int idx=0;
    circuitMap.clear();
    for (const auto &circuit: circproplist)
    {
        circuitMap[circuit->CircName] = idx++;
    }
}

void femm::FemmProblem::updateLineMap()
{
    int idx=0;
    lineMap.clear();
    for (const auto &bprop: lineproplist)
    {
        lineMap[bprop->BdryName] = idx++;
    }
}

void femm::FemmProblem::updateNodeMap()
{
    int idx=0;
    nodeMap.clear();
    for (const auto &pprop: nodeproplist)
    {
        nodeMap[pprop->PointName] = idx++;
    }
}

void femm::FemmProblem::invalidateMesh()
{
    meshnodes.clear();
    meshelems.clear();
}

bool femm::FemmProblem::isMeshed() const
{
    return !(meshelems.empty() && meshnodes.empty());
}

bool femm::FemmProblem::consistencyCheckOK() const
{
    using std::to_string;
    bool ok = true;
    // block labels
    for (const auto &label: labellist)
    {
        if (label->hasBlockType())
        {
            int idx = label->BlockType;
            if ( idx < 0 || idx >= (int)blockproplist.size() )
            {
                std::cerr << "Label has invalid block type index: " << to_string(idx)
                          << " (0 <= index < " << to_string(blockproplist.size()) << ")\n";
                ok = false;
            } else if (label->BlockTypeName != blockproplist[idx]->BlockName)
            {
                std::cerr << "Label has inconsistent block type name: " << label->BlockTypeName
                          << ", but block type " << to_string(idx)
                          << " has name " << blockproplist[idx]->BlockName << "\n";
                ok = false;
            }
        }
        if (label->isInCircuit())
        {
            int idx = label->InCircuit;
            if ( idx < 0 || idx >= (int)circproplist.size() )
            {
                std::cerr << "Label has invalid circuit index: " << to_string(idx)
                          << " (0 <= index < " << to_string(circproplist.size()) << ")\n";
                ok = false;
            } else if (label->InCircuitName != circproplist[idx]->CircName)
            {
                std::cerr << "Label has inconsistent circuit name: " << label->InCircuitName
                          << ", but circuit " << to_string(idx)
                          << " has name " << circproplist[idx]->CircName << "\n";
                ok = false;
            }
        }
    }
    // points
    for (const auto &node: nodelist)
    {
        if (node->hasBoundaryMarker())
        {
            int idx = node->BoundaryMarker;
            if ( idx < 0 || idx >= (int)nodeproplist.size() )
            {
                std::cerr << "Point has invalid boundary marker index: " << to_string(idx)
                          << " (0 <= index < " << to_string(nodeproplist.size()) << ")\n";
                ok = false;
            } else if (node->BoundaryMarkerName != nodeproplist[idx]->PointName)
            {
                std::cerr << "Point has inconsistent boundary marker name: " << node->BoundaryMarkerName
                          << ", but boundary marker " << to_string(idx)
                          << " has name " << nodeproplist[idx]->PointName << "\n";
                ok = false;
            }
        }
        if (node->isInConductor())
        {
            int idx = node->InConductor;
            if ( idx < 0 || idx >= (int)circproplist.size() )
            {
                std::cerr << "Point has invalid conductor index: " << to_string(idx)
                          << " (0 <= index < " << to_string(circproplist.size()) << ")\n";
                ok = false;
            } else if (node->InConductorName != circproplist[idx]->CircName)
            {
                std::cerr << "Point has inconsistent conductor name: " << node->InConductorName
                          << ", but conductor " << to_string(idx)
                          << " has name " << circproplist[idx]->CircName << "\n";
                ok = false;
            }
        }
    }
    // segments
    for (const auto &segm: linelist)
    {
        if (segm->hasBoundaryMarker())
        {
            int idx = segm->BoundaryMarker;
            if ( idx < 0 || idx >= (int)lineproplist.size() )
            {
                std::cerr << "Line has invalid boundary marker index: " << to_string(idx)
                          << " (0 <= index < " << to_string(lineproplist.size()) << ")\n";
                ok = false;
            } else if (segm->BoundaryMarkerName != lineproplist[idx]->BdryName)
            {
                std::cerr << "Line has inconsistent boundary marker name: " << segm->BoundaryMarkerName
                          << ", but boundary marker " << to_string(idx)
                          << " has name " << lineproplist[idx]->BdryName << "\n";
                ok = false;
            }
        }
        if (segm->isInConductor())
        {
            int idx = segm->InConductor;
            if ( idx < 0 || idx >= (int)circproplist.size() )
            {
                std::cerr << "Point has invalid circuit index: " << to_string(idx)
                          << " (0 <= index < " << to_string(circproplist.size()) << ")\n";
                ok = false;
            } else if (segm->InConductorName != circproplist[idx]->CircName)
            {
                std::cerr << "Point has inconsistent circuit name: " << segm->InConductorName
                          << ", but circuit " << to_string(idx)
                          << " has name " << circproplist[idx]->CircName << "\n";
                ok = false;
            }
        }
    }
    // arc segments
    for (const auto &asegm: arclist)
    {
        if (asegm->hasBoundaryMarker())
        {
            int idx = asegm->BoundaryMarker;
            if ( idx < 0 || idx >= (int)lineproplist.size() )
            {
                std::cerr << "Line has invalid boundary marker index: " << to_string(idx)
                          << " (0 <= index < " << to_string(lineproplist.size()) << ")\n";
                ok = false;
            } else if (asegm->BoundaryMarkerName != lineproplist[idx]->BdryName)
            {
                std::cerr << "Line has inconsistent boundary marker name: " << asegm->BoundaryMarkerName
                          << ", but boundary marker " << to_string(idx)
                          << " has name " << lineproplist[idx]->BdryName << "\n";
                ok = false;
            }
        }
        if (asegm->isInConductor())
        {
            int idx = asegm->InConductor;
            if ( idx < 0 || idx >= (int)circproplist.size() )
            {
                std::cerr << "Point has invalid circuit index: " << to_string(idx)
                          << " (0 <= index < " << to_string(circproplist.size()) << ")\n";
                ok = false;
            } else if (asegm->InConductorName != circproplist[idx]->CircName)
            {
                std::cerr << "Point has inconsistent circuit name: " << asegm->InConductorName
                          << ", but circuit " << to_string(idx)
                          << " has name " << circproplist[idx]->CircName << "\n";
                ok = false;
            }
        }
    }
    return ok;
}

bool femm::FemmProblem::getBoundingBox(double (&x)[2], double (&y)[2]) const
{
    if (nodelist.size()<2)
        return false;

    // initial estimate
    x[0]=nodelist[0]->x;  x[1]=nodelist[0]->x;
    y[0]=nodelist[0]->y;  y[1]=nodelist[0]->y;
    // expand to fit
    for (const auto &node: nodelist)
    {
        if(node->x<x[0]) x[0]=node->x;
        if(node->x>x[1]) x[1]=node->x;
        if(node->y<y[0]) y[0]=node->y;
        if(node->y>y[1]) y[1]=node->y;
    }

    for (const auto &label: labellist)
    {
        if(label->x<x[0]) x[0]=label->x;
        if(label->x>x[1]) x[1]=label->x;
        if(label->y<y[0]) y[0]=label->y;
        if(label->y>y[1]) y[1]=label->y;
    }

    // arcs can "curve" outside of the bounding box defined by their endpoints
    for (const auto &arc: arclist)
    {
        int k=(int) ceil(arc->ArcLength/arc->MaxSideLength);
        double dt = arc->ArcLength*PI/(((double) k)*180.);
        CComplex c;
        double R;
        getCircle(*arc,c,R);
        CComplex p(nodelist[arc->n0]->x, nodelist[arc->n0]->y);
        CComplex s=exp(I*dt);
        for(int j=0; j<k; j++)
        {
            p=(p-c)*s+c;
            if(p.re<x[0]) x[0]=p.re;
            if(p.re>x[1]) x[1]=p.re;
            if(p.im<y[0]) y[0]=p.im;
            if(p.im>y[1]) y[1]=p.im;
        }
    }

    return true;
}

void femm::FemmProblem::getCircle(const femm::CArcSegment &arc, CComplex &c, double &R) const
{
    CComplex a0(nodelist[arc.n0]->x,nodelist[arc.n0]->y);
    CComplex a1(nodelist[arc.n1]->x,nodelist[arc.n1]->y);
    double d=abs(a1-a0);        // distance between arc endpoints

    // figure out what the radius of the circle is...
    CComplex t=(a1-a0)/d;
    double tta=arc.ArcLength*PI/180.;

    R=d/(2.*sin(tta/2.));
    c=a0 + (d/2. + I*sqrt(R*R-d*d/4.))*t; // center of the arc segment's circle...
}

std::string femm::FemmProblem::getTitle() const
{
    return pathName;
}

femm::FemmProblem::FemmProblem(FileType ftype)
    : FileFormat(-1)
    , Frequency(0.0)
    , Precision(1.e-08)
    , MinAngle(DEFAULT_MINIMUM_ANGLE)
    , Depth(1)
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
    , blockMap()
    , circuitMap()
{}
