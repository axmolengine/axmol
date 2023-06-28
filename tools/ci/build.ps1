# //////////////////////////////////////////////////////////////////////////////////////////
# // A multi-platform support c++11 library with focus on asynchronous socket I/O for any
# // client application.
# //////////////////////////////////////////////////////////////////////////////////////////
# 
# The MIT License (MIT)
# 
# Copyright (c) 2012-2023 HALX99
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# 
# 
# The build.ps1
# options
#  -p: build target platform: win32,winuwp,linux,android,osx,ios,tvos,watchos
#      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first, 
#      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
#  -a: build arch: x86,x64,armv7,arm64
#  -cc: c/c++ compiler toolchain: clang, msvc, gcc, mingw-gcc or empty use default installed on current OS
#       msvc: msvc-120, msvc-141
#       ndk: ndk-r16b, ndk-r16b+
#  -cm: additional cmake options: i.e.  -cm '-DCXX_STD=23','-DYASIO_ENABLE_EXT_HTTP=OFF'
#  -cb: additional cross build options: i.e. -cb '--config','Release'
#  -cwd: the build workspace, i.e project root which contains root CMakeLists.txt or others
# support matrix
#   | OS        |   Build targets     |  Build toolchain     |
#   +----------+----------------------+----------------------+
#   | Windows  |  win32,winuwp        | msvc,clang,mingw-gcc |
#   | Linux    | linux,android        | gcc,clang            |        
#   | macOS    | osx,ios,tvos,watchos | clang                |
#  
#

$options = @{p = ''; a = 'x64'; cc = '';  cm = @(); cb = @(); cwd = $null; dll = $false }

$optName = $null
foreach ($arg in $args) {
    if (!$optName) {
        if ($arg.StartsWith('-')) { 
            $optName = $arg.SubString(1)
        }
    } else {
        if ($options.Contains($optName)) {
            $options[$optName] = $arg
        } else {
            Write-Host "Warning: ignore unrecognized option: $optName"
        }
        $optName = $null
    }
}

$pwsh_ver = $PSVersionTable.PSVersion.ToString()

Write-Host "PowerShell $pwsh_ver"
Write-Host $(Out-String -InputObject $options)

# The preferred cmake version to install when system installed cmake < 3.13.0
$cmake_ver = '3.26.4'
$cmake_ver_minimal = '3.13.0'

# if found or installed, the ndk_root indicate the root path of installed ndk
$ndk_root = $null
$ninja_prog = $null

$myRoot = $PSScriptRoot

$HOST_WIN   = 0 # targets: win,uwp,android
$HOST_LINUX = 1 # targets: linux,android 
$HOST_MAC   = 2 # targets: android,ios,osx(macos),tvos,watchos

# 0: windows, 1: linux, 2: macos
if ($IsWindows -or ("$env:OS" -eq 'Windows_NT')) {
    $HOST_OS = $HOST_WIN
    $envPathSep = ';'
}
else {
    $envPathSep = ':'
    if($IsLinux) {
        $HOST_OS = $HOST_LINUX
    }
    elseif($IsMacOS) {
        $HOST_OS = $HOST_MAC
    }
    else {
        throw "Unsupported host OS for building target $(options.p)"
    }
}

$exeSuffix = if ($HOST_OS -eq 0) {'.exe'} else {''}

$CONFIG_DEFAULT_OPTIONS = @()
$HOST_OS_NAME = $('windows', 'linux', 'macos').Get($HOST_OS)

# determine build target os
$BUILD_TARGET = $options.p
if (!$BUILD_TARGET) {
    $BUILD_TARGET = $('win32', 'linux', 'osx').Get($HOST_OS)
}

# determine toolchain
$TOOLCHAIN = $options.cc
$toolchains = @{ 
    'win32' = 'msvc';
    'winuwp' = 'msvc';
    'linux' = 'gcc'; 
    'android' = 'ndk';
    'osx' = 'clang';
    'ios' = 'clang';
    'tvos' = 'clang';
    'watchos' = 'clang';
}
if (!$TOOLCHAIN) {
    $TOOLCHAIN = $toolchains[$BUILD_TARGET]
}
$TOOLCHAIN_INFO = $TOOLCHAIN.Split('-')
$TOOLCHAIN_VER = $null
if ($TOOLCHAIN_INFO.Count -ge 2) {
    $toolVer = $TOOLCHAIN_INFO[$TOOLCHAIN_INFO.Count - 1]
    if ($toolVer -match "\d+") {
        $TOOLCHAIN_NAME = $TOOLCHAIN_INFO[0..($TOOLCHAIN_INFO.Count - 2)] -join '-'
        $TOOLCHAIN_VER = $toolVer
    }
}
if (!$TOOLCHAIN_VER) {
    $TOOLCHAIN_NAME = $TOOLCHAIN
}

