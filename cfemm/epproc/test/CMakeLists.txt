if(WIN32)
    set(NEWLINE_NATIVE WIN32)
else()
    set(NEWLINE_NATIVE UNIX)
endif()
## test_epproc(<name>)
# Test the postprocessor on an existing solution file.
# 1. Run epproc-testame>
# 2. Compare <name>.out against <name>.out.check
function(test_epproc name)
    # convert check file to native line endings:
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.res" "${CMAKE_CURRENT_BINARY_DIR}/${name}.res" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.out.check" "${CMAKE_CURRENT_BINARY_DIR}/${name}.out.check" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    add_test(NAME epproc_${name}
        COMMAND epproc-test "${name}.res" "${name}.out"
        )
    set_tests_properties(epproc_${name} PROPERTIES
        LABELS "electrostatics;postprocessor"
        )

    add_test(NAME epproc_${name}.out.check
        COMMAND "${CMAKE_COMMAND}"
        -E compare_files "${name}.out" "${name}.out.check"
        )
    set_tests_properties(epproc_${name}.out.check PROPERTIES
        DEPENDS epproc_${name}
        LABELS "electrostatics"
        )
endfunction()

test_epproc(test)
# vi:expandtab:tabstop=4 shiftwidth=4:
