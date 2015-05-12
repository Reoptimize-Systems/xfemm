%% femm_manual_tutorial_example.m
%
%
% This script recreates the Magnetostatics example provided in the FEMM 4.2
% Users manual using mfemm functions. 
%
% In this example, the solution for the field of an air-cored coil is
% considered. 
%
% The coil to be analyzed is show in the diagram below:
%
%                                     1 inch
%                               <----------------->                              
%                                        | 
%                                        |
%                                        x    
%                                  x     |     x                             
%                              x         |         x                         
%                           x           x|x           x                        
%                         x         x    |    x         x                       
%                       x        x       |       x        x                      
%                      x_______ x        |        x _______x                     
%                      x       |         |         |       x     ^                
%                      x       |x        |        x|       x     |                
%                      |x      | x       |       x |      x|     |                
%                      |  x    |    x    |    x    |    x  |     |                
%                      |    x  |   .    x|x   .    |  x    |     |                
%                      |       x         |         x       |     | 2 inch        
%                      |    .  |   x    .|.    x   |  .    |     |             
%                      |  .    |    .    x    .    |    .  |     |              
%                      |.      | .       |       . |      .|     |               
%                      |       |.        |        .|       |     |                
%                      |_______|         |         |_______|     v                   
%                      x        .        |        .        x                      
%                       x        .       |       .        x                       
%                         x         .    |    .         x                         
%                           x           .|.           x                            |                                     
%                              x         |         x                           
%                                  x     |     x                               
%                                        x                                     
%                                        |                                  
%                                        |                                  
%                                        |                                  
%                      <----------------------------------->
%                                     3 inch
%
% The coil has an inner diameter of 1 inch; an outer diameter of 3 inches;
% and an axial length of 2 inches. The coil is built out of 1000 turns of
% 18 AWG copper wire. For the purposes of this example, we will consider
% the case in which a steady current of 1 Amp is flowing through the wire.
% In FEMM (and therefore mfemm), one models a slice of the axisymmetric
% problem. By convention, the r = 0 axis is understood to run vertically,
% and the problem domain is restricted to the region where r ? 0 . In this
% convention, positive-valued currents flow in the into-the-page direction.
%
% 

%% Create a new mfemm problem structure
%
% newproblem_mfemm creates a new FemmProblem structure for the type of
% problem specifiec in the first argument. This argument is either a string
% or scalar value. If a string it can be either 'planar' or
% 'axisymmetric', or any unambiguous shorter starting substring, e.g.
% 'axi','pl' or even just 'a' or 'p'. If a scalar value, the value zero
% chooses a planar type simulation. Any other value selects an axisymmetric
% simulation. Here we use 'axi' to choose and axisymmetric type.
%   
% We also set the problem frequency to zero and the length units to be
% inches using optional parameter-value pairs.
FemmProblem = newproblem_mfemm('axi', ...
                               'Frequency', 0, ...
                               'LengthUnits', 'inches');
                           
%%
%
% The created FemmProblem structure contains two field, ProbInfo,
% containing the problem type specification, and Materials. The materials
% structure contains an array of structures containing the material
% specifications that will be used in the problem. newproblem_mfemm
% automatically puts the material 'Air' in this array by default when
% creating the problem. We will add more materials to this array later in
% the example for use in the model.
disp(FemmProblem)

%%
%
% The full problem specification 
disp(FemmProblem.ProbInfo)

%%
%
% The next task is to create boundaries for the solution region.
% Fundamentally, finite element solvers mesh and find a solution over a
% finite region of space that contains the objects of interest. We must
% define what this region is. In this case, we will choose our solution
% region to be a sphere with a radius of 4 inches.
%
% To create the sphere we need only two nodes which can then be linked by
% a semi-circular arc and a straight segment to make a semicircle. 
%
% first define the node locations, which are above and below the centerline
% of the cylindrical coil.
outernodes = [ 0, -4; 
               0,  4 ];
          
%%
%
% Now add these nodes to the FemmProblem structure using the function
% addnodes_mfemm. addnodes_mfemm is typical in that it can return up to
% three arguments. The first of these is the modified FemmProblem structure
% with the new nodes added. The second argument is a vector containning the
% matlab array indices of the created nodes in an array of node structures,
% while nodeids are the zero-based node ids which are always equal to the
% indices - 1. Segments are created bewtwenn node ids, which is why we want
% to store them for later use.
[FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, outernodes(:,1), outernodes(:,2));

