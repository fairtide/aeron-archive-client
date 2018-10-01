#!/bin/bash

BUILD_DIR="`pwd`/build"
SOURCE_DIR="`pwd`"

#if [ ! `(hash lcov 2>/dev/null && hash genhtml 2>/dev/null)` ]
#then
#    echo "lcov/genhtml not found - you need these installed to run the coverage build"
#    exit
#fi

ncpus=1
case "`uname`" in
  Linux*)
    ncpus=$(lscpu -p | egrep -v '^#' | wc -l)
    ;;
esac

echo "Will make with \"-j $ncpus\"."

#if [ -d "$BUILD_DIR" ] ; then
#  echo "Build directory ($BUILD_DIR) exists, removing."
#  rm -rf $BUILD_DIR
#fi

mkdir -p $BUILD_DIR

cd $BUILD_DIR
cmake -G "Unix Makefiles" "-DCOVERAGE_BUILD=ON" $SOURCE_DIR && make clean && make -j "$ncpus" all && ctest -C Release -j "$ncpus"
rm -rf coverage
mkdir -p coverage
lcov --directory . --base-directory . --capture -o coverage/cov.info
lcov -o coverage/cov.stripped.info --remove coverage/cov.info "/usr/include/*" "*/googletest/*" "*/test/cpp/*" "*/googlemock/*"
genhtml coverage/cov.stripped.info --demangle-cpp -o coverage
