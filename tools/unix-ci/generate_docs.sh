#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo -e "y" | sudo apt-get install doxygen

CONFIG_NAME=doxygen
if [ ! -z $1 ]; then
    CONFIG_NAME=$1
fi

# sed -i .bak '/AX_DEPRECATED_ATTRIBUTE AX_DLL/,/NS_AX_END/d' ../../core/base/ccUTF8.h

version=`grep -e 'AX_VERSION_STR' $DIR/../../core/axmolver.h.in | awk '{print substr($3,0,length($3))}'`
version=`echo $version|sed "s/\"//g"` # remove quotes
echo $version

cd $DIR/../../docs
#we should  read axmolver.h and retrieve the version number
sed -i.bak "s~v1.0~v$version~g" doxygen.config

doxygen ${CONFIG_NAME}.config

#restore the old doxygen.config
mv doxygen.config.bak doxygen.config

#restore the old ccUTF8.h
# mv ../core/base/ccUTF8.h.bak ../core/base/ccUTF8.h

# rm debug.txt
