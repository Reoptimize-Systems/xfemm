function(test_hsolver name)
    add_test(NAME hsolver_${name}.setup
        COMMAND "${CMAKE_COMMAND}"
        -E copy "${CMAKE_CURRENT_LIST_DIR}/${name}.feh" .
        )
    add_test(NAME hsolver_${name}.mesh
        COMMAND fmesher-bin "${name}.feh"
        )
    set_tests_properties(hsolver_${name}.mesh PROPERTIES DEPENDS hsolver_${name}.setup)
    add_test(NAME hsolver_${name}.solve
        COMMAND hsolver-bin "${name}"
        )
    set_tests_properties(hsolver_${name}.solve PROPERTIES DEPENDS hsolver_${name}.mesh)
    add_test(NAME hsolver_${name}.check
        COMMAND "${CMAKE_COMMAND}"
        -E compare_files "${name}.anh" "${CMAKE_CURRENT_LIST_DIR}/${name}.anh.check"
        )
    set_tests_properties(hsolver_${name}.check PROPERTIES DEPENDS hsolver_${name}.solve)
endfunction()

test_hsolver(Temp0)
test_hsolver(Temp1)
