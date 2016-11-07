cmake_minimum_required (VERSION 2.8.8)
project (femmcli)

add_definitions(-DDEBUG_FEMMLUA)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64") 
  add_definitions(-fPIC) 
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories(../libfemm/liblua)
include_directories(../libfemm)

if (NOT TARGET femm)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libfemm ${CMAKE_CURRENT_BINARY_DIR}/libfemm)
endif ()

#add_library(femmcli STATIC 
#  )

add_executable(femmcli-bin
  main.cpp
  )

set_target_properties(femmcli-bin
  PROPERTIES OUTPUT_NAME femmcli)

target_link_libraries(femmcli-bin femm)
# vi:expandtab:tabstop=2 shiftwidth=2:
