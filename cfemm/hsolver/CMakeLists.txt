add_library(hsolver STATIC 
    hsolver.cpp
    )
target_include_directories(hsolver PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(hsolver PUBLIC femm)

add_executable(hsolver-bin
    main.cpp
    )

set_target_properties(hsolver-bin
    PROPERTIES OUTPUT_NAME hsolver)

target_link_libraries(hsolver-bin hsolver)

add_subdirectory(test)
