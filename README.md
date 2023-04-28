<p align="center"><a href="https://axmolengine.github.io/axmol" target="_blank" rel="noopener noreferrer"><img width="160" src="docs/logo.png" alt="axmol logo"></a></p>

# Axmol Engine

## A Multi-platform Engine for Desktop, XBOX (UWP) and Mobile games. (A radical fork of Cocos2d-x-4.0)

[![dev](https://img.shields.io/github/v/release/axmolengine/axmol?include_prereleases&label=release)](https://github.com/axmolengine/axmol/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/axmolengine/axmol/blob/master/LICENSE)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/81fa1aba09ab41a98b949064b928d06e)](https://www.codacy.com/gh/axmolengine/axmol/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=axmolengine/axmol&amp;utm_campaign=Badge_Grade)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/axmolengine/axmol/pulls)
  
![issues](https://img.shields.io/github/issues/axmolengine/axmol?style=plastic)
![forks](https://img.shields.io/github/forks/axmolengine/axmol?style=plastic)
![stars](https://img.shields.io/github/stars/axmolengine/axmol?style=plastic)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/axmolengine/axmol?style=plastic)  

**[简体中文](README_CN.md)**

## Supported Platforms

| Build | Status (github) |
|-------|-----------------|
| Windows Desktop|[![Win32 CI](https://github.com/axmolengine/axmol/actions/workflows/windows.yml/badge.svg)](https://github.com/axmolengine/axmol/actions/workflows/windows.yml)|
| Windows Desktop(Clang)|[![Win32 CI](https://github.com/axmolengine/axmol/actions/workflows/windows-clang.yml/badge.svg)](https://github.com/axmolengine/axmol/actions/workflows/windows-clang.yml)|
| Windows UWP|[![Win32 CI](https://github.com/axmolengine/axmol/actions/workflows/winuwp.yml/badge.svg)](https://github.com/axmolengine/axmol/actions/workflows/winuwp.yml)|
| Android | [![Android Build Status](https://github.com/axmolengine/axmol/workflows/android/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aandroid)|
| iOS |[![iOS Build Status](https://github.com/axmolengine/axmol/workflows/ios/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aios)|
| tvOS |[![tvOS Build Status](https://github.com/axmolengine/axmol/workflows/tvos/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Atvos)|
| Linux |[![Linux Build Status](https://github.com/axmolengine/axmol/workflows/linux/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Alinux)|
| macOS |[![macOS Build Status](https://github.com/axmolengine/axmol/workflows/osx/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aosx)|

## Reimplemented VideoPlayer(VideoTexture) based on Redesigned MediaEngine

|  Platform             |    MediaEngine    | Video Compress Format | Video Pixel Format   | Backend            |
|-----------------------|-------------------|-----------------------|----------------------|--------------------|
| Windows Desktop       |    complete       | H264, HEVC, VP90      | YUY2, NV12, RGB32    | IMFMediaSession    |
| Windows UWP           |    complete       | H264, HEVC, VP90      | BGR32                | IMFMediaEngine     |
| Apple macOS           |    complete       | H264, HEVC(hvc1)      | NV12, BGR32          | AVFoundation       |
| Apple tvOS            |     complete      | H264, HEVC(hvc1)      | NV12, BGR32          | AVFoundation       |
| Apple iOS             |    in progress    | H264, HEVC(hvc1)      | NV12, BGR32          | AVFoundation       |
| Android               |    planned        | H264                  | RGB32                |                    |

### View code with vscode online

- [![github1s](https://img.shields.io/badge/github1s-green.svg)](https://github1s.com/axmolengine/axmol)
- [![vscode.dev](https://img.shields.io/badge/vscode.dev-green.svg)](https://vscode.dev/github/axmolengine/axmol)

### Thirdparty

- All thirdparty prebuilt libs are built from <https://github.com/axmolengine/buildware> via github actions automatically.
  
### Highlighted Features

- Windows UWP support, refer to: https://github.com/axmolengine/axmol/pull/1108
- Add apple M1, android x64 support, contributed by @pietpukkel
- Improve windows workflow, support linking with engine prebuilt libs, read [windows workflow guide](https://github.com/axmolengine/axmol/issues/564)
- Windows video player support (based on microsoft media foundation)
- Windows x64 build support
- Reimplement HttpClient based on yasio for concorrent http requests processing.
- ['Upstream-Version-License'](thirdparty/README.md) Third-party
  - Third-party license overview for easier publishing of your commercial apps based on axmol framework.
  - Some links to third party libs which support axmol too.
- ['Upstream-Version-License'](extensions/README.md) Extensions
  - Extensions license overview for easier publishing of your commercial apps based on axmol framework.
  - Spine-3.8 support
  - ```FairyGUI``` support
  - DragonBones support
  - Live2D support
  - **ImGui integrated, easy to write game embedded tools, very easy to use, read [ImGui](extensions/ImGui/README.md) for more info**
- Refactor AudioEngine, OpenAL for all platforms
  - [OpenAL Soft](https://github.com/kcat/openal-soft), pass -DAX_USE_ALSOFT=ON to cmake to force enabling it
  - [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no ```AX_USE_ALSOFT``` option specified, cmake script will choose it on osx/ios/tvos, even though it was marked as deprecated, but still available.
- Refactor UserDefault with [mio](https://github.com/mandreyel/mio)
- Modularize all optional extensions, move from engine core folder to an extensions folder
- Implement all .wav formats supported by ```OpenAL Soft```, such as MS-ADPCM, ADPCM, ...
- Use a modern GL loader ```Glad```
- Google [angle](https://github.com/google/angle) renderer backend support
- C++ 17/20
- IOS/TVOS SDK 9.0 as minimal deployment
- Use fast pugixml
- Use [curl](https://github.com/curl/curl) for transferring data with URL syntax
- Use SAX parser for all plist files
- ASTC 4x4/6x6/8x8 support (if hardware decoding is not supported, then software decoding is used)
- ETC2 RGB/RGBA support    (if hardware decoding is not supported, then software decoding is used)
- Supported 2D physics engines (see also [APPENDIX.md](APPENDIX.md)):
  - Box2D
  - Box2D-optimized
  - Chipmunk2D
- Supported 3D physics engines:
  - Bullet Physics SDK

[Read Full changes since cocos2d-x-4.0](CHANGELOG)

Open [APPENDIX.md](APPENDIX.md) for additional information and see [Milestones](https://github.com/axmolengine/axmol/milestones) for planed features too.

### Quick Start

#### Common Requirement [Python](https://www.python.org/downloads/)

- Python-3.7+

#### Prerequisites

  1. Enter `axmol` root directory
  2. Run `python setup.py`, restart the console after it has finished for environment variables to take effect

#### Creating A New Project

Using a console window, an example of a command to generate a new project is as follows:

```axmol new -p YOUR.UNIQUE.ID -d PROJECT_PATH -l [cpp|lua] [--portrait] PROJECT_NAME```

Type `axmol new --help` at the command line for more options you can pass to `axmol new`

Examples:

- Cpp: `axmol new -p org.axmol.hellocpp -d D:\dev\projects\ -l cpp --portrait HelloCpp`
- Lua: `axmol new -p org.axmol.hellolua -d D:\dev\projects\ -l lua --portrait HelloLua`

#### Windows (64/32 bit  Visual Studio 2019/2022)

  1. Install [CMake](https://cmake.org/)  3.22.1+  
  2. Install Visual Studio 2019/2022 (it's recommended that you only use these versions)  
  3. Create a new project as shown [here](#creating-a-new-project)
  4. In a console window, navigate into the root directory of the project you created in the previous step
  5. Generate the relevant Visual Studio project using the cmake command:
  
     ```cmake -S SOURCE_DIR -B BUILD_DIR -G VISUAL_STUDIO_VERSION_STRING -A [Win32|x64]```

     For example, say `SOURCE_DIR` is the current path `"."`, and `BUILD_DIR` (out-of-source build directory) is named `"build"`:

     a) C++ 20:
        - for 32 bit Visual Studio 2019:
            ```cmake -S . -B build -G "Visual Studio 16 2019" -A Win32```
        - for 64 bit Visual Studio 2019:
            ```cmake -S . -B build -G "Visual Studio 16 2019" -A x64```
        - for 32 bit Visual Studio 2022:
            ```cmake -S . -B build -G "Visual Studio 17 2022" -A Win32```
        - for 64 bit Visual Studio 2022:
            ```cmake -S . -B build -G "Visual Studio 17 2022" -A x64```

     b) C++17
         - add `-DCMAKE_CXX_STANDARD=17` for C++17 on your command line like:
         cmake -S . -B build `-DCMAKE_CXX_STANDARD=17` -G "Visual Studio 17 2022" -A x64


  6. Use Visual Studio to open the newly created solution file. For example, `./build/ProjectName.sln`

#### Windows UWP (Visual Studio 2022), because microsoft limit, only support C++17
  ```cmake -B build_uwp -DCMAKE_SYSTEM_NAME=WindowsStore "-DCMAKE_SYSTEM_VERSION=10.0" "-DAX_VS_DEPLOYMENT_TARGET=10.0.17763.0"```  

##### Creating the Visual Studio solution for all axmol test projects (Win/UWP)

  - Perform steps 1.-6. or the Windows UWP step above (if not done)
  7. Open the solution (".\build\axmol.sln" or ".\build_uwp\axmol.sln") in Visual Studio and build any of the test projects via the IDE.

#### Improve 'Visual Studio' workflow, support linking with engine prebuilt libs

See [windows workflow guide](https://github.com/axmolengine/axmol/issues/564)

#### Android

##### With Android Studio

  1. Install Android Studio 2021.1.1+
  2. When starting Android Studio for the first time, it will guide you to install the SDK and other tools, so ensure that you do install them.
  3. Start Android and choose [Open an existing Android Studio Project] and select your project. For example, the existing cpp-test project located in ```axmol\tests\cpp-tests\proj.android```
  4. Start Android Studio and Open [Tools][SDKManager], then switch to ```SDK Tools```, check the ```Show Package Details```, choose the following tools and click the button ```Apply``` to install them:  
     - Android SDK Platform 33  
     - Android Gradle Plugin (AGP) 7.4.2  
     - Android SDK Build-Tools 30.0.3 match with AGP, refer to: <https://developer.android.com/studio/releases/gradle-plugin>
     - Gradle 8.1  
     - NDK r23c+  
     - CMake 3.22.1+  
  5. Wait for ```Gradle sync``` finish.
  6. Note: If you use non-sdk provided CMake edition, you will need to download ```ninja``` from <https://github.com/ninja-build/ninja/releases>, and copy ```ninja.exe``` to cmake's bin directory
  
##### Without Android Studio

  1. Download command-tools from <https://developer.android.com/studio#command-tools>, for example: https://dl.google.com/android/repository/commandlinetools-win-9477386_latest.zip
  2. Install Android devtools (for example in windows)

  ```bat
  # unzip command-tools at D:\dev\adt\
  # Install android devtools
  cd D:\dev\adt\
  mkdir sdk
  .\cmdline-tools\bin\sdkmanager.bat --verbose --sdk_root=D:\dev\adt\sdk "platform-tools" "cmdline-tools;latest" "platforms;android-33" "build-tools;30.0.3" "cmake;3.22.1" "ndk;23.2.8568313"
  set ANDROID_HOME=D:\dev\adt\sdk
  
  # Goto xxx\proj.android
  .\gradlew.bat assembleRelease -PPROP_BUILD_TYPE=cmake -PPROP_APP_ABI=arm64-v8a --parallel --info
  ```

#### iOS, tvOS and macOS

  1. Ensure xcode12+ & [cmake3.21+](https://github.com/Kitware/CMake/releases) are installed, install cmake command line support: ```sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install```
  2. Create a new project as shown [here](#creating-a-new-project)
  3. In a console window, navigate into the root directory of the project you created in the previous step
  4. Execute the following command
   ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```
  5. Generate the relevant xcode project using one of the following commands:
     - for ios arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=OS64```
     - for ios armv7,arm64 combined:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=OS```
     - for ios simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=SIMULATOR64```
     - for tvos arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=TVOS```
     - for tvos simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=SIMULATOR_TVOS```
     - for macos x86_64(Intel)
     ```cmake -S . -B build -GXcode -DCMAKE_OSX_ARCHITECTURES=x86_64```
     - for macos arm64(M1)
     ```cmake -S . -B build -GXcode -DCMAKE_OSX_ARCHITECTURES=arm64```

  6. After cmake finishes generating, you can open the xcode project at ```build``` folder and run cpp-tests or other test targets.  
  7. Notes  
     - **The code signing is required to run the ios/tvos app on your device, just change the bundle identifier until the auto manage signing is solved**  
     - **axmol only provides aarm64, x86_64 prebuilt libraries for ios/tvos**

### Reference links

- Official Cocos2d-x Repo: <https://github.com/cocos2d/cocos2d-x>
- [Some interesting related projects based on axmol](https://github.com/axmolengine/axmol/discussions/694)
- [Axmols contributing guide](https://github.com/axmolengine/axmol/discussions/411)

### The axmol Active Stats
![Alt](https://repobeats.axiom.co/api/embed/6fcb8168a3af91ba9e797a1f14a3c2edc42ac56a.svg "Repobeats analytics image")
