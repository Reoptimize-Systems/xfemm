#!/bin/bash
#
## file: release.sh
#
# This script create releases of xfemm for various platforms. Windows builds
# require MXE: http://mxe.cc/
#
# Run this script from the directory containing it

# stop on first error
set -xe

# A POSIX variable
OPTIND=1  # Reset in case getopts has been used previously in the shell.

# Initialize our own variables:
version="dev"
working_copy_dir=$(pwd)
run_tests=false
copy_win_libs=true
skip_mex=false
make_zip=false
matlab_cmd="matlab"
matlab_extra_cmds=""
make_win_mex=false
matlab_win_lib_dir=""

usage="$(basename "$0") [-h] [-v <version>] [-t] [-m] [-z] [-c] [-l <matlab_win_lib_dir>] [-e <matlab_extra_cmds>] -- creates xfemm release

where:
    -h  show this help text
    -v  set the release version string (default: $version)
    -t  run tests
    -m  skip building mex files (requires matlab)
    -z  create zip file
    -c  command to invoke matlab (default: matlab)
    -w  attempt to cross-build matlab mex files (requires -l option)
    -l  matlab windows libraries directory path
    -e  additional command(s) to be run by matlab before building the mex files"

while getopts "h?v:twmzc:wl:e:" opt; do
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
    c)  matlab_cmd=$OPTARG
        echo "matlab_cmd: $matlab_cmd"
        ;;
    w)  make_win_mex=true
        echo "make_win_mex: $make_win_mex"
        ;;
    l)  matlab_win_lib_dir=$OPTARG
        echo "matlab_win_lib_dir: $matlab_win_lib_dir"
        ;;
    e)  matlab_extra_cmds=$OPTARG
        echo "matlab_extra_cmds: $matlab_extra_cmds"
        ;;
    esac
done

echo "Working copy dir is: ${working_copy_dir}"


# create release directory
release_prefix=/tmp/xfemm_release
mkdir -p ${release_prefix}

# linux 64 bit
#mkdir ${working_copy_dir}/release/xfemm_linux64
linux_64_common_dir_name="xfemm_linux64"
linux_64_release_dir="$release_prefix/${linux_64_common_dir_name}"
# export from the working directory to the release directory
#hg archive --rev stable ${linux_64_release_dir}
cd ${working_copy_dir}
mkdir -p ${linux_64_release_dir}
git archive --format=zip  HEAD --output ${linux_64_release_dir}/../release.zip
cd ${linux_64_release_dir}
unzip ../release.zip
# remove the release script
rm ${linux_64_release_dir}/release.sh
rm ${linux_64_release_dir}/test_release.sh
# create version file for cmake to find
echo "${version}" > "${linux_64_release_dir}/cfemm/VERSION"

# create temp build directory
rm -rf /tmp/${linux_64_common_dir_name}
mkdir -p /tmp/${linux_64_common_dir_name}
# run cmake from temp dir and build
cd /tmp/${linux_64_common_dir_name}
cmake -DCMAKE_BUILD_TYPE=Release ${linux_64_release_dir}/cfemm
make

if [ "$skip_mex" = false ]; then

  if ! [ -x "$(command -v ${matlab_cmd})" ]; then
    echo 'matlab is not installed, not building mex files using Matlab.' >&2
  else
    # buld the mex files using matlab
    #${matlab_cmd} -nodesktop -r "restoredefaultpath; cd('${linux_64_release_dir}/mfemm'); mfemm_setup('Runtests', true, 'ForceMexRecompile', true); quit"

    ${matlab_cmd} -nodesktop -r "restoredefaultpath; cd('${linux_64_release_dir}/mfemm'); ${matlab_extra_cmds}; mfemm_setup('Runtests', true, 'ForceMexRecompile', true); quit"

    if [ "$make_win_mex" = true ]; then
        ${matlab_cmd} -nodesktop -r "restoredefaultpath; cd('${linux_64_release_dir}/mfemm'); ${matlab_extra_cmds}; mfemm_setup('DoCrossBuildWin64', true, 'W64CrossBuildMexLibsDir', '${matlab_win_lib_dir}', 'Runtests', false, 'ForceMexRecompile', true); quit"
    fi

    #matlab -nodesktop -r "restoredefaultpath; cd('${linux_64_release_dir}/mfemm'); mfemm_setup('Runtests', false, 'ForceMexRecompile', true, 'CrossBuildW64', true); quit"
  fi
