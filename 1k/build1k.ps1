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
#  -p: build target platform: win32,winuwp,linux,android,osx,ios,tvos,watchos,wasm
#      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first, 
#      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
#  -a: build arch: x86,x64,armv7,arm64
#  -d: the build workspace, i.e project root which contains root CMakeLists.txt, empty use script run working directory aka cwd
#  -cc: The C/C++ compiler toolchain: clang, msvc, gcc, mingw-gcc or empty use default installed on current OS
#       msvc: msvc-120, msvc-141
#       ndk: ndk-r16b, ndk-r16b+
#  -xt: cross build tool, default: cmake, for android can be gradlew, can be path of cross build tool program
#  -xc: cross build tool configure options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
#  -xb: cross build tool build options: i.e. -xb '--config','Release'
#  -prefix: the install location for missing tools in system, default is "$HOME/build1k"
#  -sdk: specific windows sdk version, i.e. -sdk '10.0.19041.0', leave empty, cmake will auto choose latest avaiable
#  -setupOnly: this param present, only execute step: setup 
#  -configOnly: if this param present, will skip build step
# support matrix
#   | OS        |   Build targets     |  C/C++ compiler toolchain | Cross Build tool |
#   +----------+----------------------+---------------------------+------------------|
#   | Windows  |  win32,winuwp        | msvc,clang,mingw-gcc      | cmake            |
#   | Linux    | linux,android        | ndk                       | cmake,gradle     |    
#   | macOS    | osx,ios,tvos,watchos | xcode                     | cmake            |
#
param(
    [switch]$configOnly,
    [switch]$setupOnly
)

$myRoot = $PSScriptRoot

# ----------------- utils functions -----------------

$HOST_WIN = 0 # targets: win,uwp,android
$HOST_LINUX = 1 # targets: linux,android 
$HOST_MAC = 2 # targets: android,ios,osx(macos),tvos,watchos

# 0: windows, 1: linux, 2: macos
$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')
if ($IsWin) {
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

$exeSuffix = if ($HOST_OS -eq 0) { '.exe' } else { '' }

class build1k {
    [void] println($msg) {
        Write-Host "build1k: $msg"
    }

    [void] print($msg) {
        Write-Host "build1k: $msg" -NoNewline
    }

    [System.Boolean] isfile([string]$path) {
        return Test-Path $path -PathType Leaf
    }

    [System.Boolean] isdir([string]$path) {
        return Test-Path $path -PathType Container
    }

    [void] mkdirs([string]$path) {
        if (!(Test-Path $path -PathType Container)) {
            New-Item $path -ItemType Directory 1>$null
        }
    }

    [void] rmdirs([string]$path) {
        if ($this.isdir($path)) {
            $this.println("Deleting $path ...")
            Remove-Item $path -Recurse -Force
        }
    }

    [void] del([string]$path) {
        if ($this.isfile($path)) {
            $this.println("Deleting $path ...")
            Remove-Item $path -Force
        }
    }

    [void] mv([string]$path, [string]$dest) {
        if ($this.isdir($path) -and !$this.isdir($dest)) {
            Move-Item $path $dest
        }
    }

    [void] pause($msg) {
        if ($Global:IsWin) {
            $myProcess = [System.Diagnostics.Process]::GetCurrentProcess()
            $parentProcess = $myProcess.Parent
            if (!$parentProcess) {
                $myPID = $myProcess.Id
                $instance = Get-WmiObject Win32_Process -Filter "ProcessId = $myPID"
                $parentProcess = Get-Process -Id $instance.ParentProcessID
            }
            $parentProcessName = $parentProcess.ProcessName
            if ($parentProcessName -like "explorer") {
                $this.print("$msg, press any key to continue . . .")
                cmd /c pause 1>$null
            }
        }
        else {
            $this.println($msg)
        }
    }
}
$b1k = [build1k]::new()

# ---------------------- manifest --------------------
# mode:
# x.y.z+         : >=
# x.y.z          : ==
# *              : any
# x.y.z~x2.y2.z2 : range
$manifest = @{
    # C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Redist\MSVC\14.36.32532\vc_redist.x64.exe
    msvc         = '14.36.32532';
    ndk          = 'r23c';
    xcode        = '13.0.0~14.2.0'; # range
    # _EMIT_STL_ERROR(STL1000, "Unexpected compiler version, expected Clang 16.0.0 or newer.");
    clang        = '16.0.0+'; # clang-cl msvc14.37 require 16.0.0+
    gcc          = '9.0.0+';
    cmake        = '3.22.1+';
    ninja        = '1.11.1+';
    jdk          = '11.0.19+';
    cmdlinetools = '7.0+'; # android cmdlinetools
}

$channels = @{}

# refer to: https://developer.android.com/studio#command-line-tools-only
$cmdlinetools_rev = '9477386'

$options = @{
    p      = $null; 
    a      = 'x64'; 
    d      = $null; 
    cc     = $null; 
    xt     = 'cmake'; 
    prefix = $null; 
    xc     = @(); 
    xb     = @(); 
    sdk    = $null; 
    dll    = $false
}

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
            $b1k.println("Warning: ignore unrecognized option: $optName")
        }
        $optName = $null
    }
}

$is_wasm = $options.p -eq 'wasm'
if ($is_wasm) {
    $options.a = 'any' # forcing arch to 'any', only for identifying
}

$manifest_file = Join-Path $myRoot 'manifest.ps1'
if ($b1k.isfile($manifest_file)) {
    . $manifest_file
}

# translate xtool args
if ($options.xc.GetType() -eq [string]) {
    $options.xc = $options.xc.Split(' ')
}
if ($options.xb.GetType() -eq [string]) {
    $options.xb = $options.xb.Split(' ')
}

$pwsh_ver = $PSVersionTable.PSVersion.ToString()

$b1k.println("PowerShell $pwsh_ver")

if (!$setupOnly) {
    $b1k.println("$(Out-String -InputObject $options)")
}

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
    'wasm'    = 'emcc'; # wasm llvm-emcc
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

$prefix = if ($options.prefix) { $options.prefix } else { Join-Path $HOME 'build1k' }
if (!$b1k.isdir($prefix)) {
    $b1k.mkdirs($prefix)
}

$b1k.println("proj_dir=$((Get-Location).Path), prefix=$prefix")

function find_prog($name, $path = $null, $mode = 'ONLY', $cmd = $null, $params = @('--version'), $silent = $false) {
    if ($path) {
        $storedPATH = $env:PATH
        if ($mode -eq 'ONLY') {
            $env:PATH = $path
        }
        elseif ($mode -eq 'BOTH') {
            $env:PATH = "$path$envPathSep$env:PATH"
        }
    }
    else {
        $storedPATH = $null
    }
    if (!$cmd) { $cmd = $name }

    $params = [array]$params

    # try get match expr and preferred ver
    $checkVerCond = $null
    $requiredMin = ''
    $preferredVer = ''
    $requiredVer = $manifest[$name]
    if ($requiredVer) {
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

    # needs restore immidiately since further cmd invoke maybe require system bins
    if ($path) {
        $env:PATH = "$path$envPathSep$storedPATH"
    }

    $found_rets = $null # prog_path,prog_version
    if ($cmd_info) {
        $prog_path = $cmd_info.Source
        $verStr = $(. $cmd @params 2>$null) | Select-Object -First 1
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
                if (!$silent) { $b1k.println("Using ${name}: $prog_path, version: $foundVer") }
                $found_rets = $prog_path, $foundVer
            }
            else {
                # if (!$silent) { $b1k.println("The installed ${name}: $prog_path, version: $foundVer not match required: $requiredVer") }
                $found_rets = $null, $preferredVer
            }
        }
        else {
            if (!$silent) { $b1k.println("Using ${name}: $prog_path, version: $foundVer") }
            $found_rets = $prog_path, $foundVer
        }
    }
    else {
        if ($preferredVer) {
            # if (!$silent) { $b1k.println("Not found $name, needs install: $preferredVer") }
            $found_rets = $null, $preferredVer
        }
        else {
            throw "Not found $name, and it's not in manifest"
        }
    }

    if ($storedPATH) {
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
    if ($b1k.isfile($out)) { return }
    $b1k.println("Downloading $url to $out ...")
    if ($pwsh_ver -ge '7.0') {
        curl -L $url -o $out
    }
    else {
        Invoke-WebRequest -Uri $url -OutFile $out
    }
}

function download_and_expand($url, $out, $dest) {
    download_file $url $out
    if ($out.EndsWith('.zip')) {
        Expand-Archive -Path $out -DestinationPath $dest
    }
    elseif ($out.EndsWith('.tar.gz')) {
        if (!$dest.EndsWith('/')) {
            $b1k.mkdirs($dest)
        }
        tar xf "$out" -C $dest
    }
    elseif ($out.EndsWith('.sh')) {
        chmod 'u+x' "$out"
        $b1k.mkdirs($dest)
    }
}

# setup nuget, not add to path
function setup_nuget() {
    if (!$manifest['nuget']) { return $null }
    $nuget_bin = Join-Path $prefix 'nuget'
    $nuget_prog, $nuget_ver = find_prog -name 'nuget' -path $nuget_bin -mode 'BOTH'
    if ($nuget_prog) {
        return $nuget_prog
    }

    $b1k.rmdirs($nuget_bin)
    $b1k.mkdirs($nuget_bin)

    $nuget_prog = Join-Path $nuget_bin 'nuget.exe'
    download_file "https://dist.nuget.org/win-x86-commandline/$nuget_ver/nuget.exe" $nuget_prog

    if ($b1k.isfile($nuget_prog)) {
        $b1k.println("Using nuget: $nuget_prog, version: $nuget_ver")
        return $nuget_prog
    }

    throw "Install nuget fail"
}

# setup glslcc, not add to path
function setup_glslcc() {
    if (!$manifest['glslcc']) { return $null }
    $glslcc_bin = Join-Path $prefix 'glslcc'
    $glslcc_prog, $glslcc_ver = find_prog -name 'glslcc' -path $glslcc_bin -mode 'BOTH'
    if ($glslcc_prog) {
        return $glslcc_prog
    }

    $suffix = $('win64.zip', 'linux.tar.gz', 'osx.tar.gz').Get($HOST_OS)
    $b1k.rmdirs($glslcc_bin)
    $glslcc_pkg = "$prefix/glslcc-$suffix"
    $b1k.del($glslcc_pkg)

    download_and_expand "https://github.com/axmolengine/glslcc/releases/download/v$glslcc_ver/glslcc-$glslcc_ver-$suffix" "$glslcc_pkg" $glslcc_bin

    $glslcc_prog = (Join-Path $glslcc_bin "glslcc$exeSuffix")
    if ($b1k.isfile($glslcc_prog)) {
        $b1k.println("Using glslcc: $glslcc_prog, version: $glslcc_ver")
        return $glslcc_prog
    }

    throw "Install glslcc fail"
}

