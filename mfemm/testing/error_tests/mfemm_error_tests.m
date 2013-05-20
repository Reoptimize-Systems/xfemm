% mfemm_error_tests

%% region with no block label

testfile = fullfile(getmfilepath('mfemm_error_tests'), 'err_test_no_block_label.FEM');

analyse_mfemm(testfile, false, false)