function(test_esolver name)
    add_test(NAME esolver_${name}.setup
        COMMAND "${CMAKE_COMMAND}"
        -E copy "${CMAKE_CURRENT_LIST_DIR}/${name}.fee" .
        )
    add_test(NAME esolver_${name}.mesh
        COMMAND fmesher-bin "${name}.fee"
        )
    set_tests_properties(esolver_${name}.mesh PROPERTIES DEPENDS esolver_${name}.setup)
    add_test(NAME esolver_${name}.solve
        COMMAND esolver-bin "${name}"
        )
    set_tests_properties(esolver_${name}.solve PROPERTIES DEPENDS esolver_${name}.mesh)
    add_test(NAME esolver_${name}.check
        COMMAND "${CMAKE_COMMAND}"
        -E compare_files "${name}.res" "${CMAKE_CURRENT_LIST_DIR}/${name}.res.check"
        )
    set_tests_properties(esolver_${name}.check PROPERTIES DEPENDS esolver_${name}.solve)
endfunction()

test_esolver(test)
