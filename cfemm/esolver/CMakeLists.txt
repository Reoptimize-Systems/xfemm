add_library(esolver STATIC 
    esolver.cpp
    )
target_link_libraries(esolver femm)
# the INSTALL_INTERFACE is just there to keep cmake happy; and to remind us that a "normal" install could some day be possible...
target_include_directories(esolver PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)

add_executable(esolver-bin
    main.cpp
    )

set_target_properties(esolver-bin
    PROPERTIES OUTPUT_NAME esolver)

target_link_libraries(esolver-bin esolver)
