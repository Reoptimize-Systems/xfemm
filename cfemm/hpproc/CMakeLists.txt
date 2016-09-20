cmake_minimum_required (VERSION 2.8.8)
project (hpproc)
 
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

add_library(hpproc STATIC 
  hpproc.cpp    
  hproblem.cpp
  $<TARGET_OBJECTS:femm>
  )

add_executable(hpproc-test
  main.cpp
  )

target_link_libraries(hpproc-test hpproc)
