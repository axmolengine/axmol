######## the 1k build script for targets: win32, winuwp
## examples:
##  - build win32 x64:  tools\win-ci\build.ps1 -arch x64
##  - build win32 x64 dll:  tools\win-ci\build.ps1 -arch x64 -is_dll true
##  - build winuwp x64: tools\win-ci\build.ps1 -arch x64 -is_dll false -is_uwp true
##  - cross build winuwp arm64: tools\win-ci\build.ps1 -arch arm64 -is_dll false -is_uwp true
##

## parsing options
$options = @{ arch = 'x64'; is_dll = 'false'; is_uwp = 'false'; is_clang = 'false'; target = $null; config='Release' }

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
            Write-Output "Warning: ignore unrecognized option: $optName"
        }
        $optName = $null
    }
}

echo $options

## read options
$BUILD_ARCH = $options.arch
$BUILD_DLL = ($options.is_dll -eq 'true')
$UWP = ($options.is_uwp -eq 'true')
$CLANG = ($options.is_clang -eq 'true')

## process options
$optArch = $BUILD_ARCH

if ($optArch -eq 'x86') {
    $optArch = 'Win32'
}
elseif ($optArch -eq 'amd64_arm64') {
    $optArch = 'arm64'
    $BUILD_ARCH = 'arm64'
}

# CONFIG_ALL_OPTIONS
$CONFIG_ALL_OPTIONS=@()

# cmake generator, default is latest Visual Studio installed on target machine, such as "Visual Studio 17 2022"
if ($CLANG) {
    clang --version
    $CONFIG_ALL_OPTIONS += '-G', 'Ninja Multi-Config', '-DCMAKE_C_COMPILER=clang', '-DCMAKE_CXX_COMPILER=clang++'
}

# arch
if (!$CLANG) {
    $CONFIG_ALL_OPTIONS += '-A', $optArch
    $CONFIG_ALL_OPTIONS += '-Thost=x64'
} # TODO: how to specific arch for clang-cl toolchain?

# cxx_std, if $UWP, must be c++17, C++/CX not support c++20 or later yet.
if ($UWP) {
    $CONFIG_ALL_OPTIONS += '-DCMAKE_CXX_STANDARD=17'
}

# sdkver require xxx, but 10.0.22621.0 recommanded
# empty, cmake will choose same with runner windows OS build version
if ("$env:WINSDK_VER" -ne '') {
    $CONFIG_ALL_OPTIONS += "-DCMAKE_SYSTEM_VERSION=$env:WINSDK_VER"
}

# target
if ($UWP) {
    $CONFIG_ALL_OPTIONS += '-DCMAKE_SYSTEM_NAME=WindowsStore', '-DCMAKE_SYSTEM_VERSION=10.0'
}

# Enable extensions not enabled by engine default
$CONFIG_ALL_OPTIONS += '-DAX_ENABLE_EXT_LIVE2D=TRUE', '-DAX_ENABLE_EXT_EFFEKSEER=TRUE'
if (!$UWP) { # The extensions ImGui not support UWP yet
    $CONFIG_ALL_OPTIONS += '-DAX_ENABLE_EXT_IMGUI=TRUE'
} 

# dll
if ($BUILD_DLL) {
    $CONFIG_ALL_OPTIONS += '-DBUILD_SHARED_LIBS=TRUE'
}

# output all options for debugging
Write-Output ("CONFIG_ALL_OPTIONS=$CONFIG_ALL_OPTIONS, Count={0}" -f $CONFIG_ALL_OPTIONS.Count)

# print cmake version for debugging purpose
cmake --version

# geneate .sln
cmake -S . -B build_$BUILD_ARCH $CONFIG_ALL_OPTIONS

$target = $options.target

if ($target) {
    cmake --build build_$BUILD_ARCH --config $options.config --target $target
} else { # for github actions 
    # build cpp_test
    cmake --build build_$BUILD_ARCH --config Release --target cpp_tests

    # biuld lua_tests if not pull request
    if ( !($env:PULL_REQUEST -eq 'yes') ) {
        cmake --build build_$BUILD_ARCH --config Release --target lua_tests
    }
}
