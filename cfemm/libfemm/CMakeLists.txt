cmake_minimum_required (VERSION 3.0.0)
project (femm)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64") 
  add_definitions(-fPIC) 
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

message("PROJECT_SOURCE_DIR points to " ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/liblua)
#add_subdirectory(${PROJECT_SOURCE_DIR}/liblua)

add_library(femm
  femmconstants.h # header only
  CArcSegment.cpp
  CBlockLabel.cpp
  CBoundaryProp.cpp
  CCircuit.cpp
  CCommonPoint.cpp
  CElement.cpp
  CMaterialProp.cpp
  CMeshNode.cpp
  CNode.cpp
  CPointProp.cpp
  CPointVals.cpp
  CSegment.cpp
  cspars.cpp
  cuthill.cpp
  feasolver.cpp
  femmcomplex.cpp
  FemmProblem.cpp
  FemmReader.cpp
  FemmStateBase.cpp
  femmversion.cpp
  fparse.cpp
  fullmatrix.cpp
  IntPoint.cpp
  spars.cpp
  liblua/llimits.h
  liblua/lopcodes.h
  liblua/lua.h
  liblua/luadebug.h
  liblua/lualib.h
  liblua/lapi.cpp
  liblua/lauxlib.cpp
  liblua/lbaselib.cpp
  liblua/lcode.cpp
  liblua/ldblib.cpp
  liblua/ldebug.cpp
  liblua/ldo.cpp
  liblua/lfunc.cpp
  liblua/lgc.cpp
  liblua/liolib.cpp
  liblua/llex.cpp
  liblua/lmathlib.cpp
  liblua/lmem.cpp
  liblua/lobject.cpp
  liblua/lparser.cpp
  liblua/lstate.cpp
  liblua/lstring.cpp
  liblua/lstrlib.cpp
  liblua/ltable.cpp
  liblua/ltests.cpp
  liblua/ltm.cpp
  liblua/lundump.cpp
  liblua/lvm.cpp
  liblua/lzio.cpp
  LuaInstance.cpp
  stringTools.cpp
  )

target_include_directories(femm PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_include_directories(femm PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/liblua> $<INSTALL_INTERFACE:include/liblua>)
