add_library(femmcli STATIC
    FemmState.cpp
    LuaBaseCommands.cpp
    LuaCommonCommands.cpp
    LuaElectrostaticsCommands.cpp
    LuaMagneticsCommands.cpp
    )
target_include_directories(femmcli PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(femmcli PUBLIC femm fmesher epproc esolver fsolver fpproc)

add_executable(femmcli-bin
    main.cpp
    )

set_target_properties(femmcli-bin
    PROPERTIES OUTPUT_NAME femmcli)

target_link_libraries(femmcli-bin femm femmcli esolver fsolver)

add_subdirectory(test)
# vi:expandtab:tabstop=2 shiftwidth=2:
