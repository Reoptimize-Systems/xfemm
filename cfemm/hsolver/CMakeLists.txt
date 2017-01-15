cmake_minimum_required (VERSION 2.8.8)
project (hsolver)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64") 
  add_definitions(-fPIC) 
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories(../libfemm/liblua)
include_directories(../libfemm)

add_library(hsolver STATIC 
  hsolver.cpp
  hspars.cpp
  )
target_link_libraries(hsolver femm)

add_executable(hsolver-bin
  main.cpp
  )

set_target_properties(hsolver-bin
  PROPERTIES OUTPUT_NAME hsolver)

target_link_libraries(hsolver-bin hsolver)
