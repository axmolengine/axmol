#!/bin/bash

# exit this script if any commmand fails
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
COCOS2DX_ROOT="$DIR"/../..
HOST_NAME=""
CURL="curl --retry 999 --retry-max-time 0"

function install_android_ndk()
{
    sudo curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
    sudo python get-pip.py
    sudo python -m pip install retry
    if [ "$BUILD_TARGET" == "android" ]\
        || [ "$BUILD_TARGET" == "android_lua" ] ; then
        python $COCOS2DX_ROOT/tools/appveyor-scripts/setup_android.py
    else
        python $COCOS2DX_ROOT/tools/appveyor-scripts/setup_android.py --ndk_only
    fi
}

function install_linux_environment()
{
    echo "Installing linux dependence packages ..."
    echo -e "y" | bash $COCOS2DX_ROOT/install-deps-linux.sh
    echo "Installing linux dependence packages finished!"
}

function download_deps()
{
    # install dpes
    pushd $COCOS2DX_ROOT
    python download-deps.py -r=yes
    popd
    echo "Downloading cocos2d-x dependence finished!"
}

function install_python_module_for_osx()
{
    pip install PyYAML
    sudo pip install Cheetah
}

# set up environment according os and target
function install_environement_for_pull_request()
{
    echo "Building pull request ..."

    if [ "$TRAVIS_OS_NAME" == "linux" ]; then
        sudo apt-get update
        sudo apt-get install ninja-build
        ninja --version
        
        sudo apt-get install build-essential checkinstall
        sudo apt-get install libreadline-gplv2-dev libncursesw5-dev libssl-dev libsqlite3-dev tk-dev libgdbm-dev libc6-dev libbz2-dev
        wget https://www.python.org/ftp/python/2.7.12/Python-2.7.12.tgz
        tar -xvf Python-2.7.12.tgz
        cd Python-2.7.12
        ./configure
        make
        echo -e "y" | sudo checkinstall
        cd ..
        
        if [ "$BUILD_TARGET" == "linux" ]; then
            install_linux_environment
        fi
    fi

    if [ "$TRAVIS_OS_NAME" == "osx" ]; then
        install_python_module_for_osx
    fi

    python -V

    # use NDK's clang to generate binding codes
    install_android_ndk
    download_deps
}

# should generate binding codes & cocos_files.json after merging
function install_environement_for_after_merge()
{
    if [ "$TRAVIS_OS_NAME" == "osx" ]; then
        install_python_module_for_osx
    fi

    echo "Building merge commit ..."
    install_android_ndk
    download_deps
}

cmake --version

if [ "$BUILD_TARGET" == "android_cocos_new_test" ]; then
    sudo apt-get update
    sudo apt-get install ninja-build
    ninja --version
    download_deps
    sudo python -m pip install retry
    python $COCOS2DX_ROOT/tools/appveyor-scripts/setup_android.py
    exit 0
fi

if [ "$BUILD_TARGET" == "linux_cocos_new_test" ]; then
    download_deps
    install_linux_environment
    # linux new lua project, so need to install
    sudo python -m pip install retry
    python $COCOS2DX_ROOT/tools/appveyor-scripts/setup_android.py --ndk_only
    exit 0
fi

# build pull request
if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then
    install_environement_for_pull_request
fi

# run after merging
# - make cocos robot to send PR to cocos2d-x for new binding codes
# - generate cocos_files.json for template
if [ "$TRAVIS_PULL_REQUEST" == "false" ]; then
    # only one job need to send PR, linux virtual machine has better performance
    if [ $TRAVIS_OS_NAME == "linux" ] && [ x$GEN_BINDING_AND_COCOSFILE == x"true" ]; then
        install_environement_for_after_merge
    fi 
fi

echo "before-install.sh execution finished!"
