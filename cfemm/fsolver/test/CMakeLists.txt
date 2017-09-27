function(test_fsolver name)
    add_test(NAME fsolver_${name}.setup
        COMMAND "${CMAKE_COMMAND}"
        -E copy
        "${CMAKE_CURRENT_LIST_DIR}/${name}.edge"
        "${CMAKE_CURRENT_LIST_DIR}/${name}.ele"
        "${CMAKE_CURRENT_LIST_DIR}/${name}.fem"
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
        -E compare_files "${name}.ans" "${CMAKE_CURRENT_LIST_DIR}/${name}.ans.check"
        )
    set_tests_properties(fsolver_${name}.check PROPERTIES DEPENDS fsolver_${name}.solve)
endfunction()

test_fsolver(Temp)
