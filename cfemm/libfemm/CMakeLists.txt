cmake_minimum_required (VERSION 2.8.8)
project (femm)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64") 
  add_definitions(-fPIC) 
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

message("PROJECT_SOURCE_DIR points to " ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/liblua)
#add_subdirectory(${PROJECT_SOURCE_DIR}/liblua)

add_library(femm OBJECT
  CArcSegment.cpp
  CArcSegment.h
  CBlockLabel.cpp
  CBlockLabel.h
  CBoundaryProp.cpp
  CBoundaryProp.h
  CCircuit.cpp
  CCircuit.h
  CElement.cpp
  CElement.h
  CMaterialProp.cpp
  CMaterialProp.h
  CMeshNode.cpp
  CMeshNode.h
  CNode.cpp
  CNode.h
  CPointProp.cpp
  CPointProp.h
  CPointVals.cpp
  CPointVals.h
  CSegment.cpp
  CSegment.h
  cspars.cpp
  cspars.h
  cuthill.cpp
  feasolver.cpp
  feasolver.h
  femmcomplex.cpp
  femmcomplex.h
  femmconstants.h
  fparse.cpp
  fparse.h
  fullmatrix.cpp
  fullmatrix.h
  mesh.h
  #problem.h
  spars.cpp
  spars.h
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
  )


