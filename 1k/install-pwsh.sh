#!/bin/bash
# Install powershell 7 on macOS, Ubuntu, ArchLinux to system path
# usage: ./install-pwsh [pwsh_ver]
#

HOST_OS=$(uname)

myRoot=$(dirname "$0")

cacheDir=~/.1kiss
mkdir -p $cacheDir

pwsh_ver=$1
if [ "$pwsh_ver" = "" ] ; then
    pwsh_ver='7.4.4'
fi

pwsh_min_ver=$2
if [ "$pwsh_min_ver" = "" ] ; then
    pwsh_min_ver='7.3.0'
fi

if [[ "$pwsh_ver" < "$pwsh_min_ver" ]] ; then
    pwsh_ver=$pwsh_min_ver
fi

function check_pwsh {
    min_ver=$1
    preferred_ver=$2
    if command -v pwsh >/dev/null ; then
        verx=$(pwsh --version)
        very="PowerShell $min_ver"
        if ([ "$preferred_ver" != "$min_ver" ] && ([[ "$verx" > "$very" ]] || [ "$verx" = "$very" ])) \
          || ([ "$preferred_ver" = "$min_ver" ] && [ "$verx" = "$very" ]) ; then
            echo "1kiss: $verx installed."
            exit 0
        fi
    fi
    echo "Installing PowerShell $preferred_ver ..."
}

HOST_ARCH=$(uname -m)
if [ "$HOST_ARCH" = 'x86_64' ] ; then
    HOST_ARCH=x64
fi

if [ $HOST_OS = 'Darwin' ] ; then
    check_pwsh $pwsh_min_ver $preferred_ver
    pwsh_pkg="powershell-$pwsh_ver-osx-$HOST_ARCH.pkg"
    pwsh_pkg_out="$cacheDir/$pwsh_pkg"
    if [ ! -f  "$pwsh_pkg_out" ] ; then
        pwsh_url="https://github.com/PowerShell/PowerShell/releases/download/v$pwsh_ver/$pwsh_pkg"
        echo "Downloading $pwsh_url ..."
        curl -L "$pwsh_url" -o "$pwsh_pkg_out"
    fi
    sudo xattr -rd com.apple.quarantine "$pwsh_pkg_out"
    sudo installer -pkg "$pwsh_pkg_out" -target /
elif [ $HOST_OS = 'Linux' ] ; then
    if command -v dpkg > /dev/null; then  # Linux distro: deb (ubuntu)
        check_pwsh $pwsh_min_ver $preferred_ver
        pwsh_pkg="powershell_$pwsh_ver-1.deb_amd64.deb"
        pwsh_pkg_out="$cacheDir/$pwsh_pkg"
        if [ ! -f  "$pwsh_pkg_out" ] ; then
            curl -L "https://github.com/PowerShell/PowerShell/releases/download/v$pwsh_ver/$pwsh_pkg" -o "$pwsh_pkg_out"
        fi
        sudo_cmd=$(which sudo)
        $sudo_cmd dpkg -i "$pwsh_pkg_out"
        $sudo_cmd apt-get install -f
    elif command -v pacman > /dev/null; then # Linux distro: Arch
        # refer: https://ephos.github.io/posts/2018-9-17-Pwsh-ArchLinux
        # available pwsh version, refer to: https://aur.archlinux.org/packages/powershell-bin
        check_pwsh $pwsh_min_ver
        git clone https://aur.archlinux.org/powershell-bin.git $cacheDir/powershell-bin
        cd $cacheDir/powershell-bin
        makepkg -si --needed --noconfirm
        cd -
    fi
else
    echo "Unsupported HOST OS: $HOST_OS"
    exit 1
fi

if command -v pwsh >/dev/null ; then
    installed_pwsh_ver=$(pwsh --version)
    echo "Install PowerShell $installed_pwsh_ver succeed."
else
    echo "Install PowerShell fail, try again"
    if [ -f "$pwsh_pkg_out" ] ; then
        rm -f "$pwsh_pkg_out"
    fi
fi
