cmake_minimum_required (VERSION 2.8.8)
project (fsolver)

# configure a header file to pass some of the CMake settings
# to the source code
#configure_file (
#  "${PROJECT_SOURCE_DIR}/libfemmConfig.h.in"
#  "${PROJECT_BINARY_DIR}/libfemmConfig.h"
#  )


# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories(../libfemm/liblua)
include_directories(../libfemm)

if (NOT TARGET femm)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libfemm ${CMAKE_CURRENT_BINARY_DIR}/libfemm)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libfemm/liblua ${CMAKE_CURRENT_BINARY_DIR}/libfemm/liblua)
endif ()

add_library(hsolver STATIC 
  hsolver.cpp
  matprop.cpp
  hspars.cpp
  $<TARGET_OBJECTS:femm>
  )

add_executable(hsolver-bin
  main.cpp
  )

set_target_properties(hsolver-bin
  PROPERTIES OUTPUT_NAME hsolver)

target_link_libraries(hsolver-bin hsolver)
