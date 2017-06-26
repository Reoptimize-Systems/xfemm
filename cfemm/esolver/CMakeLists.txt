cmake_minimum_required (VERSION 2.8.8)
project (esolver)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64") 
  add_definitions(-fPIC) 
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories(../libfemm/liblua)
include_directories(../libfemm)

add_library(esolver STATIC 
  esolver.cpp
  mesh.h
  )
target_link_libraries(esolver femm)
# the INSTALL_INTERFACE is just there to keep cmake happy; and to remind us that a "normal" install could some day be possible...
target_include_directories(femm PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)

add_executable(esolver-bin
  main.cpp
  )

set_target_properties(esolver-bin
  PROPERTIES OUTPUT_NAME esolver)

target_link_libraries(esolver-bin esolver)
