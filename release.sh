#!/bin/bash
#
## file: release.sh
#
# This script create releases of xfemm for various platforms. Windows builds
# require MXE: http://mxe.cc/
#
# Run this script from the directory containing it

# stop on first error
set -e

# A POSIX variable
OPTIND=1  # Reset in case getopts has been used previously in the shell.

# Initialize our own variables:
version="dev"
working_copy_dir=$(pwd)
run_tests=false
copy_win_libs=true
skip_mex=false
make_zip=false

usage="$(basename "$0") [-h] [-v <version>] [-t] [-m] [-z]  -- creates xfemm release

where:
    -h  show this help text
    -v  set the release version string (default: $version)
    -t  run tests
    -m  skip building mex files (requires matlab)
    -z  create zip file"

while getopts "h?v:twmz" opt; do
    case "$opt" in
    h|\?)
        echo "$usage"
        exit 0
        ;;
    v)  version=$OPTARG
        echo "Version string changed to: $version"
        ;;
    t)  run_tests=true
        echo "run_tests: $run_tests"
        ;;
    m)  skip_mex=true
        echo "skip_mex: $skip_mex"
        ;;
    z)  make_zip=true
        echo "make_zip: $make_zip"
        ;;
    esac
done

echo "Working copy dir is: $working_copy_dir"
mkdir $working_copy_dir/release

# create release directory

# linux 64 bit
#mkdir $working_copy_dir/release/xfemm_linux64
LINUX_64_DIR="xfemm_linux64"
LINUX_64_RELEASE_DIR="$working_copy_dir/release/$LINUX_64_DIR"
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

if [ "$skip_mex" = false ]; then

  if ! [ -x "$(command -v matlab)" ]; then
    echo 'matlab is not installed, not building mex files using Matlab.' >&2
  else
    # buld the mex files using matlab
    matlab -nodesktop -r "restoredefaultpath; cd('$LINUX_64_RELEASE_DIR/mfemm'); mfemm_setup('Runtests', true, 'ForceMexRecompile', true); quit"
    #matlab -nodesktop -r "restoredefaultpath; cd('$LINUX_64_RELEASE_DIR/mfemm'); mfemm_setup('Runtests', false, 'ForceMexRecompile', true, 'CrossBuildW64', true); quit"
  fi
else
  echo 'not building mex files using Matlab as -m option has been used.' >&2
fi

# tar up the result in the release directory
cd $working_copy_dir/release
tar cvzf xfemm_v${version}_linux64.tar.gz $LINUX_64_DIR/


# repeat the above, but cross-compiling for windows, requires MXE: http://mxe.cc/
#

# win 32 -- i686-pc-mingw32
#svn export $working_copy_dir $working_copy_dir/release/xfemm_mingw_win32 --native-eol CRLF
#rm $working_copy_dir/release/xfemm_mingw_win32/release.sh
WIN_64_DIR="xfemm_mingw_win64"
WIN_64_RELEASE_DIR="$working_copy_dir/release/$WIN_64_DIR"
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
#cp $working_copy_dir/release/xfemm_mingw_win64/windeps/* $working_copy_dir/release/xfemm_mingw_win64/cfemm/lib/
#cp $working_copy_dir/release/xfemm_mingw_win64/windeps/* $working_copy_dir/release/xfemm_mingw_win64/cfemm/bin/
#rm -r $working_copy_dir/release/xfemm_mingw_win64/windeps/

if ! [ -x "$(command -v matlab)" ]; then
  echo 'matlab is not installed, not building mex files using Matlab.' >&2
else
  # buld the mex files using matlab
  matlab -nodesktop -r "restoredefaultpath; cd('$WIN_64_RELEASE_DIR/mfemm'); mfemm_setup('Runtests', true, 'ForceMexRecompile', true); quit"
  #matlab -nodesktop -r "restoredefaultpath; cd('$WIN_64_RELEASE_DIR/mfemm'); mfemm_setup('Runtests', false, 'ForceMexRecompile', true, 'CrossBuildW64', true); quit"
fi

# zip up the result in the release directory
#cd $working_copy_dir/release
#zip -r xfemm_v${version}_mingw_win64.zip $WIN_64_DIR/

if [ "$run_tests" = true ]; then
  # test
  cd ${working_copy_dir}
  ./test_release.sh ${release_dir}
else
  echo "Skipping Tests"
fi

