#!/bin/bash

## file: release.sh
#
## This script create releases of xfemm for various platforms.
#
## Run this script from the directory containing it

WORKING_COPY_DIR = `pwd`
mkdir $WORKING_COPY_DIR/release

# create release directory

# linux 64 bit
mkdir $WORKING_COPY_DIR/release/xfemm_linux64
# export from the working directory to the release directory
svn export $WORKING_COPY_DIR $WORKING_COPY_DIR/release/xfemm_linux64
# remove the release script
rm $WORKING_COPY_DIR/release/xfemm_linux64/release.sh
# create temp build directory
mkdir /tmp/xfemm_linux64
# run cmake from temp dir and build
cd /tmp/xfemm_linux64
cmake $WORKING_COPY_DIR $WORKING_COPY_DIR/release/xfemm_linux64/cfemm
make
# remove all the build stuff and repeat for mfemm 
rm -r *
cmake $WORKING_COPY_DIR $WORKING_COPY_DIR/release/xfemm_linux64/mfemm/cfemm
make
# tar up the result in the release directory
cd $WORKING_COPY_DIR/release
tar cvzf xfemm_linux64.tar.gz xfemm_linux64/

# repeat the above, but cross-compiling for windows

# win 32
#mkdir $WORKING_COPY_DIR/release/xfemm_mingw_win32
#svn export $WORKING_COPY_DIR $WORKING_COPY_DIR/release/xfemm_mingw_win32 --native-eol CRLF
#rm $WORKING_COPY_DIR/release/xfemm_mingw_win32/release.sh

# win 64
#mkdir $WORKING_COPY_DIR/release/xfemm_mingw_win64
#svn export $WORKING_COPY_DIR $WORKING_COPY_DIR/release/xfemm_mingw_win64 --native-eol CRLF
#rm $WORKING_COPY_DIR/release/xfemm_mingw_win64/release.sh





