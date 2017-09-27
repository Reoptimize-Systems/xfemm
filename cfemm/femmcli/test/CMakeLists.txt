function(test_lua testname)
    add_test(NAME ${testname}.lua
        COMMAND femmcli-bin --lua-base-dir "${CMAKE_CURRENT_LIST_DIR}/../debug" --lua-script "${CMAKE_CURRENT_LIST_DIR}/${testname}.lua"
        )
endfunction()
function(test_lua_setup testname file)
    add_test(NAME ${testname}.setup
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/${file}" .
        )
    set_tests_properties(${testname}.lua PROPERTIES DEPENDS ${testname}.setup)
endfunction()
function(test_lua_check testname suffix file)
    add_test(NAME ${testname}.check.${suffix}
        COMMAND "${CMAKE_COMMAND}"
        -E compare_files "${file}" "${CMAKE_CURRENT_LIST_DIR}/${file}.check"
        )
    set_tests_properties(${testname}.check.${suffix} PROPERTIES DEPENDS ${testname}.lua)
endfunction()

test_lua(femmcli_pureLua)
test_lua(femmcli_complex)
test_lua(femmcli_compatmode)
test_lua(femmcli_trace)
test_lua(femmcli_femfile)
test_lua_setup(femmcli_femfile femmcli_femfile.fem)
test_lua_check(femmcli_femfile fem "femmcli_femfile.result.fem")
test_lua_check(femmcli_femfile ans "femmcli_femfile.result.ans")
test_lua(femmcli_matlib)
test_lua_check(femmcli_matlib fem "femmcli_matlib.result.fem")

# different working directory
add_test(NAME femmcli_chdir.lua
    COMMAND femmcli-bin --lua-script "${CMAKE_CURRENT_LIST_DIR}/femmcli_chdir.lua"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    )
