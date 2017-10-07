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
endfunction()

## test_lua_setup(<name> <files>...)
# Copy files required by <name>.lua.
function(test_lua_setup testname)
    foreach(file IN LISTS ARGN)
        configure_file("${CMAKE_CURRENT_LIST_DIR}/${file}" "${CMAKE_CURRENT_BINARY_DIR}/${file}" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    endforeach()
endfunction()

## test_lua_check(<name> <suffix> <file>)
# Add a test <name>.check.<suffix> that compares an output file of <name>.lua with <file>.
function(test_lua_check testname suffix file)
    # convert check file to native line endings:
    configure_file("${CMAKE_CURRENT_LIST_DIR}/${file}.check" "${CMAKE_CURRENT_BINARY_DIR}/${file}.check" @ONLY NEWLINE_STYLE ${NEWLINE_NATIVE})
    add_test(NAME ${testname}.check.${suffix}
        COMMAND "${CMAKE_COMMAND}"
        -E compare_files "${file}" "${file}.check"
        )
    set_tests_properties(${testname}.check.${suffix} PROPERTIES DEPENDS ${testname}.lua)
endfunction()

############################################################

test_lua(femmcli_pureLua)
test_lua(femmcli_complex)
test_lua(femmcli_compatmode)
test_lua(femmcli_trace)
test_lua(femmcli_femfile)
test_lua_setup(femmcli_femfile "femmcli_femfile.fem")
test_lua_check(femmcli_femfile fem "femmcli_femfile.result.fem")
test_lua_check(femmcli_femfile ans "femmcli_femfile.result.ans")
test_lua(femmcli_matlib)
test_lua_check(femmcli_matlib fem "femmcli_matlib.result.fem")
test_lua(femmcli_epproc)
test_lua_setup(femmcli_epproc "femmcli_epproc.fee")

# different working directory
add_test(NAME femmcli_chdir.lua
    COMMAND femmcli-bin --lua-script "${CMAKE_CURRENT_LIST_DIR}/femmcli_chdir.lua"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    )
# vi:expandtab:tabstop=4 shiftwidth=4:
