#!/bin/bash

# exit this script if any commmand fails
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ADXE_ROOT="$DIR"/../..
CPU_CORES=4

function do_retry()
{
	cmd=$@
	retry_times=5
	retry_wait=3
	c=0
	while [ $c -lt $((retry_times+1)) ]; do
		c=$((c+1))
		echo "Executing \"$cmd\", try $c"
		$cmd && return $?
		if [ ! $c -eq $retry_times ]; then
			echo "Command failed, will retry in $retry_wait secs"
			sleep $retry_wait
		else
			echo "Command failed, giving up."
			return 1
		fi
	done
}

function build_linux()
{
    # source ../environment.sh
    cd $ADXE_ROOT
    set -x
    cmake . -G "Unix Makefiles" -Bbuild -DCMAKE_BUILD_TYPE=Release -DAX_ENABLE_EXT_IMGUI=ON
    cmake --build build --target cpp_tests -- -j `nproc`
    set +x
}

function build_osx()
{
    NUM_OF_CORES=`getconf _NPROCESSORS_ONLN`

    cd $ADXE_ROOT
    mkdir -p build
    cmake -S . -B build -GXcode -DAX_ENABLE_EXT_IMGUI=ON -DAX_USE_ALSOFT=ON
    cmake --build build --config Release --target cpp_tests -- -quiet

    exit 0
}

function build_ios()
{
    NUM_OF_CORES=`getconf _NPROCESSORS_ONLN`

    cd $ADXE_ROOT

    cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAX_USE_ALSOFT=ON
    # cmake .. -GXcode -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DCMAKE_SYSTEM_NAME=iOS -DPLATFORM=OS -DENABLE_ARC=0   # too much logs on console when "cmake --build ."
    cmake --build build --config Release --target cpp_tests -- -quiet -jobs $NUM_OF_CORES -destination "platform=iOS Simulator,name=iPhone Retina (4-inch)" 

    exit 0
}

function build_android()
{
    # print jdk detail
    echo "JAVA_HOME=$JAVA_HOME"
    java -version
    
    # Build all samples
    echo "Building Android samples ..."
    source ../environment.sh

    # build fairygui_tests
    pushd $ADXE_ROOT/tests/fairygui-tests/proj.android
    
    do_retry ./gradlew assembleRelease -PPROP_BUILD_TYPE=cmake -PPROP_APP_ABI=$BUILD_ARCH --parallel --info
    popd

    # build cpp_tests
    pushd $ADXE_ROOT/tests/cpp-tests/proj.android
    
    do_retry ./gradlew assembleRelease -PPROP_BUILD_TYPE=cmake -PPROP_APP_ABI=$BUILD_ARCH --parallel --info
    popd
}

function run_build()
{
    echo "Building..."
    
    if [ $BUILD_TARGET == 'osx' ]; then
        set -x
        build_osx
        exit 0
    fi

    if [ $BUILD_TARGET == 'ios' ]; then
        set -x
        build_ios
        exit 0
    fi

    # linux
    if [ $BUILD_TARGET == 'linux' ]; then
        build_linux
    fi

    # android
    if [ $BUILD_TARGET == 'android' ]; then
        build_android
    fi
}

run_build

