if(WIN32)
    set(NEWLINE_NATIVE WIN32)
else()
    set(NEWLINE_NATIVE UNIX)
endif()

## test_lua(<name>)
# Add a lua test for <name>.lua.
function(test_lua testname)
    add_test(NAME ${testname}.lua
        COMMAND femmcli-bin --lua-base-dir "${CMAKE_CURRENT_LIST_DIR}/../debug" --lua-script "${CMAKE_CURRENT_LIST_DIR}/${testname}.lua"
        )
    set_tests_properties(${testname}.lua PROPERTIES
        LABELS "lua"
        )
endfunction()

## test_lua_setup(<name> <files>...)
# Copy files required by <name>.lua.
function(test_lua_setup testname)
    foreach(file IN LISTS ARGN)
        configure_file("${CMAKE_CURRENT_LIST_DIR}/${file}" "${CMAKE_CURRENT_BINARY_DIR}/${file}" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    endforeach()
endfunction()

## test_lua_check(<name> <suffix> <file> <enabled>...)
# Add a test <name>.check.<suffix> that compares an output file of <name>.lua with <file>.
function(test_lua_check testname suffix file enabled)
    # DISABLED property was introduced in cmake 3.9
    # skip test alltogether in earlier versions
    if(enabled OR NOT "${CMAKE_VERSION}" VERSION_LESS 3.9)
        # convert check file to native line endings:
        configure_file("${CMAKE_CURRENT_LIST_DIR}/${file}.check" "${CMAKE_CURRENT_BINARY_DIR}/${file}.check" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
        add_test(NAME ${testname}.check.${suffix}
            COMMAND "${CMAKE_COMMAND}"
            -E compare_files "${file}" "${file}.check"
            )
        set_tests_properties(${testname}.check.${suffix} PROPERTIES DEPENDS ${testname}.lua)
        if(NOT enabled)
            set_tests_properties(${testname}.check.${suffix} PROPERTIES DISABLED TRUE)
        endif()
    endif()
endfunction()

############################################################

test_lua(femmcli_pureLua)
test_lua(femmcli_complex)
test_lua(femmcli_compatmode)
test_lua(femmcli_trace)
test_lua(femmcli_femfile)
test_lua_setup(femmcli_femfile "femmcli_femfile.fem")
# these tests are on a hair trigger - don't enable them by default
test_lua_check(femmcli_femfile fem "femmcli_femfile.result.fem" FALSE)
test_lua_check(femmcli_femfile ans "femmcli_femfile.result.ans" FALSE)
test_lua(femmcli_fpproc)
test_lua_setup(femmcli_fpproc "femmcli_fpproc.fem")
test_lua(femmcli_matlib)
# these tests are on a hair trigger - don't enable them by default
test_lua_check(femmcli_matlib fem "femmcli_matlib.result.fem" FALSE)
test_lua(femmcli_epproc)
test_lua_setup(femmcli_epproc "femmcli_epproc.fee")
test_lua(femmcli_hpproc)
test_lua_setup(femmcli_hpproc "femmcli_hpproc.feh")

test_lua(femmcli_TorqueBenchmark)
test_lua_setup(femmcli_TorqueBenchmark "femmcli_TorqueBenchmark.fem")

# different working directory
add_test(NAME femmcli_chdir.lua
    COMMAND femmcli-bin --lua-script "${CMAKE_CURRENT_LIST_DIR}/femmcli_chdir.lua"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    )
# vi:expandtab:tabstop=4 shiftwidth=4:
