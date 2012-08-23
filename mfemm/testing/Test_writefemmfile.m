% Test_writefemmfile

%   ProbInfo

%       Frequency - a scalar value giving the AC frequecy of the problem
%       e.g. 0
%
%       Precision - a scalar value giving the desired precesion of the
%       solution, e.g. 1e-8
%
%       MinAngle - a scalar value setting the minimum allowed angle for the
%       mesh triangles, e.g. 30
%
%       Depth - For planar problems, the depth of the simulation, will be
%       ignored for axisymmetric problems
%
%       LengthUnits - either a string containing the length units of the
%       simulaton, or a scalar value coding for one of the strings. This
%       can be one of: 'millimeters', 'centimeters', 'meters', 'mils',
%       'microns', ro 'inches'. The same values can be set by using the
%       scalar values 1, 2, 3, 4, 5, or any other value to yield 'inches'
%
%       ProblemType - a scalar value determining which type of problem is
%       to be solved. If a sclar, 0 for planar, 1 for axisymmetric. If a
%       string this can by any number of the characters from the strings
%       'planar' and 'axisymmetric', in any case e.g. 'p', 'pl', 'PlA' etc.
%       all choose a 'planar' symulation. If an axisymmetric problem is
%       specified, external regions can also optionally be specified using
%       the fields, extZo, extRo and extRi
%
%       ACSolver - Can be 0 or 1 (TODO, find out what each is!)

clear

% set up the problem info
FemmProblem.ProbInfo.Frequency = 0;
FemmProblem.ProbInfo.Precision = 1e-8;
FemmProblem.ProbInfo.MinAngle = 30;
FemmProblem.ProbInfo.Depth = 0;
FemmProblem.ProbInfo.LengthUnits = 'centimetres';
FemmProblem.ProbInfo.ProblemType = 'axi';
FemmProblem.ProbInfo.ACSolver = 0;
FemmProblem.ProbInfo.Coords = 'cart';

%   PointProps 

%   BoundaryProps 
%   <BeginBdry>
%     <BdryName> = "Pros A"
%     <BdryType> = 0
%     <A_0> = 0
%     <A_1> = 0
%     <A_2> = 0
%     <Phi> = 90
%     <c0> = 0
%     <c0i> = 0
%     <c1> = 0
%     <c1i> = 0
%     <Mu_ssd> = 0
%     <Sigma_ssd> = 0
%   <EndBdry>

FemmProblem.BoundaryProps(1).Name = 'Pros A';
FemmProblem.BoundaryProps(1).BdryType = 0;
FemmProblem.BoundaryProps(1).A_0 = 0;
FemmProblem.BoundaryProps(1).A_1 = 0;
FemmProblem.BoundaryProps(1).A_2 = 0;
FemmProblem.BoundaryProps(1).Phi = 90;
FemmProblem.BoundaryProps(1).c0 = 0;
FemmProblem.BoundaryProps(1).c0i = 0;
FemmProblem.BoundaryProps(1).c1 = 0;
FemmProblem.BoundaryProps(1).c1i = 0;
FemmProblem.BoundaryProps(1).Mu_ssd = 0; 
FemmProblem.BoundaryProps(1).Sigma_ssd = 0;

  
  
%   Materials
Matlib =  parsematlib_mfemm(fullfile(fileparts(which('mfemm_parsematlib.m')), 'matlib.dat'));

FemmProblem.Materials = Matlib([1, 47]);

%   Circuits
FemmProblem.Circuits(1).Name = 'Circuit 1';
FemmProblem.Circuits(1).TotalAmps_re = 10;
FemmProblem.Circuits(1).TotalAmps_im = 0;
FemmProblem.Circuits(1).CircType = 'series';

%   Nodes nodelist[i].x,nodelist[i].y,t,nodelist[i].InGroup
FemmProblem.Nodes(1).Coords = [0.2,-0.5];
FemmProblem.Nodes(1).BoundaryMarker = '';
FemmProblem.Nodes(1).InGroup = 1;

