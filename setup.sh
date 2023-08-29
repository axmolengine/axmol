# Install powershell 7 on macOS, ubuntu linux

HOST_OS=`uname`

myRoot=$(dirname "$0")

mkdir -p $myRoot/tmp

pwsh_prog=$(which pwsh) || true

if [ -f "$pwsh_prog" ] ; then
    echo axmol: `pwsh --version` already installed.
    exit 0
fi

if [ $HOST_OS = 'Darwin' ] ; then
    curl -L https://github.com/PowerShell/PowerShell/releases/download/v7.3.6/powershell-7.3.6-osx-x64.pkg -o $myRoot/tmp/powershell-7.3.6-osx-x64.pkg
    sudo xattr -rd com.apple.quarantine $myRoot/powershell-7.3.6-osx-x64.pkg
    sudo installer -pkg $myRoot/tmp/powershell-7.3.6-osx-x64.pkg -target /
elif [ $HOST_OS = 'Linux' ] ; then
    curl -L https://github.com/PowerShell/PowerShell/releases/download/v7.3.6/powershell_7.3.6-1.deb_amd64.deb -o $myRoot/tmp/powershell_7.3.6-1.deb_amd64.deb
    sudo dpkg -i $myRoot/tmp/powershell-lts_7.3.6-1.deb_amd64.deb
    sudo apt-get install -f
else
    echo "Unsupported HOST OS: $HOST_OS"
fi
