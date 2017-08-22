add_library(fpproc STATIC 
    fpproc.cpp
    makemask.cpp
    CMPointVals.cpp
    )
target_include_directories(fpproc PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(fpproc PUBLIC femm)

add_executable(fpproc-test
    main.cpp
    )

target_link_libraries(fpproc-test fpproc)
