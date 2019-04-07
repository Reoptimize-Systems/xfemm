if(WIN32)
    set(NEWLINE_NATIVE WIN32)
else()
    set(NEWLINE_NATIVE UNIX)
endif()
## test_hsolver(<name>)
# Add tests to mesh and solve <name>.feh, and to compare the output file against a stored version:
# 1. Run fmesher <name>.feh
# 2. Run hsolver <name>
# 3. Add disabled check to compare <name>.anh against <name>.anh.check
function(test_hsolver name)
    # convert check file to native line endings:
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.feh" "${CMAKE_CURRENT_BINARY_DIR}/${name}.feh" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.anh.check" "${CMAKE_CURRENT_BINARY_DIR}/${name}.anh.check" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    add_test(NAME hsolver_${name}.mesh
        COMMAND fmesher-bin "${name}.feh"
        )
    set_tests_properties(hsolver_${name}.mesh PROPERTIES
        LABELS "heatflow;mesher"
        )
    add_test(NAME hsolver_${name}.solve
        COMMAND hsolver-bin "${name}"
        )
    set_tests_properties(hsolver_${name}.solve PROPERTIES
        DEPENDS hsolver_${name}.mesh
        LABELS "heatflow;solver"
        )

    # DISABLED property was introduced in cmake 3.9
    # skip test alltogether in earlier versions
    if(NOT "${CMAKE_VERSION}" VERSION_LESS 3.9)
        add_test(NAME hsolver_${name}.check
            COMMAND "${CMAKE_COMMAND}"
            -E compare_files "${name}.anh" "${name}.anh.check"
            )
        set_tests_properties(hsolver_${name}.check PROPERTIES
            DEPENDS hsolver_${name}.solve
            LABELS "heatflow"
            )
        set_tests_properties(hsolver_${name}.check PROPERTIES DISABLED TRUE)
    endif()
endfunction()

test_hsolver(Temp0)
test_hsolver(Temp1)
# vi:expandtab:tabstop=4 shiftwidth=4:
