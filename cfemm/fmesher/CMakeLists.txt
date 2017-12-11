find_package(Triangle)
if (Triangle_FOUND)
    message("Found triangle version ${Triangle_VERSION}...")
else()
    message("Using builtin version of triangle...")
    add_library(triangle STATIC
        triangle/triangle.c
        )
    # a bit of a dirty hack to make the link line nicer:
    add_library(Triangle::triangle-api ALIAS triangle)
    target_include_directories(triangle PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/triangle> $<INSTALL_INTERFACE:include>)
    target_compile_definitions(triangle INTERFACE XFEMM_BUILTIN_TRIANGLE)
endif()

add_library(fmesher STATIC
    fmesher.cbp
    fmesher.cpp
    nosebl.cpp
    writepoly.cpp
    )
target_link_libraries(fmesher PUBLIC femm PRIVATE Triangle::triangle-api)
target_include_directories(fmesher PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)

add_executable(fmesher-bin
    main.cpp
    )

set_target_properties(fmesher-bin
    PROPERTIES OUTPUT_NAME fmesher)

target_link_libraries(fmesher-bin fmesher)

add_subdirectory(test)

install(
    TARGETS fmesher-bin
    RUNTIME DESTINATION bin
    COMPONENT "cli")
# vi:expandtab:tabstop=4 shiftwidth=4:
