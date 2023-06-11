#!/bin/bash

echo "This Shell Script will install dependencies for axmol" 
echo -n "Are you continue? (y/n) "
read answer
if echo "$answer" | grep -iq "^y" ;then
    echo "It will take few minutes"
else
    exit
fi

sudo apt update

# for vm, libxxf86vm-dev also required

DEPENDS=' libx11-dev'
DEPENDS+=' automake'
DEPENDS+=' libtool'
DEPENDS+=' cmake'
DEPENDS+=' libxmu-dev'
DEPENDS+=' libglu1-mesa-dev'
DEPENDS+=' libgl2ps-dev'
DEPENDS+=' libxi-dev'
DEPENDS+=' libzip-dev'
DEPENDS+=' libpng-dev'
DEPENDS+=' libfontconfig1-dev'
DEPENDS+=' libgtk-3-dev'
DEPENDS+=' binutils'
DEPENDS+=' libbsd-dev'
DEPENDS+=' libasound2-dev'
DEPENDS+=' libxxf86vm-dev'
DEPENDS+=' libvlc-dev libvlccore-dev vlc'

# if vlc encouter codec error, install
# sudo apt install ubuntu-restricted-extras

sudo apt install --allow-unauthenticated --yes $DEPENDS > /dev/null

echo "Installing latest freetype for linux ..."
mkdir buildsrc
cd buildsrc
git clone https://github.com/freetype/freetype.git
cd freetype
git checkout VER-2-13-0
sh autogen.sh
./configure --prefix=/usr --enable-freetype-config --disable-static
sudo make install
cd ..
cd ..
