#!/bin/bash
##
## Copyright 2018 Fairtide Pte. Ltd.
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##

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