# setup cmake
function setup_cmake() {
    $cmake_prog, $cmake_ver = find_prog -name 'cmake'
    if ($cmake_prog) {
        return $cmake_prog
    }

    $cmake_root = $(Join-Path $prefix 'cmake')
    $cmake_bin = Join-Path $cmake_root 'bin'
    $cmake_prog, $cmake_ver = find_prog -name 'cmake' -path $cmake_bin -mode 'ONLY' -silent $true
    if (!$cmake_prog) {
        $b1k.rmdirs($cmake_root)

        $cmake_suffix = @(".zip", ".sh", ".tar.gz").Get($HOST_OS)
        $cmake_dev_hash = $channels['cmake']
        if ($cmake_dev_hash) {
            $cmake_ver = "$cmake_ver-$cmake_dev_hash"
        }
        
        if ($HOST_OS -ne $HOST_MAC) {
            $cmake_pkg_name = "cmake-$cmake_ver-$HOST_OS_NAME-x86_64"
        }
        else {
            $cmake_pkg_name = "cmake-$cmake_ver-$HOST_OS_NAME-universal"
        }

        $cmake_pkg_path = Join-Path $prefix "$cmake_pkg_name$cmake_suffix"

        if (!$cmake_dev_hash) {
            $cmake_url = "https://github.com/Kitware/CMake/releases/download/v$cmake_ver/$cmake_pkg_name$cmake_suffix"
        }
        else {
            $cmake_url = "https://cmake.org/files/dev/$cmake_pkg_name$cmake_suffix"
        }

        $cmake_dir = Join-Path $prefix $cmake_pkg_name
        if ($IsMacOS) {
            $cmake_app_contents = Join-Path $cmake_dir 'CMake.app/Contents'
        }
        if (!$b1k.isdir($cmake_dir)) {
            download_and_expand "$cmake_url" "$cmake_pkg_path" $prefix/
        }

        if ($b1k.isdir($cmake_dir)) { 
            $cmake_root0 = $cmake_dir
            if ($IsMacOS) {
                $cmake_app_contents = Join-Path $cmake_dir 'CMake.app/Contents'
                if ($b1k.isdir($cmake_app_contents)) {
                    $cmake_root0 = $cmake_app_contents
                }
                sudo xattr -r -d com.apple.quarantine "$cmake_root0/bin/cmake"
            }
            $b1k.mv($cmake_root0, $cmake_root)

            if ($b1k.isdir($cmake_dir)) {
                $b1k.rmdirs($cmake_dir)
            }
        }
        elseif ($IsLinux) {
            $b1k.mkdirs($cmake_root)
            & "$cmake_pkg_path" '--skip-license' '--exclude-subdir' "--prefix=$cmake_root"
        }

        $cmake_prog, $_ = find_prog -name 'cmake' -path $cmake_bin -silent $true
        if (!$cmake_prog) {
            throw "Install cmake $cmake_ver fail"
        }
    }

    if (($null -ne $cmake_bin) -and ($env:PATH.IndexOf($cmake_bin) -eq -1)) {
        $env:PATH = "$cmake_bin$envPathSep$env:PATH"
    }
    $b1k.println("Using cmake: $cmake_prog, version: $cmake_ver")
    return $cmake_prog
}

function setup_ninja() {
    if (!$manifest['ninja']) { return $null }
    $suffix = $('win', 'linux', 'mac').Get($HOST_OS)
    $ninja_bin = Join-Path $prefix 'ninja'
    $ninja_prog, $ninja_ver = find_prog -name 'ninja'
    if ($ninja_prog) {
        return $ninja_prog
    }

    $ninja_prog, $ninja_ver = find_prog -name 'ninja' -path $ninja_bin -silent $true
    if (!$ninja_prog) {
        $ninja_pkg = "$prefix/ninja-$suffix.zip"
        $b1k.rmdirs($ninja_bin)
        $b1k.del($ninja_pkg)
        
        download_and_expand "https://github.com/ninja-build/ninja/releases/download/v$ninja_ver/ninja-$suffix.zip" $ninja_pkg "$prefix/ninja/"
    }
    if ($env:PATH.IndexOf($ninja_bin) -eq -1) {
        $env:PATH = "$ninja_bin$envPathSep$env:PATH"
    }
    $ninja_prog = (Join-Path $ninja_bin "ninja$exeSuffix")

    $b1k.println("Using ninja: $ninja_prog, version: $ninja_ver")
    return $ninja_prog
}

function setup_nsis() {
    if (!$manifest['nsis']) { return $null }
    $nsis_bin = Join-Path $prefix "nsis"
    $nsis_prog, $nsis_ver = find_prog -name 'nsis' -cmd 'makensis' -params '/VERSION'
    if ($nsis_prog) {
        return $nsis_prog
    }

    $nsis_prog, $nsis_ver = find_prog -name 'nsis' -cmd 'makensis' -params '/VERSION' -path $nsis_bin -silent $true
    if (!$nsis_prog) {
        $b1k.rmdirs($nsis_bin)

        download_and_expand "https://nchc.dl.sourceforge.net/project/nsis/NSIS%203/$nsis_ver/nsis-$nsis_ver.zip" "$prefix/nsis-$nsis_ver.zip" "$prefix"
        $nsis_dir = "$nsis_bin-$nsis_ver"
        if ($b1k.isdir($nsis_dir)) {
            $b1k.mv($nsis_dir, $nsis_bin)
        }
    }

    if ($env:PATH.IndexOf($nsis_bin) -eq -1) {
        $env:PATH = "$nsis_bin$envPathSep$env:PATH"
    }
    $nsis_prog = (Join-Path $nsis_bin "makensis$exeSuffix")

    $b1k.println("Using nsis: $nsis_prog, version: $nsis_ver")
    return $nsis_prog
}

