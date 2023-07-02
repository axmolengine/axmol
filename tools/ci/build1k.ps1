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
# The build1k.ps1, will be core script of project https://github.com/axmolengine/build1k
# options
#  -p: build target platform: win32,winuwp,linux,android,osx,ios,tvos,watchos
#      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first, 
#      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
#  -a: build arch: x86,x64,armv7,arm64
#  -d: the build workspace, i.e project root which contains root CMakeLists.txt, empty use script run working directory aka cwd
#  -cc: The C/C++ compiler toolchain: clang, msvc, gcc, mingw-gcc or empty use default installed on current OS
#       msvc: msvc-120, msvc-141
#       ndk: ndk-r16b, ndk-r16b+
#  -xt: cross build tool, default: cmake, for android can be gradle
#  -xc: cross build tool configure options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
#  -xb: cross build tool build options: i.e. -xb '--config','Release'
#  -prefix: the install location for missing tools in system, default is "$HOME/build1k"
#  -winsdk: specific windows sdk version, i.e. -winsdk '10.0.19041.0', leave empty, cmake will auto choose latest avaiable
# support matrix
#   | OS        |   Build targets     |  C/C++ compiler toolchain | Cross Build tool |
#   +----------+----------------------+---------------------------+------------------|
#   | Windows  |  win32,winuwp        | msvc,clang,mingw-gcc      | cmake            |
#   | Linux    | linux,android        | ndk                       | cmake,gradle     |    
#   | macOS    | osx,ios,tvos,watchos | xcode                     | cmake            |
#

# mode:
# x.y.z+         : >=
# x.y.z          : ==
# *              : any
# x.y.z~x2.y2.z2 : range
$manifest = @{
    msvc         = '143+';
    ndk          = 'r23c+';
    xcode        = '13.0.0~14.2.0'; # range
    clang        = '15.0.0+';
    gcc          = '9.0.0+';
    cmake        = '3.26.4+';
    nuget        = '*'; # any
    ninja        = '1.11.1+';
    jdk          = '11.0.19+';
    cmdlinetools = '7.0+'; # android cmdlinetools
}

# refer to: https://developer.android.com/studio#command-line-tools-only
$cmdlinetools_rev = '9477386'

function b1k_print($msg) {
    Write-Host "build1k: $msg"
}

$options = @{p = $null; a = 'x64'; d = $null; cc = $null; xt = 'cmake'; prefix = $null; xc = @(); xb = @(); winsdk = $null; dll = $false }

$optName = $null
foreach ($arg in $args) {
    if (!$optName) {
        if ($arg.StartsWith('-')) { 
            $optName = $arg.SubString(1)
        }
    }
    else {
        if ($options.Contains($optName)) {
            $options[$optName] = $arg
        }
        else {
            b1k_print("Warning: ignore unrecognized option: $optName")
        }
        $optName = $null
    }
}

$pwsh_ver = $PSVersionTable.PSVersion.ToString()

b1k_print "PowerShell $pwsh_ver"
b1k_print $(Out-String -InputObject $options)

$myRoot = $PSScriptRoot

$HOST_WIN = 0 # targets: win,uwp,android
$HOST_LINUX = 1 # targets: linux,android 
$HOST_MAC = 2 # targets: android,ios,osx(macos),tvos,watchos

# 0: windows, 1: linux, 2: macos
if ($IsWindows -or ("$env:OS" -eq 'Windows_NT')) {
    $HOST_OS = $HOST_WIN
    $envPathSep = ';'
}
else {
    $envPathSep = ':'
    if ($IsLinux) {
        $HOST_OS = $HOST_LINUX
    }
    elseif ($IsMacOS) {
        $HOST_OS = $HOST_MAC
    }
    else {
        throw "Unsupported host OS to run build1k.ps1"
    }
}

$IsWin = $HOST_OS -eq $HOST_WIN

$exeSuffix = if ($HOST_OS -eq 0) { '.exe' } else { '' }

$CONFIG_DEFAULT_OPTIONS = @()
$HOST_OS_NAME = $('windows', 'linux', 'macos').Get($HOST_OS)

