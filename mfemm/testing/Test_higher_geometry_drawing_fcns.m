% testing higher level geometry and region drawing functions


%% circle shape 

FemmProblem = newproblem_mfemm ('planar');
[FemmProblem, seginds, nodeinds, nodeids] = addcircle_mfemm (FemmProblem, 0, 0, 1);


%% curved trapezoidal shape

FemmProblem = newproblem_mfemm ('planar');

[FemmProblem, seginds, aseginds, nodeinds, nodeids, centre] = ...
    addcurvedtrapezoid_mfemm (FemmProblem, 1, 2, 0.2*pi/4, 0.6*pi/4, 'InGroup', 10);

plotfemmproblem (FemmProblem)

hold on
plot (centre(1), centre(2), 'x');
hold off


%% curved trapezoidal region

FemmProblem = newproblem_mfemm ('planar');

BlockProps.BlockType = 'Steel';
BlockProps.InGroup = 10;
SegProps.InGroup = 10;

[FemmProblem, seginds, aseginds, nodeinds, blockind, nodeids, labelloc] = ...
            addcurvedtrapezoidregion_mfemm (FemmProblem,  1, 2, 0.2*pi/4, 0.6*pi/4, BlockProps, SegProps);
        
plotfemmproblem (FemmProblem)
