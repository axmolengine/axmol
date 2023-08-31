# Install powershell 7 on macOS, Ubuntu, ArchLinux

HOST_OS=$(uname)

myRoot=$(dirname "$0")

mkdir -p $myRoot/tmp

pwsh_ver='7.3.6'

if command -v pwsh >/dev/null ; then
    pwsh_veri_a=$(pwsh --version)
    pwsh_veri_b="PowerShell $pwsh_ver"
    if [[ "$pwsh_veri_b" < "$pwsh_veri_a" || "$pwsh_veri_b" == "$pwsh_veri_a" ]] ; then
        echo axmol: $pwsh_veri_a already installed.
        exit 0
    fi
fi

echo "Installing PowerShell $pwsh_ver ..."

HOST_ARCH=$(uname -m)
if [ "$HOST_ARCH" = 'x86_64' ] ; then
    HOST_ARCH=x64
fi

if [ $HOST_OS = 'Darwin' ] ; then
    pwsh_pkg="powershell-$pwsh_ver-osx-$HOST_ARCH.pkg"
    pwsh_pkg_out="$myRoot/tmp/$pwsh_pkg"
    if [ ! -f  "$pwsh_pkg_out" ] ; then
        curl -L "https://github.com/PowerShell/PowerShell/releases/download/v$pwsh_ver/$pwsh_pkg_name" -o "$pwsh_pkg_out"
    fi
    sudo xattr -rd com.apple.quarantine "$pwsh_pkg_out"
    sudo installer -pkg "$pwsh_pkg_out" -target /
elif [ $HOST_OS = 'Linux' ] ; then
    if which dpkg > /dev/null; then  # Linux distro: deb (ubuntu)
        pwsh_pkg="powershell_$pwsh_ver-1.deb_amd64.deb"
        pwsh_pkg_out="$myRoot/tmp/$pwsh_pkg"
        if [ ! -f  "$pwsh_pkg_out" ] ; then
            curl -L "https://github.com/PowerShell/PowerShell/releases/download/v$pwsh_ver/$pwsh_pkg" -o "$pwsh_pkg_out"
        fi
        sudo dpkg -i "$pwsh_pkg_out"
        sudo apt-get install -f
    elif which pacman > /dev/null; then # Linux distro: Arch
        # refer: https://ephos.github.io/posts/2018-9-17-Pwsh-ArchLinux
        git clone https://aur.archlinux.org/powershell-bin.git $myRoot/tmp/powershell-bin
        cd $myRoot/tmp/powershell-bin
        makepkg -si --needed --noconfirm
        cd -
    fi
else
    echo "Unsupported HOST OS: $HOST_OS"
    exit 1
fi

echo "Installing PowerShell $pwsh_ver done"
