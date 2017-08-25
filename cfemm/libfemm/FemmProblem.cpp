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
    if ( filetype == FileType::Unknown )
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

    writeProblemDescription(fem);
    return true;
}

void femm::FemmProblem::writeProblemDescription(std::ostream &output) const
{
    // set floating point precision once for the whole stream
    output << std::setprecision(17);
    // when filling to a width, adjust to the left
    output.setf(std::ios::left);
    // width and fill-direction are only used to emulate the output of femm42 as close as possible.

    // ***** write properties ****
    if (filetype == FileType::MagneticsFile)
        output << "[Format]      =  4.0\n";
    else
        output << "[Format]      =  1\n";
    if (filetype != FileType::ElectrostaticsFile)
    {
        output.width(12);
        output << "[Frequency]" << "  =  " << Frequency << "\n";
    }
    output.width(12);
    output << "[Precision]" << "  =  " << Precision << "\n";
    output.width(12);
    output << "[MinAngle]" << "  =  " << MinAngle << "\n";
    output.width(12);
    output << "[Depth]" << "  =  " << Depth << "\n";
    output << "[LengthUnits] =  ";
    switch (LengthUnits) {
    case femm::LengthMillimeters:
        output << "millimeters\n";
        break;
    case femm::LengthCentimeters:
        output << "centimeters\n";
        break;
    case femm::LengthMeters:
        output << "meters\n";
        break;
    case femm::LengthMils:
        output << "mils\n";
        break;
    case femm::LengthMicrometers:
        output << "microns\n";
        break;
    case femm::LengthInches:
        output << "inches\n";
        break;
    }

    if (ProblemType == femm::PLANAR)
    {
        output << "[ProblemType] =  planar\n";
    } else {
        output << "[ProblemType] =  axisymmetric\n";
        if ( extRo != 0 && extRi != 0)
        {
            output.width(12);
            output << "[extZo]" << "  =  " << extZo << "\n";
            output.width(12);
            output << "[extRo]" << "  =  " << extRo << "\n";
            output.width(12);
            output << "[extRi]" << "  =  " << extRi << "\n";
        }
    }
    if (Coords == femm::CART)
    {
        output << "[Coordinates] =  cartesian\n";
    } else {
        output << "[Coordinates] =  polar\n";
    }

    if (filetype == FileType::MagneticsFile)
    {
        output.width(12);
        output << "[ACSolver]" << "  =  " << ACSolver <<"\n";
    }
    if (!PrevSoln.empty())
    {
        output.width(12);
        output << "[PrevSoln]" << "  = \"" << PrevSoln <<"\"\n";
    }

    std::string commentString (comment);
    // escape line-breaks
    size_t pos = commentString.find('\n');
    while (pos != std::string::npos)
        commentString.replace(pos,1,"\\n");
    output.width(12);
    output << "[Comment]" << "  =  \"" << commentString <<"\"\n";

    // ***** write data ****
    output.width(12);
    output << "[PointProps]" << "  =  " << nodeproplist.size() <<"\n";
    for( const auto &prop: nodeproplist)
    {
        prop->toStream(output);
    }

    output.width(12);
    output << "[BdryProps]" << "  = " << lineproplist.size() <<"\n";
    for( const auto &prop: lineproplist)
    {
        prop->toStream(output);
    }

    output.width(12);
    output << "[BlockProps]" << "  = " << blockproplist.size() <<"\n";
    for( const auto &prop: blockproplist)
    {
        prop->toStream(output);
    }

    std::string circuitHeader;
    if (filetype == FileType::MagneticsFile)
    {
        circuitHeader =  "[CircuitProps]";
    } else {
        circuitHeader =  "[ConductorProps]";
    }

    output.width(12);
    output << circuitHeader << "  = " << circproplist.size() <<"\n";
    for( const auto &prop: circproplist)
    {
        prop->toStream(output);
    }

    // the solver does not even read this, but write it anyways...
    output << "[NumPoints] = " << nodelist.size() <<"\n";
    for (const auto &node: nodelist)
    {
        output << node->x
               << "\t" << node->y
               << "\t" << node->BoundaryMarker+1
               << "\t" << node->InGroup;

        if ( filetype == FileType::HeatFlowFile ||
             filetype == FileType::ElectrostaticsFile )
        {
            output << "\t" << node->InConductor+1;
        }
        output << "\n";
    }
    // the solver does not even read this, but write it anyways...
    output << "[NumSegments] = " << linelist.size() <<"\n";
    for (const auto &line: linelist)
    {
        // n0 n1 MaxSideLength BoundaryMarker Hidden Group [InConductor]
        output << line->n0 << "\t" << line->n1;
        if (line->MaxSideLength < 0)
        {
            output << "\t-1";
        } else {
            output << "\t" << line->MaxSideLength;
        }

        output << "\t" << line->BoundaryMarker+1
            << "\t" << (int)line->Hidden
            << "\t" << line->InGroup;

        if ( filetype == FileType::HeatFlowFile ||
             filetype == FileType::ElectrostaticsFile )
        {
            output << "\t" << line->InConductor+1;
        }
        output << "\n";
    }
    // the solver does not even read this, but write it anyways...
    output << "[NumArcSegments] = " << arclist.size() <<"\n";
    for (const auto &arc: arclist)
    {
        // n0 n1 ArcLength MaxSideLength BoundaryMarker Hidden Group [InConductor]
        output << arc->n0 << "\t" << arc->n1
            << "\t" << arc->ArcLength << "\t" << arc->MaxSideLength
            << "\t" << arc->BoundaryMarker+1
            << "\t" << (int)arc->Hidden
            << "\t" << arc->InGroup;


        if ( filetype == FileType::HeatFlowFile ||
             filetype == FileType::ElectrostaticsFile )
        {
            output << "\t" << arc->InConductor+1;
        }
        output << "\n";
    }
    // write out list of holes;
    int numHoles=0;
    for (const auto &label: labellist)
    {
        // count holes
        if(!label->hasBlockType())
            numHoles++;
    }
    output << "[NumHoles] = " << numHoles << "\n";
    for (const auto &label: labellist)
    {
        if(!label->hasBlockType())
            output << label->x << " " << label->y << " " << label->InGroup << "\n";
    }

    output << "[NumBlockLabels] = " << labellist.size()-numHoles << "\n";
    for (const auto &label: labellist)
    {
        if(label->hasBlockType())
        {
            label->toStream(output);
        }
    }
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

// identical in fmesher, FPProc and HPProc
int femm::FemmProblem::closestArcSegment(double x, double y)
{
    if(arclist.size()==0) return -1;

    double d0=shortestDistanceFromArc(CComplex(x,y),*arclist[0]);
    int idx=0;
    for(int i=0; i<(int)arclist.size(); i++)
    {
        double d1=shortestDistanceFromArc(CComplex(x,y),*arclist[i]);
        if(d1<d0)
        {
            d0=d1;
            idx=i;
        }
    }

    return idx;
}

int femm::FemmProblem::closestBlockLabel(double x, double y)
{
    if(labellist.size()==0) return -1;

    int idx=0;
    double d0=labellist[0]->GetDistance(x,y);
    for(int i=0; i<(int)labellist.size(); i++)
    {
        double d1=labellist[i]->GetDistance(x,y);
        if(d1<d0)
        {
            d0=d1;
            idx=i;
        }
    }

    return idx;
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

int femm::FemmProblem::getArcArcIntersection(const femm::CArcSegment &arc0, const femm::CArcSegment &arc1, CComplex *p) const
{
    CComplex a0,a1,t,c0,c1;
    double d,l,R0,R1,z0,z1,c,tta0,tta1;
    int i=0;

    a0.Set(nodelist[arc0.n0]->x,nodelist[arc0.n0]->y);
    a1.Set(nodelist[arc1.n0]->x,nodelist[arc1.n0]->y);

    getCircle(arc1,c1,R1);
    getCircle(arc0,c0,R0);

    d=abs(c1-c0);			// distance between centers

    if ((d>R0+R1) || (d<1.e-08)) return 0;
    // directly eliminate case where there can't
    // be any crossings....

    l=sqrt((R0+R1-d)*(d+R0-R1)*(d-R0+R1)*(d+R0+R1))/(2.*d);
    c=1.+(R0/d)*(R0/d)-(R1/d)*(R1/d);
    t=(c1-c0)/d;
    tta0=arc0.ArcLength*PI/180;
    tta1=arc1.ArcLength*PI/180;

    p[i]=c0 + (c*d/2.+ I*l)*t;		// first possible intersection;
    z0=arg((p[i]-c0)/(a0-c0));
    z1=arg((p[i]-c1)/(a1-c1));
    if ((z0>0.) && (z0<tta0) && (z1>0.) && (z1<tta1)) i++;

    if(fabs(d-R0+R1)/(R0+R1)< 1.e-05)
    {
        p[i]=c0+ c*d*t/2.;
        return i;
    }

    p[i]=c0 + (c*d/2.-I*l)*t;		// second possible intersection
    z0=arg((p[i]-c0)/(a0-c0));
    z1=arg((p[i]-c1)/(a1-c1));
    if ((z0>0.) && (z0<tta0) && (z1>0.) && (z1<tta1)) i++;

    // returns the number of valid intersections found;
    // intersections are returned in the array p[];
    return i;
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

// identical in fmesher, FPProc and HPProc
void femm::FemmProblem::getCircle(const femm::CArcSegment &arc, CComplex &c, double &R) const
{
    // construct the coordinates of the two points on the circle
    CComplex a0(nodelist[arc.n0]->x,nodelist[arc.n0]->y);
    CComplex a1(nodelist[arc.n1]->x,nodelist[arc.n1]->y);

    // calculate distance between arc endpoints
    double d = abs(a1 - a0);

    // figure out what the radius of the circle is...

    // get unit vector pointing from a0 to a1
    CComplex t = (a1 - a0) / d;

    // convert swept angle from degrees to radians
    double tta = arc.ArcLength * PI / 180.;

    // the radius is half the chord length divided by sin of
    // half the swept angle (finding the side length of a
    // triangle formed by the two points and the centre)
    R = d / (2.*sin(tta/2.));

    // center of the arc segment's circle
    c = a0 + (d/2. + I * sqrt(R*R - d*d / 4.)) * t;
}

std::string femm::FemmProblem::getTitle() const
{
    return pathName;
}

bool femm::FemmProblem::getIntersection(int n0, int n1, int segm, double *xi, double *yi) const
{
    CComplex p0,p1,q0,q1;
    double ee,x,z;

    // Check to see if the two lines have a common endpoint
    // If they do, there can be no other intersection...
    if (n0==linelist[segm]->n0) return false;
    if (n0==linelist[segm]->n1) return false;
    if (n1==linelist[segm]->n0) return false;
    if (n1==linelist[segm]->n1) return false;

    // Get a definition of "real small" based on the lengths
    // of the lines of interest;
    p0=nodelist[linelist[segm]->n0]->CC();
    p1=nodelist[linelist[segm]->n1]->CC();
    q0=nodelist[n0]->CC();
    q1=nodelist[n1]->CC();
    ee = (std::min)(abs(p1-p0),abs(q1-q0))*1.0e-8;

    // Rotate and scale the prospective line
    q0=(q0-p0)/(p1-p0);
    q1=(q1-p0)/(p1-p0);

    // Check for cases where there is obviously no intersection
    if ((Re(q0)<=0.) && (Re(q1)<=0.)) return false;
    if ((Re(q0)>=1.) && (Re(q1)>=1.)) return false;
    if ((Im(q0)<=0.) && (Im(q1)<=0.)) return false;
    if ((Im(q0)>=0.) && (Im(q1)>=0.)) return false;

    // compute intersection
    z=Im(q0)/Im(q0-q1);

    // check to see if the line segments intersect at a point sufficiently
    // far from the segment endpoints....
    x=Re((1.0 - z)*q0 + z*q1);
    if((x < ee) || (x > (1.0 - ee))) return false;

    // return resulting intersection point
    p0 = (1.0 - z)*nodelist[n0]->CC() + z*nodelist[n1]->CC();
    *xi=Re(p0);
    *yi=Im(p0);

    return true;
}

int femm::FemmProblem::getLineArcIntersection(const femm::CSegment &seg, const femm::CArcSegment &arc, CComplex *p) const
{
    CComplex p0,p1,a0,a1,t,v,c;
    double d,l,R,z,tta;
    int i=0;

    p0.Set(nodelist[seg.n0]->x,nodelist[seg.n0]->y);
    p1.Set(nodelist[seg.n1]->x,nodelist[seg.n1]->y);
    a0.Set(nodelist[arc.n0]->x,nodelist[arc.n0]->y);
    a1.Set(nodelist[arc.n1]->x,nodelist[arc.n1]->y);
    d=abs(a1-a0);			// distance between arc endpoints

    // figure out what the radius of the circle is...
    t = (a1-a0) / d;
    tta = arc.ArcLength * PI / 180.;
    R = d / ( 2. * std::sin(tta / 2.));
    c = a0 + (d/2. + I*sqrt(R*R-d*d/4.))*t; // center of the arc segment's circle...

    // figure out the distance between line and circle's center;
    d=abs(p1-p0);
    t=(p1-p0)/d;
    v=(c-p0)/t;
    if (fabs(Im(v))>R) return 0;
    l=sqrt( R*R - Im(v)*Im(v));	// Im(v) is distance between line and center...

    if ((l/R) < 1.e-05) 		// case where line is very close to a tangent;
    {
        p[i]=p0 + Re(v)*t;		// make it be a tangent.
        R=Re((p[i]-p0)/t);
        z=arg((p[i]-c)/(a0-c));
        if ((R>0) && (R<d) && (z>0.) && (z<tta)) i++;
        return i;
    }

    p[i]=p0 + (Re(v)+l)*t;		// first possible intersection;
    R=Re((p[i]-p0)/t);
    z=arg((p[i]-c)/(a0-c));
    if ((R>0) && (R<d) && (z>0.) && (z<tta)) i++;

    p[i]=p0 + (Re(v)-l)*t;		// second possible intersection
    R=Re((p[i]-p0)/t);
    z=arg((p[i]-c)/(a0-c));
    if ((R>0) && (R<d) && (z>0.) && (z<tta)) i++;

    // returns the number of valid intersections found;
    // intersections are returned in the array p[];
    return i;
}



double femm::FemmProblem::LineLength(int i)
{
    return abs(nodelist[linelist[i]->n0]->CC()-
            nodelist[linelist[i]->n1]->CC());
}

// identical in fmesher, FPProc and HPProc
double femm::FemmProblem::shortestDistanceFromArc(CComplex p, const femm::CArcSegment &arc)
{
    double R;
    CComplex c;
    getCircle(arc,c,R);

    double d=abs(p-c);
    if(d==0) return R;

    CComplex a0(nodelist[arc.n0]->x,nodelist[arc.n0]->y);
    CComplex a1(nodelist[arc.n1]->x,nodelist[arc.n1]->y);
    CComplex t=(p-c)/d;
    double l=abs(p-c-R*t);
    double z=arg(t/(a0-c))*180/PI;
    if ((z>0) && (z<arc.ArcLength)) return l;

    z=abs(p-a0);
    l=abs(p-a1);
    if(z<l) return z;
    return l;
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
