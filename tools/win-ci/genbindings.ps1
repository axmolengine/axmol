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

## Installing python-3.10.6 and install packages
pyenv install -l
pyenv install 3.10.6

$python_root = "$pyenv_root\pyenv-win\versions\3.10.6"

$env:Path = "$python_root\Scripts\;$python_root\;$env:Path"

(Get-Command python.exe).Path
python -V

pip install PyYAML Cheetah3

## download ndk
curl -L "https://dl.google.com/android/repository/android-ndk-r19c-windows-x86_64.zip" -o "android-ndk-r19c-windows-x86_64.zip"
Expand-Archive -Path android-ndk-r19c-windows-x86_64.zip -DestinationPath .\
ls
$ndk_root=(Resolve-Path .\android-ndk-r19c).Path
# $env:ANDROID_NDK=$ndk_root

## download win64 libclang.dll 
curl -L "https://github.com/axmolengine/buildware/releases/download/llvmorg-15.0.7/llvmorg-15.0.7.zip" -o "llvmorg-15.0.7.zip"
Expand-Archive -Path llvmorg-15.0.7.zip -DestinationPath .\
Copy-Item ".\llvmorg-15.0.7\llvm\prebuilt\windows\x64\libclang.dll" -Destination "..\tools\bindings-generator\libclang"

## run genbindings.py
pwd
cd ..\tools\tolua

python genbindings.py --ndk_root "$ndk_root"

$env:Path = $storedEnvPath
cd ..\..
