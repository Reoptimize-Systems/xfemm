add_library(epproc STATIC 
    epproc.cpp
    CSPointVals.cpp
    )
target_include_directories(epproc PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(epproc PUBLIC femm)

#add_executable(epproc-test
#    main.cpp
#    )
#
#target_link_libraries(epproc-test epproc)
# vi:expandtab:tabstop=4 shiftwidth=4:
