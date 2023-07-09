#
# This script easy to build win32, linux, winuwp, ios, tvos, osx, android depends on $AX_ROOT/tools/ci/build1k.ps1
# usage: pwsh build.ps1 -p <targetPlatform> -a <arch>
# options
#  -p: build target platform: win32,winuwp,linux,android,osx,ios,tvos,watchos
#      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first, 
#      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
#  -a: build arch: x86,x64,armv7,arm64; for android can be list by ';', i.e: 'arm64;x64'
#  -cc: toolchain: for win32 you can specific -cc clang to use llvm-clang, please install llvm-clang from https://github.com/llvm/llvm-project/releases
#  -xc: additional cmake options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
#  -xb: additional cross build options: i.e. -xb '--config','Release'
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
# build.ps1 without any arguments:
# - pwsh build.ps1
#   on windows: target platform is win32, arch=x64
#   on linux: target platform is linux, arch=x64
#   on macos: target platform is osx, arch=x64
#

$options = @{p = $null; a = 'x64'; d = $null; cc = $null; xc = @(); xb = @(); winsdk = $null }

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
        $optName = $null
    }
}

function add_quote($value) {
    $ret = $null
    $valueType = $value.GetType()
    if ($valueType -eq [string]) {
        $ret = "'$value'"
    }
    elseif ($valueType -eq [object[]]) {
        $ret = ''
        for ($i = 0; $i -lt $value.Count; ++$i) {
            $subVal = $value[$i]
            $ret += "'$subVal'"
            if ($i -ne ($value.Count - 1)) {
                $ret += ','
            }
        }
    }
    return $ret
}

$myRoot = $PSScriptRoot
$workDir = $(Get-Location).Path

$AX_ROOT = $env:AX_ROOT
if (!$AX_ROOT) {
    if(Test-Path "$myRoot/core/axmolver.h.in" -PathType Leaf) {
        $AX_ROOT = $myRoot
        $env:AX_ROOT = $AX_ROOT
    }
    else {
        throw "Can't determine axmol engine root, please run python setup.py first"
    }
}

$is_engine = ($workDir -eq $AX_ROOT)
$is_android = $options.p -eq 'android'
$is_ci = $env:GITHUB_ACTIONS -eq 'true'

# start construct full cmd line
$fullCmdLine = @("$((Resolve-Path -Path "$AX_ROOT/1k/build1k.ps1").Path)")

$search_prior_dir = $options.d
if (!$search_prior_dir -and $is_engine -and $is_android) {
    if ($is_ci) {
        $search_prior_dir = Join-Path $myRoot 'tests/cpp-tests'
    } else {
        $search_prior_dir = Join-Path $myRoot 'templates/cpp-template-default'
    }
}

$search_paths = if ($search_prior_dir) { @($search_prior_dir, $workDir, $myRoot) } else { @($workDir, $myRoot) }
function search_proj($path, $type) {
    foreach ($search_path in $search_paths) {
        $full_path = Join-Path $search_path $path
        if (Test-Path $full_path -PathType $type) {
            $ret_path = if ($type -eq 'Container') { $full_path } else { $search_path }
            return $ret_path
        }
    }
    return $null
}

$search_rules = @(
    # others
    @{
        path = 'CMakeLists.txt';
        type = 'Leaf'
    },
    # android
    @{
        path = 'proj.android';
        type = 'Container';
    }
)

$search_rule = $search_rules[$is_android]
$proj_dir = search_proj $search_rule.path $search_rule.type

$proj_name = (Get-Item $myRoot).BaseName

$bti = $null
$bci = $null
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
        $options.xb += '--target', $cmake_target
    }
} else { # android
    # engine ci
    if ($is_engine -and $is_ci) {
        $fullCmdLine += "'-xc'", "'-PRELEASE_STORE_FILE=$AX_ROOT/tools/ci/axmol-ci.jks','-PRELEASE_STORE_PASSWORD=axmol-ci','-PRELEASE_KEY_ALIAS=axmol-ci','-PRELEASE_KEY_PASSWORD=axmol-ci'"
    }
}

if (!$bci) {
    $optimize_flag = @('Debug', 'Release')[$is_ci]
    $options.xb += '--config', $optimize_flag
}

if ($is_android) {
    $fullCmdLine += "'-xt'", "'gradle'"
}

if ($proj_dir) {
    $fullCmdLine += "'-d'", "'$proj_dir'"
}
$prefix = Join-Path $AX_ROOT 'tools/external'
$fullCmdLine += "'-prefix'", "'$prefix'"

# remove arg we don't want forward to
$options.Remove('d')
foreach ($option in $options.GetEnumerator()) {
    if ($option.Value) {
        $fullCmdLine += add_quote "-$($option.Key)"
        $fullCmdLine += add_quote $option.Value
    }
}

$strFullCmdLine = "$fullCmdLine"
Invoke-Expression $strFullCmdLine
