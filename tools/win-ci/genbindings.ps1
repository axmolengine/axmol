# Store env path
$storedEnvPath = $env:Path

# Make and goto ignored build directory
mkdir build
cd build

## Installing pyenv
mkdir $HOME/.pyenv/

curl -L "https://github.com/pyenv-win/pyenv-win/archive/master.zip" -o "pyenv-win.zip"
Expand-Archive -Path pyenv-win.zip -DestinationPath .\
$pyenv_root="$HOME\.pyenv"
move "pyenv-win-master\pyenv-win" "$pyenv_root\"
$pyenv_bin="$pyenv_root\pyenv-win\bin"

$env:Path = "$pyenv_bin;$env:Path"

## Installing python-2.7.18-win32 and install packages
pyenv install -l
pyenv install 2.7.18-win32

$python_root = "$pyenv_root\pyenv-win\versions\2.7.18-win32"

$env:Path = "$python_root\Scripts\;$python_root\;$env:Path"

(Get-Command python.exe).Path
python -V

pip install PyYAML Cheetah

## download ndk
curl -L "https://dl.google.com/android/repository/android-ndk-r19c-windows-x86_64.zip" -o "android-ndk-r19c-windows-x86_64.zip"
Expand-Archive -Path android-ndk-r19c-windows-x86_64.zip -DestinationPath .\
ls
$ndk_root=(Resolve-Path .\android-ndk-r19c).Path
# $env:ANDROID_NDK=$ndk_root


## run genbindings.py
pwd
cd ..\tools\tolua
python genbindings.py --ndk_root "$ndk_root"

$env:Path = $storedEnvPath
cd ..\..