function setup_jdk() {
    if (!$manifest['jdk']) { return $null }
    $suffix = $('windows-x64.zip', 'linux-x64.tar.gz', 'macOS-x64.tar.gz').Get($HOST_OS)
    $javac_prog, $jdk_ver = find_prog -name 'jdk' -cmd 'javac'
    if ($javac_prog) {
        return $javac_prog
    }

    $java_home = Join-Path $prefix "jdk"
    $jdk_bin = Join-Path $java_home 'bin'
    $javac_prog, $jdk_ver = find_prog -name 'jdk' -cmd 'javac' -path $jdk_bin -silent $true
    if (!$javac_prog) {
        $b1k.rmdirs($java_home)

        # refer to https://learn.microsoft.com/en-us/java/openjdk/download
        download_and_expand "https://aka.ms/download-jdk/microsoft-jdk-$jdk_ver-$suffix" "$prefix/microsoft-jdk-$jdk_ver-$suffix" "$prefix/"
        
        # move to plain folder name
        $folderName = (Get-ChildItem -Path $prefix -Filter "jdk-$jdk_ver+*").Name
        if ($folderName) {
            $b1k.mv("$prefix/$folderName", $java_home)
        }
    }

    $env:JAVA_HOME = $java_home
    $env:CLASSPATH = ".;$java_home\lib\dt.jar;$java_home\lib\tools.jar"
    if ($env:PATH.IndexOf($jdk_bin) -eq -1) {
        $env:PATH = "$jdk_bin$envPathSep$env:PATH"
    }
    $javac_prog = find_prog -name 'javac' -path $jdk_bin -silent $true
    if (!$javac_prog) {
        throw "Install jdk $jdk_ver fail"
    }

    $b1k.println("Using jdk: $javac_prog, version: $jdk_ver")

    return $javac_prog
}

function setup_clang() {
    if (!$manifest.Contains('clang')) { return $null }
    $clang_prog, $clang_ver = find_prog -name 'clang'
    if (!$clang_prog) {
        throw 'required clang $clang_ver not installed, please install it from: https://github.com/llvm/llvm-project/releases'
    }
}

