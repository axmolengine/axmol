$BUILD_ARCH = $args[0]

# CONFIG_ALL_OPTIONS
$CONFIG_ALL_OPTIONS=@()

if($BUILD_ARCH -eq "x86") {
   $CONFIG_ALL_OPTIONS += '-A', 'Win32'
}
else {
   $CONFIG_ALL_OPTIONS += '-A', 'Win64'
}

cmake -S . -B build_$BUILD_ARCH $CONFIG_ALL_OPTIONS -DBUILD_EXTENSION_IMGUIEXT=ON

cmake --build build_$BUILD_ARCH --config Release --target cpp-tests

