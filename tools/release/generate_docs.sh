#!/bin/sh


CONFIG_NAME=doxygen
if [ ! -z $1 ]; then
    CONFIG_NAME=$1
fi

sed -i .bak '/CC_DEPRECATED_ATTRIBUTE CC_DLL/,/NS_CC_END/d' ../../core/base/ccUTF8.h

version=`grep -e 'return' ../../core/axis.cpp | awk '{print substr($3,0,length($3)-2)}'`
echo $version

cd ../../docs
#we should  read axis.cpp and retrieve the version number
sed -i .bak "s/3\.0/${version}/g" doxygen.config

doxygen ${CONFIG_NAME}.config

#restore the old doxygen.config
mv doxygen.config.bak doxygen.config

#restore the old ccUTF8.h
mv ../core/base/ccUTF8.h.bak ../core/base/ccUTF8.h

rm debug.txt
