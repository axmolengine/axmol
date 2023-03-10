######## the 1k build script for targets: win32, winuwp
## examples:
##  - build win32 x64:  tools\win-ci\build.ps1 x64
##  - build win32 x64 dll:  tools\win-ci\build.ps1 x64 true
##  - build winuwp x64: tools\win-ci\build.ps1 x64 false true
##  - cross build winuwp arm64: tools\win-ci\build.ps1 arm64 false true
##

$BUILD_ARCH = $args[0]
$BUILD_DLL = $args[1]
$UWP = $args[2]

$optArch = $BUILD_ARCH

if ($optArch -eq 'x86') {
    $optArch = 'Win32'
}
elseif ($optArch -eq 'amd64_arm64') {
    $optArch = 'arm64'
}

$isuwp = ("$UWP" -eq 'true')

# CONFIG_ALL_OPTIONS
$CONFIG_ALL_OPTIONS=@()

# arch
$CONFIG_ALL_OPTIONS += '-A', $optArch

# cxx_std, if $UWP, must be c++17, C++/CX not support c++20 or later yet.
if ($isuwp) {
    $CONFIG_ALL_OPTIONS += '-DCMAKE_CXX_STANDARD=17'
}

# sdkver require xxx, but 10.0.22621.0 recommanded
# empty, cmake will choose same with runner windows OS build version
if ("$env:WINSDK_VER" -ne '') {
    $CONFIG_ALL_OPTIONS += "-DCMAKE_SYSTEM_VERSION=$env:WINSDK_VER"
}

# target
if ($isuwp) {
    $CONFIG_ALL_OPTIONS += '-DCMAKE_SYSTEM_NAME=WindowsStore', '-DCMAKE_SYSTEM_VERSION=10.0'
}

# Enable extensions not enabled by engine default
$CONFIG_ALL_OPTIONS += '-DAX_ENABLE_EXT_LIVE2D=TRUE', '-DAX_ENABLE_EXT_EFFEKSEER=TRUE'
if (!$isuwp) { # The extensions ImGui not support UWP yet
    $CONFIG_ALL_OPTIONS += '-DAX_ENABLE_EXT_IMGUI=TRUE'
} 

# dll
if ($BUILD_DLL -eq "true") {
    $CONFIG_ALL_OPTIONS += '-DBUILD_SHARED_LIBS=TRUE'
}

# output all options for debugging
Write-Output ("CONFIG_ALL_OPTIONS=$CONFIG_ALL_OPTIONS, Count={0}" -f $CONFIG_ALL_OPTIONS.Count)

# print cmake version for debugging purpose
cmake --version

# geneate .sln
cmake -S . -B build_$BUILD_ARCH -Thost=x64 $CONFIG_ALL_OPTIONS

# build cpp_test
cmake --build build_$BUILD_ARCH --config Release --target cpp_tests

# biuld lua_tests if not pull request
if ( !($env:PULL_REQUEST -eq 'yes') ) {
    cmake --build build_$BUILD_ARCH --config Release --target lua_tests
}