%%
%
% Next we add a straght segment between the two nodes we just created
[FemmProblem] = addsegments_mfemm(FemmProblem, nodeids(1), nodeids(2));

%%
%
% And then the arc segment between the same two nodes
[FemmProblem, rcsegind] = addarcsegments_mfemm(FemmProblem, nodeids(1), nodeids(2), 180, 'MaxSegDegrees', 2.5);

%%
%
% Next we will draw the coil, a rectangle in cross-section. The material
% type from the materials library '18 AWG' will be used for this. So first
% add this to the mfemm problem
FemmProblem = addmaterials_mfemm(FemmProblem, '18 AWG');
%%
%
% we will also create a circuit and add it to the model. We will specify
% the coil current using this circuit. We will apply a 1 Amp current in
% this circit. 
FemmProblem = addcircuit_mfemm(FemmProblem, 'Coil', 'TotalAmps_re', 1); 

%%
%
% Now we could draw the coil manually by adding the nodes, then the
% segments, and then a block label with several commands, as follows:
%
% coilnodes = [ 0.5, -1; 
%               1.5, -1; 
%               1.5, 1; 
%               0.5, 1 ];
%  
% [FemmProblem, nodeinds, nodeids] = addnodes_mfemm(FemmProblem, coilnodes(:,1), coilnodes(:,2));
% 
% [FemmProblem, seginds] = addsegments_mfemm(FemmProblem, nodeids(1:end), [nodeids(end), nodeids(1:end-1)]);
% 
% FemmProblem = addblocklabel_mfemm(FemmProblem, 0, 1.0, ...
%                                   'BlockType', '18 AWG', ...
%                                   'InCircuit', 'Coil', ...
%                                   'Turns', 1000, ...
%                                   'MaxArea', 0.1);

%%
%
% However, a more convenient method is to use the addrectregion_mfemm
% function which combines these steps into a single call. In this case we
% specify the block properties as the fields of a structure rather than the
% parameter-value pairs we have been using up to this point (e.g. 'Turns',
% 1000)
CoilBlockProps.BlockType = '18 AWG';
CoilBlockProps.InCircuit = 'Coil';
CoilBlockProps.Turns = 1000;
CoilBlockProps.MaxArea = 0.1;

FemmProblem = addrectregion_mfemm(FemmProblem, 0.5, -1, 1, 2, CoilBlockProps);

%%
%
% We also need a material for the region surrounding the coil, in this case
% Air. By default, when a new FemmProblem is created, Air is added as the
% first material, so we don't need to add it again. We can just go ahead
% and add a block label in an appropriate place.

FemmProblem = addblocklabel_mfemm(FemmProblem, 0.5, 1.5, ...
                                  'BlockType', 'Air', ...
                                  'MaxArea', 0.1);

%%
%
% Next we must choose a boundary condition for the outer boundary of our
% prolem to approximate the reality of the problem which is infinite in
% extent.
%
% In this case we will use an "asymptotic boundary condition". This
% condition approximates the impedance of an unbounded, open space. In this
% way, we can model the field produced by the coil in an unbounded space
% while still only modeling a finite region of that space. This boundary
% condition is of the form:
%
% (1 / (mu_0*mu_r)) (dA / dn) + c_0 A + c_1 = 0
%
% where A is magnetic vector potential, mu_r is the relative magnetic
% permeability of the region adjacent to the boundary, mu_0 is the
% permeability of free space, and n represents the direction normal to the
% boundary. For our asymptotic boundary condition, we need to specify:
%
% c_0 = 1 / ( mu_0 mu_r R )
% 
% c_ = 0
% 
% With mfemm this can be achieved with the addboundary_mfemm function.
inch = 0.0254;
R = 4;
mu_0 = 4 * pi *1e-7;
[FemmProblem, boundind, boundname] = addboundaryprop_mfemm(FemmProblem, 'ABC', 2, ...
                                    'c0', 1/(mu_0*R), ...
                                    'c1', 0);

