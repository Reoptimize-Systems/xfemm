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

VERSION="1_8"
WORKING_COPY_DIR=$(pwd)
echo $WORKING_COPY_DIR
mkdir $WORKING_COPY_DIR/release

# create release directory

# linux 64 bit
#mkdir $WORKING_COPY_DIR/release/xfemm_linux64
LINUX_64_DIR="xfemm_linux64"
LINUX_64_RELEASE_DIR="$WORKING_COPY_DIR/release/$LINUX_64_DIR"
# export from the working directory to the release directory
hg archive $LINUX_64_RELEASE_DIR
# remove file created by mercurial
rm $LINUX_64_RELEASE_DIR/.hg_archival.txt
# remove the release script
rm $LINUX_64_RELEASE_DIR/release.sh
rm $LINUX_64_RELEASE_DIR/test_release.sh
# create temp build directory
rm -rf /tmp/$LINUX_64_DIR
mkdir -p /tmp/$LINUX_64_DIR
# run cmake from temp dir and build
cd /tmp/$LINUX_64_DIR
cmake -DCMAKE_BUILD_TYPE=Release $LINUX_64_RELEASE_DIR/cfemm
make

if ! [ -x "$(command -v matlab)" ]; then
  echo 'matlab is not installed, not building mex files using Matlab.' >&2
else
  # buld the mex files using matlab
  matlab -nodesktop -r "restoredefaultpath; cd('$LINUX_64_RELEASE_DIR/mfemm'); mfemm_setup('Runtests', true, 'ForceMexRecompile', true); quit"
  #matlab -nodesktop -r "restoredefaultpath; cd('$LINUX_64_RELEASE_DIR/mfemm'); mfemm_setup('Runtests', false, 'ForceMexRecompile', true, 'CrossBuildW64', true); quit"
fi

# tar up the result in the release directory
cd $WORKING_COPY_DIR/release
tar cvzf xfemm_v${VERSION}_linux64.tar.gz $LINUX_64_DIR/


# repeat the above, but cross-compiling for windows, requires MXE: http://mxe.cc/
#

# win 32 -- i686-pc-mingw32
#svn export $WORKING_COPY_DIR $WORKING_COPY_DIR/release/xfemm_mingw_win32 --native-eol CRLF
#rm $WORKING_COPY_DIR/release/xfemm_mingw_win32/release.sh
WIN_64_DIR="xfemm_mingw_win64"
WIN_64_RELEASE_DIR="$WORKING_COPY_DIR/release/$WIN_64_DIR"
# win 64 -- x86_64-w64-mingw32 MXE target
hg archive $WIN_64_RELEASE_DIR # TODO: windows line endings?
# remove file created by hg
rm $WIN_64_RELEASE_DIR/.hg_archival.txt
# remove release scripts
rm $WIN_64_RELEASE_DIR/release.sh
rm $WIN_64_RELEASE_DIR/test_release.sh
# create temp build directory
rm -rf /tmp/$WIN_64_DIR
mkdir -p /tmp/$WIN_64_DIR
# run cmake from temp dir and build cfemm
cd /tmp/$WIN_64_DIR
cmake -DCMAKE_BUILD_TYPE=Release \
      -DEXTRA_CMAKE_CXX_FLAGS="-static -static-libgcc -static-libstdc++" \
      -DEXTRA_CMAKE_EXE_LINKER_FLAGS="-static  -static-libgcc -static-libstdc++" \
      -DCMAKE_TOOLCHAIN_FILE=/opt/mxe/usr/x86_64-w64-mingw32.static/share/cmake/mxe-conf.cmake $WIN_64_RELEASE_DIR/cfemm
make

# copy over the windows dlls, and delete from dist
#cp $WORKING_COPY_DIR/release/xfemm_mingw_win64/windeps/* $WORKING_COPY_DIR/release/xfemm_mingw_win64/cfemm/lib/
#cp $WORKING_COPY_DIR/release/xfemm_mingw_win64/windeps/* $WORKING_COPY_DIR/release/xfemm_mingw_win64/cfemm/bin/
#rm -r $WORKING_COPY_DIR/release/xfemm_mingw_win64/windeps/

if ! [ -x "$(command -v matlab)" ]; then
  echo 'matlab is not installed, not building mex files using Matlab.' >&2
else
  # buld the mex files using matlab
  matlab -nodesktop -r "restoredefaultpath; cd('$WIN_64_RELEASE_DIR/mfemm'); mfemm_setup('Runtests', true, 'ForceMexRecompile', true); quit"
  #matlab -nodesktop -r "restoredefaultpath; cd('$WIN_64_RELEASE_DIR/mfemm'); mfemm_setup('Runtests', false, 'ForceMexRecompile', true, 'CrossBuildW64', true); quit"
fi

# tar up the result in the release directory
cd $WORKING_COPY_DIR/release
zip -r xfemm_v${VERSION}_mingw_win64.zip $WIN_64_DIR/

# test
cd $WORKING_COPY_DIR
./test_release.sh

