#
# This script easy to build win32, linux, winuwp(winrt), ios, tvos, osx, android depends on $myRoot/1k/build.ps1
# usage: pwsh build.ps1 -p <targetPlatform> -a <arch>
# options
#  -p: build target platform: win32,winuwp(winrt),linux,android,osx,ios,tvos,wasm
#      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first, 
#      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
#  -a: build arch: x86,x64,armv7,arm64; for android can be list by ';', i.e: 'arm64;x64'
#  -cc: toolchain: for win32 you can specific -cc clang to use llvm-clang, please install llvm-clang from https://github.com/llvm/llvm-project/releases
#  -xc: additional cmake options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
#  -xb: additional cross build options: i.e. -xb '--config','Release'
#  -c(configOnly): no build, only generate natvie project file (vs .sln, xcodeproj)
#  -d: specify project dir to compile, i.e. -d /path/your/project/
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
#   - wasm: pwsh build.ps1 -p wasm
# build.ps1 without any arguments:
# - pwsh build.ps1
#   on windows: target platform is win32, arch=x64
#   on linux: target platform is linux, arch=x64
#   on macos: target platform is osx, arch=x64
#
param(
    [switch]$configOnly,
    [switch]$forceConfig,
    [switch]$setupOnly
)

$options = @{p = $null; a = $null; d = $null; cc = $null; xc = @(); xb = @(); sdk = $null; dll = $false }

$optName = $null
foreach ($arg in $args) {
    if (!$optName) {
        if ($arg.StartsWith('-')) { 
            $optName = $arg.SubString(1).TrimEnd(':')
        }
    }
    else {
        if ($options.Contains($optName)) {
            $options[$optName] = $arg
        }
        $optName = $null
    }
}

function translate_array_opt($opt) {
    if ($opt -and $opt.GetType().BaseType -ne [array]) {
        $opt = "$opt".Split(',')
    }
    return $opt
}

if ($options.xb.Count -ne 0) {
    [array]$options.xb = (translate_array_opt $options.xb)
}
if ($options.xc.Count -ne 0) {
    [array]$options.xc = (translate_array_opt $options.xc)
}

$myRoot = $PSScriptRoot
$workDir = $(Get-Location).Path

# axroot
$AX_ROOT = $null
if (Test-Path "$myRoot/core/axmolver.h.in" -PathType Leaf) {
    $AX_ROOT = $myRoot
    $env:AX_ROOT = $AX_ROOT
} else {
    throw "The axmol engine incompleted"
}

# 1k/build.ps1
if(Test-Path "$myRoot/1k/build.ps1" -PathType Leaf) {
    $b1k_root = $myRoot
}
else {
    throw "The 1k/build.ps1 not found"
}

$source_proj_dir = if($options.d) { $options.d } else { $workDir }
$is_engine = ($source_proj_dir -eq $AX_ROOT)
$is_android = $options.p -eq 'android'
$is_ci = $env:GITHUB_ACTIONS -eq 'true'

# start construct full cmd line
$b1k_script = (Resolve-Path -Path "$b1k_root/1k/build.ps1").Path
$b1k_args = @()

$cm_target_index = $options.xb.IndexOf('--target')
if($cm_target_index -ne -1) {
    $cmake_target = $options.xb[$cm_target_index + 1]
}

if ($is_engine -and $is_android) {
    if (!$cmake_target) {
        if ($is_ci) {
            $source_proj_dir = Join-Path $myRoot 'tests/cpp-tests'
        } else {
            $source_proj_dir = Join-Path $myRoot 'templates/cpp-template-default'
        }
    } else {
        $builtin_targets = @{
            'HelloCpp' = 'templates/cpp-template-default'
            'HelloLua' = 'templates/cpp-template-default'
            'cpp-tests' = 'tests/cpp-tests'
            'fairygui-tests' = 'tests/fairygui-tests'
            'live2d-tests' = 'tests/live2d-tests'
        }
        if (!$builtin_targets.Contains($cmake_target)) {
            throw "specified target '$cmake_target' not present in engine"
        }
        $source_proj_dir = Join-Path $myRoot $builtin_targets[$cmake_target]
    }
}

$search_paths = if ($source_proj_dir -ne $myRoot) { @($source_proj_dir, $myRoot) } else { @($source_proj_dir) }
function search_proj_file($file_path, $type) {
    foreach ($search_path in $search_paths) {
        $full_path = Join-Path $search_path $file_path
        if (Test-Path $full_path -PathType $type) {
            # $ret_path = if ($type -eq 'Container') { $full_path } else { $search_path }
            return $search_path
        }
    }
    return $null
}

$proj_dir = search_proj_file 'CMakeLists.txt' 'Leaf'
$proj_name = (Get-Item $proj_dir).BaseName

$use_gradle = $is_android -and (Test-Path $(Join-Path $proj_dir 'proj.android/gradlew') -PathType Leaf)
if ($use_gradle) {
    $b1k_args += '-xt', 'proj.android/gradlew'
}

if (!$use_gradle) {
    if (!$cmake_target) {
        # non android, specific cmake target
        $cmake_targets = @(
            # local developer
            @(
                # project
                $proj_name,
                # engine
                'HelloCpp'
            ),
            # github actions
            @(
                # project
                $proj_name,
                # engine
                'cpp-tests'
            )
        )
        $cmake_target = $cmake_targets[$is_ci][$is_engine]
        $options.xb += '--target', $cmake_target
    }

    if($is_android) {
        if ($options.xc.IndexOf('-DANDROID_STL')) {
            $options.xc += '-DANDROID_STL=c++_shared'
        }
    }
} else { # android gradle
    # engine ci
    if ($is_engine) {
        $options.xc += "-PKEY_STORE_FILE=$AX_ROOT/tools/ci/axmol-ci.jks", '-PKEY_STORE_PASSWORD=axmol-ci', '-PKEY_ALIAS=axmol-ci', '-PKEY_PASSWORD=axmol-ci'
    }
}

if ($proj_dir) {
    $b1k_args += '-d', "$proj_dir"
}
$prefix = Join-Path $b1k_root 'tools/external'
$b1k_args += '-prefix', "$prefix"

# remove arg we don't want forward to
$options.Remove('d')
$b1k_args = [System.Collections.ArrayList]$b1k_args
foreach ($option in $options.GetEnumerator()) {
    if ($option.Value) {
        $null = $b1k_args.Add("-$($option.Key)")
        $null = $b1k_args.Add($option.Value)
    }
}

$forward_args = @{}
if ($configOnly) {
    $forward_args['configOnly'] = $true
}
if ($forceConfig) {
    $forward_args['forceConfig'] = $true
}
if ($setupOnly) {
    $forward_args['setupOnly'] = $true
}

. $b1k_script @b1k_args @forward_args

if (!$configOnly) {
    $b1k.pause('Build done')
}
else {
    $b1k.pause('Generate done')
}

