$BUILD_ARCH = $args[0]
$BUILD_DLL = $args[1]

# CONFIG_ALL_OPTIONS
$CONFIG_ALL_OPTIONS=@()

if ($BUILD_ARCH -eq "x86") {
    $CONFIG_ALL_OPTIONS += '-A', 'Win32'
}

if ($BUILD_DLL -eq "true") {
    $CONFIG_ALL_OPTIONS += '-DBUILD_SHARED_LIBS=TRUE'
}

# windows sdk version, require xxx, but 10.0.22621.0 recommanded

if (env:$WINSDK_VER -eq '') { # empty, cmake will choose same with runner windows OS build version
    cmake -S . -B build_$BUILD_ARCH $CONFIG_ALL_OPTIONS -Thost=x64 -DAX_ENABLE_EXT_IMGUI=TRUE -DAX_ENABLE_EXT_LIVE2D=TRUE -DAX_ENABLE_EXT_EFFEKSEER=TRUE
} else {
    cmake -S . -B build_$BUILD_ARCH $CONFIG_ALL_OPTIONS -Thost=x64 -DAX_ENABLE_EXT_IMGUI=TRUE -DAX_ENABLE_EXT_LIVE2D=TRUE -DAX_ENABLE_EXT_EFFEKSEER=TRUE "-DCMAKE_SYSTEM_VERSION=$env:WINSDK_VER"
}

cmake --build build_$BUILD_ARCH --config Release --target cpp_tests

if ( !($env:PULL_REQUEST -eq 'yes') ) {
    cmake --build build_$BUILD_ARCH --config Release --target lua_tests
}