# determine build target os
$BUILD_TARGET = $options.p
if (!$BUILD_TARGET) {
    # choose host target if not specified by command line automatically
    $BUILD_TARGET = $('win32', 'linux', 'osx').Get($HOST_OS)
}

# determine toolchain
$TOOLCHAIN = $options.cc
$toolchains = @{ 
    'win32'   = 'msvc';
    'winuwp'  = 'msvc';
    'linux'   = 'gcc'; 
    'android' = 'ndk';
    'osx'     = 'xcode';
    'ios'     = 'xcode';
    'tvos'    = 'xcode';
    'watchos' = 'xcode';
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
if ($options.d) {
    Set-Location $options.d
}

$tools_dir = if ($options.prefix) { $options.prefix } else { Join-Path -Path $HOME -ChildPath 'build1k' }
if (!(Test-Path "$tools_dir" -PathType Container)) {
    mkdir $tools_dir
}

b1k_print "proj_dir=$((Get-Location).Path), tools_dir=$tools_dir"

function find_prog($name, $path = $null, $cmd = $null, $param = $null, $silent = $false) {
    if ($path) {
        $storedPATH = $env:PATH
        $env:PATH = $path
    }
    if (!$cmd) { $cmd = $name }

    # try get match expr and preferred ver
    $checkVerCond = $null
    $requiredMin = ''
    $preferredVer = ''
    if ($manifest.Contains($name)) {
        $requiredVer = $manifest[$name]
        $preferredVer = $null
        if ($requiredVer.EndsWith('+')) {
            $preferredVer = $requiredVer.TrimEnd('+')
            $checkVerCond = '$foundVer -ge $preferredVer'
        }
        elseif ($requiredVer -eq '*') {
            $checkVerCond = '$True'
            $preferredVer = 'latest'
        }
        else {
            $verArr = $requiredVer.Split('~')
            $isRange = $verArr.Count -gt 1
            $preferredVer = $verArr[$isRange]
            if ($isRange -gt 1) {
                $requiredMin = $verArr[0]
                $checkVerCond = '$foundVer -ge $requiredMin -and $foundVer -le $preferredVer'
            }
            else {
                $checkVerCond = '$foundVer -eq $preferredVer'
            }
        }
        if (!$checkVerCond) {
            throw "Invalid tool $name=$requiredVer in manifest"
        }
    }

    # find command
    $cmd_info = (Get-Command $cmd -ErrorAction SilentlyContinue)
    $found_rets = $null # prog_path,prog_version
    if ($cmd_info) {
        $prog_path = $cmd_info.Source
        $verStr = if (!$param) { $(. $cmd '--version' 2>$null) | Select-Object -First 1 } else { $(. $cmd '--version' $param 2>$null) | Select-Object -First 1 }
        if (!$verStr -or ($verStr.IndexOf('--version') -ne -1)) {
            $verInfo = $cmd_info.Version
            $verStr = "$($verInfo.Major).$($verInfo.Minor).$($verInfo.Revision)"
        }

        # full pattern: '(\d+\.)+(\*|\d+)(\-[a-z]+[0-9]*)?' can match x.y.z-rc3, but not require for us
        $matchInfo = [Regex]::Match($verStr, '(\d+\.)+(-)?(\*|\d+)')
        $foundVer = $matchInfo.Value
        [void]$requiredMin
        if ($checkVerCond) {
            $matched = Invoke-Expression $checkVerCond
            if ($matched) {
                if (!$silent) { b1k_print "Found suitable installed $name, version: $foundVer" }
                $found_rets = $prog_path, $foundVer
            }
            else {
                if (!$silent) { b1k_print "The installed $name=$foundVer not match $requiredVer" }
                $found_rets = $null, $preferredVer
            }
        }
        else {
            if (!$silent) { b1k_print "Found installed $name, version: $foundVer" }
            $found_rets = $prog_path, $foundVer
        }
    }
    else {
        if ($preferredVer) {
            if (!$silent) { b1k_print "Not found $name, needs install: $preferredVer" }
            $found_rets = $null, $preferredVer
        }
        else {
            throw "Not found $name, and it's not in manifest"
        }
    }

    if ($path) {
        $env:PATH = $storedPATH
    }
    return $found_rets
}

function exec_prog($prog, $params) {
    # & $prog_name $params
    for ($i = 0; $i -lt $params.Count; $i++) {
        $param = "'"
        $param += $params[$i]
        $param += "'"
        $params[$i] = $param
    }
    $strParams = "$params"
    return Invoke-Expression -Command "$prog $strParams"
}

function download_file($url, $out) {
    b1k_print "Downloading $url to $out ..."
    if ($pwsh_ver -ge '7.0') {
        curl -L $url -o $out
    }
    else {
        Invoke-WebRequest -Uri $url -OutFile $out
    }
}

# setup cmake
function setup_cmake() {
    $cmake_prog, $cmake_ver = find_prog -name 'cmake'
    if ($cmake_prog) {
        b1k_print "Using installed cmake $cmake_prog, version: $cmake_ver"
    }
    else {
        b1k_print "Installing cmake $cmake_ver ..."

        $cmake_suffix = @(".zip", ".sh", ".tar.gz").Get($HOST_OS)
        if ($HOST_OS -ne $HOST_MAC) {
            $cmake_dir = "cmake-$cmake_ver-$HOST_OS_NAME-x86_64"
        }
        else {
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
            elseif ($HOST_OS -eq $HOST_LINUX) {
                chmod 'u+x' "$cmake_pkg_path"
                mkdir $cmake_root
                & "$cmake_pkg_path" '--skip-license' '--exclude-subdir' "--prefix=$cmake_root"
            }
            elseif ($HOST_OS -eq $HOST_MAC) {
                tar xvf "$cmake_root.tar.gz" -C "$tools_dir/"
            }
        }

        $cmake_bin = $null
        if ($HOST_OS -ne $HOST_MAC) {
            $cmake_bin = Join-Path -Path $cmake_root -ChildPath 'bin'
        }
        else {
            $cmake_bin = "$cmake_root/CMake.app/Contents/bin"
        }
        
        $cmake_prog, $_ = find_prog -name 'cmake' -path $cmake_bin -silent $True
        if ($cmake_prog) {
            if (($null -ne $cmake_bin) -and ($env:PATH.IndexOf($cmake_bin) -eq -1)) {
                $env:PATH = "$cmake_bin$envPathSep$env:PATH"
            }
            b1k_print "Install cmake $cmake_ver succeed"
        }
        else {
            throw "Install cmake $cmake_ver fail"
        }
    }
}

# setup nuget
function setup_nuget() {
    $nuget_prog, $nuget_ver = find_prog -name 'nuget'
    if ($nuget_prog) {
        b1k_print "Using installed nuget: $nuget_prog"
        return $nuget_prog
    }

    $nuget_prog = Join-Path -Path $tools_dir -ChildPath 'nuget'
    if (!(Test-Path -Path $nuget_prog -PathType Container)) {
        mkdir $nuget_prog
    }

    $nuget_prog = Join-Path -Path $nuget_prog -ChildPath 'nuget.exe'

    if (Test-Path -Path $nuget_prog -PathType Leaf) {
        b1k_print "Using installed nuget: $nuget_prog"
        return $nuget_prog
    }
    download_file "https://dist.nuget.org/win-x86-commandline/$nuget_ver/nuget.exe" $nuget_prog

    if (Test-Path -Path $nuget_prog -PathType Leaf) {
        b1k_print "The nuget was successfully installed to: $nuget_prog"
        return $nuget_prog
    }
    else {
        throw "Install nuget fail"
    }
}

function setup_jdk() {
    $javac_prog, $jdk_ver = find_prog -name 'jdk' -cmd 'javac'
    if ($javac_prog) {
        b1k_print "Using installed jdk: $javac_prog, version: $jdk_ver"
        return $javac_prog
    }

    b1k_print "Installing jdk $jdk_ver ..."
    $suffix = $('windows-x64.zip', 'linux-x64.tar.gz', 'macOS-x64.tar.gz').Get($HOST_OS)
    $java_home = Join-Path -Path $tools_dir -ChildPath jdk-$jdk_ver
    if (!(Test-Path $java_home -PathType Container)) {
        # refer to https://learn.microsoft.com/en-us/java/openjdk/download
        if (!(Test-Path "$tools_dir/microsoft-jdk-$jdk_ver-$suffix" -PathType Leaf)) {
            download_file "https://aka.ms/download-jdk/microsoft-jdk-$jdk_ver-$suffix" "$tools_dir/microsoft-jdk-$jdk_ver-$suffix"
        }

        # uncompress
        if ($IsWin) {
            Expand-Archive -Path "$tools_dir/microsoft-jdk-$jdk_ver-$suffix" -DestinationPath "$tools_dir/"
        }
        else {
            tar xvf "$tools_dir/microsoft-jdk-$jdk_ver-$suffix" -C "$tools_dir/"
        }

        # move to plain folder name
        $folderName = (Get-ChildItem -Path $tools_dir -Filter "jdk-$jdk_ver+*").Name
        if ($folderName) {
            Move-Item "$tools_dir/$folderName" $java_home
        }
    }
    $env:JAVA_HOME = $java_home
    $env:CLASSPATH = ".;$java_home\lib\dt.jar;$java_home\lib\tools.jar"
    $jdk_bin = Join-Path -Path $java_home -ChildPath 'bin'
    if ($env:PATH.IndexOf($jdk_bin) -eq -1) {
        $env:PATH = "$jdk_bin$envPathSep$env:PATH"
    }
    $javac_prog = (find_prog -name 'javac' -path $jdk_bin)
    if (!$javac_prog) {
        throw "Install jdk $jdk_ver fail"
    }

    return $javac_prog
}

function setup_ninja() {
    $ninja_prog, $ninja_ver = find_prog -name 'ninja'
    if ($ninja_prog) {
        b1k_print "Using installed ninja: $ninja_prog, version: $(ninja --version)"
        return $ninja_prog
    }

    b1k_print "Installing ninja $ninja_ver ..."

    $suffix = $('win', 'linux', 'mac').Get($HOST_OS)
    $ninja_bin = (Resolve-Path "$tools_dir/ninja-$suffix" -ErrorAction SilentlyContinue).Path
    if (!$ninja_bin) {
        download_file "https://github.com/ninja-build/ninja/releases/download/v$ninja_ver/ninja-$suffix.zip" "$tools_dir/ninja-$suffix.zip"
        Expand-Archive -Path $tools_dir/ninja-$suffix.zip -DestinationPath "$tools_dir/ninja-$suffix/"
        $ninja_bin = (Resolve-Path "$tools_dir/ninja-$suffix" -ErrorAction SilentlyContinue).Path
    }
    if ($env:PATH.IndexOf($ninja_bin) -eq -1) {
        $env:PATH = "$ninja_bin$envPathSep$env:PATH"
    }
    $ninja_prog = (Join-Path -Path $ninja_bin -ChildPath ninja$exeSuffix)
    return $ninja_prog
}

function setup_android_sdk() {
    # setup ndk
    $ndk_ver = $TOOLCHAIN_VER
    if (!$ndk_ver) {
        $ndk_ver = $manifest['ndk']
    }

    $IsGraterThan = if ($ndk_ver.EndsWith('+')) { '+' } else { $null }
    if ($IsGraterThan) {
        $ndk_ver = $ndk_ver.Substring(0, $ndk_ver.Length - 1)
    }

    $sdk_root_envs = @('ANDROID_HOME', 'ANDROID_SDK_ROOT')

    $ndk_minor_base = [int][char]'a'
    
    # looking up require ndk installed in exists sdk roots
    $sdk_root = $null
    foreach ($sdk_root_env in $sdk_root_envs) {
        $sdk_dir = [Environment]::GetEnvironmentVariable($sdk_root_env)
        b1k_print "Looking require $ndk_ver$IsGraterThan in env:$sdk_root_env=$sdk_dir"
        if ("$sdk_dir" -ne '') {
            $sdk_root = $sdk_dir
            $ndk_root = $null

            $ndk_major = ($ndk_ver -replace '[^0-9]', '')
            $ndk_minor_off = "$ndk_major".Length + 1
            $ndk_minor = if ($ndk_minor_off -lt $ndk_ver.Length) { "$([int][char]$ndk_ver.Substring($ndk_minor_off) - $ndk_minor_base)" } else { '0' }
            $ndk_rev_base = "$ndk_major.$ndk_minor"

            # find ndk in sdk
            $ndks = [ordered]@{}
            $ndk_rev_max = '0.0'
            foreach ($item in $(Get-ChildItem -Path "$env:ANDROID_HOME/ndk")) {
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
            }
            else {
                $ndk_root = $ndks[$ndk_rev_base]
            }

            if ($null -ne $ndk_root) {
                b1k_print "Found $ndk_root in $sdk_root ..."
                break
            }
        }
    }

    if (!(Test-Path "$ndk_root" -PathType Container)) {
        $sdkmanager_prog, $sdkmanager_ver = $null, $null
        if (Test-Path "$sdk_root" -PathType Container) {
            $sdkmanager_prog, $sdkmanager_ver = (find_prog -name 'cmdlinetools' -cmd 'sdkmanager' -path "$sdk_root/cmdline-tools/latest/bin" -param "--sdk_root=$sdk_root")
        }
        else {
            $sdk_root = Join-Path -Path $tools_dir -ChildPath 'adt/sdk'
            if (!(Test-Path -Path $sdk_root -PathType Container)) {
                mkdir $sdk_root
            }
        }

        if (!$sdkmanager_prog) {
            $sdkmanager_prog, $sdkmanager_ver = (find_prog -name 'cmdlinetools' -cmd 'sdkmanager' -path "$tools_dir/cmdline-tools/bin" -param "--sdk_root=$sdk_root")
            $suffix = $('win', 'linux', 'mac').Get($HOST_OS)
            if (!$sdkmanager_prog) {
                b1k_print "Installing cmdlinetools version: $sdkmanager_ver ..."

                $cmdlinetools_pkg_name = "commandlinetools-$suffix-$($cmdlinetools_rev)_latest.zip"
                $cmdlinetools_pkg_path = Join-Path -Path $tools_dir -ChildPath $cmdlinetools_pkg_name
                $cmdlinetools_url = "https://dl.google.com/android/repository/$cmdlinetools_pkg_name"
                download_file $cmdlinetools_url $cmdlinetools_pkg_path
                Expand-Archive -Path $cmdlinetools_pkg_path -DestinationPath "$tools_dir/"
                $sdkmanager_prog, $_ = (find_prog -name 'cmdlinetools' -cmd 'sdkmanager' -path "$tools_dir/cmdline-tools/bin" -param "--sdk_root=$sdk_root" -silent $True)
                if (!$sdkmanager_prog) {
                    throw "Install cmdlinetools version: $sdkmanager_ver fail"
                }
            }
        }

        $matchInfos = (exec_prog -prog $sdkmanager_prog -params "--sdk_root=$sdk_root", '--list' | Select-String 'ndk;')
        if ($null -ne $matchInfos -and $matchInfos.Count -gt 0) {
            b1k_print "Not found suitable android ndk, installing ..."

            $ndks = @{}
            foreach ($matchInfo in $matchInfos) {
                $fullVer = $matchInfo.Line.Trim().Split(' ')[0] # "ndk;23.2.8568313"
                $verNums = $fullVer.Split(';')[1].Split('.')
                $ndkVer = 'r'
                $ndkVer += $verNums[0]

                $ndk_minor = [int]$verNums[1]
                if ($ndk_minor -gt 0) {
                    $ndkVer += [char]($ndk_minor_base + $ndk_minor)
                }
                if (!$ndks.Contains($ndkVer)) {
                    $ndks.Add($ndkVer, $fullVer)
                }
            }

            $ndkFullVer = $ndks[$ndk_ver]

            exec_prog -prog $sdkmanager_prog -params '--verbose', "--sdk_root=$sdk_root", 'platform-tools', 'cmdline-tools;latest', 'platforms;android-33', 'build-tools;30.0.3', 'cmake;3.22.1', $ndkFullVer | Out-Host

            $fullVer = $ndkFullVer.Split(';')[1]
            $ndk_root = (Resolve-Path -Path "$sdk_root/ndk/$fullVer").Path
        }
    }

    return $sdk_root, $ndk_root
}

function setup_clang(){
    $clang_prog, $clang_ver = find_prog -name 'clang'
    if ($clang_prog) {
        b1k_print "Using installed clang: $clang_prog, version: $clang_ver"
    }
    else {
        throw 'required clang $clang_ver not installed, please install it from: https://github.com/llvm/llvm-project/releases'
    }
}

# preprocess methods: 
#   <param>-inputOptions</param> [CMAKE_OPTIONS]
function preprocess_win([string[]]$inputOptions) {
    $outputOptions = $inputOptions

    if ($options.winsdk) {
        $outputOptions += "-DCMAKE_SYSTEM_VERSION=$($options.winsdk)"
    }

    if ($TOOLCHAIN_NAME -eq 'msvc') {
        # Generate vs2019 on github ci
        # Determine arch name
        $arch = if ($options.a -eq 'x86') { 'Win32' } else { $options.a }

        $VSWHERE_EXE = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
        $eap = $ErrorActionPreference
        $ErrorActionPreference = 'SilentlyContinue'
        $VS2019_OR_LATER_VESION = $null
        $VS2019_OR_LATER_VESION = (& $VSWHERE_EXE -version '16.0' -property installationVersion)
        $ErrorActionPreference = $eap

        # arch
        if ($VS2019_OR_LATER_VESION) {
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
            if (!$gen) {
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
    elseif ($TOOLCHAIN_NAME -eq 'clang') {
        $outputOptions += '-G', 'Ninja Multi-Config', '-DCMAKE_C_COMPILER=clang', '-DCMAKE_CXX_COMPILER=clang++'
    }
    else {
        # Generate mingw
        $outputOptions += '-G', 'Ninja Multi-Config'
    }
    return $outputOptions
}

function preprocess_linux([string[]]$inputOptions) {
    $outputOptions = $inputOptions
    return $outputOptions
}

$ninja_prog = $null
function preprocess_andorid([string[]]$inputOptions) {
    $outputOptions = $inputOptions

    $t_archs = @{arm64 = 'arm64-v8a'; armv7 = 'armeabi-v7a'; x64 = 'x86_64'; x86 = 'x86'; }

    if ($options.xt -eq 'gradle') {
        if ($options.a.GetType() -eq [object[]]) {
            $archlist = [string[]]$options.a
        }
        else {
            $archlist = $options.a.Split(';')
        }
        for ($i = 0; $i -lt $archlist.Count; ++$i) {
            $arch = $archlist[$i]
            $archlist[$i] = $t_archs[$arch]
        }
    
        $archs = $archlist -join ':' # TODO: modify gradle, split by ';'

        $outputOptions += "-PPROP_APP_ABI=$archs"
        $outputOptions += '--parallel', '--info'
    }
    else {
        $cmake_toolchain_file = "$ndk_root\build\cmake\android.toolchain.cmake"
        $arch = $t_archs[$options.a]
        $outputOptions += '-G', 'Ninja', '-DANDROID_STL=c++_shared', "-DCMAKE_MAKE_PROGRAM=$ninja_prog", "-DCMAKE_TOOLCHAIN_FILE=$cmake_toolchain_file", "-DANDROID_ABI=$arch"
        $outputOptions += '-DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH'
        $outputOptions += '-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=BOTH'
        $outputOptions += '-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=BOTH'
        # by default, we want find host program only when cross-compiling
        $outputOptions += '-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER'
    }
    
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
        'host'      = @{'macos' = $True };
        'toolchain' = @{'xcode' = $True; };
    };
    $validTable = @{
        'win32'   = @{
            'host'      = @{'windows' = $True };
            'toolchain' = @{'msvc' = $True; 'clang' = $True; 'mingw-gcc' = $True };
        };
        'winuwp'  = @{
            'host'      = @{'windows' = $True };
            'toolchain' = @{'msvc' = $True; };
        };
        'linux'   = @{
            'host'      = @{'linux' = $True };
            'toolchain' = @{'gcc' = $True; };
        };
        'android' = @{
            'host'      = @{'windows' = $True; 'linux' = $True; 'macos' = $True };
            'toolchain' = @{'ndk' = $True; };
        };
        'osx'     = $appleTable;
        'ios'     = $appleTable;
        'tvos'    = $appleTable;
        'watchos' = $appleTable;
    }
    $validInfo = $validTable[$BUILD_TARGET]
    $validOS = $validInfo.host[$HOST_OS_NAME]
    if (!$validOS) {
        throw "Can't build target $BUILD_TARGET on $HOST_OS_NAME"
    }
    $validToolchain = $validInfo.toolchain[$TOOLCHAIN_NAME]
    if (!$validToolchain) {
        throw "Can't build target $BUILD_TARGET with toolchain $TOOLCHAIN_NAME"
    }
}

$proprocessTable = @{ 
    'win32'   = ${function:preprocess_win};
    'winuwp'  = ${function:preprocess_win};
    'linux'   = ${function:preprocess_linux}; 
    'android' = ${function:preprocess_andorid};
    'osx'     = ${function:preprocess_osx};
    'ios'     = ${function:preprocess_ios};
    'tvos'    = ${function:preprocess_ios};
    'watchos' = ${function:preprocess_ios};
}

validHostAndToolchain

########## setup build tools if not installed #######

$cmake_prog = setup_cmake

if ($BUILD_TARGET -eq 'win32') {
    $nuget_prog = setup_nuget
    if ($TOOLCHAIN_NAME -ne 'msvc') {
        $ninja_prog = setup_ninja
        $null = setup_clang
    }
}
elseif ($BUILD_TARGET -eq 'android') {
    $sdk_root, $ndk_root = setup_android_sdk
    $env:ANDROID_HOME = $sdk_root
    $env:ANDROID_NDK = $ndk_root
    # we assume 'gradle' to build apk, so require setup jdk11+
    # otherwise, build for android libs, needs setup ninja
    if ($options.xt -eq 'gradle') {
        $null = setup_jdk
    }
    else {
        $ninja_prog = setup_ninja
    }
}

# enter building steps
b1k_print "Building target $BUILD_TARGET on $HOST_OS_NAME with toolchain $TOOLCHAIN ..."

# step1. preprocess cross make options
$CONFIG_ALL_OPTIONS = [array]$(& $proprocessTable[$BUILD_TARGET] -inputOptions $CONFIG_DEFAULT_OPTIONS)

if (!$CONFIG_ALL_OPTIONS) {
    $CONFIG_ALL_OPTIONS = @()
}

# step2. apply additional cross make options
$xopts = $options.xc
if ($xopts.Count -gt 0) {
    b1k_print ("Apply additional cross make options: $($xopts), Count={0}" -f $xopts.Count)
    $CONFIG_ALL_OPTIONS += $xopts
}
if ("$($xopts)".IndexOf('-B') -eq -1) {
    $BUILD_DIR = "build_$($options.a)"
}
else {
    foreach ($opt in $xopts) {
        if ($opt.StartsWith('-B')) {
            $BUILD_DIR = $opt.Substring(2).Trim()
            break
        }
    }
}
b1k_print ("CONFIG_ALL_OPTIONS=$CONFIG_ALL_OPTIONS, Count={0}" -f $CONFIG_ALL_OPTIONS.Count)

if (($BUILD_TARGET -eq 'android') -and ($options.xt -eq 'gradle')) {
    ./gradlew assembleRelease $CONFIG_ALL_OPTIONS
} 
else {
    # step3. configure
    cmake -B $BUILD_DIR $CONFIG_ALL_OPTIONS

    # step4. build
    # apply additional build options
    $xb_opts = $options.xb
    $BUILD_ALL_OPTIONS = if ("$($xb_opts)".IndexOf('--config') -eq -1) { @('--config', 'Release') } else { @() }
    if ($xb_opts) {
        $BUILD_ALL_OPTIONS += $xb_opts
    }

    $BUILD_ALL_OPTIONS += "--parallel"
    if ($BUILD_TARGET -eq 'linux') {
        $BUILD_ALL_OPTIONS += "$(nproc)"
    }
    if ($TOOLCHAIN_NAME -eq 'xcode') {
        $BUILD_ALL_OPTIONS += '--', '-quiet'
    }
    b1k_print ("BUILD_ALL_OPTIONS=$BUILD_ALL_OPTIONS, Count={0}" -f $BUILD_ALL_OPTIONS.Count)

    cmake --build $BUILD_DIR $BUILD_ALL_OPTIONS
}

Set-Location $stored_cwd
