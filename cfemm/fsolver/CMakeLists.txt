cmake_minimum_required (VERSION 2.8.8)
project (fsolver)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64") 
  add_definitions(-fPIC) 
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories(../libfemm/liblua)
include_directories(../libfemm)

if (NOT TARGET femm)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libfemm ${CMAKE_CURRENT_BINARY_DIR}/libfemm)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libfemm/liblua ${CMAKE_CURRENT_BINARY_DIR}/libfemm/liblua)
endif ()

add_library(fsolver STATIC 
  fsolver.cpp
  harmonic2d.cpp
  harmonicaxi.cpp
  matprop.cpp
  mmesh.h
  static2d.cpp
  staticaxi.cpp
  $<TARGET_OBJECTS:femm>
  )

add_executable(fsolver-bin
  main.cpp
  )

set_target_properties(fsolver-bin
  PROPERTIES OUTPUT_NAME fsolver)

target_link_libraries(fsolver-bin fsolver)
