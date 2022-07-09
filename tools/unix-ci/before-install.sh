#!/bin/bash

# exit this script if any commmand fails
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
AXIS_ROOT="$DIR"/../..
HOST_NAME=""
CURL="curl --retry 999 --retry-max-time 0"

function install_android_sdk()
{
    echo "Installing android sdk,ndk ..."
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
        python $AXIS_ROOT/tools/unix-ci/setup_android.py
    else
        python $AXIS_ROOT/tools/unix-ci/setup_android.py --ndk_only
    fi
}

function install_linux_environment()
{
    echo "Installing linux dependence packages ..."
    echo -e "y" | bash $AXIS_ROOT/install-deps-linux.sh
    echo "Installing linux dependence packages finished!"
}

function install_python_module_for_osx()
{
    pip install PyYAML
    sudo pip install Cheetah
}

# set up environment according os and target
function install_environement()
{
    echo "Installing environement ..."

    if [ "$GH_OS_NAME" == "linux" ]; then
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

    if [ "$GH_OS_NAME" == "osx" ]; then
        brew install nasm
        nasm -v
        
        install_python_module_for_osx
    fi
}

# install newer python for android for ssl connection
if [ "$GH_OS_NAME" == "linux" ]; then
    echo "Installing pyenv for github ci..."
    curl -L https://raw.githubusercontent.com/pyenv/pyenv-installer/master/bin/pyenv-installer | bash
    export PATH="/home/runner/.pyenv/bin:$PATH"
    pyenv install --list
    pyenv install $PYENV_VERSION
    pyenv versions
fi

python -V
cmake --version

install_environement

echo "before-install.sh execution finished!"
