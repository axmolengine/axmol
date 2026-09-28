# The Axmol SDK & tools references

Versions below describe the current repository configuration. A preferred version
is installed when no suitable tool is found; an existing version that meets the
minimum requirement can be reused. Pinned versions and CI baselines are listed
separately. Upstream badges show upstream releases, not Axmol's configured versions.

## PowerShell

- [![github](https://img.shields.io/github/v/release/PowerShell/PowerShell?label=Upstream)](https://github.com/PowerShell/PowerShell)
- Minimum version: 7.4.0
- Preferred version: 7.6.6
- License: MIT
- Platform: Win32/macOS/Linux
- Managed by: `1k/pwshi.sh`
- Note: Lua binding generation also requires PowerShell 7.4+. No separate .NET SDK is required for `axmol genbindings`.

## CMake

- [![github](https://img.shields.io/github/v/release/Kitware/CMake?label=Upstream)](https://github.com/Kitware/CMake)
- Minimum version (Axmol command line): 4.2.0
- Preferred version: 4.4.3
- License: [BSD-3-Clause](https://cmake.org/licensing/)
- Platform: Win32/macOS/Linux
- Managed by: `1k/build.profiles`
- Note: The root `CMakeLists.txt` declares a minimum of 3.22; `cmake/Modules/AXConfigDefine.cmake` raises it to 3.27 on Windows. These configuration minimums differ from the command-line tool requirement.

## axslcc

- [![github](https://img.shields.io/github/v/release/axmolengine/axslcc?label=Upstream)](https://github.com/axmolengine/axslcc)
- Pinned version: 3.99.2
- License: [MIT](https://github.com/axmolengine/axslcc/blob/master/LICENSE)
- Platform: Win32/macOS/Linux
- Managed by: `1k/build.profiles`
- Note: Axmol's HLSL-first cross-platform shader compiler, used when building shaders for the engine and applications.

## LLVM

- [![github](https://img.shields.io/github/v/release/llvm/llvm-project?label=Upstream)](https://github.com/llvm/llvm-project)
- Pinned version: 21.1.8
- clang-format CI baseline: 21.x (major version only; no pinned patch release)
- License: [Apache-2.0 WITH LLVM-exception](https://llvm.org/docs/DeveloperPolicy.html#new-llvm-project-license-framework)
- Platform: Win32/macOS/Linux
- Managed by: `1k/build.profiles`, `1k/llvm.ps1`; libclang: `tools/cmdline/plugins/genbindings.ps1`; clang-format CI: `.github/workflows/source-tidy.yml`
- Clang: Supplies the compiler toolchain where selected. Android NDK, Xcode and emsdk provide their own compiler toolchains.
- libclang: Lua binding generation uses the managed library package directly through `axmol genbindings`.
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html): Formats source code; use the same major version as CI to keep formatting results consistent.

## Microsoft.Windows.CppWinRT

- [![nuget](https://img.shields.io/nuget/v/Microsoft.Windows.CppWinRT?label=Upstream)](https://www.nuget.org/packages/Microsoft.Windows.CppWinRT)
- Version: 3.0.260818.1
- License: MIT
- Platform: WinRT/WinUWP
- Managed by: `cmake/Modules/AXConfigDefine.cmake`

## Microsoft.Web.WebView2

- [![nuget](https://img.shields.io/nuget/v/Microsoft.Web.WebView2?label=Upstream)](https://www.nuget.org/packages/Microsoft.Web.WebView2)
- Version: 1.0.4258.31
- License: https://www.nuget.org/packages/Microsoft.Web.WebView2/1.0.4191.47/License
- Platform: Win32
- Managed by: `cmake/Modules/AXConfigDefine.cmake`

## Microsoft Build of OpenJDK

- [![Upstream JDK 17](https://img.shields.io/badge/dynamic/regex?url=https%3A%2F%2Flearn.microsoft.com%2Fen-us%2Fjava%2Fopenjdk%2Fdownload&search=microsoft-jdk-%2817%5C.%5B0-9.%5D%2B%29-linux-x64&replace=%241&label=Upstream%20JDK%2017)](https://learn.microsoft.com/en-us/java/openjdk/download#openjdk-17)
- Minimum version: 17.0.10
- Preferred version: 17.0.20.1
- License: [GPLv2 with Classpath Exception](https://learn.microsoft.com/en-us/java/openjdk/faq#how-are-these-binaries-licensed)
- Platform: Android (build host: Win32/macOS/Linux)
- Managed by: `1k/build.profiles`
- Note: Used by Android SDK command-line tools and Gradle builds.

## Android NDK

- [![github](https://img.shields.io/github/v/release/android/ndk?label=Upstream)](https://github.com/android/ndk/releases)
- Pinned version: r27d
- License: [Android SDK License Agreement and component licenses](https://developer.android.com/studio/terms)
- Platform: Android (build host: Win32/macOS/Linux)
- Managed by: `1k/build.profiles`
- Note: Used for Android native builds and the Android target headers consumed by Lua binding generation.

## Android SDK

- [![Upstream API](https://img.shields.io/badge/dynamic/xml?url=https%3A%2F%2Fdl.google.com%2Fandroid%2Frepository%2Frepository2-3.xml&query=floor%28%28%2F%2FremotePackage%5Bstarts-with%28%40path%2C%27platforms%3Bandroid-%27%29%20and%20translate%28substring-after%28%40path%2C%27platforms%3Bandroid-%27%29%2C%270123456789.%27%2C%27%27%29%3D%27%27%20and%20channelRef%2F%40ref%3D%27channel-0%27%20and%20not%28obsolete%29%5D%2Ftype-details%2Fapi-level%29%5B1%5D%29&label=Upstream%20API)](https://developer.android.com/guide/topics/manifest/uses-sdk-element#ApiLevels)
- Default compileSdk / targetSdk: 37 (Android 17)
- License: [Android SDK License Agreement and component licenses](https://developer.android.com/studio/terms)
- Platform: Android (build host: Win32/macOS/Linux)
- Managed by: `1k/build.profiles`
- Note: Command-line Tools provide `sdkmanager`; Build-tools and the SDK platform are separate packages with independently configured versions. The upstream badge shows the latest stable main API level, excluding minor and extension levels.

## Gradle

- [![github](https://img.shields.io/github/v/release/gradle/gradle?label=Upstream)](https://github.com/gradle/gradle)
- Pinned version: 9.8.0
- License: Apache-2.0
- Platform: Android
- Managed by: `1k/build.profiles`

## Android Gradle Plugin

- [![Upstream AGP](https://img.shields.io/maven-metadata/v?metadataUrl=https%3A%2F%2Fdl.google.com%2Fdl%2Fandroid%2Fmaven2%2Fcom%2Fandroid%2Ftools%2Fbuild%2Fgradle%2Fmaven-metadata.xml&filter=%21%2A-%2A&label=Upstream)](https://developer.android.com/build/releases/gradle-plugin)
- Pinned version: 9.4.1
- License: Apache-2.0
- Platform: Android
- Managed by: `1k/build.profiles`, `setup.ps1`
- Note: `setup.ps1` synchronizes the plugin version in Android templates and test projects. The upstream badge excludes prerelease versions.

## emsdk

- [![github](https://img.shields.io/github/v/tag/emscripten-core/emsdk?label=Upstream)](https://github.com/emscripten-core/emsdk)
- Minimum version: 3.1.73
- Preferred version: 6.0.10
- License: Apache-2.0
- Platform: WebAssembly (build host: Win32/macOS/Linux)
- Managed by: `1k/build.profiles`

## ninja-build

- [![github](https://img.shields.io/github/v/release/ninja-build/ninja?label=Upstream)](https://github.com/ninja-build/ninja)
- Minimum version: 1.10.0
- Preferred version: 1.13.2
- License: Apache-2.0
- Platform: Android/WebAssembly and desktop builds using a Ninja generator
- Managed by: `1k/build.profiles`
- Note: Android, WASM and WASM64 builds use Ninja by default. Desktop builds also require it when using a Ninja generator.