# determine build script workspace
$stored_cwd = $(Get-Location).Path
if ($options.cwd) {
    Set-Location $options.cwd
}
$cwd = $(Get-Location).Path

$tools_dir = Join-Path -Path $cwd -ChildPath 'tools'
if (!(Test-Path "$tools_dir" -PathType Container)) {
    mkdir $tools_dir
}

Write-Host "cwd=$cwd, tools_dir=$tools_dir"

function download_file($url, $out) {
    Write-Host "Downloading $url to $out ..."
    if ($pwsh_ver -ge '7.0')  {
        curl -L $url -o $out
    } else {
        Invoke-WebRequest -Uri $url -OutFile $out
    }
}

# now windows only
function setup_cmake() {
    $cmake_prog = (Get-Command "cmake" -ErrorAction SilentlyContinue).Source
    if ($cmake_prog) {
        $_cmake_ver = $($(cmake --version | Select-Object -First 1) -split ' ')[2]
    } else {
        $_cmake_ver = '0.0.0'
    }
    if ($_cmake_ver -ge $cmake_ver_minimal) {
        Write-Host "Using installed cmake $cmake_prog, version: $_cmake_ver"
    } else {
        
        Write-Host "The installed cmake $_cmake_ver too old, installing newer $cmake_ver ..."

        $cmake_suffix = @(".zip", ".sh", ".tar.gz").Get($HOST_OS)
        if ($HOST_OS -ne $HOST_MAC) {
            $cmake_dir = "cmake-$cmake_ver-$HOST_OS_NAME-x86_64"
        } else {
            $cmake_dir = "cmake-$cmake_ver-$HOST_OS_NAME-universal"
        }
        $cmake_root = $(Join-Path -Path $tools_dir -ChildPath $cmake_dir)
        $cmake_pkg_name = "$cmake_dir$cmake_suffix"
        $cmake_pkg_path = "$cmake_root$cmake_suffix"
        if (!(Test-Path $cmake_root -PathType Container)) {
            $cmake_base_uri = 'https://github.com/Kitware/CMake/releases/download'
            $cmake_url = "$cmake_base_uri/v$cmake_ver/$cmake_pkg_name"
            if (!(Test-Path $cmake_pkg_path -PathType Leaf)) {
                download_file "$cmake_url" "$cmake_pkg_path"
            }

            if ($HOST_OS -eq $HOST_WIN) {
                Expand-Archive -Path $cmake_pkg_path -DestinationPath $tools_dir\
            }
            elseif($HOST_OS -eq $HOST_LINUX) {
                chmod 'u+x' "$cmake_pkg_path"
                mkdir $cmake_root
                & "$cmake_pkg_path" '--skip-license' '--exclude-subdir' "--prefix=$cmake_root"
            }
            elseif($HOST_OS -eq $HOST_MAC) {
                tar xvf "$cmake_root.tar.gz" -C "$tools_dir/"
            }
        }

        $cmake_bin = $null
        if ($HOST_OS -ne $HOST_MAC) {
            $cmake_bin = Join-Path -Path $cmake_root -ChildPath 'bin'
        } else {
            if ((Test-Path '/Applications/CMake.app' -PathType Container)) { # upgrade installed cmake
                Remove-Item '/Applications/CMake.app' -Recurse
                Move-Item "$cmake_root/CMake.app" '/Applications/'
            } else {
                $cmake_bin = "$cmake_root/CMake.app/Contents/bin"
            }
        }
        if (($null -ne $cmake_bin) -and ($env:PATH.IndexOf($cmake_bin) -eq -1)) {
            $env:PATH = "$cmake_bin$envPathSep$env:PATH"
        }
        $cmake_prog=(Get-Command "cmake" -ErrorAction SilentlyContinue).Source
        if ($cmake_prog) {
            $_cmake_ver = $($(cmake --version | Select-Object -First 1) -split ' ')[2]
        }
        if ($_cmake_ver -ge $cmake_ver_minimal) {
            Write-Host "Install cmake $_cmake_ver succeed"
        }
        else {
            throw "Install cmake $_cmake_ver fail"
        }
    }
}

