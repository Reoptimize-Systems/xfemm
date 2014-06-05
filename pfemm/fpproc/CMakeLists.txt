cmake_minimum_required (VERSION 2.8.8)
project (fpproc)

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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libfemm ${CMAKE_CURRENT_BINARY_DIR}/libfemm)

add_library(fpproc STATIC 
  fpproc.cpp    
  makemask.cpp
  problem.cpp
  $<TARGET_OBJECTS:femm>
  )

add_executable(fpproc-test
  main.cpp
  )

target_link_libraries(fpproc-test fpproc)
