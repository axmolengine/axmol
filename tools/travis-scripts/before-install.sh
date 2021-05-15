#!/bin/bash

# exit this script if any commmand fails
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
COCOS2DX_ROOT="$DIR"/../..
HOST_NAME=""
CURL="curl --retry 999 --retry-max-time 0"

function install_android_sdk()
{
    echo "Installing android ndk ..."
    # sudo curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
    # sudo python get-pip.py
    # sudo python -m pip install retry
    python -m pip install --upgrade pip
    which python
    which pip
    python -V
    pip -V
    pip install retry
    mkdir -p ~/.android/ && touch ~/.android/repositories.cfg # Ensure cmdline-tools works well
    # cmdlinetools: commandlinetools-mac-7302050_latest.zip commandlinetools-win-7302050_latest.zip commandlinetools-linux-7302050_latest.zip
    # platforms:android-30 build-tools:30.0.3
    # full cmd: echo yes|cmdline-tools/bin/sdkmanager --verbose --sdk_root=sdk platform-tools "cmdline-tools;latest" "platforms;android-28" "build-tools;29.0.2" "ndk;19.2.5345600"
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

        sudo apt-get install nasm
        nasm -v
        
        if [ "$BUILD_TARGET" == "linux" ]; then
            install_linux_environment
        fi

        if [ "$BUILD_TARGET" == "android" ]; then
            install_android_sdk
        fi
    fi

    if [ "$TRAVIS_OS_NAME" == "osx" ]; then
        brew install nasm
        nasm -v
        
        install_python_module_for_osx
    fi
}

# should generate binding codes & cocos_files.json after merging
# function install_environement_for_after_merge()
# {
#     if [ "$TRAVIS_OS_NAME" == "osx" ]; then
#         install_python_module_for_osx
#     fi

#     echo "Building merge commit ..."
# }

# install newer python for android for ssl connection
if [ "$BUILD_TARGET" == "android" ]; then
    if [ $GITHUB_CI ]; then
        echo "Installing pyenv for github ci..."
        curl -L https://raw.githubusercontent.com/pyenv/pyenv-installer/master/bin/pyenv-installer | bash
        export PATH="/home/runner/.pyenv/bin:$PATH"
    else
        # upgrade pyenv
        cd $(pyenv root) && git checkout master && git pull && cd -
    fi
    pyenv install --list
    pyenv install $PYENV_VERSION
    pyenv versions
    # pip install pyOpenSSL ndg-httpsclient pyasn1
    # set by PYENV_VERSION environment variable implicit
    # pyenv global $PYENV_VERSION
fi

python -V
cmake --version

if [ "$BUILD_TARGET" == "android_cocos_new_test" ]; then
    sudo apt-get update
    sudo apt-get install ninja-build
    ninja --version
    sudo pip install retry
    python $COCOS2DX_ROOT/tools/appveyor-scripts/setup_android.py
    exit 0
fi

if [ "$BUILD_TARGET" == "linux_cocos_new_test" ]; then
    install_linux_environment
    # linux new lua project, so need to install
    sudo pip install retry
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
# if [ "$TRAVIS_PULL_REQUEST" == "false" ]; then
#     # only one job need to send PR, linux virtual machine has better performance
#     if [ $TRAVIS_OS_NAME == "linux" ] && [ x$GEN_BINDING_AND_COCOSFILE == x"true" ]; then
#         install_environement_for_after_merge
#     fi 
# fi

echo "before-install.sh execution finished!"