function setup_android_sdk() {
    if (!$manifest['ndk']) { return $null }
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
        $b1k.println("Looking require $ndk_ver$IsGraterThan in env:$sdk_root_env=$sdk_dir")
        if ("$sdk_dir" -ne '') {
            $sdk_root = $sdk_dir
            $ndk_root = $null

            $ndk_major = ($ndk_ver -replace '[^0-9]', '')
            $ndk_minor_off = "$ndk_major".Length + 1
            $ndk_minor = if ($ndk_minor_off -lt $ndk_ver.Length) { "$([int][char]$ndk_ver.Substring($ndk_minor_off) - $ndk_minor_base)" } else { '0' }
            $ndk_rev_base = "$ndk_major.$ndk_minor"

            $ndk_parent = Join-Path $sdk_dir 'ndk'
            if (!$b1k.isdir($ndk_parent)) {
                continue
            }

            # find ndk in sdk
            $ndks = [ordered]@{}
            $ndk_rev_max = '0.0'
            foreach ($item in $(Get-ChildItem -Path "$ndk_parent")) {
                $ndkDir = $item.FullName
                $sourceProps = "$ndkDir/source.properties"
                if ($b1k.isfile($sourceProps)) {
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
                $b1k.println("Found $ndk_root in $sdk_root ...")
                break
            }
        }
    }

    if (!$b1k.isdir("$ndk_root")) {
        $sdkmanager_prog, $sdkmanager_ver = $null, $null
        if ($b1k.isdir($sdk_root)) {
            $sdkmanager_prog, $sdkmanager_ver = (find_prog -name 'cmdlinetools' -cmd 'sdkmanager' -path "$sdk_root/cmdline-tools/latest/bin" -params "--version", "--sdk_root=$sdk_root")
        }
        else {
            $sdk_root = Join-Path $prefix 'adt/sdk'
            if (!$b1k.isdir($sdk_root)) {
                $b1k.mkdirs($sdk_root)
            }
        }

        if (!$sdkmanager_prog) {
            $sdkmanager_prog, $sdkmanager_ver = (find_prog -name 'cmdlinetools' -cmd 'sdkmanager' -path "$prefix/cmdline-tools/bin" -params "--version", "--sdk_root=$sdk_root")
            $suffix = $('win', 'linux', 'mac').Get($HOST_OS)
            if (!$sdkmanager_prog) {
                $b1k.println("Installing cmdlinetools version: $sdkmanager_ver ...")

                $cmdlinetools_pkg_name = "commandlinetools-$suffix-$($cmdlinetools_rev)_latest.zip"
                $cmdlinetools_pkg_path = Join-Path $prefix $cmdlinetools_pkg_name
                $cmdlinetools_url = "https://dl.google.com/android/repository/$cmdlinetools_pkg_name"
                download_file $cmdlinetools_url $cmdlinetools_pkg_path
                Expand-Archive -Path $cmdlinetools_pkg_path -DestinationPath "$prefix/"
                $sdkmanager_prog, $_ = (find_prog -name 'cmdlinetools' -cmd 'sdkmanager' -path "$prefix/cmdline-tools/bin" -params "--version", "--sdk_root=$sdk_root" -silent $True)
                if (!$sdkmanager_prog) {
                    throw "Install cmdlinetools version: $sdkmanager_ver fail"
                }
            }
        }

        $matchInfos = (exec_prog -prog $sdkmanager_prog -params "--sdk_root=$sdk_root", '--list' | Select-String 'ndk;')
        if ($null -ne $matchInfos -and $matchInfos.Count -gt 0) {
            $b1k.println("Not found suitable android ndk, installing ...")

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

            ((1..10 | ForEach-Object { "yes"; Start-Sleep -Milliseconds 100 }) | . $sdkmanager_prog --licenses --sdk_root=$sdk_root) | Out-Host
            exec_prog -prog $sdkmanager_prog -params '--verbose', "--sdk_root=$sdk_root", 'platform-tools', 'cmdline-tools;latest', 'platforms;android-33', 'build-tools;30.0.3', 'cmake;3.22.1', $ndkFullVer | Out-Host

            $fullVer = $ndkFullVer.Split(';')[1]
            $ndk_root = (Resolve-Path -Path "$sdk_root/ndk/$fullVer").Path
        }
    }

    return $sdk_root, $ndk_root
}

# enable emsdk emcmake
function setup_emsdk() {
    $emsdk_cmd = (Get-Command emsdk -ErrorAction SilentlyContinue)
    if (!$emsdk_cmd) {
        $emsdk_root = Join-Path $prefix 'emsdk'
        if (!(Test-Path $emsdk_root -PathType Container)) {
            git clone 'https://github.com/emscripten-core/emsdk.git' $emsdk_root
        }
        else {
            git -C $emsdk_root pull
        }
    }
    else {
        $emsdk_root = Split-Path $emsdk_cmd.Source -Parent
    }

    $emcmake = (Get-Command emcmake -ErrorAction SilentlyContinue)
    if (!$emcmake) {
        Set-Location $emsdk_root
        ./emsdk install latest
        ./emsdk activate latest
        . ./emsdk_env.ps1
        Set-Location -
    }
}