function setup_ninja() {
    $ninja_prog=(Get-Command "ninja" -ErrorAction SilentlyContinue).Source
    if (!$ninja_prog) {
        $suffix = $('win', 'linux', 'mac').Get($HOST_OS)
        $ninja_bin = (Resolve-Path "$tools_dir/ninja-$suffix" -ErrorAction SilentlyContinue).Path
        if (!$ninja_bin) {
            download_file "https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-$suffix.zip" "$tools_dir/ninja-$suffix.zip"
            Expand-Archive -Path $tools_dir/ninja-$suffix.zip -DestinationPath "$tools_dir/ninja-$suffix/"
            $ninja_bin = (Resolve-Path "$tools_dir/ninja-$suffix" -ErrorAction SilentlyContinue).Path
        }
        if ($env:PATH.IndexOf($ninja_bin) -eq -1) {
            $env:PATH = "$ninja_bin$envPathSep$env:PATH"
        }
        $ninja_prog = (Join-Path -Path $ninja_bin -ChildPath ninja$exeSuffix)
    } else {
        Write-Host "Using installed ninja: $ninja_prog, version: $(ninja --version)"
    }
    return $ninja_prog
}

function setup_ndk() {
    # setup ndk
    $ndk_ver = $TOOLCHAIN_VER
    if (!$ndk_ver) {
        $ndk_ver = 'r16b+'
    }

    $IsGraterThan = $ndk_ver.EndsWith('+')
    if($IsGraterThan) {
        $ndk_ver = $ndk_ver.Substring(0, $ndk_ver.Length - 1)
    }

    # find ndk from env:ANDROID_HOME
    if("$env:ANDROID_HOME" -ne '') {
        $ndk_minor_base = [int][char]'a'

        $ndk_major = ($ndk_ver -replace '[^0-9]', '')
        $ndk_minor_off = "$ndk_major".Length + 1
        $ndk_minor = if($ndk_minor_off -lt $ndk_ver.Length) {"$([int][char]$ndk_ver.Substring($ndk_minor_off) - $ndk_minor_base)"} else {'0'}
        $ndk_rev_base = "$ndk_major.$ndk_minor"

        # find ndk in sdk
        $ndks = [ordered]@{}
        $ndk_rev_max = '0.0'
        foreach($item in $(Get-ChildItem -Path "$env:ANDROID_HOME/ndk")) {
            $ndkDir = $item.FullName
            $sourceProps = "$ndkDir/source.properties"
            if (Test-Path $sourceProps -PathType Leaf) {
                $verLine = $(Get-Content $sourceProps | Select-Object -Index 1)
                $ndk_rev = $($verLine -split '=').Trim()[1].split('.')[0..1] -join '.'
                $ndks.Add($ndk_rev, $ndkDir)
                if ($ndk_rev_max -le $ndk_rev) {
                    $ndk_rev_max = $ndk_rev
                }
            }
        }
        if ($IsGraterThan) {
            if ($ndk_rev_max -ge $ndk_rev_base) {
                $ndk_root = $ndks[$ndk_rev_max]
            }
        } else {
            $ndk_root = $ndks[$ndk_rev_base]
        }
    }
    
    if (Test-Path "$ndk_root" -PathType Container)
    {
        Write-Host "Using installed ndk: $ndk_root ..."
    }
    else {
        $suffix = "$(('windows', 'linux', 'darwin').Get($HOST_OS))$(if ("$ndk_ver" -le "r22z") {'-x86_64'} else {''})"
        $ndk_root = "$tools_dir/android-ndk-$ndk_ver"
        if (!(Test-Path "$ndk_root" -PathType Container)) {
            $ndk_package="android-ndk-$ndk_ver-$suffix"
            download_file "https://dl.google.com/android/repository/$ndk_package.zip" "$tools_dir/$ndk_package.zip"
            Expand-Archive -Path $tools_dir/$ndk_package.zip -DestinationPath $tools_dir/
        }
    }

    return $ndk_root
}

