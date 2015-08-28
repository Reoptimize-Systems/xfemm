#!/bin/bash
#
## file: release.sh
#
# This script create releases of xfemm for various platforms. Windows builds require
# MXE: http://mxe.cc/
#
# Run this script from the directory containing it

# stop on first error
set -e

VERSION="1_5"
WORKING_COPY_DIR=$(pwd)
echo $WORKING_COPY_DIR
mkdir $WORKING_COPY_DIR/release

# create release directory

# linux 64 bit
#mkdir $WORKING_COPY_DIR/release/xfemm_linux64
# export from the working directory to the release directory
svn export $WORKING_COPY_DIR $WORKING_COPY_DIR/release/xfemm_linux64
# remove the release script
rm $WORKING_COPY_DIR/release/xfemm_linux64/release.sh
# create temp build directory
rm -rf /tmp/xfemm_linux64
mkdir -p /tmp/xfemm_linux64
# run cmake from temp dir and build
cd /tmp/xfemm_linux64
cmake -DCMAKE_BUILD_TYPE=Release $WORKING_COPY_DIR/release/xfemm_linux64/cfemm
make

# tar up the result in the release directory
cd $WORKING_COPY_DIR/release
tar cvzf xfemm_v${VERSION}_linux64.tar.gz xfemm_linux64/


# repeat the above, but cross-compiling for windows, requires MXE: http://mxe.cc/
#

# win 32 -- i686-pc-mingw32
#svn export $WORKING_COPY_DIR $WORKING_COPY_DIR/release/xfemm_mingw_win32 --native-eol CRLF
#rm $WORKING_COPY_DIR/release/xfemm_mingw_win32/release.sh

# win 64 -- x86_64-w64-mingw32 MXE target
svn export $WORKING_COPY_DIR $WORKING_COPY_DIR/release/xfemm_mingw_win64 --native-eol CRLF
rm $WORKING_COPY_DIR/release/xfemm_mingw_win64/release.sh
# create temp build directory
rm -rf /tmp/xfemm_mingw_win64
mkdir -p /tmp/xfemm_mingw_win64
# run cmake from temp dir and build cfemm
cd /tmp/xfemm_mingw_win64
cmake -DCMAKE_BUILD_TYPE=Release \
      -DEXTRA_CMAKE_CXX_FLAGS="-static -static-libgcc -static-libstdc++" \
      -DEXTRA_CMAKE_EXE_LINKER_FLAGS="-static  -static-libgcc -static-libstdc++" \
      -DCMAKE_TOOLCHAIN_FILE=/opt/mxe/usr/x86_64-w64-mingw32.static/share/cmake/mxe-conf.cmake $WORKING_COPY_DIR/release/xfemm_mingw_win64/cfemm
make

# copy over the windows dlls, and delete from dist
#cp $WORKING_COPY_DIR/release/xfemm_mingw_win64/windeps/* $WORKING_COPY_DIR/release/xfemm_mingw_win64/cfemm/lib/
#cp $WORKING_COPY_DIR/release/xfemm_mingw_win64/windeps/* $WORKING_COPY_DIR/release/xfemm_mingw_win64/cfemm/bin/
#rm -r $WORKING_COPY_DIR/release/xfemm_mingw_win64/windeps/

# tar up the result in the release directory
cd $WORKING_COPY_DIR/release
zip -r xfemm_v${VERSION}_mingw_win64.zip xfemm_mingw_win64/



# test
cd $WORKING_COPY_DIR
./test_release.sh

