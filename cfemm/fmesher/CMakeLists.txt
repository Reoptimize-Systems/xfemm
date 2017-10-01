add_library(triangle STATIC
    triangle/triangle.c
    )
target_include_directories(triangle PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/triangle> $<INSTALL_INTERFACE:include>)

add_library(fmesher STATIC
    fmesher.cbp
    fmesher.cpp
    main.cpp
    nosebl.cpp
    writepoly.cpp
    )
target_link_libraries(fmesher PUBLIC femm PRIVATE triangle)
target_include_directories(fmesher PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)

add_executable(fmesher-bin
    main.cpp
    )

set_target_properties(fmesher-bin
    PROPERTIES OUTPUT_NAME fmesher)

target_link_libraries(fmesher-bin fmesher femm)

add_subdirectory(test)

install(
    TARGETS fmesher-bin
    RUNTIME DESTINATION bin
    COMPONENT "cli")
# vi:expandtab:tabstop=4 shiftwidth=4:
