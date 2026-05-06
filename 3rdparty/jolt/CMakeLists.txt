cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(JoltPhysics VERSION 5.5.1 LANGUAGES CXX)

# When turning this option on, the library will be compiled using assertions. By default asserts are enabled in Debug build.
option(USE_ASSERTS "Enable asserts" OFF)

# When turning this option on, the library will be compiled using doubles for positions. This allows for much bigger worlds.
option(DOUBLE_PRECISION "Use double precision math" OFF)

# Which type of debug symbols to generate, e.g. using source-map when compiling with emscripten makes compilation a lot faster
set(JPH_DEBUG_SYMBOL_FORMAT "" CACHE STRING "Which type of debug symbols to generate")

# When turning this option on, the library will be compiled in such a way to attempt to keep the simulation deterministic across platforms
option(CROSS_PLATFORM_DETERMINISTIC "Cross platform deterministic" OFF)

# When turning this option on, the library will be compiled for ARM using the CROSS_COMPILE_ARM_TARGET architecture, requires compiling with clang
option(CROSS_COMPILE_ARM "Cross compile to the CROSS_COMPILE_ARM_TARGET architecture" OFF)

# When cross compiling to ARM this specifies which target to use. Can be 'aarch64-linux-gnu' for 64-bit or 'arm-linux-gnueabihf' for 32-bit
set(CROSS_COMPILE_ARM_TARGET "aarch64-linux-gnu" CACHE STRING "The target to use")

# When turning this option on, Jolt will be compiled as a shared library and public symbols will be exported.
option(JPH_BUILD_SHARED_LIBS "Compile Jolt as a shared library" ${BUILD_SHARED_LIBS})

# When turning this option on, the library will be compiled with interprocedural optimizations enabled, also known as link-time optimizations or link-time code generation.
# Note that if you turn this on you need to use SET_INTERPROCEDURAL_OPTIMIZATION() or set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON) to enable LTO specifically for your own project as well.
# If you don't do this you may get an error: /usr/bin/ld: libJolt.a: error adding symbols: file format not recognized
option(INTERPROCEDURAL_OPTIMIZATION "Enable interprocedural optimizations" ON)

# When turning this on, in Debug and Release mode, the library will emit extra code to ensure that the 4th component of a 3-vector is kept the same as the 3rd component
# and will enable floating point exceptions during simulation to detect divisions by zero.
# Note that this currently only works using MSVC. Clang turns Float2 into a SIMD vector sometimes causing floating point exceptions (the option is ignored).
option(FLOATING_POINT_EXCEPTIONS_ENABLED "Enable floating point exceptions" ON)

# Number of bits to use in ObjectLayer. Can be 16 or 32.
option(OBJECT_LAYER_BITS "Number of bits in ObjectLayer" 16)

# Select X86 processor features to use (if everything is off it will be SSE2 compatible)
option(USE_SSE4_1 "Enable SSE4.1" ON)
option(USE_SSE4_2 "Enable SSE4.2" ON)
option(USE_AVX "Enable AVX" ON)
option(USE_AVX2 "Enable AVX2" ON)
option(USE_AVX512 "Enable AVX512" OFF)
option(USE_LZCNT "Enable LZCNT" ON)
option(USE_TZCNT "Enable TZCNT" ON)
option(USE_F16C "Enable F16C" ON)
option(USE_FMADD "Enable FMADD" ON)

# Enable SIMD for the WASM build. Note that this is currently off by default since not all browsers support this.
# See: https://caniuse.com/?search=WebAssembly%20SIMD (Safari got support in March 2023 and was the last major browser to get support).
option(USE_WASM_SIMD "Enable SIMD for WASM" OFF)

# Enable 64 bit WASM instead of the default 32 bit WASM. Note that this currently requires special commandline flags in browsers and nodejs to enable.
# E.g. use 'node --experimental-wasm-memory64 UnitTests.js' to run the unit tests in nodejs in 64 bit.
option(JPH_USE_WASM64 "Enable 64 bit WASM" OFF)

# Enable all warnings
option(ENABLE_ALL_WARNINGS "Enable all warnings and warnings as errors" ON)

# Setting to periodically trace broadphase stats to help determine if the broadphase layer configuration is optimal
option(TRACK_BROADPHASE_STATS "Track Broadphase Stats" OFF)

# Setting to periodically trace narrowphase stats to help determine which collision queries could be optimized
option(TRACK_NARROWPHASE_STATS "Track Narrowphase Stats" OFF)

# Setting to track simulation timings per body
option(JPH_TRACK_SIMULATION_STATS "Track Simulation Stats" OFF)

# Enable the debug renderer in the Debug and Release builds. Note that DEBUG_RENDERER_IN_DISTRIBUTION will override this setting.
option(DEBUG_RENDERER_IN_DEBUG_AND_RELEASE "Enable debug renderer in Debug and Release builds" OFF)

# Setting to enable the debug renderer in all builds.
# Note that enabling this reduces the performance of the library even if you're not drawing anything.
option(DEBUG_RENDERER_IN_DISTRIBUTION "Enable debug renderer in all builds" OFF)

# Enable the profiler in Debug and Release builds. Note that PROFILER_IN_DISTRIBUTION will override this setting.
option(PROFILER_IN_DEBUG_AND_RELEASE "Enable the profiler in Debug and Release builds" OFF)

# Enable the profiler in all builds.
# Note that enabling this reduces the performance of the library.
option(PROFILER_IN_DISTRIBUTION "Enable the profiler in all builds" OFF)

# Ability to use the external profiler using CMake config. Defines preprocessor JPH_EXTERNAL_PROFILE.
# Use external profiler set using ProfileStartMeasurement and ProfileEndMeasurement to profile.
# Option is available only when profiling is enabled using PROFILER_IN_DEBUG_AND_RELEASE or PROFILER_IN_DISTRIBUTION.
option(JPH_USE_EXTERNAL_PROFILE "Use external profiler when profiling is enabled" OFF)

# Setting this option will force the library to use malloc/free instead of allowing the user to override the memory allocator
option(DISABLE_CUSTOM_ALLOCATOR "Disable support for a custom memory allocator" OFF)

# Setting this option will force the library to use the STL vector instead of the custom Array class
option(USE_STD_VECTOR "Use std::vector instead of own Array class" ON)

# Setting this option will compile the ObjectStream class and RTTI attribute information
option(ENABLE_OBJECT_STREAM "Compile the ObjectStream class and RTTI attribute information" OFF)

# Option to compile with DirectX 12 compute
option(JPH_USE_DX12 "Use DirectX" OFF)

# Option to compile with Vulkan compute
option(JPH_USE_VK "Use Vulkan" OFF)

# Option to compile with Metal compute
option(JPH_USE_MTL "Use Metal" OFF)

# Option to compile with CPU fallback compute
option(JPH_USE_CPU_COMPUTE "Use CPU Compute" OFF)

# Option to enable shader debug symbols
option(JPH_SHADER_DEBUG_SYMBOLS "Shader Debug Symbols" OFF)

# Option to enable shader optimizations. When this is on, the shaders will be optimized for performance, otherwise they're suitable for debugging.
option(JPH_SHADER_OPTIMIZATION "Shader Optimization" ON)

set(PHYSICS_REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}")

include("${CMAKE_CURRENT_LIST_DIR}/Jolt/Jolt.cmake")
