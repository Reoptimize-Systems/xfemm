cmake_minimum_required (VERSION 2.8.8)
project (femmcli)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64") 
  add_definitions(-fPIC) 
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories(../libfemm/liblua)
include_directories(../libfemm)

include_directories(../esolver)
include_directories(../fmesher)
include_directories(../fsolver)
include_directories(../fpproc)

add_library(femmcli STATIC
  FemmState.cpp
  LuaBaseCommands.cpp
  LuaCommonCommands.cpp
  LuaElectrostaticsCommands.cpp
  LuaMagneticsCommands.cpp
  )
target_link_libraries(femmcli femm fmesher esolver fsolver fpproc)

add_executable(femmcli-bin
  main.cpp
  )

set_target_properties(femmcli-bin
  PROPERTIES OUTPUT_NAME femmcli)

target_link_libraries(femmcli-bin femm femmcli esolver fsolver)
# vi:expandtab:tabstop=2 shiftwidth=2:
