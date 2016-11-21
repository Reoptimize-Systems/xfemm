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
  FemmStateBase.cpp
  fparse.cpp
  fullmatrix.cpp
  IntPoint.cpp
  MagneticsDocument.cpp
  spars.cpp
  liblua/lapi.cpp
  liblua/lcode.cpp 
  liblua/ldo.cpp  
  liblua/liolib.cpp  
  liblua/lmem.cpp   
  liblua/lstate.cpp  
  liblua/ltable.cpp 
  liblua/lundump.cpp
  liblua/lauxlib.cpp 
  liblua/ldblib.cpp
  liblua/lfunc.cpp
  liblua/llex.cpp    
  liblua/lobject.cpp
  liblua/lstring.cpp
  liblua/ltests.cpp
  liblua/lvm.cpp
  liblua/lbaselib.cpp
  liblua/ldebug.cpp
  liblua/lgc.cpp  
  liblua/lmathlib.cpp 
  liblua/lparser.cpp 
  liblua/lstrlib.cpp
  liblua/ltm.cpp   
  liblua/lzio.cpp
  LuaInstance.cpp
  stringTools.cpp
  )

target_include_directories(femm PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_include_directories(femm PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/liblua> $<INSTALL_INTERFACE:include/liblua>)