%%
%
% We then need to assign this boundary condition to the arc segment. This
% is achieved by changing the 'BoundaryMarker' of the appropriate arc
% segment to the same name as our new boundary.
%
FemmProblem.ArcSegments(rcsegind).BoundaryMarker = boundname;

%%
%
% Finally we can take a look at the problem using Matlab's plotting
% commands
plotfemmproblem(FemmProblem);

%%
%
% However, this isn't very well developend yet. If the original FEMM is
% installed, and it's m-file interface present on the Matlab search path,
% we can open the file in FEMM as well.
if exist('openfemm.m', 'file')
    openfemm;
    openprobleminfemm_mfemm(FemmProblem);
else
    fprintf(1, 'Looks like femm isn''t installed, or at least its m-files aren''t on the path.\n');
end

%% Solve the Problem
%
% Now that the problem is defined, we want to go ahead and solve it. The
% first step in doing this is to create a mesh. To do this we call the
% fmesher function. Doing this will require that you have set up (i.e.
% compiled) the fmesher, fsolver and fpproc mexfunctions, otherwise they
% won't work/exist.
%

%%
%
% Create a .fem file describing the problem on disk
filename = 'mag_tutorial.fem';
writefemmfile(filename, FemmProblem);

%%
%
% mesh the problem using fmesher. We could have called fmesher with the
% FemmProblem directly, i.e.
%
% filename = fmesher(filename, FemmProblem);
%
% Or even just 
%
% filename = fmesher(FemmProblem);
%
% Which puts the .fem file in a temporary file location, the path of which
% is returned in filename, but then we would not have shown how to use
% writefemmfile to generate the file.
filename = fmesher(filename);

%%
%
% fmesher creates a number of mesh files based on the file name with
% different extensions, e.g. .poly, .node etc. Armed with this, we can now
% solve the finite-element problem. This is achieved by calling fsolver.
%
% The results are written to disk. The file name (.ans file) is returned by
% fsolver (but is always just the name of the input file with the extesion
% replaced with .ans
ansfile = fsolver(filename);


%% Extracting Results
%
% Having solved te problem, it is generally desired that we do some
% post-processing on the results. The file which is produced is perfectly
% compatible with FEMM, and if it is installed, we can open the file and
% use all the normal post-processing functions
% if exist('openfemm.m', 'file')
%     opendocument(ansfile);
% else
%     fprintf(1, 'Looks like femm isn''t installed, or at least its m-files aren''t on the path.\n');
% end
%
% However, some post-processing methods are also provided with mfemm. The
% main method is a class based method for loading and manipulating the
% data. The class is called fpproc, and also requires some compilation
% before use as it is an interface to more C++ code.
%
% So to manipulate the output, we first create an fpproc class
myfpproc = fpproc();

%%
%
% and then load the results file using the opendocument method
myfpproc.opendocument(ansfile);

%%
%
% once loaded fpproc provides a number of post-processing functions, which
% are really calls to the C++ versions supplied in the original FEMM code.
% You can view a list of all the avaialable methods using the methods
% function
if ~mfemmdeps.isoctave
    methods(myfpproc);
end

%%
%
% for example, extract the values from the solution at the point (0,0)
pvals = myfpproc.getpointvalues(0,0)

%%
%
% we can also get multiple points, the results of which are returned in a
% matrix with each point's values in its own column
manypvals = myfpproc.getpointvalues([0,0.1,0.2,0.3,0.4,0.5],[0,0,0,0,0,0])

%%
%
% alternatively, select the coil and integrate it's area. The first two
% arguments are the location of a point inside the block area to be
% selected, and the (optional) third argument determines whether any
% existing selections should be cleared. If the third argument is not
% supplied, it defaults to false, keeping existing selections.
myfpproc.selectblock(1, 0, true);

%%
%
% The area integral is selected by caling fpproc.blockintegral with the
% number 5
coilarea = myfpproc.blockintegral(5)

%%
%
% Plots of the B and H vector fields can also be produced using fpproc like
% so:

myfpproc.plotBfield(0, -1, -4.5, 6, 9, [30, 45]);

myfpproc.plotHfield(0, -1, -4.5, 6, 9, [30, 45]);

% The vector potential can also be plotted
myfpproc.plotAfield(0, -1, -4.5, 6, 9, 100);


