# preprocess methods: 
#   <param>-inputOptions</param> [CMAKE_OPTIONS]
function preprocess_win([string[]]$inputOptions) {
    $outputOptions = $inputOptions

    if ($TOOLCHAIN_NAME -eq 'msvc') { # Generate vs2019 on github ci
        # Determine arch name
        $arch = if ($options.a -eq 'x86') {'Win32'} else {$options.a}

        $VSWHERE_EXE = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
        $eap = $ErrorActionPreference
        $ErrorActionPreference = 'SilentlyContinue'
        $VS2019_OR_LATER_VESION = $null
        $VS2019_OR_LATER_VESION = (& $VSWHERE_EXE -version '16.0' -property installationVersion)
        $ErrorActionPreference = $eap

        # arch
        if($VS2019_OR_LATER_VESION) {
            $outputOptions += '-A', $arch
            if ($TOOLCHAIN_VER) {
                $outputOptions += "-Tv$TOOLCHAIN_VER"
            }
        }
        else {
            $gens = @{
                '120' = 'Visual Studio 12 2013';
                '140' = 'Visual Studio 14 2015'
                "150" = 'Visual Studio 15 2017';
            }
            $gen = $gens[$TOOLCHAIN_VER]
            if(!$gen) {
                throw "Unsupported toolchain: $TOOLCHAIN"
            }
            if ($options.a -eq "x64") {
                $gen += ' Win64'
            }
            $outputOptions += '-G', $gen
        }
        
        # platform
        if ($BUILD_TARGET -eq "winuwp") {
            '-DCMAKE_SYSTEM_NAME=WindowsStore', '-DCMAKE_SYSTEM_VERSION=10.0'
        }

        if ($options.dll) {
            $outputOptions += '-DBUILD_SHARED_LIBS=TRUE'
        }
    }
    elseif($TOOLCHAIN_NAME -eq 'clang') {
        Write-Host (clang --version)
        $outputOptions += '-G', 'Ninja Multi-Config', '-DCMAKE_C_COMPILER=clang', '-DCMAKE_CXX_COMPILER=clang++'
    }
    else { # Generate mingw
        $outputOptions += '-G', 'Ninja Multi-Config'
    }
    return $outputOptions
}

function preprocess_linux([string[]]$inputOptions) {
    $outputOptions = $inputOptions
    return $outputOptions
}

function preprocess_andorid([string[]]$inputOptions) {
    $outputOptions = $inputOptions

    $t_archs = @{arm64 = 'arm64-v8a'; armv7 = 'armeabi-v7a'; x64 = 'x86_64'; x86 = 'x86';}

    if ($options.a.GetType() -eq [object[]]) {
        $archlist = [string[]]$options.a
    } else {
        $archlist = $options.a.Split(';')
    }
    for ($i = 0; $i -lt $archlist.Count; ++$i) {
        $arch = $archlist[$i]
        $archlist[$i] = $t_archs[$arch]
    }

    $archs = $archlist -join ':' # TODO: modify gradle, split by ';'
    
    $outputOptions += "-PPROP_APP_ABI=$archs"

    # $cmake_toolchain_file = "$ndk_root\build\cmake\android.toolchain.cmake"
    # $outputOptions += '-G', 'Ninja', '-DANDROID_STL=c++_shared', "-DCMAKE_MAKE_PROGRAM=$ninja_prog", "-DCMAKE_TOOLCHAIN_FILE=$cmake_toolchain_file", "-DANDROID_ABI=$arch"
    # $outputOptions += '-DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH'
    # $outputOptions += '-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=BOTH'
    # $outputOptions += '-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=BOTH'
    # # by default, we want find host program only when cross-compiling
    # $outputOptions += '-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER'
    
    return $outputOptions
}

function preprocess_osx([string[]]$inputOptions) {
    $outputOptions = $inputOptions
    $arch = $options.a
    if ($arch -eq 'x64') {
        $arch = 'x86_64'
    }

    $outputOptions += '-GXcode', "-DCMAKE_OSX_ARCHITECTURES=$arch"
    return $outputOptions
}

# build ios famliy (ios,tvos,watchos)
function preprocess_ios([string[]]$inputOptions) {
    $outputOptions = $inputOptions
    $arch = $options.a
    if ($arch -eq 'x64') {
        $arch = 'x86_64'
    }
    $cmake_toolchain_file = Join-Path -Path $myRoot -ChildPath 'ios.cmake'
    $outputOptions += '-GXcode', "-DCMAKE_TOOLCHAIN_FILE=$cmake_toolchain_file", "-DARCHS=$arch"
    if ($BUILD_TARGET -eq 'tvos') {
        $outputOptions += '-DPLAT=tvOS'
    }
    elseif ($BUILD_TARGET -eq 'watchos') {
        $outputOptions += '-DPLAT=watchOS'
    }
    return $outputOptions
}

