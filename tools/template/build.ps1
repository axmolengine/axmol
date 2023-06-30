#
# This script easy to build win32, linux, winuwp, ios, tvos, osx, android
# usage: pwsh build.ps1 -p <targetPlatform> -a <arch>
# options
#  -p: build target platform: win32,winuwp,linux,android,osx,ios,tvos,watchos
#      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first, 
#      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
#  -a: build arch: x86,x64,armv7,arm64; for android can be list by ';', i.e: 'arm64;x64'
#  -cc: toolchain: for win32 you can specific -cc clang to use llvm-clang, please install llvm-clang from https://github.com/llvm/llvm-project/releases
#  -xc: additional cmake options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
#  -xb: additional cross build options: i.e. -xb '--config','Release'
# examples:
#   - win32: 
#     - pwsh build.ps1 -p win32
#     - pwsh build.ps1 -p win32 -cc clang
#   - winuwp: pwsh build.ps1 -p winuwp
#   - linux: pwsh build.ps1 -p linux
#   - android: 
#     - pwsh build.ps1 -p android -a arm64
#     - pwsh build.ps1 -p android -a 'arm64;x64'
#   - osx: 
#     - pwsh build.ps1 -p osx -a x64
#     - pwsh build.ps1 -p osx -a arm64
#   - ios: pwsh build.ps1 -p ios -a x64
#   - tvos: pwsh build.ps1 -p tvos -a x64
# build.ps1 without any arguments:
# - pwsh build.ps1
#   on windows: target platform is win32, arch=x64
#   on linux: target platform is linux, arch=x64
#   on macos: target platform is osx, arch=x64
#

$fullCmdLine = @("$((Resolve-Path -Path "$env:AX_ROOT/tools/ci/build1k.ps1").Path)")

$options = @{p = ''; a = 'x64'; cc = ''; xc = @(); xb = @(); }

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
            Write-Host "Warning: ignore unrecognized option: $optName"
        }
        $optName = $null
    }
}

if (!$options.p) {
    if ($IsWindows -or ("$env:OS" -eq 'Windows_NT')) {
        $options.p = 'win32'
    }
    else {
        if ($IsLinux) {
            $options.p = 'linux'
        }
        elseif ($IsMacOS) {
            $options.p = 'osx'
        }
        else {
            throw "Unsupported host OS for building target $(options.p)"
        }
    }
}

function add_quote($value) {
    $ret = "'"
    $ret += $value
    $ret += "'"
    return $ret
}

foreach ($option in $options.GetEnumerator()) {
    $fullCmdLine += add_quote "-$($option.Key)"
    $fullCmdLine += add_quote $option.Value
}

if ($options.p -eq 'android') {
    $fullCmdLine += "'-xt'", "'gradle'"
    $proj_dir = (Resolve-Path "$PSScriptRoot/proj.android").Path
    if(!$proj_dir) {
        $proj_dir = (Resolve-Path "$((Get-Location).Path)/proj.android").Path
    }
} else { 
    $proj_dir = $PSScriptRoot 
}
$fullCmdLine += "'-d'", "'$proj_dir'"

$prefix = Join-Path -Path $env:AX_ROOT -ChildPath 'tools/external'
$fullCmdLine += "'-prefix'", "'$prefix'"

$strFullCmdLine = "$fullCmdLine"
Invoke-Expression -Command $strFullCmdLine
