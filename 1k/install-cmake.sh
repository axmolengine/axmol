#!/bin/bash
# Install cmake to system path

cmake_ver=$1
if [ "$cmake_ver" = "" ] ; then
    cmake_ver='3.29.1'
fi

cacheDir=~/.1kiss
mkdir -p $cacheDir

install_script=$cacheDir/cmake-${cmake_ver}-linux-x86_64.sh

if [ ! -f "${install_script}" ] ; then
    wget https://github.com/Kitware/CMake/releases/download/v${cmake_ver}/cmake-${cmake_ver}-linux-x86_64.sh -O $install_script
fi

chmod u+x ${install_script}
${install_script} --skip-license --prefix=/usr/local
