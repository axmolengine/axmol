#!/bin/bash
# PowerShell Install Script
# Support macOS, Ubuntu, ArchLinux, Debian to system path
# usage: ./pwshi.sh [pwsh_ver] [min_ver]
#

HOST_OS=$(uname)

myRoot=$(dirname "$0")

cacheDir=~/.1kiss
mkdir -p $cacheDir

pwsh_ver=$1
if [ "$pwsh_ver" = "" ] ; then
    pwsh_ver='7.5.4'
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
    echo "pwshi: Installing PowerShell $preferred_ver ..."
}

HOST_ARCH=$(uname -m)
if [ "$HOST_ARCH" = 'x86_64' ] ; then
    pwsh_arch=x64
    icu_arch=amd64
else
    pwsh_arch=$HOST_ARCH
    icu_arch=$HOST_ARCH
fi

icu_pkg_out=""
if [ $HOST_OS = 'Darwin' ] ; then
    check_pwsh $pwsh_min_ver $pwsh_ver
    pwsh_pkg="powershell-$pwsh_ver-osx-$pwsh_arch.pkg"
    pwsh_pkg_out="$cacheDir/$pwsh_pkg"
    if [ ! -f  "$pwsh_pkg_out" ] ; then
        pwsh_url="https://github.com/PowerShell/PowerShell/releases/download/v$pwsh_ver/$pwsh_pkg"
        echo "pwshi: Downloading $pwsh_url ..."
        curl -L "$pwsh_url" -o "$pwsh_pkg_out"
    fi
    sudo xattr -rd com.apple.quarantine "$pwsh_pkg_out"
    sudo installer -pkg "$pwsh_pkg_out" -target /
elif [ $HOST_OS = 'Linux' ] ; then
    if command -v dpkg > /dev/null; then  # Linux distro: deb (ubuntu)
        check_pwsh $pwsh_min_ver $pwsh_ver

        sudo_cmd=$(which sudo)
        if ! command -v curl >/dev/null 2>&1; then
            $sudo_cmd apt update
            $sudo_cmd apt install --allow-unauthenticated --yes curl
        fi
        # if uname -a | grep -q "Ubuntu"; then
            # $sudo_cmd snap install powershell --classic
        # else
        pwsh_pkg="powershell_$pwsh_ver-1.deb_$icu_arch.deb"
        pwsh_pkg_out="$cacheDir/$pwsh_pkg"
        if [ ! -f  "$pwsh_pkg_out" ] ; then
            curl -L "https://github.com/PowerShell/PowerShell/releases/download/v$pwsh_ver/$pwsh_pkg" -o "$pwsh_pkg_out"
        fi

        # parse icu dep
        pwsh_deps=$(dpkg-deb -f $pwsh_pkg_out Depends)
        icu_pkg_list=$(echo "$pwsh_deps" | grep -oE 'libicu[0-9]+' | sort -u -r)

        echo $icu_pkg_list

        echo "pwshi: PowerShell requires on of follow icu versions"
        echo "$icu_pkg_list"

        # check does one of icu above icu_pkg_list installed
        found_pkg=""
        for icu_pkg in $icu_pkg_list; do
            if dpkg-query -W -f='${Status}' "$icu_pkg" 2>/dev/null | grep -q "install ok installed"; then
                echo "pwshi: Found installed ICU package: $icu_pkg"
                found_pkg="$icu_pkg"
                break
            fi
        done

        if [ -z "$found_pkg" ]; then
            echo "pwshi: No compatible ICU package is installed, starting download and installation ..."
            # resolve download url
            if uname -a | grep -q "Ubuntu"; then
                BASE_URL='https://archive.ubuntu.com/ubuntu/pool/main/i/icu/'
            else
                BASE_URL='https://deb.debian.org/debian/pool/main/i/icu/'
            fi

            icu_resp=$(curl -sL "$BASE_URL")
            remote_icu_pkg=""
            for icu_pkg in $icu_pkg_list; do
                candidates=$(echo "$icu_resp" \
                    | grep -o "${icu_pkg}_[0-9][^\"']*$icu_arch\.deb" \
                    | sort -u)

                best_ver=""
                best_file=""
                for f in $candidates; do
                    v=${f#${icu_pkg}_}        # strip prefix
                    v=${v%_${icu_arch}.deb}   # strip suffix
                    if [ -z "$best_ver" ] || dpkg --compare-versions "$v" gt "$best_ver"; then
                        best_ver="$v"
                        best_file="$f"
                    fi
                done

                if [ "$best_file" ]; then
                    remote_icu_pkg=$best_file
                    break
                fi
            done

            if [ -z "$remote_icu_pkg" ] ; then
                echo "pwshi: Resolve libicu download url fail!"
                exit 1
            fi

            icu_pkg_url="${BASE_URL}${remote_icu_pkg}"
            icu_pkg_out="$cacheDir/$remote_icu_pkg"
            # download icu package and install
            if [ ! -f  "$icu_pkg_out" ] ; then
                echo "pwshi: Downloading $icu_pkg_url to $icu_pkg_out ..."
                curl -L "$icu_pkg_url" -o "$icu_pkg_out"
            fi
            # install libicu
            $sudo_cmd dpkg -i "$icu_pkg_out"
        fi

        # install powershell
        $sudo_cmd dpkg -i "$pwsh_pkg_out"
        $sudo_cmd apt install -f --allow-unauthenticated --yes powershell
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
    echo "pwshi: Unsupported HOST OS: $HOST_OS"
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