function validHostAndToolchain() {
    $appleTable = @{
        'host' = @{'macos' = $True};
        'toolchain' = @{'clang' = $True; };
    };
    $validTable = @{
        'win32' = @{
            'host' = @{'windows' = $True};
            'toolchain' = @{'msvc' = $True; 'clang' = $True; 'mingw-gcc' = $True};
        };
        'winuwp' = @{
            'host' = @{'windows' = $True};
            'toolchain' = @{'msvc' = $True; };
        };
        'linux' = @{
            'host' = @{'linux' = $True};
            'toolchain' = @{'gcc' = $True; };
        };
        'android' = @{
            'host' = @{'windows' = $True; 'linux' = $True; 'macos' = $True};
            'toolchain' = @{'ndk' = $True; };
        };
        'osx' = $appleTable;
        'ios' = $appleTable;
        'tvos' = $appleTable;
        'watchos' = $appleTable;
    }
    $validInfo = $validTable[$BUILD_TARGET]
    $validOS = $validInfo.host[$HOST_OS_NAME]
    if (!$validOS) {
        throw "Can't build target $BUILD_TARGET on $HOST_OS_NAME"
    }
    $validToolchain = $validInfo.toolchain[$TOOLCHAIN_NAME]
    if(!$validToolchain) {
        throw "Can't build target $BUILD_TARGET with toolchain $TOOLCHAIN_NAME"
    }
}

$proprocessTable = @{ 
    'win32' = ${function:preprocess_win};
    'winuwp' = ${function:preprocess_win};
    'linux' = ${function:preprocess_linux}; 
    'android' = ${function:preprocess_andorid};
    'osx' = ${function:preprocess_osx};
    'ios' = ${function:preprocess_ios};
    'tvos' = ${function:preprocess_ios};
    'watchos' = ${function:preprocess_ios};
}

validHostAndToolchain

# setup tools: cmake, ninja, ndk if required for target build
setup_cmake

if (($BUILD_TARGET -eq 'android' -or $BUILD_TARGET -eq 'win32') -and ($TOOLCHAIN_NAME -ne 'msvc')) {
    $ninja_prog = setup_ninja
}

if ($BUILD_TARGET -eq 'android') {
    $ndk_root = setup_ndk
}

# enter building steps
Write-Host "Building target $BUILD_TARGET on $HOST_OS_NAME with toolchain $TOOLCHAIN ..."

# step1. preprocess cross make options
$CONFIG_ALL_OPTIONS = $(& $proprocessTable[$BUILD_TARGET] -inputOptions $CONFIG_DEFAULT_OPTIONS)

# step2. apply additional cross make options
if ($options.cm.Count -gt 0) {
    Write-Host ("Apply additional cross make options: $($options.cm), Count={0}" -f $options.cm.Count)
    $CONFIG_ALL_OPTIONS += $options.cm
}
if ("$($options.cm)".IndexOf('-B') -eq -1) {
    $BUILD_DIR = "build_$($options.a)"
} else {
    foreach($opt in $options.cm) {
        if ($opt.StartsWith('-B')) {
            $BUILD_DIR = $opt.Substring(2).Trim()
            break
        }
    }
}
Write-Host ("CONFIG_ALL_OPTIONS=$CONFIG_ALL_OPTIONS, Count={0}" -f $CONFIG_ALL_OPTIONS.Count)

if ($BUILD_TARGET -ne 'android') {
    # step3. configure
    cmake -B $BUILD_DIR $CONFIG_ALL_OPTIONS

    # step4. build
    # apply additional build options
    $BUILD_ALL_OPTIONS = if ("$($options.cb)".IndexOf('--config') -eq -1) {@('--config','Release')} else {@()}
    $BUILD_ALL_OPTIONS += $options.cb

    Write-Host ("BUILD_ALL_OPTIONS=$BUILD_ALL_OPTIONS, Count={0}" -f $BUILD_ALL_OPTIONS.Count)
    cmake --build $BUILD_DIR $BUILD_ALL_OPTIONS
} else {
    $CONFIG_ALL_OPTIONS += '--parallel', '--info'
    ./gradlew assembleRelease '-PPROP_BUILD_TYPE=cmake' $CONFIG_ALL_OPTIONS
}

Set-Location $stored_cwd