FemmProblem.Nodes(2) = FemmProblem.Nodes(1);
FemmProblem.Nodes(3) = FemmProblem.Nodes(1);
FemmProblem.Nodes(4) = FemmProblem.Nodes(1);

FemmProblem.Nodes(2).Coords = [1,-0.5];
FemmProblem.Nodes(3).Coords = [1,0.5];
FemmProblem.Nodes(4).Coords = [0.2,0.5];


FemmProblem.Nodes(5:6) = FemmProblem.Nodes(1);
FemmProblem.Nodes(5).Coords = [0,-10];
FemmProblem.Nodes(6).Coords = [0,10];

%   Segments 
FemmProblem.Segments(1).n0 = 0;
FemmProblem.Segments(1).n1 = 1;
FemmProblem.Segments(1).MaxSideLength = -1;
FemmProblem.Segments(1).Hidden = 0;
FemmProblem.Segments(1).InGroup = 1;

FemmProblem.Segments(2).n0 = 1;
FemmProblem.Segments(2).n1 = 2;
FemmProblem.Segments(2).MaxSideLength = -1;
FemmProblem.Segments(2).Hidden = 0;
FemmProblem.Segments(2).InGroup = 1;

FemmProblem.Segments(3).n0 = 2;
FemmProblem.Segments(3).n1 = 3;
FemmProblem.Segments(3).MaxSideLength = -1;
FemmProblem.Segments(3).Hidden = 0;
FemmProblem.Segments(3).InGroup = 1;

FemmProblem.Segments(4).n0 = 3;
FemmProblem.Segments(4).n1 = 0;
FemmProblem.Segments(4).MaxSideLength = -1;
FemmProblem.Segments(4).Hidden = 0;
FemmProblem.Segments(4).InGroup = 1;


FemmProblem.Segments(5).n0 = 4;
FemmProblem.Segments(5).n1 = 5;
FemmProblem.Segments(5).MaxSideLength = -1;
FemmProblem.Segments(5).Hidden = 0;
FemmProblem.Segments(5).InGroup = 1;

%   ArcSegments
FemmProblem.ArcSegments(1).n0 = 4;
FemmProblem.ArcSegments(1).n1 = 5;
FemmProblem.ArcSegments(1).ArcLength = 90;
FemmProblem.ArcSegments(1).MaxSideLength = 5;
FemmProblem.ArcSegments(1).Hidden = 0;
FemmProblem.ArcSegments(1).InGroup = 1;
FemmProblem.ArcSegments(1).BoundaryMarker = 'Pros A';


%   BlockLabels 

% copper
FemmProblem.BlockLabels(1).Coords = [0.3, 0];
FemmProblem.BlockLabels(1).BlockType = FemmProblem.Materials(2).Name;
FemmProblem.BlockLabels(1).MaxArea = 0.001;
FemmProblem.BlockLabels(1).InCircuit = FemmProblem.Circuits(1).Name;
FemmProblem.BlockLabels(1).MagDir = 0;
FemmProblem.BlockLabels(1).InGroup = 1;
FemmProblem.BlockLabels(1).Turns = 1;
FemmProblem.BlockLabels(1).IsExternal = 0;

% air
FemmProblem.BlockLabels(2).Coords = [0.1, 0];
FemmProblem.BlockLabels(2).BlockType = FemmProblem.Materials(1).Name;
FemmProblem.BlockLabels(2).MaxArea = 0.001;
FemmProblem.BlockLabels(2).InCircuit = '';
FemmProblem.BlockLabels(2).MagDir = 0;
FemmProblem.BlockLabels(2).InGroup = 0;
FemmProblem.BlockLabels(2).Turns = 0;
FemmProblem.BlockLabels(2).IsExternal = 0;            
            
filename = 'test2.fem';

writefemmfile(filename, FemmProblem)


