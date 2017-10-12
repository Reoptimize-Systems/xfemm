add_library(femmcli STATIC
    FemmState.cpp
    LuaBaseCommands.cpp
    LuaCommonCommands.cpp
    LuaElectrostaticsCommands.cpp
    LuaHeatflowCommands.cpp
    LuaMagneticsCommands.cpp
    )
target_include_directories(femmcli PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(femmcli
    PUBLIC femm fmesher
    esolver epproc
    fsolver fpproc
    hsolver hpproc
    )

add_executable(femmcli-bin
    main.cpp
    )

set_target_properties(femmcli-bin
    PROPERTIES OUTPUT_NAME femmcli)

target_link_libraries(femmcli-bin femmcli )

add_subdirectory(test)

install(
    TARGETS femmcli-bin
    RUNTIME DESTINATION bin
    COMPONENT "cli")
# vi:expandtab:tabstop=4 shiftwidth=4:
