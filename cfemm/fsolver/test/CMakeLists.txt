if(WIN32)
    set(NEWLINE_NATIVE WIN32)
else()
    set(NEWLINE_NATIVE UNIX)
endif()
## test_fsolver_premeshed(<name>)
# Add tests to solve <name>.fem, and to compare the output file against a stored version:
# 1. Run fsolver <name>
# 2. Compare <name>.ans against <name>.ans.check
function(test_fsolver_premeshed name)
    # convert check file to native line endings:
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.fem" "${CMAKE_CURRENT_BINARY_DIR}/${name}.fem" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.ans.check" "${CMAKE_CURRENT_BINARY_DIR}/${name}.ans.check" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    # copy files at test time because the solver deletes them
    # (otherwise you would need to re-run cmake after each ctest run)
    add_test(NAME fsolver_${name}.setup
        COMMAND "${CMAKE_COMMAND}"
        -E copy
        "${CMAKE_CURRENT_LIST_DIR}/${name}.edge"
        "${CMAKE_CURRENT_LIST_DIR}/${name}.ele"
        "${CMAKE_CURRENT_LIST_DIR}/${name}.node"
        "${CMAKE_CURRENT_LIST_DIR}/${name}.pbc"
        "${CMAKE_CURRENT_LIST_DIR}/${name}.poly"
        .
        )
    add_test(NAME fsolver_${name}.solve
        COMMAND fsolver-bin "${name}"
        )
    set_tests_properties(fsolver_${name}.solve PROPERTIES DEPENDS fsolver_${name}.setup)
    add_test(NAME fsolver_${name}.check
        COMMAND "${CMAKE_COMMAND}"
        -E compare_files "${name}.ans" "${name}.ans.check"
        )
    set_tests_properties(fsolver_${name}.check PROPERTIES DEPENDS fsolver_${name}.solve)
endfunction()

## test_fsolver(<name>)
# Add tests to mesh and solve <name>.fem, and to compare the output file against a stored version:
# 1. Run fmesher <name>.fem
# 2. Run fsolver <name>
# 3. Compare <name>.ans against <name>.ans.check
function(test_fsolver name)
    # convert check file to native line endings:
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.fem" "${CMAKE_CURRENT_BINARY_DIR}/${name}.fem" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.ans.check" "${CMAKE_CURRENT_BINARY_DIR}/${name}.ans.check" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    add_test(NAME fsolver_${name}.mesh
        COMMAND fmesher-bin "${name}.fem"
        )
    add_test(NAME fsolver_${name}.solve
        COMMAND fsolver-bin "${name}"
        )
    set_tests_properties(fsolver_${name}.solve PROPERTIES DEPENDS fsolver_${name}.mesh)
    add_test(NAME fsolver_${name}.check
        COMMAND "${CMAKE_COMMAND}"
        -E compare_files "${name}.ans" "${name}.ans.check"
        )
    set_tests_properties(fsolver_${name}.check PROPERTIES DEPENDS fsolver_${name}.solve)
endfunction()

test_fsolver_premeshed(Temp)
test_fsolver(Temp1)
# vi:expandtab:tabstop=4 shiftwidth=4:
