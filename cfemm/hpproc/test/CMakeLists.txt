if(WIN32)
    set(NEWLINE_NATIVE WIN32)
else()
    set(NEWLINE_NATIVE UNIX)
endif()
## test_hpproc(<name>)
# Test the postprocessor on an existing solution file.
# 1. Run hpproc-testame>
# 2. Compare <name>.out against <name>.out.check (if not WIN32)
function(test_hpproc name)
    # convert check file to native line endings:
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.anh" "${CMAKE_CURRENT_BINARY_DIR}/${name}.anh" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${name}.out.check" "${CMAKE_CURRENT_BINARY_DIR}/${name}.out.check" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    add_test(NAME hpproc_${name}
        COMMAND hpproc-test "${name}.anh" "${name}.out"
        )

    # DISABLED property was introduced in cmake 3.9
    # skip test alltogether in earlier versions
    if(NOT WIN32 OR NOT "${CMAKE_VERSION}" VERSION_LESS 3.9)
        add_test(NAME hpproc_${name}.out.check
            COMMAND "${CMAKE_COMMAND}"
            -E compare_files "${name}.out" "${name}.out.check"
            )
        set_tests_properties(hpproc_${name}.out.check PROPERTIES DEPENDS hpproc_${name})
        if (WIN32)
            set_tests_properties(hpproc_${name}.out.check PROPERTIES DISABLED TRUE)
        endif()
    endif()
endfunction()

test_hpproc(Temp0)
test_hpproc(Temp1)
# vi:expandtab:tabstop=4 shiftwidth=4:
