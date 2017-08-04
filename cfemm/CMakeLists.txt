cmake_minimum_required (VERSION 3.0.0)
project (cfemm)

set (CMAKE_CXX_STANDARD 14)

set(GCC_COVERAGE_COMPILE_FLAGS)
set(GCC_WARNING_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CMAKE_CXX_FLAGS} ${GCC_WARNING_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} ${EXTRA_CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}")

# set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

macro(add_flag _flag _docstring)
  option(${_flag} ${_docstring} OFF)
  if (${_flag})
      message(STATUS "Enabling ${_flag}")
    add_definitions(-D${_flag})
  endif()
endmacro()
add_flag(DEBUG_FEMMLUA "Enable debug output for lua interface")
add_flag(DEBUG_FEMMCLI "Enable debug output for femmcli")
add_flag(DEBUG_PARSER "Enable debug output for parser functions")


add_subdirectory(libfemm)
#add_subdirectory(esolver)
add_subdirectory(femmcli)
add_subdirectory(fmesher)
add_subdirectory(fsolver)
add_subdirectory(fpproc)
add_subdirectory(hsolver)
add_subdirectory(hpproc)
