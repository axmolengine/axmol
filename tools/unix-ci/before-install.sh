#!/bin/bash

# exit this script if any commmand fails
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
AX_ROOT="$DIR"/../..
HOST_NAME=""
CURL="curl --retry 999 --retry-max-time 0"

function install_linux_environment()
{
    echo "Installing linux dependence packages ..."
    echo -e "y" | bash $AX_ROOT/install-deps-linux.sh
    echo "Installing linux dependence packages finished!"
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
    fi

    if [ "$GH_OS_NAME" == "osx" ]; then
        brew install nasm
        nasm -v
    fi
}

python -V
cmake --version

install_environement

echo "before-install.sh execution finished!"
