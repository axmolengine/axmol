#
# This script easy to build win32, linux, winuwp, ios, tvos, osx, android depends on $myRoot/1k/build1k.ps1
# usage: pwsh build.ps1 -p <targetPlatform> -a <arch>
# options
#  -p: build target platform: win32,winuwp,linux,android,osx,ios,tvos,wasm
#      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first, 
#      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
#  -a: build arch: x86,x64,armv7,arm64; for android can be list by ';', i.e: 'arm64;x64'
#  -cc: toolchain: for win32 you can specific -cc clang to use llvm-clang, please install llvm-clang from https://github.com/llvm/llvm-project/releases
#  -xc: additional cmake options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
#  -xb: additional cross build options: i.e. -xb '--config','Release'
#  -nb: no build, only generate natvie project file (vs .sln, xcodeproj)
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
    [switch]$forceConfig
)

$options = @{p = $null; a = 'x64'; d = $null; cc = $null; xc = @(); xb = @(); sdk = $null }

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

$options.xb = translate_array_opt $options.xb
$options.xc = translate_array_opt $options.xc

$myRoot = $PSScriptRoot
$workDir = $(Get-Location).Path

# axroot
$AX_ROOT = $env:AX_ROOT
if (!$AX_ROOT -and (Test-Path "$myRoot/core/axmolver.h.in" -PathType Leaf)) {
    $AX_ROOT = $myRoot
    $env:AX_ROOT = $AX_ROOT
}

# b1kroot preferred axmol, but b1k(1k,build.ps1) can be copy to any isolated directory to work
$b1k_root = $AX_ROOT
if(!$b1k_root) {
    if(Test-Path "$myRoot/1k/build1k.ps1" -PathType Leaf) {
        $b1k_root = $myRoot
    }
    else {
        throw "The build1k.ps1 not found"
    }
}

$source_proj_dir = if($options.d) { $options.d } else { $workDir }
$is_engine = ($source_proj_dir -eq $AX_ROOT)
$is_android = $options.p -eq 'android'
$is_ci = $env:GITHUB_ACTIONS -eq 'true'

# start construct full cmd line
$b1k_script = (Resolve-Path -Path "$b1k_root/1k/build1k.ps1").Path
$b1k_args = @()

if ($is_engine -and $is_android) {
    if ($is_ci) {
        $source_proj_dir = Join-Path $myRoot 'tests/cpp-tests'
    } else {
        $source_proj_dir = Join-Path $myRoot 'templates/cpp-template-default'
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

$bti = $null # cmake target param index
$bci = $null # cmake optimize flag param index
# parsing build options
$nopts = $options.xb.Count
for ($i = 0; $i -lt $nopts; ++$i) {
    $optv = $options.xb[$i]
    if ($optv -eq '--target') {
        if ($i -lt ($nopts - 1)) {
            $bti = $i + 1
        }
    }
    elseif($optv -eq '--config') {
        if ($i -lt ($nopts - 1)) {
            $bci = $i + 1
        }
    }
}

if (!$is_android) {
    if (!$bti) {
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
                'cpp_tests'
            )
        )
        $cmake_target = $cmake_targets[$is_ci][$is_engine]

        # reason:
        #   - android package not accept '-'
        #   - ios deploy device may failed with unknown error
        $cmake_target = $cmake_target.Replace('-', '_')
        $options.xb += '--target', $cmake_target
        $env:b1k_override_target = $null
    } else{
        $cmake_target = $options.xb[$bti]
        $env:b1k_override_target = $true
    }
} else { # android
    # engine ci
    if ($is_engine -and $is_ci) {
        $options.xc += "-PRELEASE_STORE_FILE=$AX_ROOT/tools/ci/axmol-ci.jks", '-PRELEASE_STORE_PASSWORD=axmol-ci', '-PRELEASE_KEY_ALIAS=axmol-ci', '-PRELEASE_KEY_PASSWORD=axmol-ci'
    }
}

if (!$bci) {
    $optimize_flag = @('Debug', 'Release')[$is_ci]
    $options.xb += '--config', $optimize_flag
} else {
    $optimize_flag = $options.xb[$bci]
}

if ($is_android -and (Test-Path $(Join-Path $proj_dir 'proj.android/gradlew') -PathType Leaf)) {
    $b1k_args += '-xt', 'proj.android/gradlew'
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

. $b1k_script @b1k_args @forward_args

if (!$nb) {
    $b1k.pause('Build done')
}
else {
    $b1k.pause('Generate done')
}
