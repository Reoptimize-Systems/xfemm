% Test_mergeproblems_mfemm


fp1 = newproblem_mfemm ('planar');

BlockProps = struct ( 'BlockType' ,  'Air' );

fp1 = addrectregion_mfemm(fp1, 0, 0, 1, 1, BlockProps);


fp2 = newproblem_mfemm ('planar');

BlockProps = struct ( 'BlockType' ,  'Air' );

fp2 = addrectregion_mfemm(fp2, 1.5, 0, 0.5, 1.5, BlockProps);


fpmerged = mergeproblems_mfemm ([ fp1, fp2 ]);

plotfemmproblem (fpmerged);