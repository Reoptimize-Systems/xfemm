% Test_splitsegment_mfemm

% one positive segment
fp = newproblem_mfemm ('planar');

[fp, ~, nodeids] = addnodes_mfemm (fp, [0; 1], [0; 0]);

[fp, segind] = addsegments_mfemm (fp, nodeids(1), nodeids(2));

fp = splitsegment_mfemm (fp, segind-1, 0.5);

plotfemmproblem (fp);

%% one positive segment
fp = newproblem_mfemm ('planar');

[fp, ~, nodeids] = addnodes_mfemm (fp, [0; 1], [0; 1]);

[fp, segind] = addsegments_mfemm (fp, nodeids(1), nodeids(2));

fp = splitsegment_mfemm (fp, segind-1, 0.5);

plotfemmproblem (fp);

%% one negative segment
fp = newproblem_mfemm ('planar');

[fp, ~, nodeids] = addnodes_mfemm (fp, [0; 1], [0; 0]);

[fp, segind] = addsegments_mfemm (fp, nodeids(1), nodeids(2));

fp = splitsegment_mfemm (fp, segind-1, -0.75);

plotfemmproblem (fp);

%% multiple segments
fp = newproblem_mfemm ('planar');

[fp, ~, nodeids] = addnodes_mfemm (fp, [0; 1], [0; 0]);

[fp, segind] = addsegments_mfemm (fp, nodeids(1), nodeids(2));

fp = splitsegment_mfemm (fp, segind-1, [0.1, 0.5, -0.75]);

plotfemmproblem (fp);