else
  echo 'not building mex files using Matlab as -m option has been used.' >&2
fi

# tar up the result in the release directory
cd ${release_prefix}
tar cvzf xfemm_v${version}_linux64.tar.gz ${linux_64_common_dir_name}/


# repeat the above, but cross-compiling for windows, requires MXE: http://mxe.cc/
#

# win 32 -- i686-pc-mingw32
#svn export ${working_copy_dir} ${working_copy_dir}/release/xfemm_mingw_win32 --native-eol CRLF
#rm ${working_copy_dir}/release/xfemm_mingw_win32/release.sh
win_64_common_dir_name="xfemm_mingw_win64"
win_64_release_dir="${release_prefix}/${win_64_common_dir_name}"
# win 64 -- x86_64-w64-mingw32 MXE target
cd ${working_copy_dir}
mkdir -p ${win_64_release_dir}
git archive --format=zip  HEAD --output ${win_64_release_dir}/../release.zip
cd ${win_64_release_dir}
unzip ../release.zip
# remove release scripts
rm ${win_64_release_dir}/release.sh
rm ${win_64_release_dir}/test_release.sh
# create version file for cmake to find
echo "${version}" > "${win_64_release_dir}/cfemm/VERSION"
# create temp build directory
rm -rf /tmp/${win_64_common_dir_name}
mkdir -p /tmp/${win_64_common_dir_name}
# run cmake from temp dir and build cfemm
cd /tmp/${win_64_common_dir_name}
cmake -DCMAKE_BUILD_TYPE=Release \
      -DEXTRA_CMAKE_CXX_FLAGS="-static -static-libgcc -static-libstdc++" \
      -DEXTRA_CMAKE_EXE_LINKER_FLAGS="-static  -static-libgcc -static-libstdc++" \
      -DCMAKE_TOOLCHAIN_FILE=/opt/mxe/usr/x86_64-w64-mingw32.static/share/cmake/mxe-conf.cmake ${win_64_release_dir}/cfemm
make

# copy over the windows dlls, and delete from dist
#cp ${working_copy_dir}/release/xfemm_mingw_win64/windeps/* ${working_copy_dir}/release/xfemm_mingw_win64/cfemm/lib/
#cp ${working_copy_dir}/release/xfemm_mingw_win64/windeps/* ${working_copy_dir}/release/xfemm_mingw_win64/cfemm/bin/
#rm -r ${working_copy_dir}/release/xfemm_mingw_win64/windeps/

if [ "$skip_mex" = false ]; then

  if ! [ -x "$(command -v ${matlab_cmd})" ]; then
    echo 'matlab is not installed, not building mex files using Matlab.' >&2
  else
    # buld the mex files using matlab
    ${matlab_cmd} -nodesktop -r "restoredefaultpath; cd('${win_64_release_dir}/mfemm'); ${matlab_extra_cmds}; mfemm_setup('Runtests', true, 'ForceMexRecompile', true); quit"

    if [ "$make_win_mex" = true ]; then
        ${matlab_cmd} -nodesktop -r "restoredefaultpath; cd('${win_64_release_dir}/mfemm'); ${matlab_extra_cmds}; mfemm_setup('DoCrossBuildWin64', true, 'W64CrossBuildMexLibsDir', '${matlab_win_lib_dir}', 'Runtests', false, 'ForceMexRecompile', true); quit"
    fi
  fi
else
  echo 'not building mex files using Matlab as -m option has been used.' >&2
fi

# zip up the result in the release directory
#cd ${working_copy_dir}/release
#zip -r xfemm_v${version}_mingw_win64.zip ${win_64_common_dir_name}/

if [ "$run_tests" = true ]; then
  # test
  cd ${working_copy_dir}
  ./test_release.sh ${release_dir}
else
  echo "Skipping Tests"
fi

