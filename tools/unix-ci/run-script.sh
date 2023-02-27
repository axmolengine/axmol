#!/bin/bash

# exit this script if any commmand fails
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
AX_ROOT="$DIR"/../..

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
    cmake . -G "Unix Makefiles" -Bbuild -DCMAKE_BUILD_TYPE=Release -DAX_ENABLE_EXT_IMGUI=ON -DAX_ENABLE_EXT_EFFEKSEER=ON
    cmake --build build --parallel `nproc` --target cpp_tests
}

function build_osx()
{
    cmake -S . -B build -GXcode -DCMAKE_OSX_ARCHITECTURES=$BUILD_ARCH -DAX_ENABLE_EXT_IMGUI=ON -DAX_ENABLE_EXT_EFFEKSEER=ON -DAX_USE_ALSOFT=ON
    cmake --build build --parallel --config Release --target cpp_tests -- -quiet
}

function build_ios()
{
    cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.toolchain.cmake -DPLATFORM=SIMULATOR64 -DENABLE_ARC=OFF -DDEPLOYMENT_TARGET=11.0 -DAX_ENABLE_EXT_EFFEKSEER=ON -DAX_USE_ALSOFT=ON
    cmake --build build --parallel --config Release --target cpp_tests -- -quiet -destination "platform=iOS Simulator,name=iPhone Retina (4-inch)"
}

function build_tvos()
{
    cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.toolchain.cmake -DPLATFORM=SIMULATOR_TVOS -DENABLE_ARC=OFF -DDEPLOYMENT_TARGET=14.5 -DAX_ENABLE_EXT_EFFEKSEER=ON -DAX_USE_ALSOFT=ON
    cmake --build build --parallel --config Release --target cpp_tests -- -quiet -destination "platform=tvOS Simulator,name=Apple TV Simulator"
}

function build_android()
{
    # print jdk detail
    echo "JAVA_HOME=$JAVA_HOME"
    java -version

    # Build all samples
    echo "Building Android samples ..."

    # build fairygui_tests
    pushd $AX_ROOT/tests/fairygui-tests/proj.android
    do_retry ./gradlew assembleRelease -PPROP_BUILD_TYPE=cmake -PPROP_APP_ABI=$BUILD_ARCH -PRELEASE_STORE_FILE=$AX_ROOT/tools/unix-ci/axmol-ci.jks -PRELEASE_STORE_PASSWORD=axmol-ci -PRELEASE_KEY_ALIAS=axmol-ci -PRELEASE_KEY_PASSWORD=axmol-ci  --parallel --info
    popd

    # build cpp_tests
    pushd $AX_ROOT/tests/cpp-tests/proj.android
    do_retry ./gradlew assembleRelease -PPROP_BUILD_TYPE=cmake -PPROP_APP_ABI=$BUILD_ARCH -PAX_ENABLE_EXT_EFFEKSEER=ON -PRELEASE_STORE_FILE=$AX_ROOT/tools/unix-ci/axmol-ci.jks -PRELEASE_STORE_PASSWORD=axmol-ci -PRELEASE_KEY_ALIAS=axmol-ci -PRELEASE_KEY_PASSWORD=axmol-ci --parallel --info
    popd
    
    # build cpp-template-default, template project not adapte to build in engine directory for android, so needs export env AX_ROOT
    export AX_ROOT=$AX_ROOT
    pushd $AX_ROOT/templates/cpp-template-default/proj.android
    do_retry ./gradlew assembleRelease -PPROP_BUILD_TYPE=cmake -PPROP_APP_ABI=$BUILD_ARCH -PAX_ENABLE_EXT_EFFEKSEER=ON -PRELEASE_STORE_FILE=$AX_ROOT/tools/unix-ci/axmol-ci.jks -PRELEASE_STORE_PASSWORD=axmol-ci -PRELEASE_KEY_ALIAS=axmol-ci -PRELEASE_KEY_PASSWORD=axmol-ci --parallel --info
    popd
}

function run_build()
{
    set -x
    cd $AX_ROOT
    echo "Building..."

    if [ $BUILD_TARGET == 'osx' ]; then
        build_osx
    fi

    if [ $BUILD_TARGET == 'ios' ]; then
        build_ios
    fi

    if [ $BUILD_TARGET == 'tvos' ]; then
        build_tvos
    fi

    if [ $BUILD_TARGET == 'linux' ]; then
        build_linux
    fi

    if [ $BUILD_TARGET == 'android' ]; then
        build_android
    fi
}

run_build