# preprocess methods: 
#   <param>-inputOptions</param> [CMAKE_OPTIONS]
function preprocess_win([string[]]$inputOptions) {
    $outputOptions = $inputOptions

    if ($options.sdk) {
        $outputOptions += "-DCMAKE_SYSTEM_VERSION=$($options.sdk)"
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
$is_gradlew = $options.xt.IndexOf('gradlew') -ne -1
function preprocess_andorid([string[]]$inputOptions) {
    $outputOptions = $inputOptions

    $t_archs = @{arm64 = 'arm64-v8a'; armv7 = 'armeabi-v7a'; x64 = 'x86_64'; x86 = 'x86'; }

    if ($is_gradlew) {
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
        # If set to ONLY, then only the roots in CMAKE_FIND_ROOT_PATH will be searched
        # If set to BOTH, then the host system paths and the paths in CMAKE_FIND_ROOT_PATH will be searched
        # If set to NEVER, then the roots in CMAKE_FIND_ROOT_PATH will be ignored and only the host system root will be used
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
    $cmake_toolchain_file = Join-Path $myRoot 'ios.cmake'
    $outputOptions += '-GXcode', "-DCMAKE_TOOLCHAIN_FILE=$cmake_toolchain_file", "-DARCHS=$arch"
    if ($BUILD_TARGET -eq 'tvos') {
        $outputOptions += '-DPLAT=tvOS'
    }
    elseif ($BUILD_TARGET -eq 'watchos') {
        $outputOptions += '-DPLAT=watchOS'
    }
    return $outputOptions
}

function preprocess_wasm([string[]]$inputOptions) {
    return $inputOptions
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
        'wasm'    = @{
            'host'      = @{'windows' = $True; 'linux' = $True; 'macos' = $True };
            'toolchain' = @{'emcc' = $True; };
        };
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
    'wasm'    = ${Function:preprocess_wasm};
}

validHostAndToolchain

########## setup build tools if not installed #######

$null = setup_glslcc

$cmake_prog = setup_cmake

if ($BUILD_TARGET -eq 'win32') {
    $nuget_prog = setup_nuget
    $nsis_prog = setup_nsis
    if ($TOOLCHAIN_NAME -eq 'clang') {
        $ninja_prog = setup_ninja
        $null = setup_clang
    }
}
elseif ($BUILD_TARGET -eq 'android') {
    $null = setup_jdk # setup android sdk cmdlinetools require jdk
    $sdk_root, $ndk_root = setup_android_sdk
    $env:ANDROID_HOME = $sdk_root
    $env:ANDROID_NDK = $ndk_root
    # we assume 'gradlew' to build apk, so require setup jdk11+
    # otherwise, build for android libs, needs setup ninja
    if ($is_gradlew) {
        $ninja_prog = setup_ninja
    }  
}
elseif ($BUILD_TARGET -eq 'wasm') {
    . setup_emsdk
}

if (!$setupOnly) {
    $stored_cwd = $(Get-Location).Path
    if ($options.d) {
        Set-Location $options.d
    }

    # parsing build optimize flag from build_options
    $buildOptions = [array]$options.xb
    $nopts = $buildOptions.Count
    $optimize_flag = $null
    for ($i = 0; $i -lt $nopts; ++$i) {
        $optv = $buildOptions[$i]
        if ($optv -eq '--config') {
            if ($i -lt ($nopts - 1)) {
                $optimize_flag = $buildOptions[$i + 1]
                ++$i
            }
            break
        }
    }

    $BUILD_ALL_OPTIONS = @()
    $BUILD_ALL_OPTIONS += $buildOptions
    if (!$optimize_flag) {
        $BUILD_ALL_OPTIONS += '--config', 'Release'
        $optimize_flag = 'Release'
    }

    # enter building steps
    $b1k.println("Building target $BUILD_TARGET on $HOST_OS_NAME with toolchain $TOOLCHAIN ...")

    # step1. preprocess cross make options
    $CONFIG_ALL_OPTIONS = [array]$(& $proprocessTable[$BUILD_TARGET] -inputOptions $CONFIG_DEFAULT_OPTIONS)

    if (!$CONFIG_ALL_OPTIONS) {
        $CONFIG_ALL_OPTIONS = @()
    }

    # step2. apply additional cross make options
    $xopts = [array]$options.xc
    if ($xopts.Count -gt 0) {
        $b1k.println("Apply additional cross make options: $($xopts), Count={0}" -f $xopts.Count)
        $CONFIG_ALL_OPTIONS += $xopts
    }
    if ("$($xopts)".IndexOf('-B') -eq -1) {
        $is_host_target = ($BUILD_TARGET -eq 'win32') -or ($BUILD_TARGET -eq 'linux') -or ($BUILD_TARGET -eq 'osx')
        if ($is_host_target) {
            # wheither building host target?
            $BUILD_DIR = "build_$($options.a)"
        }
        else {
            $BUILD_DIR = "build_${BUILD_TARGET}"
            if (!$is_wasm) {
                $BUILD_DIR += "_$($options.a)"
            }
        }
    }
    else {
        foreach ($opt in $xopts) {
            if ($opt.StartsWith('-B')) {
                $BUILD_DIR = $opt.Substring(2).Trim()
                break
            }
        }
    }
    $b1k.println("CONFIG_ALL_OPTIONS=$CONFIG_ALL_OPTIONS, Count={0}" -f $CONFIG_ALL_OPTIONS.Count)

    if (($BUILD_TARGET -eq 'android') -and $is_gradlew) {
        $storedLocation = (Get-Location).Path
        $build_tool = (Get-Command $options.xt).Source
        $build_tool_dir = Split-Path $build_tool -Parent
        Set-Location $build_tool_dir
        if ($optimize_flag -eq 'Debug') {
            & $build_tool assembleDebug $CONFIG_ALL_OPTIONS | Out-Host
        }
        else {
            & $build_tool assembleRelease $CONFIG_ALL_OPTIONS | Out-Host
        }
        Set-Location $storedLocation
    } 
    else {
        # step3. configure
       
        $workDir = $(Get-Location).Path

        $mainDep = Join-Path $workDir 'CMakeLists.txt'
        if (!$b1k.isfile($mainDep)) {
            throw "Missing CMakeLists.txt in $workDir"
        }

        $mainDepChanged = $false
        # A Windows file time is a 64-bit value that represents the number of 100-nanosecond
        $tempFileItem = Get-Item $mainDep
        $lastWriteTime = $tempFileItem.LastWriteTime.ToFileTimeUTC()
        $tempFile = Join-Path $BUILD_DIR 'b1k_cache.txt'

        $storeTime = 0
        if ($b1k.isfile($tempFile)) {
            $storeTime = Get-Content $tempFile -Raw
        }
        $mainDepChanged = "$storeTime" -ne "$lastWriteTime"
        $cmakeCachePath = Join-Path $workDir "$BUILD_DIR/CMakeCache.txt"

        if ($mainDepChanged -or !$b1k.isfile($cmakeCachePath)) {
            if ($options.p -eq 'linux' -or $options.p -eq 'wasm') {
                if($optimize_flag -eq 'Debug') {
                    $CONFIG_ALL_OPTIONS += '-DCMAKE_BUILD_TYPE=Debug'
                }
                else {
                    $CONFIG_ALL_OPTIONS += '-DCMAKE_BUILD_TYPE=Release'
                }
            }
            if (!$is_wasm) {
                cmake -B $BUILD_DIR $CONFIG_ALL_OPTIONS | Out-Host
            }
            else {
                emcmake cmake -B $BUILD_DIR $CONFIG_ALL_OPTIONS | Out-Host
            }
            Set-Content $tempFile $lastWriteTime -NoNewline
        }

        if (!$configOnly) {
            # step4. build
            # apply additional build options


            $BUILD_ALL_OPTIONS += "--parallel"
            if ($BUILD_TARGET -eq 'linux') {
                $BUILD_ALL_OPTIONS += "$(nproc)"
            }
            if ($TOOLCHAIN_NAME -eq 'xcode') {
                $BUILD_ALL_OPTIONS += '--', '-quiet'
                if ($options.a -eq 'x64') {
                    switch ($options.p) {
                        'ios' { $BUILD_ALL_OPTIONS += '-sdk', 'iphonesimulator' }
                        'tvos' { $BUILD_ALL_OPTIONS += '-sdk', 'appletvsimulator' }
                        'watchos' { $BUILD_ALL_OPTIONS += '-sdk', 'watchsimulator' }
                    }
                }
            }
            $b1k.println("BUILD_ALL_OPTIONS=$BUILD_ALL_OPTIONS, Count={0}" -f $BUILD_ALL_OPTIONS.Count)

            cmake --build $BUILD_DIR $BUILD_ALL_OPTIONS | Out-Host
        }
    }

    $env:buildResult = ConvertTo-Json @{
        buildDir   = $BUILD_DIR;
        targetOS   = $BUILD_TARGET;
        compilerID = $TOOLCHAIN_NAME;
    }

    Set-Location $stored_cwd
}
