$BUILD_ARCH = $args[0]

# CONFIG_ALL_OPTIONS
$CONFIG_ALL_OPTIONS=@()

if($BUILD_ARCH -eq "x86") {
   $CONFIG_ALL_OPTIONS += '-A', 'Win32'
}

cmake -S . -B build_$BUILD_ARCH $CONFIG_ALL_OPTIONS -Thost=x64 -DAX_ENABLE_EXT_IMGUI=ON

cmake --build build_$BUILD_ARCH --config Release --target cpp_tests
cmake --build build_$BUILD_ARCH --config Release --target lua_tests
