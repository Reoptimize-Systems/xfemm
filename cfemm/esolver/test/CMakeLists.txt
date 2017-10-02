if(WIN32)
    set(NEWLINE_NATIVE WIN32)
else()
    set(NEWLINE_NATIVE UNIX)
endif()
## test_esolver(<name>)
# Add tests to mesh and solve <name>.fee, and to compare the output file against a stored version:
# 1. Run fmesher <name>.fee
# 2. Run esolver <name>
# 3. Compare <name>.res against <name>.res.check
function(test_esolver name)
    # convert check file to native line endings:
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.fee" "${CMAKE_CURRENT_BINARY_DIR}/${name}.fee" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.res.check" "${CMAKE_CURRENT_BINARY_DIR}/${name}.res.check" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    add_test(NAME esolver_${name}.mesh
        COMMAND fmesher-bin "${name}.fee"
        )
    add_test(NAME esolver_${name}.solve
        COMMAND esolver-bin "${name}"
        )
    set_tests_properties(esolver_${name}.solve PROPERTIES DEPENDS esolver_${name}.mesh)
    add_test(NAME esolver_${name}.check
        COMMAND "${CMAKE_COMMAND}"
        -E compare_files "${name}.res" "${name}.res.check"
        )
    set_tests_properties(esolver_${name}.check PROPERTIES DEPENDS esolver_${name}.solve)
endfunction()

############################################################

test_esolver(test)
# vi:expandtab:tabstop=4 shiftwidth=4:
