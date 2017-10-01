add_library(hpproc STATIC 
    hpproc.cpp
    CHPointVals.cpp
    )
target_include_directories(hpproc PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(hpproc PUBLIC femm)

add_executable(hpproc-test
    main.cpp
    )

target_link_libraries(hpproc-test hpproc)

install(
    TARGETS hpproc-test
    RUNTIME DESTINATION bin
    COMPONENT "cli (debug)"
    CONFIGURATIONS "Debug")
# vi:expandtab:tabstop=4 shiftwidth=4:
